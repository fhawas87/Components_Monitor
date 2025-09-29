#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <thread>

#include "cpu.h"
#include "gpu.h"
#include "ram.h"

#include <imgui.h>


struct cpu_stats {

  std::string cpu_model;

  std::vector<float> cpu_temps;
  std::vector<float> cpu_freqs;

  float cpu_usage;

};

struct gpu_stats {
  
  std::string gpu_model;

  std::vector<unsigned int> gpu_vram;

  unsigned int gpu_temp;
  unsigned int gpu_usage;
  unsigned int gpu_freq;

};

struct ram_stats {

 unsigned int ram_usage;

};

struct stats {

  cpu_stats cpu;
  gpu_stats gpu;
  ram_stats ram;

};

class sampler {
public :

  const stats &snapshot() const { return sample; }

  void refresh_samples() {

    sample.cpu.cpu_model = get_cpu_model();
    sample.cpu.cpu_temps = get_cpu_core_thermal_values();
    sample.cpu.cpu_usage = get_cpu_utilization();
    if (sample.cpu.cpu_temps.size() == 0) { printf("EMPTY\n"); }
    //sample.cpu.cpu_freqs = get_cpu_core_frequencies(sample.cpu.cpu_temps.size());
    sample.cpu.cpu_freqs = get_cpu_core_frequencies();

    sample.gpu.gpu_model = get_accessible_device_name();
    sample.gpu.gpu_temp  = get_current_gpu_temperature();
    sample.gpu.gpu_freq  = get_gpu_clock_frequency();
    sample.gpu.gpu_vram  = get_gpu_VRAM_info();
    sample.gpu.gpu_usage = get_core_utilization_percentage_rate();

    sample.ram.ram_usage = get_ram_memory_usage();
  }
private:

  stats sample{};
};


// TODO : create ImGui components dashboard, refresh it every 0.5s here i guess
