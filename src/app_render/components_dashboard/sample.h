#pragma once

#include <vector>
#include <string>

#include "cpu.h"
#include "gpu.h"
#include "ram.h"

#include <imgui.h>

struct min_max {

  std::vector<float> min_core_temp_veq;
  std::vector<float> max_core_temp_veq;
  std::vector<float> min_core_freq_veq;
  std::vector<float> max_core_freq_veq;

  float min_cpu_usage;
  float max_cpu_usage;

  unsigned int min_gpu_vram;
  unsigned int max_gpu_vram;
  unsigned int min_gpu_usage;
  unsigned int max_gpu_usage;
  unsigned int min_gpu_temp;
  unsigned int max_gpu_temp;
  unsigned int min_gpu_freq;
  unsigned int max_gpu_freq;

  unsigned int min_ram_usage;
  unsigned int max_ram_usage;
};

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

stats refresh_samples() {

  stats current_stats{};

  current_stats.cpu.cpu_model = get_cpu_model();
  current_stats.cpu.cpu_usage = get_cpu_utilization();
  current_stats.cpu.cpu_temps = get_cpu_core_thermal_values();
  current_stats.cpu.cpu_freqs = get_cpu_core_frequencies();

  current_stats.gpu.gpu_model = get_accessible_device_name();
  current_stats.gpu.gpu_usage = get_core_utilization_percentage_rate();
  current_stats.gpu.gpu_temp = get_current_gpu_temperature();
  current_stats.gpu.gpu_freq = get_gpu_clock_frequency();
  current_stats.gpu.gpu_vram = get_gpu_VRAM_info();

  current_stats.ram.ram_usage = get_ram_memory_usage(); 

  return current_stats;
}

bool is_min_max_base_set = false;

void update_min_max(stats &current_stats, min_max &mm) {
  
  if (!is_min_max_base_set) {
    
    mm.min_core_temp_veq.resize(current_stats.cpu.cpu_temps.size());
    mm.max_core_temp_veq.resize(current_stats.cpu.cpu_temps.size());
    mm.min_core_freq_veq.resize(current_stats.cpu.cpu_temps.size());
    mm.max_core_freq_veq.resize(current_stats.cpu.cpu_temps.size());
    for (size_t core = 0; core < current_stats.cpu.cpu_temps.size(); core++) {
      
      mm.min_core_temp_veq[core] = current_stats.cpu.cpu_temps[core];
      mm.max_core_temp_veq[core] = current_stats.cpu.cpu_temps[core];
      mm.min_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core];
      mm.max_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core];
    }
    current_stats.gpu.gpu_vram.resize(3);
    current_stats.gpu.gpu_vram.resize(3);

    mm.min_gpu_vram  = current_stats.gpu.gpu_vram[1];
    mm.max_gpu_vram  = current_stats.gpu.gpu_vram[1];
    mm.min_cpu_usage = current_stats.cpu.cpu_usage;
    mm.max_cpu_usage = current_stats.cpu.cpu_usage;
    mm.min_gpu_usage = current_stats.gpu.gpu_usage;
    mm.max_gpu_usage = current_stats.gpu.gpu_usage;
    mm.min_gpu_temp  = current_stats.gpu.gpu_temp;
    mm.max_gpu_temp  = current_stats.gpu.gpu_temp;
    mm.min_gpu_freq  = current_stats.gpu.gpu_freq;
    mm.max_gpu_freq  = current_stats.gpu.gpu_freq;
    mm.min_ram_usage = current_stats.ram.ram_usage;
    mm.max_ram_usage = current_stats.ram.ram_usage;

    is_min_max_base_set = true;
  }

  for (size_t core = 0; core < current_stats.cpu.cpu_temps.size(); core++) {

    if (current_stats.cpu.cpu_temps[core] < mm.min_core_temp_veq[core]) { mm.min_core_temp_veq[core] = current_stats.cpu.cpu_temps[core]; }
    if (current_stats.cpu.cpu_temps[core] > mm.max_core_temp_veq[core]) { mm.max_core_temp_veq[core] = current_stats.cpu.cpu_temps[core]; }
    if (current_stats.cpu.cpu_freqs[core] < mm.min_core_freq_veq[core]) { mm.min_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core]; }
    if (current_stats.cpu.cpu_freqs[core] > mm.max_core_freq_veq[core]) { mm.max_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core]; }
  }
  if (current_stats.gpu.gpu_vram[1] < mm.min_gpu_vram) { mm.min_gpu_vram = current_stats.gpu.gpu_vram[1]; }
  if (current_stats.gpu.gpu_vram[1] > mm.max_gpu_vram) { mm.max_gpu_vram = current_stats.gpu.gpu_vram[1]; }
  if (current_stats.gpu.gpu_usage < mm.min_gpu_usage) { mm.min_gpu_usage = current_stats.gpu.gpu_usage; }
  if (current_stats.gpu.gpu_usage > mm.max_gpu_usage) { mm.max_gpu_usage = current_stats.gpu.gpu_usage; }
  if (current_stats.gpu.gpu_temp < mm.min_gpu_temp) { mm.min_gpu_temp = current_stats.gpu.gpu_temp; }
  if (current_stats.gpu.gpu_temp > mm.max_gpu_temp) { mm.max_gpu_temp = current_stats.gpu.gpu_temp; }
  if (current_stats.gpu.gpu_freq < mm.min_gpu_freq) { mm.min_gpu_freq = current_stats.gpu.gpu_freq; }
  if (current_stats.gpu.gpu_freq > mm.max_gpu_freq) { mm.max_gpu_freq = current_stats.gpu.gpu_freq; }
  if (current_stats.cpu.cpu_usage < mm.min_cpu_usage) { mm.min_cpu_usage = current_stats.cpu.cpu_usage; }
  if (current_stats.cpu.cpu_usage > mm.max_cpu_usage) { mm.max_cpu_usage = current_stats.cpu.cpu_usage; }
  if (current_stats.ram.ram_usage < mm.min_ram_usage) { mm.min_ram_usage = current_stats.ram.ram_usage; }
  if (current_stats.ram.ram_usage > mm.max_ram_usage) { mm.max_ram_usage = current_stats.ram.ram_usage; }
}

