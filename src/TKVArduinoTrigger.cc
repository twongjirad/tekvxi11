#include "TKVArduinoTrigger.hh"
#include "arduino-serial-lib.h"

#include <iostream>
#include <string>

TKVArduinoTrigger::TKVArduinoTrigger() {
  state = 0;
  connect();
}

TKVArduinoTrigger::~TKVArduinoTrigger() {
  vetomode = false;
  if ( connected )
    disconnect();
}

int TKVArduinoTrigger::connect() {
  portid = serialport_init( "/dev/ttyACM0", 9600 );
  if ( portid==-1 ) {
    std::cout << "Error, could not open serial port" << std::endl;
    connected = false;
    return -1;
  }
  serialport_flush(portid);
  connected = true;
  return 0; // success
}

int TKVArduinoTrigger::setONstate() {
  if ( portid==-1 || !isconnected() ) {
    std::cout << "Arduino not connected!" << std::endl;
    return -1;
  }
  state = 1;
  if ( !vetomode )
    serialport_write( portid, std::string("1").c_str() );
  else
    serialport_write( portid, std::string("0").c_str() );
}

int TKVArduinoTrigger::setOFFstate() {
  if ( portid==-1 || !isconnected() ) {
    std::cout << "Arduino not connected!" << std::endl;
    return -1;
  }
  state = 0;
  if ( !vetomode )
    serialport_write( portid, std::string("0").c_str() );
  else
    serialport_write( portid, std::string("1").c_str() );
}

void TKVArduinoTrigger::disconnect() {
  if ( isconnected() )
    serialport_close( portid );
  connected = false;
}

void TKVArduinoTrigger::setVetoMode() {
  vetomode=true;
  if ( state==0 )
    setOFFstate();
  else
    setONstate();
}

void TKVArduinoTrigger::setTriggerMode() {
  vetomode=false;
  if ( state==0 )
    setOFFstate();
  else
    setONstate();
}

bool TKVArduinoTrigger::isVetoMode() {
  return vetomode;
}
