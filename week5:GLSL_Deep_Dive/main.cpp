#include "gl_framework.hpp"
#include "../shader_util.hpp"
#include <filesystem>
#include <stdexcept>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#define BUFFER_OFFSET(offset) ((void*)(offset))

GLuint shaderProgram;
GLuint vbo, vao;

namespace {
std::string resolveShaderPath(const std::string& shader_file)
{
    namespace fs = std::filesystem;
    const fs::path shader_dir("week5:GLSL_Deep_Dive");
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

const int num_vertices = 36;

int tri_idx=0;
glm::vec4 v_positions[num_vertices];
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

void quad(int a, int b, int c, int d, glm::vec3 normal)
{
    v_positions[tri_idx] = positions[a]; v_normals[tri_idx] = normal; tri_idx++;
    v_positions[tri_idx] = positions[b]; v_normals[tri_idx] = normal; tri_idx++;
    v_positions[tri_idx] = positions[c]; v_normals[tri_idx] = normal; tri_idx++;
    v_positions[tri_idx] = positions[a]; v_normals[tri_idx] = normal; tri_idx++;
    v_positions[tri_idx] = positions[c]; v_normals[tri_idx] = normal; tri_idx++;
    v_positions[tri_idx] = positions[d]; v_normals[tri_idx] = normal; tri_idx++;
}

void colorcube()
{
        tri_idx = 0;
        quad( 1, 0, 3, 2, glm::vec3(0, 0, 1) );    // +Z (Front)
        quad( 2, 3, 7, 6, glm::vec3(0, 0, -1) );   // -Z (Back)
        quad( 3, 0, 4, 7, glm::vec3(-1, 0, 0) );   // -X (Left)
        quad( 6, 5, 1, 2, glm::vec3(1, 0, 0) );    // +X (Right)
        quad( 4, 5, 6, 7, glm::vec3(0, 1, 0) );    // +Y (Top)
        quad( 5, 4, 0, 1, glm::vec3(0, -1, 0) );   // -Y (Bottom)
}

void initShader(){
    std::string vertex_shader_file = resolveShaderPath("vshader.glsl");
    std::string fragment_shader_file = resolveShaderPath("fshader.glsl");

    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);
}

void initVertexBufferGL(void)
{
    // Build cube using week3-style generator and per-face normals (like week4)
    colorcube();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_positions) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(v_normals), v_normals);

    // position (vec4)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)));
}

void renderGL(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    // Camera and transforms
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(25.0f), glm::vec3(1.0f, 0.6f, 0.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(2.5f,2.0f,2.5f), glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 mvp = proj * view * model;

    GLint locMVP = glGetUniformLocation(shaderProgram, "uModelViewProjectMatrix");
    GLint locModel = glGetUniformLocation(shaderProgram, "uModelMatrix");
    GLint locNormalMat = glGetUniformLocation(shaderProgram, "uNormalMatrix");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
    glUniformMatrix3fv(locNormalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

    // Setup uniforms for light and material
    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "light.position");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram, "light.color");
    GLint matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    GLint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    GLint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    GLint matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");
    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    glUniform3f(lightPosLoc, 2.0f, 2.0f, 2.0f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(matAmbientLoc, 0.1f, 0.1f, 0.1f);
    glUniform3f(matDiffuseLoc, 0.4f, 0.6f, 0.8f);
    glUniform3f(matSpecularLoc, 0.9f, 0.9f, 0.9f);
    glUniform1f(matShineLoc, 64.0f);
    glUniform3f(viewPosLoc, 2.5f, 2.0f, 2.5f);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(int argc, char** argv)
{
    GLFWwindow* window;
    glfwSetErrorCallback(csX75::error_callback);
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Week 5: GLSL Deep Dive", NULL, NULL);
    if (!window){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr<<"GLEW Init Failed"<<std::endl; }

    std::cout<<"GL Version: "<<glGetString(GL_VERSION)<<std::endl;

    glfwSetKeyCallback(window, csX75::key_callback);
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    csX75::initGL();
    initShader();
    initVertexBufferGL();

    int fw, fh; glfwGetFramebufferSize(window, &fw, &fh); glViewport(0,0,fw,fh);

    while (!glfwWindowShouldClose(window)) {
        renderGL();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
