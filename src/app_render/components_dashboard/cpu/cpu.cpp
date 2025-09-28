#include <cpuid.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <sensors/sensors.h>

#include "cpu.h"

std::string get_cpu_model() {

  char cpu_brand_name[49];

  unsigned int cpu_info[4] = {0, 0, 0, 0};
  unsigned int limit_address = __get_cpuid_max(0x80000000, NULL);

  if (limit_address < 0x80000004) {return cpu_brand_name; }

  __cpuid(0x80000002, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
  memcpy(cpu_brand_name, cpu_info, 16);
  __cpuid(0x80000003, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
  memcpy(cpu_brand_name + 16, cpu_info, 16);
  __cpuid(0x80000004, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
  memcpy(cpu_brand_name + 32, cpu_info, 16);

  cpu_brand_name[48] = '\0';

  return std::string(cpu_brand_name);
}

std::vector<float> get_cpu_core_thermal_values() {

  std::vector<float> temps_vec;
  int chip_index = 0;
  const sensors_chip_name* chip_name;
  char chip_buffer[64];

  while ((chip_name = sensors_get_detected_chips(NULL, &chip_index)) != NULL) {

    sensors_snprintf_chip_name(chip_buffer, sizeof(chip_buffer), chip_name);
    if (strncmp(chip_buffer, "coretemp-isa-0000", 17) != 0) {
      continue;
    }
    int feature_index = 0;
    const sensors_feature* feature;

    while ((feature = sensors_get_features(chip_name, &feature_index)) != NULL) {
      if (feature->type != SENSORS_FEATURE_TEMP) { continue; }

      char *label = sensors_get_label(chip_name, feature);
      if (strncmp(label, "Package", 7) == 0) { free(label); continue; }
      free(label);

      const sensors_subfeature* sub_feature = sensors_get_subfeature(chip_name, feature, SENSORS_SUBFEATURE_TEMP_INPUT);
      if (!sub_feature) { continue; }

      double current_core_temperature = 0.0;
      if (sensors_get_value(chip_name, sub_feature->number, &current_core_temperature) == 0) {
        temps_vec.emplace_back((float)current_core_temperature);
      }
    }

    break;
  }

  return temps_vec;
}

std::vector<float> get_cpu_core_frequencies(size_t number_of_cores) {
 
  std::vector<float> freqs_vec;
  char path_buffer[128];
  freqs_vec.resize(number_of_cores);

  for (size_t core = 0; core < number_of_cores; core++) {

    int number_of_chars = snprintf(path_buffer, sizeof(path_buffer),
    "/sys/devices/system/cpu/cpu%zu/cpufreq/scaling_cur_freq", core);

    FILE* scaling_cur_freq_info = fopen(path_buffer, "r");
    if (!scaling_cur_freq_info) {
      printf("PATH PROBLEM\n");
      printf("Core stop : %zu\n", core);
      freqs_vec.emplace_back(0);
      continue;
    }

    unsigned int current_freq_kHz = 0;
    fscanf(scaling_cur_freq_info, "%d", &current_freq_kHz);

    float current_freq_mHz = (float)(current_freq_kHz / 1000);
    freqs_vec.emplace_back(current_freq_mHz);
  }

  return freqs_vec;
}

struct cpu_proc_stats {

  unsigned long long user;
  unsigned long long nice;
  unsigned long long system;
  unsigned long long idle;
  unsigned long long iowait;
  unsigned long long irq;
  unsigned long long softirq;
  unsigned long long steal;
  unsigned long long guest;
  unsigned long long guest_nice;
};

cpu_proc_stats fill_struct_of_cpu_stats() {

  FILE *cpu_stats_file_info = fopen("/proc/stat", "r");
  if (!cpu_stats_file_info) {
    printf("Something wrong with the /proc/stat path\n");
    fclose(cpu_stats_file_info);

    return {};
  }

  cpu_proc_stats proc_info_stat_read;
  char stats_buffer[512];
  fgets(stats_buffer, sizeof(stats_buffer), cpu_stats_file_info);
  fclose(cpu_stats_file_info);

  int stats_read = sscanf(stats_buffer,
                    "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                    &proc_info_stat_read.user,
                    &proc_info_stat_read.nice,
                    &proc_info_stat_read.system,
                    &proc_info_stat_read.idle,
                    &proc_info_stat_read.iowait,
                    &proc_info_stat_read.irq,
                    &proc_info_stat_read.softirq,
                    &proc_info_stat_read.steal,
                    &proc_info_stat_read.guest,
                    &proc_info_stat_read.guest_nice);

  return proc_info_stat_read;
}

float get_cpu_utilization() {
  
  // not sure if formula of this cpu utlization calculations is correct
  // but final result in percentage value looks fine

  cpu_proc_stats first_sample = fill_struct_of_cpu_stats();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  cpu_proc_stats second_sample = fill_struct_of_cpu_stats();

  unsigned long long first_sample_sum = first_sample.user    + first_sample.nice + 
                                        first_sample.system  + first_sample.idle +
                                        first_sample.iowait  + first_sample.irq +
                                        first_sample.softirq + first_sample.steal +
                                        first_sample.guest   + first_sample.guest_nice;

  unsigned long long second_sample_sum = second_sample.user    + second_sample.nice +
                                         second_sample.system  + second_sample.idle +
                                         second_sample.iowait  + second_sample.irq +
                                         second_sample.softirq + second_sample.steal +
                                         second_sample.guest   + second_sample.guest_nice;

  unsigned long long idle1_iowait1 = first_sample.idle + first_sample.iowait;
  unsigned long long idle2_iowait2 = second_sample.idle + second_sample.iowait;
  unsigned long long delta_of_two_samples = second_sample_sum - first_sample_sum;
  unsigned long long idle_iowait_delta_of_two_samples = idle2_iowait2 - idle1_iowait1;

  float cpu_utilization = (((float)(delta_of_two_samples)) - ((float)(idle_iowait_delta_of_two_samples))) / delta_of_two_samples * 100;
  
  return cpu_utilization;
}
