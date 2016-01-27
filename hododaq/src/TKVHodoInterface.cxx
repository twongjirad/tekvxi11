#include "TKVHodoInterface.hh"

#include <iostream>

TKVHodoInterface* TKVHodoInterface::gTheHodo = NULL; 

TKVHodoInterface::TKVHodoInterface() {
  olord = NULL;
}

TKVHodoInterface::~TKVHodoInterface() {
  BringDown();
}

bool TKVHodoInterface::Initialize() {
  std::cout << "Initializing" << std::endl;
  olord = new Overlord;
  olord->EnableHK();
  olord->CBCReset();
  olord->EnableDAQ();
  return true;
}

bool TKVHodoInterface::BringDown() {
  // we just delete the overlord?
  if ( olord )
    delete olord;
  olord = NULL;
}


int TKVHodoInterface::GetOneEvent( std::vector<HodoHit_t>& hits ) {

  int nHits = 0;
  hits.clear();

  if ( olord->DataAvail() ) {
    olord->ReadBuffer(true);
    olord->ProcessEventBuffer();
    nHits = olord->getData( cbus,stac,pmt,adc0,adc1,tdc,tick,deadtime );
    // for ( int ihit=0; ihit<nHits; ihit++) {
    //   _CBUS->at(ihit) = cbus[ihit];
    //   _STAC->at(ihit) = stac[ihit];
    //   _PMT->at(ihit)  = pmt[ihit];
    //   _ADC0->at(ihit) = adc0[ihit];
    //   _ADC1->at(ihit) = adc1[ihit];
    //   _TDC->at(ihit)  = tdc[ihit];
    //   _TICK->at(ihit) = tick[ihit];
    //   _DEADTIME->at(ihit) = deadtime[ihit];
    // }
    for (int ihit=0; ihit<nHits; ihit++) {
      HodoHit_t hit;
      hit.cbus = cbus[ihit];
      hit.stac = stac[ihit];
      hit.pmt  = pmt[ihit];
      hit.adc0 = adc0[ihit];
      hit.adc1 = adc1[ihit];
      hit.tdc  = tdc[ihit];
      hit.tick = tick[ihit];
      hit.deadtime = deadtime[ihit];
      hits.push_back( hit );
    }
  }
  return nHits;
}


