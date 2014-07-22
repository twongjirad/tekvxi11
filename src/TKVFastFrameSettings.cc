#include "TKVFastFrameSettings.hh"
#include <cstring>
#include <cstdlib>
#include <iostream>

TKVFastFrameSettings::TKVFastFrameSettings() {
  // if ( input!=NULL )
  //   updateParameters( input );
}

TKVFastFrameSettings::~TKVFastFrameSettings() {}

void TKVFastFrameSettings::updateParameters( char* input ) {
  // Expecting input from Tektronic 5000 series query: HOR:FAST?
  // See programmers manual for more details.
  // std::cout << input << std::endl;
  char* pch = std::strtok( input, ";" );
  int itoken=0; 
  while ( pch!=NULL ) {
    //std::cout << "[" << itoken << "] " << pch << std::endl;
    if ( itoken==1 ) {
      if ( strcmp( pch , "ON" )==0 )
	activated = true;
      else
	activated = false;
    }
    else if ( itoken==27 )
      numframes = std::atoi( pch );
    else if ( itoken==26 )
      framelength = std::atoi( pch );
    else if ( itoken>50 )
      break;
    itoken++;
    pch = std::strtok( NULL, ";" );
  }
}

void TKVFastFrameSettings::print() {
  std::string state;
  if ( activated==false )
    state = "ON";
  else
    state = "OFF";

  std::cout << "--------------------------" << std::endl;
  std::cout << " Fast Frame Settings" << std::endl;
  std::cout << "--------- " << std::endl;
  std::cout << " state: " << state << std::endl;
  std::cout << " number of frames (per buffer): " << numframes << std::endl;
  std::cout << " samples per frames: " << framelength << std::endl;
  std::cout << "--------------------------" << std::endl;

}
