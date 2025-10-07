#include <vector>
#include <string>

#include <imgui.h>
#include <implot.h>

#include "sample.h"

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
