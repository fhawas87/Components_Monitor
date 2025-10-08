#include <stdio.h>
#include <string.h>
#include <vector>

std::vector<float> get_ram_memory() {

  std::vector<float> ram_params;
  FILE *mem_info = fopen("/proc/meminfo", "r");
  if (!mem_info) {
    printf("Something wrong with given path\n");
    return {};
  }

  char memory_info_buffer[512];

  unsigned long long total_ram = 0;
  unsigned long long available_ram = 0;

  for (int line_idx = 0; line_idx < 3; line_idx++) {
    fgets(memory_info_buffer, sizeof(memory_info_buffer), mem_info);
    if (strncmp(memory_info_buffer, "MemTotal", 8) == 0) {
      sscanf(memory_info_buffer, "%*s %llu", &total_ram);
    }
    if (strncmp(memory_info_buffer, "MemAvailable", 12) == 0) {
      sscanf(memory_info_buffer, "%*s %llu", &available_ram);
    }
  }

  fclose(mem_info);

  float total_ram_MiB = (float)(total_ram / 1024);
  float available_ram_MiB = (float)(available_ram / 1024);
  float ram_usage = (total_ram_MiB - available_ram_MiB) / total_ram_MiB * 100;
  float used_ram_MiB = total_ram_MiB - available_ram_MiB;

  ram_params.emplace_back(total_ram_MiB);
  ram_params.emplace_back(available_ram_MiB);
  ram_params.emplace_back(used_ram_MiB);
  ram_params.emplace_back(ram_usage);

  return ram_params;
}
