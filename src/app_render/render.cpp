#include <stdexcept>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "render.h"
#include "theme.h"

#include "components_dashboard/sample.h"


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
  
  double last_time = glfwGetTime();
  static sampler cs; // current system snaphot varaible

  while (!glfwWindowShouldClose(main_window)) {

    glfwSetErrorCallback(glfw_error_callback);
    glfwPollEvents();

    // INFO - render here
    
    double current_time = glfwGetTime();
    if (current_time - last_time >= 1.0) {
      cs.refresh_samples();
      last_time = current_time;
    }
    
    const stats& sample_c = cs.snapshot();
    sample_c.number_of_cores = sample_c.cpu.cpu_temps.size();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Components Dashboard");
    ImGui::Separator();
    ImGui::Text("%s   fps %1.f", sample_c.cpu.cpu_model.c_str(), io->Framerate);
    ImGui::Separator();
    for (int i = 0; i < number_of_cores; i++) {
      ImGui::Text("CPU Core #%d : %.1f C   %.0f MHz", i, sample_c.cpu.cpu_temps[i], sample_c.cpu.cpu_freqs[i]);
    }
    ImGui::Text("CPU Usage : %.1f%%", sample_c.cpu.cpu_usage);
    ImGui::Separator();
    ImGui::Text("%s", sample_c.gpu.gpu_model.c_str());
    ImGui::Separator();
    ImGui::Text("GPU Usage : %u%%", sample_c.gpu.gpu_usage);
    ImGui::Text("GPU Core Temp : %u C", sample_c.gpu.gpu_temp);
    ImGui::Text("GPU Core Freq : %u MHz", sample_c.gpu.gpu_freq);
    if (!sample_c.gpu.gpu_vram.empty()) {
      ImGui::Text("GPU VRAM : %u MiB / %u MiB", sample_c.gpu.gpu_vram[1], sample_c.gpu.gpu_vram[0]);
    }
    ImGui::Separator();
    ImGui::Text("RAM Usage : %u%%", sample_c.ram.ram_usage);
    ImGui::Separator();
    ImGui::End();
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
