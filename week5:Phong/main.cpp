#include "../gl_framework.hpp"
#include "../shader_util.hpp"
#include <filesystem>
#include <stdexcept>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define BUFFER_OFFSET(offset) ((void*)(offset))

// Rotation Parameters (driven by the shared arrow-key callback in ../gl_framework.cpp)
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
glm::mat4 rotation_matrix;

// 6 faces , 2 triangles/face, 3 vertices/triangle
const int num_vertices = 36;
GLuint shaderProgram;
GLuint vbo, vao;

glm::mat4 view_matrix;
glm::mat4 projection_matrix;

// Uniform locations for the Phong pipeline
GLint uModelMatrix;
GLint uModelViewProjectMatrix;
GLint uNormalMatrix;
GLint uLightPos;
GLint uViewPos;
GLint uLightColor;

// Where the camera sits (also used as uViewPos for the specular term)
const glm::vec3 eye_position = glm::vec3(3.0f, 3.0f, 3.0f);
const glm::vec3 light_position = glm::vec3(4.0f, 5.0f, 6.0f);
const glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

namespace
{
std::string resolveShaderPath(const std::string& shader_file)
{
    namespace fs = std::filesystem;

    const fs::path shader_dir("week5:Phong");
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

int tri_idx=0;
glm::vec4 v_positions[num_vertices];
glm::vec4 v_colors[num_vertices];
glm::vec3 v_normals[num_vertices];

glm::vec4 positions[8] = {
  glm::vec4(-0.5, -0.5, 0.5, 1.0),
  glm::vec4(-0.5, 0.5, 0.5, 1.0),
  glm::vec4(0.5, 0.5, 0.5, 1.0),
  glm::vec4(0.5, -0.5, 0.5, 1.0),
  glm::vec4(-0.5, -0.5, -0.5, 1.0),
  glm::vec4(-0.5, 0.5, -0.5, 1.0),
  glm::vec4(0.5, 0.5, -0.5, 1.0),
  glm::vec4(0.5, -0.5, -0.5, 1.0)
};

// A single object color; the lighting math in the fragment shader shades it.
glm::vec4 object_color = glm::vec4(0.85f, 0.35f, 0.2f, 1.0f);

void quad(int a, int b, int c, int d, glm::vec3 normal)
{
  v_colors[tri_idx] = object_color; v_positions[tri_idx] = positions[a]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = object_color; v_positions[tri_idx] = positions[b]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = object_color; v_positions[tri_idx] = positions[c]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = object_color; v_positions[tri_idx] = positions[a]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = object_color; v_positions[tri_idx] = positions[c]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = object_color; v_positions[tri_idx] = positions[d]; v_normals[tri_idx] = normal; tri_idx++;
}

// generate 12 triangles: 36 vertices, each face carries its outward normal
void colorcube()
{
    quad( 1, 0, 3, 2, glm::vec3(0, 0, 1) );    // +Z (Front)
    quad( 2, 3, 7, 6, glm::vec3(1, 0, 0) );    // +X (Right)
    quad( 3, 0, 4, 7, glm::vec3(0, -1, 0) );   // -Y (Bottom)
    quad( 6, 5, 1, 2, glm::vec3(0, 1, 0) );    // +Y (Top)
    quad( 4, 5, 6, 7, glm::vec3(0, 0, -1) );   // -Z (Back)
    quad( 5, 4, 0, 1, glm::vec3(-1, 0, 0) );   // -X (Left)
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

    uModelMatrix             = requireUniform(shaderProgram, "uModelMatrix");
    uModelViewProjectMatrix  = requireUniform(shaderProgram, "uModelViewProjectMatrix");
    uNormalMatrix            = requireUniform(shaderProgram, "uNormalMatrix");
    uLightPos                = requireUniform(shaderProgram, "uLightPos");
    uViewPos                 = requireUniform(shaderProgram, "uViewPos");
    uLightColor              = requireUniform(shaderProgram, "uLightColor");
}

void initVertexBufferGL(void)
{
    tri_idx = 0;
    colorcube();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_positions) + sizeof(v_colors) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(v_colors), v_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(v_positions) + sizeof(v_colors), sizeof(v_normals), v_normals);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions) + sizeof(v_colors)));
}

void renderGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    // Model = interactive rotation from the arrow keys
    rotation_matrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f,0.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, yrot, glm::vec3(0.0f,1.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, zrot, glm::vec3(0.0f,0.0f,1.0f));
    glm::mat4 model = rotation_matrix;

    view_matrix = glm::lookAt(eye_position, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
    projection_matrix = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection_matrix * view_matrix * model;

    // Normal matrix keeps normals correct under non-uniform transforms
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model)));

    glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glUniform3fv(uLightPos, 1, glm::value_ptr(light_position));
    glUniform3fv(uViewPos, 1, glm::value_ptr(eye_position));
    glUniform3fv(uLightColor, 1, glm::value_ptr(light_color));

    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
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

    window = glfwCreateWindow(640, 480, "Week 5 - Phong Lighting", NULL, NULL);
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
