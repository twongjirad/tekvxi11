#include "TKVTekHorizontalSettings.hh"
#include <iostream>

TKVTekHorizontalSettings::TKVTekHorizontalSettings( char* input ) {
  updateParameters( input );
}

TKVTekHorizontalSettings::~TKVTekHorizontalSettings() {}

void TKVTekHorizontalSettings::updateParameters( char* input ) {
  std::cout << "horizontal: " << input << std::endl;
}
