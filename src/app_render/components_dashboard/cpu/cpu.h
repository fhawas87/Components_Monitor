#pragma once

#include <string>
#include <vector>
#include <cstdlib>

/*class cpu {

public :

  cpu_model();

  void load_sample_data() {

    get_cpu_core_thermal_values();
    get_cpu_core_frequencies();
    get_cpu_utilization();
  }

private :
  
  std::string cpu_model();
  
  std::vector<float> get_cpu_core_thermal_values();
  std::vector<float> get_cpu_core_frequencies(const size_t& number_of_cores);

  float get_cpu_utilization();


};
*/


std::string cpu_model = get_cpu_model();

std::vector<float> get_cpu_core_thermal_values();
std::vector<float> get_cpu_core_frequencies();

float get_cpu_utilization();

static unsigned int retrieve_fabric_cpu_freq(std::string &cpu_model_name) {
  
  const char *char_pointer = cpu_model_name.c_str();
  char cpu_freq_buffer[4];
  cpu_freq_buffer[3] = '\0';
  
  while (*char_pointer != '@') { char_pointer++; }
  
  if (*char_pointer == '@') {

    char_pointer++;
    char_pointer++;
    cpu_freq_buffer[0] = *char_pointer; char_pointer++; char_pointer++;
    cpu_freq_buffer[1] = *char_pointer; char_pointer++;
    cpu_freq_buffer[2] = *char_pointer;
  }

  return (10 * std::atoi(cpu_freq_buffer));
}
