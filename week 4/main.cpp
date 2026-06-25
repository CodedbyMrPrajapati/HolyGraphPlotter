#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader program handles
GLuint program_id;
GLuint vao;
GLuint vbo;
GLint uMVP_loc;
GLint uFaceColors_loc;

// Camera state (Orbit Camera)
float cameraRadius = 6.0f;
float cameraTheta = 0.6f; // Orbit angles
float cameraPhi = 0.5f;

// Cubie structure
struct Cubie {
    glm::ivec3 gridPos;          // Coordinates in range [-1, 1]
    glm::mat4 currentRotation;   // Accumulated rotation matrix
    glm::vec4 faceColors[6];     // Stationary colors for (+X, -X, +Y, -Y, +Z, -Z)
};

std::vector<Cubie> cubies;

// Animation state
bool isAnimating = false;
int activeAxis = 0;          // 0 = X, 1 = Y, 2 = Z
int activeLayer = 0;         // -1, 0, or 1
float animAngle = 0.0f;
float targetAngle = 0.0f;
float animSpeed = 6.0f;      // Radians per second
float lastFrameTime = 0.0f;

// Rubik's standard face colors:
// +X: Blue, -X: Green, +Y: Yellow, -Y: White, +Z: Red, -Z: Orange
glm::vec4 rubikColors[6] = {
    glm::vec4(0.0f, 0.27f, 0.68f, 1.0f),  // Blue (+X)
    glm::vec4(0.0f, 0.6f, 0.28f, 1.0f),   // Green (-X)
    glm::vec4(1.0f, 0.84f, 0.0f, 1.0f),   // Yellow (+Y)
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // White (-Y)
    glm::vec4(0.72f, 0.07f, 0.2f, 1.0f),   // Red (+Z)
    glm::vec4(1.0f, 0.35f, 0.0f, 1.0f)    // Orange (-Z)
};

// 36 vertices with normals for drawing a single cube
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

Vertex cubeVertices[36] = {
    // Front Face (+Z)
    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },
    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f) },

    // Back Face (-Z)
    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },
    { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f) },

    // Left Face (-X)
    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },
    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },
    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },
    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },
    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },
    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f) },

    // Right Face (+X)
    { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
    { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
    { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },

    // Top Face (+Y)
    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },
    { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f) },

    // Bottom Face (-Y)
    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f) }
};

// Shader compilation helper
GLuint initShadersGL(const std::string& vShaderFile, const std::string& fShaderFile) {
    std::ifstream vShaderFileStream(vShaderFile);
    if (!vShaderFileStream.is_open()) return 0;
    std::stringstream vShaderStream;
    vShaderStream << vShaderFileStream.rdbuf();
    std::string vShaderSourceStr = vShaderStream.str();
    const char* vShaderSource = vShaderSourceStr.c_str();

    std::ifstream fShaderFileStream(fShaderFile);
    if (!fShaderFileStream.is_open()) return 0;
    std::stringstream fShaderStream;
    fShaderStream << fShaderFileStream.rdbuf();
    std::string fShaderSourceStr = fShaderStream.str();
    const char* fShaderSource = fShaderSourceStr.c_str();

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderSource, NULL);
    glCompileShader(vShader);
    GLint compiled;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) return 0;

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderSource, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return program;
}

// Function to kick off a face rotation animation
void startRotation(int axis, int layer, bool clockwise) {
    if (isAnimating) return;
    
    activeAxis = axis;
    activeLayer = layer;
    isAnimating = true;
    animAngle = 0.0f;
    
    // Direction depends on axis and standard convention
    float angle = 90.0f;
    if (clockwise) {
        angle = -90.0f;
    }
    
    targetAngle = glm::radians(angle);
}

// Complete the animation rotation in variables
void finalizeRotation() {
    isAnimating = false;
    
    glm::vec3 axisVec(0.0f);
    axisVec[activeAxis] = 1.0f;
    
    glm::mat4 rotationStep = glm::rotate(glm::mat4(1.0f), targetAngle, axisVec);
    
    for (auto& c : cubies) {
        if (c.gridPos[activeAxis] == activeLayer) {
            // Update grid position
            glm::vec4 pos(c.gridPos.x, c.gridPos.y, c.gridPos.z, 1.0f);
            glm::vec4 rotatedPos = rotationStep * pos;
            c.gridPos.x = std::round(rotatedPos.x);
            c.gridPos.y = std::round(rotatedPos.y);
            c.gridPos.z = std::round(rotatedPos.z);
            
            // Accumulate rotation matrix
            c.currentRotation = rotationStep * c.currentRotation;
        }
    }
    
    animAngle = 0.0f;
}

namespace csX75 {
    void error_callback(int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            bool shift = (mods & GLFW_MOD_SHIFT) != 0;
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GL_TRUE);
                    break;
                case GLFW_KEY_R:
                    startRotation(0, 1, !shift); // Right face (x=1)
                    break;
                case GLFW_KEY_L:
                    startRotation(0, -1, shift); // Left face (x=-1)
                    break;
                case GLFW_KEY_U:
                    startRotation(1, 1, !shift); // Up face (y=1)
                    break;
                case GLFW_KEY_D:
                    startRotation(1, -1, shift); // Down face (y=-1)
                    break;
                case GLFW_KEY_F:
                    startRotation(2, 1, !shift); // Front face (z=1)
                    break;
                case GLFW_KEY_B:
                    startRotation(2, -1, shift); // Back face (z=-1)
            }
        }
    }

    void initGL() {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.12f, 0.12f, 0.16f, 1.0f); // Sleek background
    }
}

