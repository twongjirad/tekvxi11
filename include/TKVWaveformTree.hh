#ifndef __TKVWaveformTree__
#define __TKVWaveformTree__

class TKVWaveformBuffer;
class TKVWaveformBufferCollection;
#ifdef ROOTENABLED
class TTree;
#endif

class TKVWaveformTree {

public:
  TKVWaveformTree( TKVWaveformBufferCollection* waveforms, int numchannels );
  ~TKVWaveformTree();
  int entries();

#ifdef ROOTENABLED
  TTree* waveformdata;
  TTree* waveforminfo;
#endif

  // waveforminfo
  int numchannels;
  int* activechannels;
  int samples_per_waveform;

  // waveformdata
  double** waveforms_array; // [numchannels][sample array]

public:
  int appendWaveforms( TKVWaveformBufferCollection* waveforms );

};

#endif
