#include "TKVTekHorizontalSettings.hh"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

TKVTekHorizontalSettings::TKVTekHorizontalSettings( char* input ) {
  updateParameters( input );
}

TKVTekHorizontalSettings::~TKVTekHorizontalSettings() {}

void TKVTekHorizontalSettings::updateParameters( char* input ) {
  // Expecting input from Tektronic 5000 series query: HOR:FAST?
  // See programmers manual for more details.
  char* pch = std::strtok( input, ";" );
  int itoken=0; 
  while ( pch!=NULL ) {
    //std::cout << "[" << itoken << "] " << pch << std::endl;
    if ( itoken==0 )
      interpratio = std::atof( pch );
    else if (itoken==1)
      scale = std::atof( pch );
    else if (itoken==2)
      position = std::atof( pch );
    else if (itoken==3)
      samplerate = std::atof( pch );
    else if (itoken==4) {
      char scan[100];
      sscanf( pch, "%s", scan );
      units = std::string(scan);
    }
    else if ( itoken>4 )
      break;
    itoken++;
    pch = std::strtok( NULL, ";" );
  }
}

void TKVTekHorizontalSettings::print() {
  std::cout << "--------------------------" << std::endl;
  std::cout << " Horizontal (TDC) Settings" << std::endl;
  std::cout << "--------- " << std::endl;
  std::cout << " units: " << units << std::endl;
  std::cout << " samples per waveform: " << recordlength << std::endl;
  std::cout << " sampling rate: " << samplerate << " Hz" << std::endl;
  std::cout << " window size: " << float(recordlength)/samplerate << " secs" << std::endl;
  std::cout << " trigger position: " << position << " divs" << std::endl;
  std::cout << " scale: " << scale << " sec/div" << std::endl;
  //std::cout << " interpolation ratio: " << interpratio << std::endl;
  std::cout << "--------------------------" << std::endl;

}
