#pragma once

#include <vector>
#include <string>
#include <stdexcept>

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
  unsigned int min_ram_usage_m;
  unsigned int max_ram_usage_m;
};

struct cpu_stats {

  //std::string cpu_model;

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
  
  std::vector<unsigned int> ram_info;
};

struct stats {

  cpu_stats cpu;
  gpu_stats gpu;
  ram_stats ram;
};

struct ring {
  
  static unsigned int FABRIC_CPU_FREQ = retrieve_fabric_cpu_freq(cpu_model);
  //static unsigned int MAX_CPU_FREQ = FABRIC_CPU_FREQ;

  static constexpr unsigned int MAX_SAMPLES_HISTORY = 150;

  std::vector<std::vector<float>> cpu_freq_ring;
  std::vector<std::vector<flaot>> cpu_temp_ring;
  std::vector<std::vector<float>> gpu_vram_ring;
  std::vector<std::vector<float>> ram_ring;
  
  std::vector<float> cpu_usage_ring;
  std::vector<float> gpu_usage_ring;
  std::vector<float> gpu_freq_ring;
  std::vector<float> gpu_temp_ring;
};

void manage_ring_data(const float &val , const std::vector<float> &vec, std::vector<float> &ring_1, std::vector<std::vector<float>> &ring_2) {

  if (val == 0 && ring_1.empty()) {
    if (ring_2[0].size() >= (MAX_SAMPLES_HISTORY - (MAX_SAMPLES_HISTORY * 0.1))) {
      for (size_t i = 0; i < vec.size(); i++) {
        ring_2[i].erase(ring_2.begin());
      }
    }
    for (size_t i = 0; i < vec.size(); i++) {
      ring_2[i].emplace_back(vec[i]);

      if (vec[i] > FABRIC_CPU_FREQ) {
        FABRIC_CPU_FREQ = (unsigned int)(vec[i] * 1.25);
      }
    }
  }
  if (vec.empty() && ring_2.empty()) {
    if (ring_1.size() >= (MAX_SAMPLES_HISTORY - (MAX_SAMPLES_HISTORY * 0.1))) {
      ring_1.erase(ring_1.begin());
      ring_1.emplace_back(val);
    }
  }
  else { throw std::runtime_error("\nManaging ring data error\n"); }
}

ring ring_data{};

bool been_resized = false;

stats refresh_samples() {
  
  if (!been_resized) {

    ring_data.cpu_freq_ring.resize(cores);
    ring_data.cpu_temp_ring.resize(cores);
    ring_data.gpu_vram_ring.resize(4);
    ring_data.ram_ring.resize(4);
  }

  stats current_stats{};

  //current_stats.cpu.cpu_model = get_cpu_model();
  current_stats.gpu.gpu_model     = get_accessible_device_name();
  current_stats.cpu.cpu_usage     = get_cpu_utilization();                        manage_ring_data(current_stats.cpu.cpu_usage, {}, ring_data.cpu_usage_ring, {});
  current_stats.gpu.gpu_usage     = get_core_utilization_percentage_rate();       manage_ring_data(current_stats.gpu.gpu_usage, {}, ring_data.gpu_usage_ring, {});
  current_stats.gpu.gpu_temp      = get_current_gpu_temperature();                manage_ring_data(current_stats.gpu.gpu_temp, {}, ring_data.gpu_temp_ring, {});
  current_stats.gpu.gpu_freq      = get_gpu_clock_frequency();                    manage_ring_data(current_stats.gpu.gpu_freq, {}, ring_data.gpu_freq_ring, {});

  current_stats.cpu.cpu_temps     = get_cpu_core_thermal_values();
  current_stats.cpu.cpu_freqs     = get_cpu_core_frequencies();
  current_stats.gpu.gpu_vram      = get_gpu_VRAM_info();
  current_stats.ram.ram_info      = get_ram_memory();

  if (!been_resized) {

    ring_data.cpu_temp_ring.resize(cores);                                        // HAD TO RESIZE VECTORS BEFORE LOOPING THROUGHT THEM IN manage_ring_data() FUNCTION  
    ring_data.cpu_freq_ring.resize(cores);
    ring_data.gpu_vram_ring.resize(4);
    ring_data.ram_ring.resize(4);

    for (size_t i = 0; i < cores; i++) {
      ring_data.cpu_temp_ring[i].emplace_back(current_stats.cpu.cpu_temps[i]);
      ring_data.cpu_freq_ring[i].emplace_back(current_stats.cpu.cpu_freqs[i]);
    }
    for (size_t i = 0; i < 4; i++) {
      ring_data.gpu_vram_ring[i].emplace_back(current_stats.gpu.gpu_vram[i]);
      ring_data.ram_ring[i].emplace_back(current_stats.ram.ram_info[i]);
    }

    been_resized = true;  // ONLY NEED TO RESIZE AND PUSH SOMETHING TO THE VECTORS ONCE SO FLAG IS NEEDED FOR STOPPING IT IG
  }
                                                                                  manage_ring_data(0, current_stats.cpu.cpu_temps, {}, ring_data.cpu_temps);                                
                                                                                  manage_ring_data(0, current_stats.cpu.cpu_freqs, {}, ring_data.cpu_freqs);
                                                                                  manage_ring_data(0, current_stats.gpu.gpu_vram, {}, ring_data.gpu_vram_ring);
                                                                                  manage_ring_data(0, current_stats.ram.ram_info, {}, ring_data.ram_ring);
  return current_stats;
}

