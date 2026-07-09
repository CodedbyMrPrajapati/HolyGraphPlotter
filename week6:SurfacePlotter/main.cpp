#include "../gl_framework.hpp"
#include "../shader_util.hpp"
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <cmath>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define BUFFER_OFFSET(offset) ((void*)(offset))

// Rotation Parameters (driven by the shared arrow-key callback in ../gl_framework.cpp)
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
glm::mat4 rotation_matrix;

// Surface sampling grid. N x N vertices over [DOMAIN_MIN, DOMAIN_MAX] in both x and y.
const int   N = 120;
const float DOMAIN_MIN = -3.14159265f;
const float DOMAIN_MAX =  3.14159265f;
const float Z_MIN = -1.0f;   // range of z = sin(x)cos(y), used to normalise height color
const float Z_MAX =  1.0f;

GLuint shaderProgram;
GLuint vao, vbo, ebo;
int index_count = 0;

glm::mat4 view_matrix;
glm::mat4 projection_matrix;

GLint uModelMatrix;
GLint uModelViewProjectMatrix;
GLint uNormalMatrix;
GLint uLightPos;
GLint uZMin;
GLint uZMax;

const glm::vec3 eye_position   = glm::vec3(6.0f, 6.0f, 6.0f);
const glm::vec3 light_position = glm::vec3(5.0f, 8.0f, 5.0f);

// Interleaved per-vertex data: position (vec4) + normal (vec3)
struct Vertex {
    glm::vec4 pos;
    glm::vec3 normal;
};
std::vector<Vertex> vertices;
std::vector<GLuint> indices;

namespace
{
std::string resolveShaderPath(const std::string& shader_file)
{
    namespace fs = std::filesystem;

    const fs::path shader_dir("week6:SurfacePlotter");
    const fs::path cwd = fs::current_path();
    const fs::path candidates[] = {
        cwd / shader_file,
        cwd / shader_dir / shader_file,
        cwd.parent_path() / shader_dir / shader_file,
        cwd.parent_path().parent_path() / shader_dir / shader_file
    };

    for (const fs::path& candidate : candidates) {
        if (fs::exists(candidate)) {
            return candidate.string();
        }
    }

    throw std::runtime_error("Cannot find shader file: " + shader_file);
}
}

// The hardcoded surface for this week.
float surfaceZ(float x, float y) {
    return std::sin(x) * std::cos(y);
}

// Analytic smooth normal from the gradient of f(x,y) = sin(x)cos(y).
//   dz/dx =  cos(x)cos(y)
//   dz/dy = -sin(x)sin(y)
// Surface normal of z = f(x,y) is normalize(-df/dx, -df/dy, 1).
glm::vec3 surfaceNormal(float x, float y) {
    float dzdx = std::cos(x) * std::cos(y);
    float dzdy = -std::sin(x) * std::sin(y);
    return glm::normalize(glm::vec3(-dzdx, -dzdy, 1.0f));
}

void buildSurfaceMesh(int n)
{
    vertices.clear();
    indices.clear();
    vertices.reserve(n * n);

    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            float u = float(i) / float(n - 1);
            float v = float(j) / float(n - 1);
            float x = DOMAIN_MIN + u * (DOMAIN_MAX - DOMAIN_MIN);
            float y = DOMAIN_MIN + v * (DOMAIN_MAX - DOMAIN_MIN);
            float z = surfaceZ(x, y);

            Vertex vert;
            vert.pos = glm::vec4(x, y, z, 1.0f);
            vert.normal = surfaceNormal(x, y);
            vertices.push_back(vert);
        }
    }

    // Two triangles per grid cell.
    for (int j = 0; j < n - 1; ++j) {
        for (int i = 0; i < n - 1; ++i) {
            GLuint a = j * n + i;
            GLuint b = j * n + (i + 1);
            GLuint c = (j + 1) * n + i;
            GLuint d = (j + 1) * n + (i + 1);

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
    std::string vertex_shader_file = resolveShaderPath("vshader.glsl");
    std::string fragment_shader_file = resolveShaderPath("fshader.glsl");

    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);

    uModelMatrix            = requireUniform(shaderProgram, "uModelMatrix");
    uModelViewProjectMatrix = requireUniform(shaderProgram, "uModelViewProjectMatrix");
    uNormalMatrix           = requireUniform(shaderProgram, "uNormalMatrix");
    uLightPos               = requireUniform(shaderProgram, "uLightPos");
    uZMin                   = requireUniform(shaderProgram, "uZMin");
    uZMax                   = requireUniform(shaderProgram, "uZMax");
}

void initVertexBufferGL(void)
{
    buildSurfaceMesh(N);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // position: attribute 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
    // normal: attribute 2 (attribute 1 reserved for color, unused here)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(glm::vec4)));
}

void renderGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    rotation_matrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f,0.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, yrot, glm::vec3(0.0f,1.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, zrot, glm::vec3(0.0f,0.0f,1.0f));
    glm::mat4 model = rotation_matrix;

    view_matrix = glm::lookAt(eye_position, glm::vec3(0.0f), glm::vec3(0.0f,0.0f,1.0f));
    projection_matrix = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection_matrix * view_matrix * model;
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));

    glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glUniform3fv(uLightPos, 1, glm::value_ptr(light_position));
    glUniform1f(uZMin, Z_MIN);
    glUniform1f(uZMax, Z_MAX);

    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}

int main(int argc, char** argv)
{
    (void)argc; (void)argv;
    GLFWwindow* window;

    glfwSetErrorCallback(csX75::error_callback);

    if (!glfwInit())
        return -1;

    //We want OpenGL 4.4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Week 6 - Surface Plotter", NULL, NULL);
    if (!window){
        std::cerr << "Can't Initialize Window";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }
    std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
    std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

    glfwSetKeyCallback(window, csX75::key_callback);
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    csX75::initGL();
    initShader();
    initVertexBufferGL();

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    while (!glfwWindowShouldClose(window)) {
        renderGL();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
