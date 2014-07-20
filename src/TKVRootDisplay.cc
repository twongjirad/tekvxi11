#include "TKVRootDisplay.hh"

#include <iostream>
#ifdef ROOTENABLED
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#endif
#include "TKVWaveformTree.hh"

TKVRootDisplay::TKVRootDisplay() {
  lastwfm = -1;
  nchannels = -1;
#ifdef ROOTENABLED
  canvas = NULL;
  wfms = NULL;
#endif
}

TKVRootDisplay::~TKVRootDisplay() {
#ifdef ROOTENABLED
  delete canvas;
  delete [] wfms;
#endif
}

void TKVRootDisplay::display( TKVWaveformTree* wfmdata, int wfm_num ) {
  if ( wfm_num==-1 ) {
    lastwfm++;
  }
  else
    lastwfm = wfm_num;

#ifdef ROOTENABLED
  wfmdata->waveformdata->GetEntry( lastwfm );

  nchannels=wfmdata->numchannels;
  delete [] wfms;
  wfms = new TGraph[nchannels];
  
  if ( !canvas ) {
    canvas = new TCanvas("cWaveforms","Waveforms",800,400*nchannels);
    canvas->Divide(1,nchannels);
  }
  canvas->Draw();
  int ngraphs = 0;
  for (int ich=0; ich<nchannels; ich++) {
    canvas->cd(ich+1);
    if ( wfmdata->activechannels[ich]==1 ) {
      ngraphs++;
      wfms[ich].Set( wfmdata->samples_per_waveform );
      for (int i=0; i<wfmdata->samples_per_waveform; i++) {
	wfms[ich].SetPoint( i, i, wfmdata->waveforms_array[ich][i] );
      } 
      wfms[ich].Draw("ALP");
    }
  }
  canvas->Update();
  std::cin.get();
#endif
  
}