bool is_min_max_base_set = false;

void update_min_max(stats &current_stats, min_max &mm) {
  
  if (!is_min_max_base_set) {
    
    // If there was no value set yet, set it to current var reading

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
    current_stats.ram.ram_info.resize(4);

    mm.min_gpu_vram     = current_stats.gpu.gpu_vram[1];
    mm.max_gpu_vram     = current_stats.gpu.gpu_vram[1];
    mm.min_cpu_usage    = current_stats.cpu.cpu_usage;
    mm.max_cpu_usage    = current_stats.cpu.cpu_usage;
    mm.min_gpu_usage    = current_stats.gpu.gpu_usage;
    mm.max_gpu_usage    = current_stats.gpu.gpu_usage;
    mm.min_gpu_temp     = current_stats.gpu.gpu_temp;
    mm.max_gpu_temp     = current_stats.gpu.gpu_temp;
    mm.min_gpu_freq     = current_stats.gpu.gpu_freq;
    mm.max_gpu_freq     = current_stats.gpu.gpu_freq;
    mm.min_ram_usage_m  = current_stats.ram.ram_info[2];
    mm.max_ram_usage_m  = current_stats.ram.ram_info[2];
    mm.min_ram_usage    = current_stats.ram.ram_info[3];
    mm.max_ram_usage    = current_stats.ram.ram_info[3];

    is_min_max_base_set = true;
  }

  for (size_t core = 0; core < current_stats.cpu.cpu_temps.size(); core++) {

    if (current_stats.cpu.cpu_temps[core] < mm.min_core_temp_veq[core]) { mm.min_core_temp_veq[core] = current_stats.cpu.cpu_temps[core]; }
    if (current_stats.cpu.cpu_temps[core] > mm.max_core_temp_veq[core]) { mm.max_core_temp_veq[core] = current_stats.cpu.cpu_temps[core]; }
    if (current_stats.cpu.cpu_freqs[core] < mm.min_core_freq_veq[core]) { mm.min_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core]; }
    if (current_stats.cpu.cpu_freqs[core] > mm.max_core_freq_veq[core]) { mm.max_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core]; }
  }
  if (current_stats.gpu.gpu_vram[1]   < mm.min_gpu_vram)      { mm.min_gpu_vram     = current_stats.gpu.gpu_vram[1]; }
  if (current_stats.gpu.gpu_vram[1]   > mm.max_gpu_vram)      { mm.max_gpu_vram     = current_stats.gpu.gpu_vram[1]; }
  if (current_stats.gpu.gpu_usage     < mm.min_gpu_usage)     { mm.min_gpu_usage    = current_stats.gpu.gpu_usage; }
  if (current_stats.gpu.gpu_usage     > mm.max_gpu_usage)     { mm.max_gpu_usage    = current_stats.gpu.gpu_usage; }
  if (current_stats.gpu.gpu_temp      < mm.min_gpu_temp)      { mm.min_gpu_temp     = current_stats.gpu.gpu_temp; }
  if (current_stats.gpu.gpu_temp      > mm.max_gpu_temp)      { mm.max_gpu_temp     = current_stats.gpu.gpu_temp; }
  if (current_stats.gpu.gpu_freq      < mm.min_gpu_freq)      { mm.min_gpu_freq     = current_stats.gpu.gpu_freq; }
  if (current_stats.gpu.gpu_freq      > mm.max_gpu_freq)      { mm.max_gpu_freq     = current_stats.gpu.gpu_freq; }
  if (current_stats.cpu.cpu_usage     < mm.min_cpu_usage)     { mm.min_cpu_usage    = current_stats.cpu.cpu_usage; }
  if (current_stats.cpu.cpu_usage     > mm.max_cpu_usage)     { mm.max_cpu_usage    = current_stats.cpu.cpu_usage; }
  if (current_stats.ram.ram_info[2]   < mm.min_ram_usage_m)   { mm.min_ram_usage_m  = current_stats.ram.ram_info[2]; }
  if (current_stats.ram.ram_info[2]   > mm.max_ram_usage_m)   { mm.max_ram_usage_m  = current_stats.ram.ram_info[2]; }
  if (current_stats.ram.ram_info[3]   < mm.min_ram_usage)     { mm.min_ram_usage    = current_stats.ram.ram_info[3]; }
  if (current_stats.ram.ram_info[3]   > mm.max_ram_usage)     { mm.max_ram_usage    = current_stats.ram.ram_info[3]; }

}

