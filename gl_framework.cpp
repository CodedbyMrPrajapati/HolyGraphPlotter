#include "gl_framework.hpp"
extern GLfloat xrot,yrot,zrot;
namespace csX75
{
  //! Initialize GL State
  void initGL(void)
  {
    //Set framebuffer clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Set depth buffer furthest depth
    glClearDepth(1.0);
    //Set depth test to less-than
    glDepthFunc(GL_LESS);
    //Enable depth testing
    glEnable(GL_DEPTH_TEST); 
  }
  
  //!GLFW Eror Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }
  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
  }
    
  //!GLFW keyboard callback
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    if ((action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        if (key == GLFW_KEY_LEFT)
            yrot -= 0.1f;
        else if (key == GLFW_KEY_RIGHT)
            yrot += 0.1f;
        else if (key == GLFW_KEY_UP)
            xrot += 0.1f;
        else if (key == GLFW_KEY_DOWN)
            xrot -= 0.1f;
        else if (key == GLFW_KEY_PAGE_UP)
            zrot += 0.1f;
        else if (key == GLFW_KEY_PAGE_DOWN)
            zrot -= 0.1f;
    }
  }
};  
  

