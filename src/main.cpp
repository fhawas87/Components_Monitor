#include <stdio.h>

#include <sensors/sensors.h>
#include <nvml.h>

#include <render.h>

int main(void) {
  
  // RUN APP
  
  nvmlReturn_t initialize_result;
  initialize_result = nvmlInit_v2();
  if (initialize_result != NVML_SUCCESS) {
    printf("\nERROR CODE : ( %d ), info : '%s'\n", (int)initialize_result, nvmlErrorString(initialize_result));
  }
  if (sensors_init(NULL) != 0) {
    printf("\nERROR code : libsensors library initialization error\n");
  }

  MainWindow app;

  app.RunMainWindow();

  nvmlShutdown();
  sensors_cleanup();

  return 0;
}
