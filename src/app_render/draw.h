#include <vector>
#include <string>

#include <imgui.h>
#include <implot.h>

#include "components_dashboard/sample.h"
#include "cpu.h"
#include "gpu.h"

class ImGui_Theme {
public:

  const ImVec4 LightGrey = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
  const ImVec4 WindowBG = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);

  inline void ApplyTheme() {

    ImGuiStyle &gui_style = ImGui::GetStyle();

    gui_style.Colors[ImGuiCol_WindowBg] = WindowBG;
  }
};

class ImPlot_Theme {
public:
  
  const ImVec4 LightGrey = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
  const ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  const ImVec4 ChartsBG = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);

  inline void ApplyTheme() {

    ImPlotStyle &plot_style = ImPlot::GetStyle();
    
    plot_style.LineWeight = 1.0f;

    plot_style.Colors[ImPlotCol_PlotBg] = ChartsBG;
    plot_style.Colors[ImPlotCol_FrameBg] = ChartsBG;
    plot_style.Colors[ImPlotCol_AxisText] = White;
    plot_style.Colors[ImPlotCol_AxisGrid] = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);

    static bool is_colormap_installed = false;

    if (!is_colormap_installed) {
      ImVec4 mono[2] = { LightGrey };

      ImPlot::AddColormap("MonoAccent", mono, 2);
      ImPlot::PushColormap("MonoAccent");

      is_colormap_installed = true;
    }
  }

  void SetMarkersTheme() {

    ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 1.5, White, IMPLOT_AUTO, White);
  }
};

void draw_system_dashboard(stats &current_stats, min_max &mm) {
  
  const ImGuiIO& ioo = ImGui::GetIO();
  
  ImGui::Begin("System Dashboard");
  ImGui::Separator();
  ImGui::Text("CPU - %s      fps %.0f", cpu_model.c_str(), ioo.Framerate);
  ImGui::Separator();  
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
    ImGui::Text("GPU VRAM : %.0f MiB / %.0f MiB      min %u MiB max %u MiB", current_stats.gpu.gpu_vram[1], current_stats.gpu.gpu_vram[0], mm.min_gpu_vram, mm.max_gpu_vram);
  }
  ImGui::Separator();
  ImGui::Text("RAM");
  ImGui::Separator();
  if (!current_stats.ram.ram_info.empty()) {
    ImGui::Text("RAM Usage : %.0f MiB / %.0f MiB   min %u MiB max %u MiB", current_stats.ram.ram_info[2], current_stats.ram.ram_info[0], mm.min_ram_usage_m, mm.max_ram_usage_m);
    ImGui::Text("RAM Usage : %.0f %%                   min %u %% max %u %%", current_stats.ram.ram_info[3], mm.min_ram_usage, mm.max_ram_usage);
  }
  ImGui::End();
}

void cpu_chart() {
  
  ImPlot_Theme ipth;

  if (ImPlot::BeginPlot("CPU Usage")) {
    ImPlot::SetupAxes("t[s]", "%");
    ImPlot::SetupAxesLimits(0, MSH, 0, 100, ImGuiCond_Always);
    
    if (!ring_data.cpu_usage_ring.empty()) {
      ImPlot::PlotLine("cpu usage", ring_data.cpu_usage_ring.data(), ring_data.cpu_usage_ring.size());

      ipth.SetMarkersTheme();
      
      ImPlot::PlotScatter("cpu usage", ring_data.cpu_usage_ring.data(), ring_data.cpu_usage_ring.size());
    }
    ImPlot::EndPlot();
  }
  
  for (size_t core = 0; core < current_stats.cpu.cpu_temps.size(); core++) {
    std::string current_core_index = "Core " + std::to_string(core + 1) + " Temperature";

    if (ImPlot::BeginPlot(current_core_index.c_str())) {
      ImPlot::SetupAxes("t[s]", "C");
      ImPlot::SetupAxesLimits(0, MSH, 0, 90, ImGuiCond_Always);

      if (!current_stats.cpu.cpu_temps.empty()) {
        ImPlot::PlotLine(current_core_index.c_str(), ring_data.cpu_temp_ring[core].data(), ring_data.cpu_temp_ring[core].size());

        ipth.SetMarkersTheme();

        ImPlot::PlotScatter(current_core_index.c_str(), ring_data.cpu_temp_ring[core].data(), ring_data.cpu_temp_ring[core].size());
      }
      ImPlot::EndPlot();
    }
  }
  
  for (size_t core = 0; core < current_stats.cpu.cpu_freqs.size(); core++) {
    std::string current_core_index = "Core " + std::to_string(core + 1) + " Frequency";

    if (ImPlot::BeginPlot(current_core_index.c_str())) {
      ImPlot::SetupAxes("t[s]", "MHz");
      ImPlot::SetupAxesLimits(0, MSH, 0, fcq, ImGuiCond_Always);

      if (!ring_data.cpu_freq_ring.empty()) {
        ImPlot::PlotLine(current_core_index.c_str(), ring_data.cpu_freq_ring[core].data(), ring_data.cpu_freq_ring[core].size());

        ipth.SetMarkersTheme();

        ImPlot::PlotScatter(current_core_index.c_str(), ring_data.cpu_freq_ring[core].data(), ring_data.cpu_freq_ring[core].size());
      }
      ImPlot::EndPlot();
    }
  }
}

