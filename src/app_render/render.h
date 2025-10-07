#pragma once

#include <cstdint>

#include <sensors/sensors.h>
#include <nvml.h>

struct GLFWwindow;
struct ImGuiIO;

struct WindowMacros {
  
  static constexpr uint16_t   WIDTH                 = 1600;
  static constexpr uint16_t   HEIGHT                = 900;
  static constexpr char       TITLE[]               = "Custom Components Monitor Application";

  static constexpr bool       DOCKING               = true;
  static constexpr bool       VIEWPORT              = false;
  static constexpr bool       ShowWindow_1          = true;
  static constexpr bool       ShowWindow_2          = true;

  static constexpr bool       FULL_WINDOW_ENABLED   = false;
}; 
  
class MainWindow {
public:
  
  MainWindow() {
    
    window_init();
    opengl_init();
    apis_init();
    window_features();
  }
  
  int run_window() { return main_loop(); }

  ~MainWindow() noexcept{

    clean_up();

    nvmlShutdown();
    sensors_cleanup();
  }

private:

  void window_init();
  void opengl_init();
  void apis_init();
  void window_features();
  void clean_up();
  
  int main_loop();

  GLFWwindow* main_window = nullptr;
  ImGuiIO* io = nullptr;
};
