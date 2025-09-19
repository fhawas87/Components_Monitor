#include <stdio.h>
#include <string.h>


unsigned int get_ram_memory_usage(void) {

  FILE *mem_info = fopen("/proc/meminfo", "r");
  if (!mem_info) {
    printf("Something worng with given '/proc/meminfo' path\n");
    fclose(mem_info);
    return 1;
  }
  
  char memory_proc_info_buffer[512];
  unsigned long long total_ram_memory_KiB = 0;
  unsigned long long available_ram_memory_KiB = 0;
  
  for (int line_idx = 0; line_idx < 3; line_idx++) {
    fgets(memory_proc_info_buffer, sizeof(memory_proc_info_buffer), mem_info);
    if (strncmp(memory_proc_info_buffer, "MemTotal", 8) == 0) {
      sscanf(memory_proc_info_buffer, "%*s %llu", &total_ram_memory_KiB);
    }
    if (strncmp(memory_proc_info_buffer, "MemAvailable", 12) == 0) {
      sscanf(memory_proc_info_buffer, "%*s %llu", &available_ram_memory_KiB);
    }
  }
  fclose(mem_info);
  
  double total_ram_memory_MiB = (double)(total_ram_memory_KiB / 1024);
  double available_ram_memory_MiB = (double)(available_ram_memory_KiB / 1024);
  double ram_usage_float_value = (total_ram_memory_MiB - available_ram_memory_MiB) / total_ram_memory_MiB * 100;

  return (unsigned int)ram_usage_float_value;
}
