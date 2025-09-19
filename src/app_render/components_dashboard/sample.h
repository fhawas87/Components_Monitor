#pragma once

#include <vector>
#include <string>

#include "cpu.h"
#include "gpu.h"
#include "ram.h"

#include <imgui.h>

struct dashboard_stats {
  
  /*
  unsigned int ram_memory = get_ram_memory_usage();
  unsigned int gpu_temp = get_current_gpu_temperature();
  unsigned int gpu_usage = get_core_utilization_percentage_rate();
  unsigned int gpu_freq = get_gpu_clock_frequency();

  std::string gpu_model = get_accessible_device_name();
  std::string cpu_model = get_cpu_model();
  
  std::vector<unsigned int> gpu_vram = get_gpu_VRAM_info();
  std::vector<float> cpu_temps = get_cpu_core_thermal_values();
  std::vector<float> cpu_freqs = get_cpu_core_frequencies(cpu_temps.size());

  float cpu_usage = get_cpu_utilization();
  */

  unsigned int ram_memory;
  unsigned int gpu_temp;
  unsigned int gpu_usage;
  unsigned int gpu_freq;

  std::string gpu_model;
  std::String cpu_model;

  std::vector<unsigned int> gpu_vram;
  std::vector<float> cpu_temps;
  std::vector<float> cpu_freqs;

  float cpu_usage;
};

// TODO : create ImGui components dashboard, refresh it every 0.5s here i guess
