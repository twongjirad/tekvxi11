#ifndef __TKVHODOTREE__
#define __TKVHODOTREE__

#include <vector>
#include <string>

#include "TKVWaveformTree.hh"
#include "TKVHodoHit.hh"


class TKVHodoTree : public TKVWaveformTree {
  // This class just appends some hodoscope variables to the output tree

public:

  TKVHodoTree( std::string filename );
  virtual ~TKVHodoTree();


  // Hodoscope variables
  std::vector<int>* _CBUS;
  std::vector<int>* _STAC;
  std::vector<int>* _PMT;
  std::vector<int>* _ADC0;
  std::vector<int>* _ADC1;
  std::vector<int>* _TDC;
  std::vector<int>* _TICK;
  std::vector<int>* _DEADTIME;
  
  std::vector<int>* _vars[8];

  void clearvariables();
  void resizevars(int nhits);
  HodoHit_t GetHit( int ihit );
  int StoreHits( std::vector<HodoHit_t>& hits );
};

#endif
