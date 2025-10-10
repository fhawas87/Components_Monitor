#pragma once

#define MSH 200 // MAX SAMPLE HISTORY

#include <vector>
#include <string>
#include <stdexcept>

#include "cpu.h"
#include "gpu.h"
#include "ram.h"

#include <imgui.h>

struct min_max {  // Tracking min/max values of each parameter

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

  std::vector<float> cpu_temps;
  std::vector<float> cpu_freqs;

  float cpu_usage;
};

struct gpu_stats {
  
  std::string gpu_model;

  std::vector<float> gpu_vram;

  unsigned int gpu_temp;
  unsigned int gpu_usage;
  unsigned int gpu_freq;
};

struct ram_stats {
  
  std::vector<float> ram_info;
};

struct stats {

  cpu_stats cpu;
  gpu_stats gpu;
  ram_stats ram;
};

static unsigned int fcq = retrieve_fabric_cpu_freq(cpu_model);

struct ring {
  
  std::vector<std::vector<float>> cpu_freq_ring;
  std::vector<std::vector<float>> cpu_temp_ring;
  std::vector<std::vector<float>> gpu_vram_ring;
  std::vector<std::vector<float>> ram_ring;
  
  std::vector<float> cpu_usage_ring;
  std::vector<float> gpu_usage_ring;
  std::vector<float> gpu_freq_ring;
  std::vector<float> gpu_temp_ring;
};

void manage_ring_data_val(float val, std::vector<float> &ring) {

  if (!ring.empty() && ring.size() >= (MSH - (MSH * 0.1))) {
    ring.erase(ring.begin());
  }
  ring.emplace_back(val);
}

void manage_ring_data_vec(std::vector<float> vec, std::vector<std::vector<float>> &ring) {

  if (!ring.empty() && ring[0].size() >= (MSH - (MSH * 0.1))) {
    for (size_t i = 0; i < vec.size(); i++) {
      ring[i].erase(ring[i].begin());
    }
  }
  for (size_t i = 0; i < vec.size(); i++) {
    ring[i].emplace_back(vec[i]);
  }
}

inline ring ring_data{};
inline stats current_stats{};
inline min_max mm{};

stats refresh_samples() {
  
  static bool been_vec_rings_resized = false;
  static bool been_val_rings_initialized = false;

  current_stats.gpu.gpu_model = get_accessible_device_name();
  current_stats.cpu.cpu_usage = get_cpu_utilization();
  current_stats.gpu.gpu_usage = get_core_utilization_percentage_rate();
  current_stats.gpu.gpu_temp = get_current_gpu_temperature();
  current_stats.gpu.gpu_freq = get_gpu_clock_frequency();
  
  if (!been_val_rings_initialized) {

    ring_data.cpu_usage_ring.emplace_back(current_stats.cpu.cpu_usage);
    ring_data.gpu_usage_ring.emplace_back(current_stats.gpu.gpu_usage);
    ring_data.gpu_temp_ring.emplace_back(current_stats.gpu.gpu_temp);
    ring_data.gpu_freq_ring.emplace_back(current_stats.gpu.gpu_freq);

    been_val_rings_initialized = true;
  }

  manage_ring_data_val((float)current_stats.cpu.cpu_usage, ring_data.cpu_usage_ring);
  manage_ring_data_val((float)current_stats.gpu.gpu_usage, ring_data.gpu_usage_ring);
  manage_ring_data_val((float)current_stats.gpu.gpu_temp, ring_data.gpu_temp_ring);
  manage_ring_data_val((float)current_stats.gpu.gpu_freq, ring_data.gpu_freq_ring);

  current_stats.cpu.cpu_temps = get_cpu_core_thermal_values();
  current_stats.cpu.cpu_freqs = get_cpu_core_frequencies();
  current_stats.gpu.gpu_vram = get_gpu_VRAM_info();
  current_stats.ram.ram_info = get_ram_memory();

  for (size_t i = 0; i < current_stats.cpu.cpu_freqs.size(); i++) {
    if (current_stats.cpu.cpu_freqs[i] > fcq) { fcq *= 1.25; }
  }

  if (!been_vec_rings_resized) {

    ring_data.cpu_temp_ring.resize(current_stats.cpu.cpu_temps.size());    // HAD TO RESIZE VECTORS BEFORE LOOPING THROUGHT THEM IN manage_ring_data() FUNCTION  
    ring_data.cpu_freq_ring.resize(current_stats.cpu.cpu_temps.size());
    ring_data.gpu_vram_ring.resize(4);
    ring_data.ram_ring.resize(4);

    for (size_t i = 0; i < current_stats.cpu.cpu_temps.size(); i++) {
      float current_core_temp_value = current_stats.cpu.cpu_temps[i];
      float current_core_freq_value = current_stats.cpu.cpu_freqs[i];
      ring_data.cpu_temp_ring[i].emplace_back(current_core_temp_value);
      ring_data.cpu_freq_ring[i].emplace_back(current_core_freq_value);
    }
    for (size_t i = 0; i < 4; i++) {
      float current_vram_value = current_stats.gpu.gpu_vram[i];
      float current_ram_value = current_stats.ram.ram_info[i];
      ring_data.gpu_vram_ring[i].emplace_back(current_vram_value);
      ring_data.ram_ring[i].emplace_back(current_ram_value);
    }

    been_vec_rings_resized = true;  // ONLY NEED TO RESIZE AND PUSH SOMETHING TO THE VECTORS ONCE SO FLAG IS NEEDED FOR STOPPING IT IG
  }

  manage_ring_data_vec(current_stats.cpu.cpu_temps, ring_data.cpu_temp_ring); 
  manage_ring_data_vec(current_stats.cpu.cpu_freqs, ring_data.cpu_freq_ring);
  manage_ring_data_vec(current_stats.gpu.gpu_vram, ring_data.gpu_vram_ring);
  manage_ring_data_vec(current_stats.ram.ram_info, ring_data.ram_ring);

  return current_stats;
}

