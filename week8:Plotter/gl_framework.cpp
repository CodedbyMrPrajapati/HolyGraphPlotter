#include "gl_framework.hpp"

// Camera rotation (arrow keys / page up-down)
extern GLfloat xrot, yrot, zrot;

// Playback + render-mode state, owned by main.cpp
extern bool  gPaused;      // SPACE toggles
extern float gSpeed;       // +/- adjust
extern int   gRenderMode;  // 0 = wireframe, 1 = flat, 2 = phong

namespace csX75
{
  //! Initialize GL State
  void initGL(void)
  {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
  }

  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }

  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    glViewport(0, 0, width, height);
  }

  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    (void)scancode; (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

    // Camera orbit (held keys repeat)
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
      if (key == GLFW_KEY_LEFT)            yrot -= 0.1f;
      else if (key == GLFW_KEY_RIGHT)      yrot += 0.1f;
      else if (key == GLFW_KEY_UP)         xrot += 0.1f;
      else if (key == GLFW_KEY_DOWN)       xrot -= 0.1f;
      else if (key == GLFW_KEY_PAGE_UP)    zrot += 0.1f;
      else if (key == GLFW_KEY_PAGE_DOWN)  zrot -= 0.1f;
    }

    // Playback + rendering controls (edge-triggered)
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_SPACE:
          gPaused = !gPaused;
          std::cout << (gPaused ? "[paused]" : "[playing]") << std::endl;
          break;

        // '+'/'=' speed up, '-' slow down (main and keypad)
        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
          gSpeed += 0.25f;
          std::cout << "speed = " << gSpeed << std::endl;
          break;
        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
          gSpeed -= 0.25f;
          if (gSpeed < 0.0f) gSpeed = 0.0f;
          std::cout << "speed = " << gSpeed << std::endl;
          break;

        // Render modes
        case GLFW_KEY_1: gRenderMode = 0; std::cout << "mode = wireframe" << std::endl; break;
        case GLFW_KEY_2: gRenderMode = 1; std::cout << "mode = flat" << std::endl; break;
        case GLFW_KEY_3: gRenderMode = 2; std::cout << "mode = phong" << std::endl; break;
        case GLFW_KEY_M:
          gRenderMode = (gRenderMode + 1) % 3;
          std::cout << "mode = "
                    << (gRenderMode == 0 ? "wireframe" : gRenderMode == 1 ? "flat" : "phong")
                    << std::endl;
          break;
      }
    }
  }
};
