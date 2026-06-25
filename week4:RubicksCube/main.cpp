#include "gl_framework.hpp"
#include "../shader_util.hpp"
#include <filesystem>
#include <stdexcept>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define BUFFER_OFFSET(offset) ((void*)(offset))
struct Cubie {
    glm::ivec3 gridPos;         // -1 , 0 or 1 in x/y/z
    glm::mat4 currentRotation;  // orientation of that cube
    glm::vec4 faceColors[6];    // colors for each face: +X, -X, +Y, -Y, +Z, -Z
};
bool isAnimating = false;
int activeAxis = 0;     // 0=X, 1=Y, 2=Z
int activeLayer = 0;    // -1, 0, 1
float animAngle = 0.0f;
float targetAngle = 0.0f;
float animSpeed = glm::radians(180.0f); // 180 deg/sec
float lastFrameTime = 0.0f;
std::vector<Cubie> Cubies;
// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
glm::mat4 rotation_matrix;
//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
// 6 faces , 2 triangles/face, 3vertices/triangle
const int num_vertices = 36;
GLuint shaderProgram;
GLuint vbo, vao;

glm::mat4 view_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelviewproject_matrix;
GLint uModelViewProjectMatrix;

namespace
{
std::string resolveShaderPath(const std::string& shader_file)
{
    namespace fs = std::filesystem;

    const fs::path shader_dir("week3:3dCube");
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
glm::mat4 rotationForAxis(int axis, float angle) {
    if (axis == 0) return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1,0,0));
    if (axis == 1) return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0,1,0));
    return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0,0,1));
}
void startRotation(int axis, int layer, bool clockwise) {
    if (isAnimating) return;
    activeAxis = axis;
    activeLayer = layer;
    isAnimating = true;
    animAngle = 0.0f;
    targetAngle = glm::radians(clockwise ? -90.0f : 90.0f);
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
//RGBA colors - Rubik's standard colors
glm::vec4 colors[6] = {
  glm::vec4(0.0f, 0.27f, 0.68f, 1.0f),  // Blue (+X)
  glm::vec4(0.0f, 0.6f, 0.28f, 1.0f),   // Green (-X)
  glm::vec4(1.0f, 0.84f, 0.0f, 1.0f),   // Yellow (+Y)
  glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // White (-Y)
  glm::vec4(1.0f, 0.35f, 0.0f, 1.0f),   // Orange (+Z)
  glm::vec4(0.72f, 0.07f, 0.2f, 1.0f)   // Red (-Z)
};

glm::vec4 interiorColor = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);  // Dark gray for interior faces

glm::vec4 faceColor(glm::vec3 normal, glm::ivec3 gridPos) {
    if (normal.x > 0.5f) return (gridPos.x == 1) ? colors[0] : interiorColor;  // +X -> Blue if exterior
    if (normal.x < -0.5f) return (gridPos.x == -1) ? colors[1] : interiorColor; // -X -> Green if exterior
    if (normal.y > 0.5f) return (gridPos.y == 1) ? colors[2] : interiorColor;  // +Y -> Yellow if exterior
    if (normal.y < -0.5f) return (gridPos.y == -1) ? colors[3] : interiorColor; // -Y -> White if exterior
    if (normal.z > 0.5f) return (gridPos.z == 1) ? colors[4] : interiorColor;  // +Z -> Orange if exterior
    return (gridPos.z == -1) ? colors[5] : interiorColor;                       // -Z -> Red if exterior
}
void quad(int a, int b, int c, int d, glm::vec3 normal, glm::ivec3 cubiePos)
{
  glm::vec4 fColor = faceColor(normal, cubiePos);
  v_colors[tri_idx] = fColor; v_positions[tri_idx] = positions[a]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = fColor; v_positions[tri_idx] = positions[b]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = fColor; v_positions[tri_idx] = positions[c]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = fColor; v_positions[tri_idx] = positions[a]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = fColor; v_positions[tri_idx] = positions[c]; v_normals[tri_idx] = normal; tri_idx++;
  v_colors[tri_idx] = fColor; v_positions[tri_idx] = positions[d]; v_normals[tri_idx] = normal; tri_idx++;
 }

