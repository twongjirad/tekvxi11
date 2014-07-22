#include "TKVTekChannelSettings.hh"
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cmath>


TKVTekChannelSettings::TKVTekChannelSettings(int chnumber){
  chID = chnumber;
  setToRecord(false);
}

TKVTekChannelSettings::~TKVTekChannelSettings() {
}

bool TKVTekChannelSettings::willRecord() {
  if ( record==true && state==1 )
    return true;
  else
    return false;
}

void TKVTekChannelSettings::updateParameters( char* input ) {
  // Expecting input from Tektronic 5000 series query: CH<chnumber>?
  // See programmers manual for more details.
  //std::cout << input << std::endl;
  char* pch = std::strtok( input, ";" );
  int itoken=0; 
  while ( pch!=NULL ) {
    //std::cout << pch << std::endl;
    if ( itoken==0 ) {
      bandwidth = std::atof( pch );
    }
    else if ( itoken==1 )
      if ( strcmp( pch , "DC" )==0 )
	coupling = DC;
      else
	coupling = AC;
    else if ( itoken==3 )
      offset = std::atof( pch ); // may need to check this in the future
    else if ( itoken==5 )
      position = std::atof( pch );
    else if ( itoken==6 )
      scale = std::atof( pch );
    else if ( itoken==7 )
      termination = std::atof( pch );
    else if ( itoken>11 )
      break;
    itoken++;
    pch = std::strtok( NULL, ";" );
  }
  
}

void TKVTekChannelSettings::print() {
  std::string scoupling;
  if ( coupling==DC )
    scoupling = "DC";
  else
    scoupling = "AC";
  std::string strstate;
  if ( state==1 )
    strstate = "ON";
  else
    strstate = "OFF";

  std::cout << "--------------------------" << std::endl;
  std::cout << " Channel " << chID << " Settings" << std::endl;
  std::cout << "--------- " << std::endl;
  std::cout << " state: " << strstate << std::endl;
  std::cout << " coupling: " << scoupling << std::endl;
  std::cout << " scale: " << scale << " V/div" << std::endl;
  std::cout << " offset: " << offset << " V" << std::endl;
  std::cout << " cursor position: " << position << " y-axis divs" << std::endl;
  std::cout << " termination: " << termination << " Ohms" << std::endl;
  std::cout << "--------------------------" << std::endl;
}
