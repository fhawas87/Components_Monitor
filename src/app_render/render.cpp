#include <stdexcept>
#include <string>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <sensors/sensors.h>
#include <nvml.h>

#include "render.h"
#include "theme.h"
#include "draw.h"
#include "components_dashboard/sample.h"


static void glfw_error_callback(int error, const char* error_description) { fprintf(stderr, "%d : %s\n", error, error_description);}

const char* OPENGL_VERSION;

void MainWindow::window_init() {

  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) { throw std::runtime_error("GLFW Initialization Failed!\n\n"); }

  OPENGL_VERSION = "#version 330";

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
  main_window = glfwCreateWindow(WindowMacros::WIDTH, WindowMacros::HEIGHT, WindowMacros::TITLE, nullptr, nullptr);

  if (main_window == nullptr) { throw std::runtime_error("Window failed!\n"); }

  glfwMakeContextCurrent(main_window);
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

void MainWindow::apis_init() {

  nvmlReturn_t initialize_result;
  initialize_result = nvmlInit_v2();

  if (initialize_result != NVML_SUCCESS) {
    
    std::string error_code;
    error_code = nvmlErrorString(initialize_result);

    throw std::runtime_error(std::string("\nERROR INFO : ") + error_code);
    nvmlShutdown();
  }
  if (sensors_init(NULL) != 0) {

    throw std::runtime_error("\nERROR INFO : 'libsensors library initialization error'\n");
    sensors_cleanup();
  }
}

void MainWindow::window_features() {

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImPlot::CreateContext();

  io = &ImGui::GetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  if (WindowMacros::DOCKING && io != nullptr) { io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; }
  if (WindowMacros::VIEWPORT && io != nullptr) { io->ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable; }

  Theme theme;
  theme.ApplyTheme();

  implot_theme(theme.LightGrey);
  
  ImGui_ImplGlfw_InitForOpenGL(main_window, true);
  ImGui_ImplOpenGL3_Init(OPENGL_VERSION);
}

int MainWindow::main_loop() {
  
  double last_time = glfwGetTime();

  while (!glfwWindowShouldClose(main_window)) {

    glfwSetErrorCallback(glfw_error_callback);
    glfwPollEvents();

    double current_time = glfwGetTime();
    if (current_time - last_time >= 0.5) {

      current_stats = refresh_samples();
      update_min_max(current_stats, mm);
      last_time = current_time;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  
    draw_system_dashboard(current_stats, mm);
    draw_system_charts();

    ImGui::Render();

    glClearColor(0.16f, 0.16f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(main_window);
  }

  return 0;
}

void MainWindow::clean_up() {

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(main_window);
  glfwTerminate();
}
