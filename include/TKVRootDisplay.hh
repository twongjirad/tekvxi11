#ifndef __TKVRootDisplay__
#define __TKVRootDisplay__

#include <string>
#include <vector>
#ifdef ROOTENABLED
class TCanvas;
class TGraph;
class TKVWaveformTree;
#endif

class TKVRootDisplay {

public:

  TKVRootDisplay( std::string label="" );
  ~TKVRootDisplay();

  int lastwfm;
  int nchannels;
  void display( TKVWaveformTree* wfmdata, int wfm_num=-1 );
  void display( std::vector< TKVWaveformTree* >& wfmdata, int wfm_num=-1 );
  bool isnumber( std::string s );
  void setbatchmode( bool set ) { m_batch_mode = set; };

  std::string m_label;
  bool m_batch_mode;

#ifdef ROOTENABLED
  TCanvas* canvas;
  TGraph* wfms;
#endif

};

#endif
