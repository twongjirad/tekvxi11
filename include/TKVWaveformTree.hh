#ifndef __TKVWaveformTree__
#define __TKVWaveformTree__

#include "TKVVWaveformOutput.hh"

class TKVWaveformBuffer;
class TKVWaveformBufferCollection;
#ifdef ROOTENABLED
class TTree;
class TFile;
#endif


class TKVWaveformTree : public  TKVVWaveformOutput {

public:

  TKVWaveformTree( std::string filename );
  ~TKVWaveformTree();
  int entries();

#ifdef ROOTENABLED
  TFile* m_outfile;
  TTree* waveformdata;
  TTree* waveforminfo;
#endif

  // waveforminfo
  int numchannels;
  int* activechannels;
  double* secs_per_sample;
  int samples_per_waveform;
  // waveformdata
  double** waveforms_array; // [numchannels][sample array]

public:
  virtual int appendWaveforms( TKVWaveformBufferCollection* waveforms );
  virtual void saveWaveforms();
  virtual void setupForOutput();
  virtual void setupForInput();
  
protected:
  typedef enum { UNSPECIFIED, WRITE, READ } IOMode_t;
  IOMode_t fMode;
  void setupTrees( TKVWaveformBufferCollection* waveforms );
  void setupInputTrees();
  
};

#endif
