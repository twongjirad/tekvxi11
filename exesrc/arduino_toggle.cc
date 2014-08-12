#include <iostream>
#include <unistd.h>

#include "TKVArduinoTrigger.hh"

int main( int narg, char** argv ) {

  std::cout << "Loading Arduino Trigger..." << std::endl;
  TKVArduinoTrigger* trig = new TKVArduinoTrigger();
  if ( !trig->isconnected() ) {
    std::cout << "Could not connect arduino. quitting." << std::endl;
    delete trig;
    return -1;
  }
  trig->setOFFstate();

  bool finished = false;
  bool valid = false;
  std::string response;
  do {
    std::cout << "Set Arduino Mode" << std::endl;
    std::cout << " [0]: Set Off State" << std::endl;
    std::cout << " [1]: Set On State" << std::endl;
    if ( trig->isVetoMode() )
      std::cout << " [t]: Set Trigger Mode [ON state = arduino signal sent; OFF state = no signal sent]" << std::endl;
    else
      std::cout << " [v]: Set Veto Mode [ON state = no arduino signal sent; OFF state = signal sent]" << std::endl;
    std::cout << " [q]: Quit." << std::endl;
    std::cin >> response;
    valid = true;
    if ( response!="q" &&
	 response!="0" && 
	 response!="1" && 
	 response!="v" ) {
      valid = false;
      std::cout << "Not a valid response." << std::endl;
    }
    else if ( response=="0" ) {
      trig->setOFFstate();
    }
    else if ( response=="1" ) {
      trig->setONstate();
    }
    else if ( response=="v" ) {
      if ( trig->isVetoMode() ) {
	trig->setTriggerMode();
	std::cout << "Now in TRIGGER Mode (Arduino sends signal when in ON state; no signal while in OFF state)" << std::endl;
      }
      else {
	trig->setVetoMode();
	std::cout << "Now in VETO Mode (Arduino sends signal, when in OFF state; no signal while in ON state)" << std::endl;
      }
    }
    else if ( response=="q" ) {
      finished = true;
    }
    
  } while ( !finished || !valid );
  
  delete trig;

  return 0;
}