// generate 12 triangles: 36 vertices and 36 colors
void colorcube(glm::ivec3 cubiePos)
{
    quad( 1, 0, 3, 2, glm::vec3(0, 0, 1), cubiePos );    // +Z (Front)
    quad( 2, 3, 7, 6, glm::vec3(0, 0, -1), cubiePos );   // -Z (Back)
    quad( 3, 0, 4, 7, glm::vec3(-1, 0, 0), cubiePos );   // -X (Left)
    quad( 6, 5, 1, 2, glm::vec3(1, 0, 0), cubiePos );    // +X (Right)
    quad( 4, 5, 6, 7, glm::vec3(0, 1, 0), cubiePos );    // +Y (Top)
    quad( 5, 4, 0, 1, glm::vec3(0, -1, 0), cubiePos );   // -Y (Bottom)
}

void initShader(){
    std::string vertex_shader_file = resolveShaderPath("vshader.glsl");
    std::string fragment_shader_file = resolveShaderPath("fshader.glsl");

    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);

    uModelViewProjectMatrix =
        glGetUniformLocation(shaderProgram, "uModelViewProjectMatrix");
    if (uModelViewProjectMatrix < 0) {
        throw std::runtime_error("Could not find uModelViewProjectMatrix uniform");
    }
}
void initVertexBufferGL(void)
{
    // Build the standard cube geometry once with stored colors
    tri_idx = 0;
    
    // +Z Front - Orange
    glm::vec4 zPosColor = glm::vec4(1.0f, 0.35f, 0.0f, 1.0f);
    v_colors[tri_idx] = zPosColor; v_positions[tri_idx] = positions[1]; v_normals[tri_idx] = glm::vec3(0, 0, 1); tri_idx++;
    v_colors[tri_idx] = zPosColor; v_positions[tri_idx] = positions[0]; v_normals[tri_idx] = glm::vec3(0, 0, 1); tri_idx++;
    v_colors[tri_idx] = zPosColor; v_positions[tri_idx] = positions[3]; v_normals[tri_idx] = glm::vec3(0, 0, 1); tri_idx++;
    v_colors[tri_idx] = zPosColor; v_positions[tri_idx] = positions[1]; v_normals[tri_idx] = glm::vec3(0, 0, 1); tri_idx++;
    v_colors[tri_idx] = zPosColor; v_positions[tri_idx] = positions[3]; v_normals[tri_idx] = glm::vec3(0, 0, 1); tri_idx++;
    v_colors[tri_idx] = zPosColor; v_positions[tri_idx] = positions[2]; v_normals[tri_idx] = glm::vec3(0, 0, 1); tri_idx++;
    
    // -Z Back - Red
    glm::vec4 zNegColor = glm::vec4(0.72f, 0.07f, 0.2f, 1.0f);
    v_colors[tri_idx] = zNegColor; v_positions[tri_idx] = positions[2]; v_normals[tri_idx] = glm::vec3(0, 0, -1); tri_idx++;
    v_colors[tri_idx] = zNegColor; v_positions[tri_idx] = positions[3]; v_normals[tri_idx] = glm::vec3(0, 0, -1); tri_idx++;
    v_colors[tri_idx] = zNegColor; v_positions[tri_idx] = positions[7]; v_normals[tri_idx] = glm::vec3(0, 0, -1); tri_idx++;
    v_colors[tri_idx] = zNegColor; v_positions[tri_idx] = positions[2]; v_normals[tri_idx] = glm::vec3(0, 0, -1); tri_idx++;
    v_colors[tri_idx] = zNegColor; v_positions[tri_idx] = positions[7]; v_normals[tri_idx] = glm::vec3(0, 0, -1); tri_idx++;
    v_colors[tri_idx] = zNegColor; v_positions[tri_idx] = positions[6]; v_normals[tri_idx] = glm::vec3(0, 0, -1); tri_idx++;
    
    // -X Left - Green
    glm::vec4 xNegColor = glm::vec4(0.0f, 0.6f, 0.28f, 1.0f);
    v_colors[tri_idx] = xNegColor; v_positions[tri_idx] = positions[3]; v_normals[tri_idx] = glm::vec3(-1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xNegColor; v_positions[tri_idx] = positions[0]; v_normals[tri_idx] = glm::vec3(-1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xNegColor; v_positions[tri_idx] = positions[4]; v_normals[tri_idx] = glm::vec3(-1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xNegColor; v_positions[tri_idx] = positions[3]; v_normals[tri_idx] = glm::vec3(-1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xNegColor; v_positions[tri_idx] = positions[4]; v_normals[tri_idx] = glm::vec3(-1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xNegColor; v_positions[tri_idx] = positions[7]; v_normals[tri_idx] = glm::vec3(-1, 0, 0); tri_idx++;
    
    // +X Right - Blue
    glm::vec4 xPosColor = glm::vec4(0.0f, 0.27f, 0.68f, 1.0f);
    v_colors[tri_idx] = xPosColor; v_positions[tri_idx] = positions[6]; v_normals[tri_idx] = glm::vec3(1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xPosColor; v_positions[tri_idx] = positions[5]; v_normals[tri_idx] = glm::vec3(1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xPosColor; v_positions[tri_idx] = positions[1]; v_normals[tri_idx] = glm::vec3(1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xPosColor; v_positions[tri_idx] = positions[6]; v_normals[tri_idx] = glm::vec3(1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xPosColor; v_positions[tri_idx] = positions[1]; v_normals[tri_idx] = glm::vec3(1, 0, 0); tri_idx++;
    v_colors[tri_idx] = xPosColor; v_positions[tri_idx] = positions[2]; v_normals[tri_idx] = glm::vec3(1, 0, 0); tri_idx++;
    
    // +Y Top - Yellow
    glm::vec4 yPosColor = glm::vec4(1.0f, 0.84f, 0.0f, 1.0f);
    v_colors[tri_idx] = yPosColor; v_positions[tri_idx] = positions[4]; v_normals[tri_idx] = glm::vec3(0, 1, 0); tri_idx++;
    v_colors[tri_idx] = yPosColor; v_positions[tri_idx] = positions[5]; v_normals[tri_idx] = glm::vec3(0, 1, 0); tri_idx++;
    v_colors[tri_idx] = yPosColor; v_positions[tri_idx] = positions[6]; v_normals[tri_idx] = glm::vec3(0, 1, 0); tri_idx++;
    v_colors[tri_idx] = yPosColor; v_positions[tri_idx] = positions[4]; v_normals[tri_idx] = glm::vec3(0, 1, 0); tri_idx++;
    v_colors[tri_idx] = yPosColor; v_positions[tri_idx] = positions[6]; v_normals[tri_idx] = glm::vec3(0, 1, 0); tri_idx++;
    v_colors[tri_idx] = yPosColor; v_positions[tri_idx] = positions[7]; v_normals[tri_idx] = glm::vec3(0, 1, 0); tri_idx++;
    
    // -Y Bottom - White
    glm::vec4 yNegColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    v_colors[tri_idx] = yNegColor; v_positions[tri_idx] = positions[5]; v_normals[tri_idx] = glm::vec3(0, -1, 0); tri_idx++;
    v_colors[tri_idx] = yNegColor; v_positions[tri_idx] = positions[4]; v_normals[tri_idx] = glm::vec3(0, -1, 0); tri_idx++;
    v_colors[tri_idx] = yNegColor; v_positions[tri_idx] = positions[0]; v_normals[tri_idx] = glm::vec3(0, -1, 0); tri_idx++;
    v_colors[tri_idx] = yNegColor; v_positions[tri_idx] = positions[5]; v_normals[tri_idx] = glm::vec3(0, -1, 0); tri_idx++;
    v_colors[tri_idx] = yNegColor; v_positions[tri_idx] = positions[0]; v_normals[tri_idx] = glm::vec3(0, -1, 0); tri_idx++;
    v_colors[tri_idx] = yNegColor; v_positions[tri_idx] = positions[1]; v_normals[tri_idx] = glm::vec3(0, -1, 0); tri_idx++;
    
    //Ask GL for a Vertex Attribute Object (vao)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof(v_positions) + sizeof(v_colors) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(v_colors), v_colors );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(v_positions) + sizeof(v_colors), sizeof(v_normals), v_normals );

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

    rotation_matrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f,0.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, yrot, glm::vec3(0.0f,1.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, zrot, glm::vec3(0.0f,0.0f,1.0f));
    // Eye center up 
    view_matrix = glm::lookAt(glm::vec3(5,5.0,5.0),glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,1.0,0.0));
    // Only objects inside this are visible
    ortho_matrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -20.0f, 20.0f);
    auto world = ortho_matrix * view_matrix * rotation_matrix;

    for (auto& c : Cubies) {
        glm::mat4 model = glm::mat4(1.0f);
        
        
        // Then: translate to grid position
        // First: apply layer rotation around cube center if animating
        if (isAnimating && c.gridPos[activeAxis] == activeLayer) {
            model = rotationForAxis(activeAxis, animAngle);
        }
        model = model * glm::translate(glm::mat4(1.0f), glm::vec3(c.gridPos));
        
        // Finally: apply cubie's persistent rotation
        model = model * c.currentRotation;
        
        model = glm::scale(model, glm::vec3(0.95f)); // small gap
        glm::mat4 mvp = world * model;
        glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE,
                            glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    }
    // glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE, glm::value_ptr(modelviewproject_matrix));
    // // Draw 
    // glDrawArrays(GL_TRIANGLES, 0, num_vertices);
  
}

int main(int argc, char** argv)
{   
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
            for (int z = -1; z <= 1; ++z) {
            Cubie c;
            c.gridPos = glm::ivec3(x, y, z);
            c.currentRotation = glm::mat4(1.0f);
            
            // Initialize face colors based on initial position (never changes)
            c.faceColors[0] = (x == 1) ? glm::vec4(0.0f, 0.27f, 0.68f, 1.0f) : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // +X
            c.faceColors[1] = (x == -1) ? glm::vec4(0.0f, 0.6f, 0.28f, 1.0f) : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // -X
            c.faceColors[2] = (y == 1) ? glm::vec4(1.0f, 0.84f, 0.0f, 1.0f) : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // +Y
            c.faceColors[3] = (y == -1) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // -Y
            c.faceColors[4] = (z == 1) ? glm::vec4(1.0f, 0.35f, 0.0f, 1.0f) : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // +Z
            c.faceColors[5] = (z == -1) ? glm::vec4(0.72f, 0.07f, 0.2f, 1.0f) : glm::vec4(0.05f, 0.05f, 0.05f, 1.0f); // -Z
            
            Cubies.push_back(c);
            }
    //! The pointer to the GLFW window
    GLFWwindow* window;

    //! Setting up the GLFW Error callback
    glfwSetErrorCallback(csX75::error_callback);

    //! Initialize GLFW
    if (!glfwInit())
    return -1;

    //We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    //! Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Triangles", NULL, NULL);
    if (!window){
        std::cerr << "Can't Initialize Window";
        glfwTerminate();
        return -1;
    }

    //! Make the window's context current 
    glfwMakeContextCurrent(window);

    //Initialize GLEW
    //Turn this on to get Shader based OpenGL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        //Problem: glewInit failed, something is seriously wrong.
        std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }
    //Print and see what context got enabled
    std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
    std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
    std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
    std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

    //Keyboard Callback
    glfwSetKeyCallback(window, csX75::key_callback);
    //Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    //Initialize GL state
    csX75::initGL();
    initShader();
    initVertexBufferGL();

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    // Loop until the user closes the window
    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt = now - lastTime;
        lastTime = now;

        if (isAnimating) {
            animAngle += animSpeed * dt;
            if (fabs(animAngle) >= fabs(targetAngle)) {
                animAngle = targetAngle;
                // commit the rotation
                glm::mat4 finalRot = rotationForAxis(activeAxis, targetAngle);
                for (auto& c : Cubies) {
                    if (c.gridPos[activeAxis] == activeLayer) {
                        glm::vec4 p(c.gridPos, 1.0f);
                        glm::vec4 q = finalRot * p;
                        c.gridPos = glm::round(glm::vec3(q));
                        c.currentRotation = finalRot * c.currentRotation;
                    }
                }
                isAnimating = false;
                animAngle = 0.0f;
            }
        }

        renderGL();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
