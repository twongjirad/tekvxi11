#include "TKVDataSettings.hh"
#include <iostream>

TKVDataSettings::TKVDataSettings() {}

TKVDataSettings::~TKVDataSettings() {}

void TKVDataSettings::updateParametersFromBuffer( char* input ) {

}

void TKVDataSettings::print() {
  std::cout << "---------------------------" << std::endl;
  std::cout << " Waveform Format Settings" << std::endl;
  std::cout << "-------------------------- " << std::endl;
  std::cout << " mode: " << mode << std::endl;
  std::cout << " samples per trace: " << nsamples << std::endl;
  std::cout << " traces per buffer: " << framesinbuffer << std::endl;
  std::cout << " time between samples: " << secspertdc << std::endl;
  std::cout << " volts per ADC: " << voltsperadc << std::endl;
  std::cout << " offset in ADC counts: " << adcoffset << std::endl;
  std::cout << " bytes per word: " << bytesperword << std::endl;
  std::cout << "--------------------------" << std::endl;
}
