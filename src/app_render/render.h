#pragma once

#include <cstdint>

struct GLFWwindow;
struct ImGuiIO;

struct WindowMacros {
  
  static constexpr uint16_t   WIDTH                 = 1600;
  static constexpr uint16_t   HEIGHT                = 900;
  static constexpr char       TITLE[]               = "Application";

  static constexpr bool       DOCKING               = true;
  static constexpr bool       VIEWPORT              = false;
  static constexpr bool       ShowWindow_1          = true;
  static constexpr bool       ShowWindow_2          = true;

  static constexpr bool       FULL_WINDOW_ENABLED   = false;

}; 
  
struct ViewportMacros {
  
  int x_cor = 0;
  int y_cor = 0;
  uint16_t VP_WIDTH = 1600;
  uint16_t VP_HEIGHT = 900;
};

class MainWindow {

public:

  void RunMainWindow() {

    window_init();
    opengl_init();
    window_features();
    main_loop();
    clean_up();
  }

private:

  void window_init();
  void opengl_init();
  void window_features();
  void main_loop();
  void clean_up();

  GLFWwindow* main_window = nullptr;
  ImGuiIO* io = nullptr;
};
