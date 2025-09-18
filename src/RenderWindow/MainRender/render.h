#pragma once

#include <cstdint>

struct GLFWwindow;

struct WindowMacros{
  
  static constexpr uint16_t   WIDTH     = 1600;
  static constexpr uint16_t   HEIGHT    = 900;
  static constexpr char       TITLE[]   = "Application";

  static constexpr bool       DOCKING   = true;
  static constexpr bool       VIEWPORT  = true;

};

class MainWindow {

public:

  void RunMainWindow() {

    window_init();
    opengl_init();
    main_loop();
    clean_up();
  }

private:

  void window_init();
  void opengl_init();
  void main_loop();
  void clean_up();

  GLFWwindow* main_window = nullptr;
};
