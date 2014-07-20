#ifndef __TKVWaveform__
#define __TKVWaveform__

// Collection of waveforms converted from the raw buffer

class TKVFastFrameSettings;
class TKVDataSettings;

class TKVWaveformBuffer {

public:
  TKVWaveformBuffer();
  ~TKVWaveformBuffer();
  
  void extractFromBuffer( char* buf, TKVFastFrameSettings* ff, TKVDataSettings* data );

  int nframes;
  int nsamples;
  
  double** adc;
  double** volts;
  double* t;
  bool allocated;

  // double* getADCArray( int framenum );
  // void copyADCArray( int framenum, double* samples );
  
  // double* getVoltsArray( int framenum );
  // void copyVoltsArray( int framenum, double* samples );

  // double *getTimeArray();
  // void copyTimeArray( double* times );
  
  void configureMemBlock( int nsamples, int nframes );
  void destroyMemBlock();

};

#endif
