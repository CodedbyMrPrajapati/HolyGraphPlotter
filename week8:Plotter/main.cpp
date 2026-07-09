#include "gl_framework.hpp"
#include "../shader_util.hpp"
#include "parser.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define BUFFER_OFFSET(offset) ((void*)(offset))

// --- State shared with the local gl_framework key callback -----------------
GLfloat xrot=0.0, yrot=0.0, zrot=0.0;  // camera orbit
bool    gPaused    = false;            // SPACE toggles playback
float   gSpeed     = 1.0f;             // +/- adjusts animation speed
int     gRenderMode = 2;               // 0 = wireframe, 1 = flat, 2 = phong

// --- Surface sampling grid --------------------------------------------------
const int   N = 100;
const float DOMAIN_MIN = -3.14159265f;
const float DOMAIN_MAX =  3.14159265f;

GLuint shaderProgram;
GLuint vao, vbo, ebo;
int index_count = 0;

glm::mat4 view_matrix;
glm::mat4 projection_matrix;

GLint uModelMatrix;
GLint uModelViewProjectMatrix;
GLint uNormalMatrix;
GLint uLightPos;
GLint uViewPos;
GLint uZMin;
GLint uZMax;
GLint uShadingMode;

const glm::vec3 eye_position   = glm::vec3(7.0f, 7.0f, 7.0f);
const glm::vec3 light_position = glm::vec3(5.0f, 8.0f, 6.0f);

// The parsed surface equation z = f(x, y, t)
dsl::Expression g_surface;
bool g_usesTime = true;   // does the equation actually depend on t? (set at load)

struct Vertex {
    glm::vec4 pos;
    glm::vec3 normal;
};
std::vector<Vertex> vertices;
std::vector<GLuint> indices;
std::vector<float>  zgrid;   // scratch height grid for normal estimation

float g_zmin = -1.0f, g_zmax = 1.0f;

namespace
{
std::string resolveDataPath(const std::string& file)
{
    namespace fs = std::filesystem;

    const fs::path dir("week8:Plotter");
    const fs::path cwd = fs::current_path();
    const fs::path candidates[] = {
        cwd / file,
        cwd / dir / file,
        cwd.parent_path() / dir / file,
        cwd.parent_path().parent_path() / dir / file
    };

    for (const fs::path& candidate : candidates) {
        if (fs::exists(candidate)) {
            return candidate.string();
        }
    }

    throw std::runtime_error("Cannot find file: " + file);
}

// Read the first non-comment, non-empty line from the equation file.
std::string readEquationFile(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open equation file: " + path);

    std::string line;
    while (std::getline(f, line)) {
        // trim leading whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;      // blank line
        if (line[start] == '#') continue;              // comment
        return line.substr(start);
    }
    throw std::runtime_error("No equation found in file: " + path);
}
}

// Rebuild the mesh for a given time t by evaluating the parsed expression over
// the grid. Normals are estimated by central differences of the height grid
// (the expression is arbitrary, so an analytic gradient is not available).
void rebuildMesh(float t)
{
    // 1. Sample heights. Reuse a single Env across the whole grid (just update
    // the bindings) instead of allocating a fresh map per vertex each frame.
    dsl::Env env{ {"x", 0.0}, {"y", 0.0}, {"t", double(t)} };
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) {
            float u = float(i) / float(N - 1);
            float v = float(j) / float(N - 1);
            float x = DOMAIN_MIN + u * (DOMAIN_MAX - DOMAIN_MIN);
            float y = DOMAIN_MIN + v * (DOMAIN_MAX - DOMAIN_MIN);

            env["x"] = x;
            env["y"] = y;
            double z = g_surface.eval(env);
            if (!std::isfinite(z)) z = 0.0;

            int idx = j * N + i;
            vertices[idx].pos = glm::vec4(x, y, float(z), 1.0f);
            zgrid[idx] = float(z);
        }
    }

    // 2. Track height range for the color map
    g_zmin = zgrid[0];
    g_zmax = zgrid[0];
    for (float z : zgrid) { g_zmin = std::min(g_zmin, z); g_zmax = std::max(g_zmax, z); }
    if (g_zmax - g_zmin < 1e-4f) g_zmax = g_zmin + 1e-4f;

    // 3. Central-difference normals
    const float dx = (DOMAIN_MAX - DOMAIN_MIN) / float(N - 1);
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) {
            int idx = j * N + i;
            int il = (i > 0)     ? i - 1 : i;
            int ir = (i < N - 1) ? i + 1 : i;
            int jd = (j > 0)     ? j - 1 : j;
            int ju = (j < N - 1) ? j + 1 : j;

            float dzdx = (zgrid[j * N + ir] - zgrid[j * N + il]) / ((ir - il) * dx);
            float dzdy = (zgrid[ju * N + i] - zgrid[jd * N + i]) / ((ju - jd) * dx);
            vertices[idx].normal = glm::normalize(glm::vec3(-dzdx, -dzdy, 1.0f));
        }
    }

    // 4. Upload
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}

