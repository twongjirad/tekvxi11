#ifndef __TKVTekChannelSettings__
#define __TKVTekChannelSettings__

#include <string>

class TKVScope;

class TKVTekChannelSettings {
  friend class TKVScope;

public:
  TKVTekChannelSettings( int chnumber );
  ~TKVTekChannelSettings();

  typedef enum { DC=0, AC } coupling_t;
  void updateParameters( char* scope_output_string );
  void print();
  void setToRecord( bool dowerecord=true ) { record = dowerecord; };
  bool willRecord();

protected:
  int chID;
  bool record;
  double bandwidth;
  coupling_t coupling;
  double offset;
  double position;
  double scale;
  double termination;
  int state;

};

#endif
