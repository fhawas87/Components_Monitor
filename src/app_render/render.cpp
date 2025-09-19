#include <stdexcept>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "render.h"
#include "theme.h"


// INFO : very useful function in case of any glfw init errors

static void glfw_error_callback(int error, const char* error_description) { fprintf(stderr, "%d : %s\n", error, error_description);}
//static void framebuffer_size_callback(GLFWwindow* window, int width, int height);


const char* OPENGL_VERSION;

void MainWindow::window_init() {

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) { throw std::runtime_error("GLFW Initialization Failed!\n\n"); }

  OPENGL_VERSION = "#version 330";

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  /*
  if (WindowMacros::FULL_WINDOW_ENABLED) {
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    main_window = glfwCreateWindow(mode->width, mode->height, WindowMacros::TITLE, monitor, nullptr);
  }
  if (!WindowMacros::FULL_WINDOW_ENABLED) {
    main_window = glfwCreateWindow(WindowMacros::WIDTH, WindowMacros::HEIGHT, WindowMacros::TITLE, nullptr, nullptr);
  }

  if (main_window == nullptr || main_window == NULL) { throw std::runtime_error("Window Creation Failure!\n\n"); }
  */
  
  main_window = glfwCreateWindow(WindowMacros::WIDTH, WindowMacros::HEIGHT, WindowMacros::TITLE, nullptr, nullptr);

  if (main_window == nullptr) { throw std::runtime_error("Window failed!\n"); }

  glfwMakeContextCurrent(main_window);
  //glfwSetFramebufferSizeCallback(main_window, framebuffer_size_callback);
  glfwSwapInterval(1);
}

void MainWindow::opengl_init() {

  #if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      throw std::runtime_error("Failed to load using GLAD!\n\n");
  }
  #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    if (glewInit() != GLEW_OK) {
      throw std::runtime_error("Failed to load using GLEW!\n\n");
  }
  #endif
}

void MainWindow::window_features() {

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  io = &ImGui::GetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  
  // FIX : cannot compile those two flags, compiler can't find those functions 
  // INFO : forgot to add docking branch from imgui github and where those functions are located, now it is working 

  if (WindowMacros::DOCKING && io != nullptr) { io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }
  if (WindowMacros::VIEWPORT && io != nullptr) { io->ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable; }

  // INFO - apply theme here

  Theme theme;
  theme.ApplyTheme();
  
  ImGui_ImplGlfw_InitForOpenGL(main_window, true);
  ImGui_ImplOpenGL3_Init(OPENGL_VERSION);
}

void MainWindow::main_loop() {

  while (!glfwWindowShouldClose(main_window)) {
    
    glfwSetErrorCallback(glfw_error_callback);
    glfwPollEvents();

    // INFO - render here
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (WindowMacros::ShowWindow_1) {

      ImGui::Begin("Window 1");
      ImGui::Text("First Window");
      ImGui::Text("fps %.1f", io->Framerate);
      ImGui::End();
    }
    if (WindowMacros::ShowWindow_2) {

      ImGui::Begin("Window 2");
      ImGui::Text("Second Window");
      ImGui::Text("fps %1.f", io->Framerate);
      ImGui::End();
    }

    ImGui::Render();

    glClearColor(0.16f, 0.16f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(main_window);
  }
}

void MainWindow::clean_up() {
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(main_window);
  glfwTerminate();
}

//void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