void update_min_max(stats &current_stats, min_max &mm) {

  static bool is_min_max_base_set = false;
  
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
    current_stats.gpu.gpu_vram.resize(4);
    current_stats.ram.ram_info.resize(4);

    mm.min_gpu_vram = current_stats.gpu.gpu_vram[1];
    mm.max_gpu_vram = current_stats.gpu.gpu_vram[1];
    mm.min_cpu_usage = current_stats.cpu.cpu_usage;
    mm.max_cpu_usage = current_stats.cpu.cpu_usage;
    mm.min_gpu_usage = current_stats.gpu.gpu_usage;
    mm.max_gpu_usage = current_stats.gpu.gpu_usage;
    mm.min_gpu_temp = current_stats.gpu.gpu_temp;
    mm.max_gpu_temp = current_stats.gpu.gpu_temp;
    mm.min_gpu_freq = current_stats.gpu.gpu_freq;
    mm.max_gpu_freq = current_stats.gpu.gpu_freq;
    mm.min_ram_usage_m = current_stats.ram.ram_info[2];
    mm.max_ram_usage_m = current_stats.ram.ram_info[2];
    mm.min_ram_usage = current_stats.ram.ram_info[3];
    mm.max_ram_usage = current_stats.ram.ram_info[3];

    is_min_max_base_set = true;
  }

  for (size_t core = 0; core < current_stats.cpu.cpu_temps.size(); core++) {

    if (current_stats.cpu.cpu_temps[core] < mm.min_core_temp_veq[core]) { mm.min_core_temp_veq[core] = current_stats.cpu.cpu_temps[core]; }
    if (current_stats.cpu.cpu_temps[core] > mm.max_core_temp_veq[core]) { mm.max_core_temp_veq[core] = current_stats.cpu.cpu_temps[core]; }
    if (current_stats.cpu.cpu_freqs[core] < mm.min_core_freq_veq[core]) { mm.min_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core]; }
    if (current_stats.cpu.cpu_freqs[core] > mm.max_core_freq_veq[core]) { mm.max_core_freq_veq[core] = current_stats.cpu.cpu_freqs[core]; }
  }
  if (current_stats.gpu.gpu_usage     < mm.min_gpu_usage)     { mm.min_gpu_usage    = current_stats.gpu.gpu_usage; }
  if (current_stats.gpu.gpu_usage     > mm.max_gpu_usage)     { mm.max_gpu_usage    = current_stats.gpu.gpu_usage; }
  if (current_stats.gpu.gpu_temp      < mm.min_gpu_temp)      { mm.min_gpu_temp     = current_stats.gpu.gpu_temp; }
  if (current_stats.gpu.gpu_temp      > mm.max_gpu_temp)      { mm.max_gpu_temp     = current_stats.gpu.gpu_temp; }
  if (current_stats.gpu.gpu_freq      < mm.min_gpu_freq)      { mm.min_gpu_freq     = current_stats.gpu.gpu_freq; }
  if (current_stats.gpu.gpu_freq      > mm.max_gpu_freq)      { mm.max_gpu_freq     = current_stats.gpu.gpu_freq; }
  if (current_stats.cpu.cpu_usage     < mm.min_cpu_usage)     { mm.min_cpu_usage    = current_stats.cpu.cpu_usage; }
  if (current_stats.cpu.cpu_usage     > mm.max_cpu_usage)     { mm.max_cpu_usage    = current_stats.cpu.cpu_usage; }
  if (current_stats.gpu.gpu_vram[1]   < mm.min_gpu_vram)      { mm.min_gpu_vram     = current_stats.gpu.gpu_vram[1]; }
  if (current_stats.gpu.gpu_vram[1]   > mm.max_gpu_vram)      { mm.max_gpu_vram     = current_stats.gpu.gpu_vram[1]; }
  if (current_stats.ram.ram_info[2]   < mm.min_ram_usage_m)   { mm.min_ram_usage_m  = current_stats.ram.ram_info[2]; }
  if (current_stats.ram.ram_info[2]   > mm.max_ram_usage_m)   { mm.max_ram_usage_m  = current_stats.ram.ram_info[2]; }
  if (current_stats.ram.ram_info[3]   < mm.min_ram_usage)     { mm.min_ram_usage    = current_stats.ram.ram_info[3]; }
  if (current_stats.ram.ram_info[3]   > mm.max_ram_usage)     { mm.max_ram_usage    = current_stats.ram.ram_info[3]; }
}
