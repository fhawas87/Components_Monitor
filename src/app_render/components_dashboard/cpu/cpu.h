#pragma once

#include <string>
#include <vector>

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


std::string get_cpu_model();
std::vector<float> get_cpu_core_thermal_values();
std::vector<float> get_cpu_core_frequencies(size_t number_of_cores);

float get_cpu_utilization();
