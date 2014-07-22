#ifndef __TKVDataSettings__
#define __TKVDataSettings__

#include <string>

class TKVScope;

class TKVDataSettings {
  friend class TKVScope;
  friend class TKVWaveformBuffer;

public:
  TKVDataSettings();
  ~TKVDataSettings();

public:
  void print();

protected:
  void updateParametersFromBuffer( char* input );

  short bytesperword;
  double voltsperadc;
  double secspertdc;
  int adcoffset;
  int framesinbuffer;
  int nsamples;
  int nframes;
  std::string mode;


};

#endif