void buildIndices()
{
    indices.clear();
    for (int j = 0; j < N - 1; ++j) {
        for (int i = 0; i < N - 1; ++i) {
            GLuint a = j * N + i;
            GLuint b = j * N + (i + 1);
            GLuint c = (j + 1) * N + i;
            GLuint d = (j + 1) * N + (i + 1);
            indices.push_back(a); indices.push_back(b); indices.push_back(c);
            indices.push_back(c); indices.push_back(b); indices.push_back(d);
        }
    }
    index_count = static_cast<int>(indices.size());
}

// Look up a uniform location, aborting if it is missing (renamed in the shader
// or optimized out) instead of silently no-op'ing at draw time.
static GLint requireUniform(GLuint program, const char* name) {
    GLint loc = glGetUniformLocation(program, name);
    if (loc < 0)
        throw std::runtime_error(std::string("Could not find uniform: ") + name);
    return loc;
}

void initShader(){
    std::string vertex_shader_file = resolveDataPath("vshader.glsl");
    std::string fragment_shader_file = resolveDataPath("fshader.glsl");

    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);

    uModelMatrix            = requireUniform(shaderProgram, "uModelMatrix");
    uModelViewProjectMatrix = requireUniform(shaderProgram, "uModelViewProjectMatrix");
    uNormalMatrix           = requireUniform(shaderProgram, "uNormalMatrix");
    uLightPos               = requireUniform(shaderProgram, "uLightPos");
    uViewPos                = requireUniform(shaderProgram, "uViewPos");
    uZMin                   = requireUniform(shaderProgram, "uZMin");
    uZMax                   = requireUniform(shaderProgram, "uZMax");
    uShadingMode            = requireUniform(shaderProgram, "uShadingMode");
}

void initVertexBufferGL(void)
{
    vertices.assign(N * N, Vertex{});
    zgrid.assign(N * N, 0.0f);
    buildIndices();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec4)));

    rebuildMesh(0.0f);
}

void renderGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    // Wireframe vs filled
    glPolygonMode(GL_FRONT_AND_BACK, (gRenderMode == 0) ? GL_LINE : GL_FILL);

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1,0,0));
    model = glm::rotate(model, yrot, glm::vec3(0,1,0));
    model = glm::rotate(model, zrot, glm::vec3(0,0,1));

    view_matrix = glm::lookAt(eye_position, glm::vec3(0.0f), glm::vec3(0.0f,0.0f,1.0f));
    projection_matrix = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection_matrix * view_matrix * model;
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));

    glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glUniform3fv(uLightPos, 1, glm::value_ptr(light_position));
    glUniform3fv(uViewPos, 1, glm::value_ptr(eye_position));
    glUniform1f(uZMin, g_zmin);
    glUniform1f(uZMax, g_zmax);
    // flat shading uses a per-face normal (mode 1); wireframe & phong use the smooth normal
    glUniform1i(uShadingMode, (gRenderMode == 1) ? 1 : 0);

    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}

int main(int argc, char** argv)
{
    // Load the equation: argv[1] if given, else the bundled equation.txt
    try {
        std::string eqPath = (argc > 1) ? std::string(argv[1]) : resolveDataPath("equation.txt");
        std::string eqText = readEquationFile(eqPath);
        g_surface = plotter::parseExpression(eqText);
        std::cout << "Surface: z = " << g_surface.str() << std::endl;

        // Probe-evaluate once with x/y/t all bound. This surfaces unbound
        // variables ("z", "w", ...) or unknown functions ("sinh", ...) NOW,
        // cleanly, instead of aborting the process later inside rebuildMesh.
        g_surface.eval({ {"x", 0.0}, {"y", 0.0}, {"t", 0.0} });

        // Detect time dependence exactly: evaluate with t left unbound. If the
        // equation uses t it throws (unbound t) — every other variable is
        // already known to be x/y from the probe above. A static surface only
        // needs to be built once.
        try {
            g_surface.eval({ {"x", 0.0}, {"y", 0.0} });
            g_usesTime = false;
        } catch (const std::exception&) {
            g_usesTime = true;
        }
        std::cout << (g_usesTime ? "(time-varying: animated)" : "(static: built once)") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load equation: " << e.what() << std::endl;
        return -1;
    }

    GLFWwindow* window;
    glfwSetErrorCallback(csX75::error_callback);

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Week 8 - Holy Graph Plotter", NULL, NULL);
    if (!window){
        std::cerr << "Can't Initialize Window";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
        std::cerr<<"GLEW Init Failed"<<std::endl;

    std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
    std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;
    std::cout<<"\nControls: SPACE play/pause | +/- speed | 1 wireframe 2 flat 3 phong (M cycle) "
               "| arrows/PgUp/PgDn orbit | ESC quit"<<std::endl;

    glfwSetKeyCallback(window, csX75::key_callback);
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    csX75::initGL();
    initShader();
    initVertexBufferGL();

    int fb_w = 0, fb_h = 0;
    glfwGetFramebufferSize(window, &fb_w, &fb_h);
    glViewport(0, 0, fb_w, fb_h);

    float lastTime = static_cast<float>(glfwGetTime());
    float t = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt = now - lastTime;
        lastTime = now;

        // Static surfaces are built once (in initVertexBufferGL); only rebuild
        // when the equation actually depends on t and we're not paused.
        if (!gPaused && g_usesTime) {
            t += gSpeed * dt;
            rebuildMesh(t);
        }

        renderGL();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
