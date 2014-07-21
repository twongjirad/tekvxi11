#ifndef __TKVRootDisplay__
#define __TKVRootDisplay__

#include <string>
#ifdef ROOTENABLED
class TCanvas;
class TGraph;
class TKVWaveformTree;
#endif

class TKVRootDisplay {

public:

  TKVRootDisplay();
  ~TKVRootDisplay();

  int lastwfm;
  int nchannels;
  void display( TKVWaveformTree* wfmdata, int wfm_num=-1 );
  bool isnumber( std::string s );

#ifdef ROOTENABLED
  TCanvas* canvas;
  TGraph* wfms;
#endif

};

#endif