void gpu_chart() {
  
  ImPlot_Theme ipth;

  if (ImPlot::BeginPlot("GPU Usage")) {
    ImPlot::SetupAxes("t[s]", "%");
    ImPlot::SetupAxesLimits(0, MSH, 0, 100, ImGuiCond_Always);

    if (!ring_data.gpu_usage_ring.empty()) {
      ImPlot::PlotLine("gpu usage", ring_data.gpu_usage_ring.data(), ring_data.gpu_usage_ring.size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("gpu usage", ring_data.gpu_usage_ring.data(), ring_data.gpu_usage_ring.size());
    }
    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("GPU Temperature")) {
    ImPlot::SetupAxes("t[s]", "C");
  ImPlot::SetupAxesLimits(0, MSH, 0, 90, ImGuiCond_Always);

    if (!ring_data.gpu_temp_ring.empty()) {
      ImPlot::PlotLine("gpu temperature", ring_data.gpu_temp_ring.data(), ring_data.gpu_temp_ring.size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("gpu temperature", ring_data.gpu_temp_ring.data(), ring_data.gpu_temp_ring.size());
    }
    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("GPU Frequency")) {
    ImPlot::SetupAxes("t[s]", "MHz");
    ImPlot::SetupAxesLimits(0, MSH, 0, 3000, ImGuiCond_Always);

    if (!ring_data.gpu_freq_ring.empty()) {
      ImPlot::PlotLine("gpu frequencie", ring_data.gpu_freq_ring.data(), ring_data.gpu_freq_ring.size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("gpu frequency", ring_data.gpu_freq_ring.data(), ring_data.gpu_freq_ring.size());
    }
    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("GPU VRAM Usage (MiB)")) {
    ImPlot::SetupAxes("t[s]", "MiB");
    ImPlot::SetupAxesLimits(0, MSH, 0, current_stats.gpu.gpu_vram[0], ImGuiCond_Always);

    if (!ring_data.gpu_vram_ring.empty()) {
      ImPlot::PlotLine("gpu vram MiB", ring_data.gpu_vram_ring[1].data(), ring_data.gpu_vram_ring[1].size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("gpu vram MiB", ring_data.gpu_vram_ring[1].data(), ring_data.gpu_vram_ring[1].size());
    }
    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("GPU VRAM Usage (%)")) {
    ImPlot::SetupAxes("t[s]", "%");
    ImPlot::SetupAxesLimits(0, MSH, 0, 100, ImGuiCond_Always);

    if (!ring_data.gpu_vram_ring.empty()) {
      ImPlot::PlotLine("gpu vram %", ring_data.gpu_vram_ring[3].data(), ring_data.gpu_vram_ring[3].size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("gpu vram %", ring_data.gpu_vram_ring[3].data(), ring_data.gpu_vram_ring[3].size());
    }
    ImPlot::EndPlot();
  }
}

void ram_chart() {

  ImPlot_Theme ipth;

  if (ImPlot::BeginPlot("RAM Usage (MiB)")) {
    ImPlot::SetupAxes("t[s]", "MiB");
    ImPlot::SetupAxesLimits(0, MSH, 0, current_stats.ram.ram_info[0], ImGuiCond_Always);

    if (!ring_data.ram_ring.empty()) {
      ImPlot::PlotLine("ram usage MiB", ring_data.ram_ring[2].data(), ring_data.ram_ring[2].size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("ram usage MiB", ring_data.ram_ring[2].data(), ring_data.ram_ring[2].size());
    }
    ImPlot::EndPlot();
  }

  if (ImPlot::BeginPlot("RAM Usage (%)")) {
    ImPlot::SetupAxes("t[s]", "%");
    ImPlot::SetupAxesLimits(0, MSH, 0, 100, ImGuiCond_Always);

    if (!ring_data.ram_ring.empty()) {
      ImPlot::PlotLine("ram usage %", ring_data.ram_ring[3].data(), ring_data.ram_ring[3].size());

      ipth.SetMarkersTheme();

      ImPlot::PlotScatter("ram usage %", ring_data.ram_ring[3].data(), ring_data.ram_ring[3].size());
    }
    ImPlot::EndPlot();
  }
}

void draw_system_charts() {
  
  const ImGuiIO &ioo = ImGui::GetIO();

  ImGui::Begin("Components Charts");
  ImGui::Separator();
  ImGui::Text("fps : %.0f", ioo.Framerate);
  ImGui::Separator();
  
  if (ImGui::CollapsingHeader((cpu_model + "##CPU").c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    cpu_chart();
  }

  if (ImGui::CollapsingHeader((current_stats.gpu.gpu_model + "##GPU").c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    gpu_chart();
  }

  if (ImGui::CollapsingHeader("RAM", ImGuiTreeNodeFlags_DefaultOpen)) {
    ram_chart();
  }
  ImGui::End();
}
