#pragma once

#include <nvml.h>

#ifdef __cplusplus

  #include <string>
  #include <vector>

  extern "C" {
#endif

nvmlDevice_t get_device(void);

unsigned int get_current_gpu_temperature(void);
unsigned int get_core_utilization_percentage_rate(void);
unsigned int get_gpu_clock_frequency(void);

#ifdef __cplusplus
  }
  std::string get_accessible_device_name();
  std::vector<unsigned int> get_gpu_VRAM_info();
#endif
