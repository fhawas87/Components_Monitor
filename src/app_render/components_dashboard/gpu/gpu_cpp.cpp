#include <stdio.h>
#include <string>
#include <vector>

#include <nvml.h>

#include "gpu.h"


std::string get_accessible_device_name() {
  
  std::string device_name_string;
  unsigned int accessible_device_counter = 0;
  nvmlReturn_t device_count_result = nvmlDeviceGetCount_v2(&accessible_device_counter);

  if (accessible_device_counter == 0) {
    printf("ERROR code : (%d)\n", (int)(device_count_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(device_count_result));
    device_name_string = "1";
  }
  else if (accessible_device_counter != 1) {
    printf("ERROR code : (%d)\n", (int)(device_count_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(device_count_result));
    device_name_string = "1";
  }
  else {
    nvmlDevice_t device = get_device();

    char device_name[NVML_DEVICE_NAME_BUFFER_SIZE];
    device_name[NVML_DEVICE_NAME_BUFFER_SIZE - 1] = '\0';
    nvmlDeviceGetName(device, device_name, NVML_DEVICE_NAME_BUFFER_SIZE);
  
    char* char_pointer = device_name;
    while (*char_pointer != '\0') {
      device_name_string.push_back(*char_pointer);
      char_pointer++;
    }
  }

  return device_name_string;
}

std::vector<unsigned int> get_gpu_VRAM_info() {

  std::vector<unsigned int> memory_info;

  nvmlDevice_t device = get_device();
  nvmlMemory_t memory;
  nvmlReturn_t memory_result = nvmlDeviceGetMemoryInfo(device, &memory);

  if (memory_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)(memory_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(memory_result));
    return {};
  }
  float VRAM_memory_percentage_usage  = 100 * ((float)memory.used / (float)memory.total);

  unsigned int total_VRAM_installed_MiB = (memory.total / 1024 / 1024);
  unsigned int total_VRAM_used_MiB = (memory.used / 1024 / 1024);
  unsigned int total_VRAM_free_MiB = (memory.free / 1204 / 1024);

  memory_info.emplace_back(total_VRAM_installed_MiB);
  memory_info.emplace_back(total_VRAM_used_MiB);
  memory_info.emplace_back(total_VRAM_free_MiB);
  memory_info.emplace_back(VRAM_memory_percentage_usage);

  return memory_info;
}