void draw_system_dashboard(stats &current_stats, min_max &mm) {
  
  const ImGuiIO& ioo = ImGui::GetIO();
  
  ImGui::Begin("System Dashboard");
  ImGui::Separator();
  ImGui::Text("CPU - %s      fps %.0f", current_stats.cpu.cpu_model.c_str(), ioo.Framerate);
  ImGui::Separator();
  // There is a problem with min value of cpu usage which is always 0% and it is not possible I guess TODO FIX
  ImGui::Text("CPU Usage : %.0f %%                 min %0.f %% max %.0f %%", current_stats.cpu.cpu_usage, mm.min_cpu_usage, mm.max_cpu_usage);
  ImGui::Separator();
  for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
    ImGui::Text("CPU Core %zu : %.0f C                min %.0f C max %.0f C", i, current_stats.cpu.cpu_temps[i], mm.min_core_temp_veq[i], mm.max_core_temp_veq[i]);
  }
  ImGui::Separator();
  for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
    ImGui::Text("CPU Core %zu : %.0f MHz            min %.0f MHz max %.0f Mhz", i, current_stats.cpu.cpu_freqs[i], mm.min_core_freq_veq[i], mm.max_core_freq_veq[i]);
  }
  ImGui::Separator();
  ImGui::Text("GPU - %s", current_stats.gpu.gpu_model.c_str());
  ImGui::Separator();
  ImGui::Text("GPU Usage : %u %%                 min %u %% max %u %%", current_stats.gpu.gpu_usage, mm.min_gpu_usage, mm.max_gpu_usage);
  ImGui::Text("GPU Temp : %u C                  min %u C max %u C", current_stats.gpu.gpu_temp, mm.min_gpu_temp, mm.max_gpu_temp);
  ImGui::Text("GPU Freq : %u MHz              min %u MHz max %u MHz", current_stats.gpu.gpu_freq, mm.min_gpu_freq, mm.max_gpu_freq);
  if (!current_stats.gpu.gpu_vram.empty()) {
    ImGui::Text("GPU VRAM : %u MiB / %u MiB    min %u MiB max %u MiB", current_stats.gpu.gpu_vram[1], current_stats.gpu.gpu_vram[0], mm.min_gpu_vram, mm.max_gpu_vram);
  }
  ImGui::Separator();
  ImGui::Text("RAM");
  ImGui::Separator();
  ImGui::Text("RAM Usage : %u %%                 min %u %% max %u %%", current_stats.ram.ram_usage, mm.min_ram_usage, mm.max_ram_usage);
  ImGui::End();
}


// TODO : create ImGui components dashboard, refresh it every 0.5s here i guess