void initBuffersGL() {
    // Generate 27 cubies
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                Cubie c;
                c.gridPos = glm::ivec3(x, y, z);
                c.currentRotation = glm::mat4(1.0f);
                
                // Initialize static face colors based on initial outer position
                c.faceColors[0] = (x == 1) ? rubikColors[0] : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // +X
                c.faceColors[1] = (x == -1) ? rubikColors[1] : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // -X
                c.faceColors[2] = (y == 1) ? rubikColors[2] : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // +Y
                c.faceColors[3] = (y == -1) ? rubikColors[3] : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // -Y
                c.faceColors[4] = (z == 1) ? rubikColors[4] : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // +Z
                c.faceColors[5] = (z == -1) ? rubikColors[5] : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // -Z
                
                cubies.push_back(c);
            }
        }
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    program_id = initShadersGL("vshader.glsl", "fshader.glsl");
    if (program_id == 0) {
        std::cerr << "Shader program initialization failed!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glUseProgram(program_id);

    GLint loc_pos = glGetAttribLocation(program_id, "vPosition");
    if (loc_pos != -1) {
        glEnableVertexAttribArray(loc_pos);
        glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    }

    GLint loc_norm = glGetAttribLocation(program_id, "vNormal");
    if (loc_norm != -1) {
        glEnableVertexAttribArray(loc_norm);
        glVertexAttribPointer(loc_norm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }

    uMVP_loc = glGetUniformLocation(program_id, "uMVP");
    uFaceColors_loc = glGetUniformLocation(program_id, "uFaceColors");
}

void renderGL(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update animation angle
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    if (isAnimating) {
        float step = animSpeed * deltaTime;
        if (targetAngle < 0.0f) {
            animAngle -= step;
            if (animAngle <= targetAngle) {
                finalizeRotation();
            }
        } else {
            animAngle += step;
            if (animAngle >= targetAngle) {
                finalizeRotation();
            }
        }
    }

    // Camera view setup using sequential rotations for natural wrapping
    // Smooth camera orbit on holding arrow keys (frame-rate independent)
    float cameraSpeed = 2.2f; // radians per second
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraTheta -= cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraTheta += cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cameraPhi += cameraSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cameraPhi -= cameraSpeed * deltaTime;
    }

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -cameraRadius));
    view = glm::rotate(view, cameraPhi, glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, cameraTheta, glm::vec3(0.0f, 1.0f, 0.0f));

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 20.0f);

    glUseProgram(program_id);
    glBindVertexArray(vao);

    // Draw the 27 cubies
    for (const auto& c : cubies) {
        // Base Translation and Rotation
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(c.gridPos));
        model = model * c.currentRotation;

        // If this cubie is currently rotating in the active layer animation
        if (isAnimating && c.gridPos[activeAxis] == activeLayer) {
            glm::vec3 axisVec(0.0f);
            axisVec[activeAxis] = 1.0f;
            
            // Pre-multiply rotation to rotate around the center of the Rubik's cube
            model = glm::rotate(glm::mat4(1.0f), animAngle, axisVec) * model;
        }

        // Scale down slightly to leave gaps (like a real Rubik's cube)
        glm::mat4 finalModel = glm::scale(model, glm::vec3(0.93f));
        glm::mat4 MVP = projection * view * finalModel;

        glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));

        glUniform4fv(uFaceColors_loc, 6, glm::value_ptr(c.faceColors[0]));

        // Draw the cubie
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

int main() {
    glfwSetErrorCallback(csX75::error_callback);

    if (!glfwInit()) {
        std::cerr << "GLFW Init Failed!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(700, 700, "OpenGL Interactive Rubik's Cube", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Window creation failed!" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "GLEW Init Failed: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, csX75::key_callback);
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

    csX75::initGL();
    initBuffersGL();

    std::cout << "\n=== RUBIK'S CUBE KEYBOARD CONTROLS ===" << std::endl;
    std::cout << "Rotate Faces (Clockwise: press key | Counter-Clockwise: SHIFT + key):" << std::endl;
    std::cout << "  U - Up (Yellow face)" << std::endl;
    std::cout << "  D - Down (White face)" << std::endl;
    std::cout << "  L - Left (Green face)" << std::endl;
    std::cout << "  R - Right (Blue face)" << std::endl;
    std::cout << "  F - Front (Red face)" << std::endl;
    std::cout << "  B - Back (Orange face)" << std::endl;
    std::cout << "\nCamera controls:" << std::endl;
    std::cout << "  Arrow Keys : Orbit camera around the Rubik's cube" << std::endl;
    std::cout << "  ESC        : Close application\n" << std::endl;

    lastFrameTime = (float)glfwGetTime();

    while (glfwWindowShouldClose(window) == 0) {
        renderGL(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program_id);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
