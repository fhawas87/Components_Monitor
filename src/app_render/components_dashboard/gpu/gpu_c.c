#include <stdio.h>

#include <nvml.h>

#include "gpu.h"


nvmlDevice_t get_device(void) {
  nvmlDevice_t device;
  nvmlReturn_t device_result = nvmlDeviceGetHandleByIndex_v2(0, &device);

  if (device_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)device_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(device_result));
    return NULL;
  }
  return device;
}

unsigned int get_gpu_clock_frequency(void) {

  nvmlDevice_t device = get_device();
  unsigned int core_clock_freq;
  nvmlReturn_t core_clock_freq_result = nvmlDeviceGetClockInfo(device, NVML_CLOCK_GRAPHICS, &core_clock_freq);

  if (core_clock_freq_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)core_clock_freq_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(core_clock_freq_result));
    return 1;
  }

  return core_clock_freq;
}

unsigned int get_current_gpu_temperature(void) {

  nvmlDevice_t device = get_device();
  unsigned int gpu_temp = 0;
  nvmlReturn_t gpu_temp_result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &gpu_temp);
  
  if (gpu_temp_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)gpu_temp_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(gpu_temp_result));
  }

  return gpu_temp;
}

unsigned int get_core_utilization_percentage_rate(void) {

  nvmlDevice_t device = get_device();
  nvmlUtilization_t core_utilization_rate;
  nvmlReturn_t core_utilization_rate_result = nvmlDeviceGetUtilizationRates(device, &core_utilization_rate);

  if (core_utilization_rate_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)core_utilization_rate_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(core_utilization_rate_result));
    return 1;
  }

  return core_utilization_rate.gpu;
}

