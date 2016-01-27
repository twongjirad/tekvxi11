#ifndef __TKVHodoInterface__
#define __TKVHodoInterface__

// CREST Hodoscope Interfaces
#include "Overlord.h"
#include "HVController.h"
#include "TKVHodoHit.hh"

class TKVHodoInterface {
  // this should probably a singleton. Should we ever have two interfaces to the Hodoscope?

  TKVHodoInterface();
  virtual ~TKVHodoInterface();

  static TKVHodoInterface* gTheHodo;
  Overlord* olord;
  // Hodoscope interface
  int cbus[128];
  int stac[128];
  int pmt[128];
  int adc0[128];
  int adc1[128];
  int tdc[128];
  int tick[128];
  int deadtime[128];

public:

  static TKVHodoInterface& Hodo() { 
    if ( gTheHodo==NULL )
      gTheHodo = new TKVHodoInterface();
    return *gTheHodo; 
  };

  bool Initialize();
  bool BringDown();
  HodoHit_t GetHit( int ihit );
  int GetOneEvent( std::vector<HodoHit_t>& hits );


};

#endif
