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

/*
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
*/


stats refresh_samples() {

  stats current_stats{};

  current_stats.cpu.cpu_model = get_cpu_model();
  //current_stats.cpu.cpu_usage = get_cpu_utilization();
  current_stats.cpu.cpu_temps = get_cpu_core_thermal_values();
  current_stats.cpu.cpu_freqs = get_cpu_core_frequencies();

  current_stats.gpu.gpu_model = get_accessible_device_name();
  current_stats.gpu.gpu_usage = get_core_utilization_percentage_rate();
  current_stats.gpu.gpu_temp = get_current_gpu_temperature();
  current_stats.gpu.gpu_freq = get_gpu_clock_frequency();
  current_stats.gpu.gpu_vram = get_gpu_VRAM_info();

  //current_stats.ram.ram_usage = get_ram_memory_usage();

  return current_stats;
}

void draw_system_dashboard(stats &current_stats) {
  
  const ImGuiIO& ioo = ImGui::GetIO();

  ImGui::Begin("System Dashboard");
  ImGui::Separator();
  ImGui::Text("%s   %.1f", current_stats.cpu.cpu_model.c_str(), ioo.Framerate);
  ImGui::Separator();
  //ImGui::Text("CPU Usage  : %.1f%%", current_stats.cpu.cpu_usage);
  for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
    ImGui::Text("CPU Core #%zu : %.1f C    %.1f MHz", i, current_stats.cpu.cpu_temps[i], current_stats.cpu.cpu_freqs[i + 6]);
  }
  /*
  for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
    ImGui::Text("CPU Core #%zu : %.1f C", i, current_stats.cpu.cpu_temps[i]);
  }
  for (size_t i = 0; i < current_stats.cpu.cpu_freqs.size(); i++) {
    ImGui::Text("CPU Core #%zu : %.1f MHz", i, current_stats.cpu.cpu_freqs[i]);
  }
  */
  ImGui::Separator();
  ImGui::Text("%s", current_stats.gpu.gpu_model.c_str());
  ImGui::Separator();
  ImGui::Text("GPU Usage  : %u%%", current_stats.gpu.gpu_usage);
  ImGui::Text("GPU Temp   : %u C", current_stats.gpu.gpu_temp);
  ImGui::Text("GPU Freq   : %u MHz", current_stats.gpu.gpu_freq);
  if (!current_stats.gpu.gpu_vram.empty()) {
    ImGui::Text("GPU VRAM   : %u MiB / %u MiB", current_stats.gpu.gpu_vram[1], current_stats.gpu.gpu_vram[0]);
  }
  ImGui::Separator();
  //ImGui::Text("RAM Usage  : %u%%", current_stats.ram.ram_usage);
  ImGui::End();
}


// TODO : create ImGui components dashboard, refresh it every 0.5s here i guess
