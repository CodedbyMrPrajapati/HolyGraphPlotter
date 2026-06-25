#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
const int SCREEN_HEIGHT = 100;
const int SCREEN_WIDTH  = 200;

void error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error
              << ": " << description << '\n';
}

int main() {
    if (!glfwInit ()) 
    {
        std::cerr<<"ERROR: could not start GLFW3"<<std::endl;
        return 1;
    } 

    glfwSetErrorCallback(error_callback);

    // glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    // glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        SCREEN_WIDTH,SCREEN_HEIGHT,"Initializing Window",
        NULL,NULL
    );
    if (!window){
        std::cerr << "Can't Initialize Window";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    std::cout<<"Renderer: "<<renderer<<std::endl;
    std::cout<<"OpenGL version supported "<<version<<std::endl;

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}