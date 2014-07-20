#ifndef __TKVTekChannelSettings__
#define __TKVTekChannelSettings__

#include <string>

class TKVScope;

class TKVTekChannelSettings {
  friend class TKVScope;

public:
  TKVTekChannelSettings( int chnumber, char* scope_output_string );
  ~TKVTekChannelSettings();

  typedef enum { DC=0, AC } coupling_t;
  void updateParameters( char* scope_output_string );
  void print();
  void setToRecord( bool dowerecord=true ) { record = dowerecord; };
  bool willRecord() { return record; };

protected:
  int chID;
  bool record;
  double bandwidth;
  coupling_t coupling;
  double offset;
  double position;
  double scale;
  double termination;
  double gain;
  std::string units;

};

#endif
