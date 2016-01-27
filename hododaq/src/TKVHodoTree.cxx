#include "TKVHodoTree.hh"
#include "TTree.h"

#include <iostream>

TKVHodoTree::TKVHodoTree( std::string filename ) 
  : TKVWaveformTree( filename ) {
  fBranchesSetup = false;
}

TKVHodoTree::~TKVHodoTree() {
  for (int v=0;v<8;v++)
    delete _vars[v];
}

void TKVHodoTree::clearvariables() {
  for (int v=0; v<8; v++)
    _vars[v]->clear();
}

void TKVHodoTree::resizevars( int nhits ) {
  for (int v=0; v<8; v++)
    _vars[v]->resize(nhits,0);
}


int TKVHodoTree::StoreHits( std::vector<HodoHit_t>& hits ) {
  
  if ( !fBranchesSetup ) {
    setupHodoBranches();
  }

  usleep(100);
  int nHits = hits.size();
  resizevars(nHits);
  for ( int ihit=0; ihit<nHits; ihit++) {
    _CBUS->at(ihit) = hits.at(ihit).cbus;
    _STAC->at(ihit) = hits.at(ihit).stac;
    _PMT->at(ihit)  = hits.at(ihit).pmt;
    _ADC0->at(ihit) = hits.at(ihit).adc0;
    _ADC1->at(ihit) = hits.at(ihit).adc1;
    _TDC->at(ihit)  = hits.at(ihit).tdc;
    _TICK->at(ihit) = hits.at(ihit).tick;
    _DEADTIME->at(ihit) = hits.at(ihit).deadtime;
  }

  return nHits;
}

HodoHit_t TKVHodoTree::GetHit( int ihit ) {
  HodoHit_t hit;
  hit.cbus = _CBUS->at(ihit);
  hit.stac = _STAC->at(ihit);
  hit.pmt  = _PMT->at(ihit);
  hit.adc0 = _ADC0->at(ihit);
  hit.adc1 = _ADC1->at(ihit);
  hit.tdc  = _TDC->at(ihit);
  hit.tick = _TICK->at(ihit);
  hit.deadtime = _DEADTIME->at(ihit);
  return hit;
}

void TKVHodoTree::setupHodoBranches() {

  if ( waveformdata==NULL ) {
    std::cout << "WAVEFORM TREE NOT CREATED YET!!" << std::endl;
  }

  // append hodoscope variables to tree
  _CBUS = new std::vector<int>;
  _STAC = new std::vector<int>;
  _PMT  = new std::vector<int>;
  _ADC0 = new std::vector<int>;
  _ADC1 = new std::vector<int>;
  _TDC  = new std::vector<int>;
  _TICK = new std::vector<int>;
  _DEADTIME = new std::vector<int>;

  _vars[0] = _CBUS;
  _vars[1] = _STAC;
  _vars[2] = _PMT;
  _vars[3] = _ADC0;
  _vars[4] = _ADC1;
  _vars[5] = _TDC;
  _vars[6] = _TICK;
  _vars[7] = _DEADTIME;

  waveformdata->Branch("cbus", &(*_CBUS));
  waveformdata->Branch("stac", &(*_STAC));
  waveformdata->Branch("pmt",  &(*_PMT));
  waveformdata->Branch("adc0", &(*_ADC0));
  waveformdata->Branch("adc1", &(*_ADC1));
  waveformdata->Branch("tdc",  &(*_TDC));
  waveformdata->Branch("tick", &(*_TICK));
  waveformdata->Branch("deadtime", &(*_DEADTIME));

}
