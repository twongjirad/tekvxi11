#ifndef __TKVArduinoTrigger__
#define __TKVArduinoTrigger__

// This class is meant to talk to the arduino 
// unit running in such a way to provide
// a TTL ON signal when passed '1' through the serial port.
// Any other character and the arduino outpus TTL OFF.

class TKVArduinoTrigger {

public:
  TKVArduinoTrigger();
  ~TKVArduinoTrigger();

  int connect();
  void disconnect();
  bool isconnected() { return connected; };
  int setONstate();
  int setOFFstate();
  void setVetoMode(); // sends 1, when OFF state asked for
  void setTriggerMode(); // sends 1, when ON state asked for
  bool isVetoMode();

protected:
  bool connected;
  int portid;
  bool vetomode;
  int state;

};

#endif
