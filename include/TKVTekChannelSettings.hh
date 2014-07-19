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

protected:
  int chID;
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
