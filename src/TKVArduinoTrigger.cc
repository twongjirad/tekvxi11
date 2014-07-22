#include "TKVArduinoTrigger.hh"
#include "arduino-serial-lib.h"

#include <iostream>
#include <string>

TKVArduinoTrigger::TKVArduinoTrigger() {
  connected = false;
}

TKVArduinoTrigger::~TKVArduinoTrigger() {
  if ( connected )
    disconnect();
}

int TKVArduinoTrigger::connect() {
  portid = serialport_init( "/dev/ttyACM0", 9600 );
  if ( portid==-1 ) {
    std::cout << "Error, could not open serial port" << std::endl;
    connected = false;
    serialport_flush(portid);
    return -1;
  }
  return 0; // success
}

int TKVArduinoTrigger::setONstate() {
  if ( portid==-1 || !isconnected() ) {
    std::cout << "Arduino not connected!" << std::endl;
    return -1;
  }
  
  int rc = serialport_write( portid, std::string("1\n").c_str() );

}

int TKVArduinoTrigger::setOFFstate() {
  if ( portid==-1 || !isconnected() ) {
    std::cout << "Arduino not connected!" << std::endl;
    return -1;
  }
  
  int rc = serialport_write( portid, std::string("0\n").c_str() );

}

void TKVArduinoTrigger::disconnect() {
  if ( isconnected() )
    serialport_close( portid );
  connected = false;
}