void draw_system_dashboard(stats &current_stats, min_max &mm) {
  
  const ImGuiIO& ioo = ImGui::GetIO();
  
  ImGui::Begin("System Dashboard");
  ImGui::Separator();
  ImGui::Text("CPU - %s      fps %.0f", cpu_model.c_str(), ioo.Framerate);
  ImGui::Separator();

  // There is a problem with min value of cpu usage which is always 0% and it is not possible I guess TODO FIX
  
  ImGui::Text("CPU Usage : %.0f %%                   min %0.f %% max %.0f %%", current_stats.cpu.cpu_usage, mm.min_cpu_usage, mm.max_cpu_usage);
  ImGui::Separator();
  for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
    ImGui::Text("CPU Core %zu : %.0f C                  min %.0f C max %.0f C", i, current_stats.cpu.cpu_temps[i], mm.min_core_temp_veq[i], mm.max_core_temp_veq[i]);
  }
  ImGui::Separator();
  for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
    ImGui::Text("CPU Core %zu : %.0f MHz              min %.0f MHz max %.0f Mhz", i, current_stats.cpu.cpu_freqs[i], mm.min_core_freq_veq[i], mm.max_core_freq_veq[i]);
  }
  ImGui::Separator();
  ImGui::Text("GPU - %s", current_stats.gpu.gpu_model.c_str());
  ImGui::Separator();
  ImGui::Text("GPU Usage : %u %%                   min %u %% max %u %%", current_stats.gpu.gpu_usage, mm.min_gpu_usage, mm.max_gpu_usage);
  ImGui::Text("GPU Temp : %u C                    min %u C max %u C", current_stats.gpu.gpu_temp, mm.min_gpu_temp, mm.max_gpu_temp);
  ImGui::Text("GPU Freq : %u MHz                min %u MHz max %u MHz", current_stats.gpu.gpu_freq, mm.min_gpu_freq, mm.max_gpu_freq);
  if (!current_stats.gpu.gpu_vram.empty()) {
    ImGui::Text("GPU VRAM : %u MiB / %u MiB      min %u MiB max %u MiB", current_stats.gpu.gpu_vram[1], current_stats.gpu.gpu_vram[0], mm.min_gpu_vram, mm.max_gpu_vram);
  }
  ImGui::Separator();
  ImGui::Text("RAM");
  ImGui::Separator();
  if (!current_stats.ram.ram_info.empty()) {
    ImGui::Text("RAM Usage : %u MiB / %u MiB   min %u MiB max %u MiB", current_stats.ram.ram_info[2], current_stats.ram.ram_info[0], mm.min_ram_usage_m, mm.max_ram_usage_m);
    ImGui::Text("RAM Usage : %u %%                   min %u %% max %u %%", current_stats.ram.ram_info[3], mm.min_ram_usage, mm.max_ram_usage);
  }
  ImGui::End();
}


// TODO : create ImGui components dashboard, refresh it every 0.5s here i guess
