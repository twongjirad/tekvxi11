#include "TKVFastFrameSettings.hh"
#include <cstring>
#include <cstdlib>
#include <iostream>

TKVFastFrameSettings::TKVFastFrameSettings( char* input ) {
  if ( input!=NULL )
    updateParameters( input );
}

TKVFastFrameSettings::~TKVFastFrameSettings() {}

void TKVFastFrameSettings::updateParameters( char* input ) {
  // Expecting input from Tektronic 5000 series query: HOR:FAST?
  // See programmers manual for more details.
  //std::cout << input << std::endl;
  char* pch = std::strtok( input, ";" );
  int itoken=0; 
  while ( pch!=NULL ) {
    std::cout << pch << std::endl;
    if ( itoken==0 ) {
      if ( strcmp( pch , "ON" )==0 )
	activated = true;
      else
	activated = false;
    }
    else if ( itoken==2 )
      numframes = std::atoi( pch );
    else if ( itoken==3 )
      framelength = std::atoi( pch );
    else if ( itoken>3 )
      break;
    itoken++;
    pch = std::strtok( NULL, ";" );
  }
}
