#include "TKVWaveformTree.hh"

#include "TKVWaveformBuffer.hh"
#include "TKVWaveformBufferCollection.hh"
#ifdef ROOTENABLED
#include "TTree.h"
#endif

TKVWaveformTree::TKVWaveformTree( TKVWaveformBufferCollection* waveforms, int maxchannels ) {

  char branchinfo[100];
  char branchname[100];
  waveforms_array = NULL;
  activechannels = NULL;
  samples_per_waveform = 1;

#ifdef ROOTENABLED
  // Setup Variables
  numchannels = maxchannels;
  waveforms_array = new double*[numchannels];
  activechannels = new int[numchannels];

  // Setup information tree
  waveforminfo = new TTree( "waveforminfo", "Info about waveforms stored in file" );
  waveforminfo->Branch( "numchannels", &numchannels, "numchannels/I" );
  sprintf( branchinfo, "activechannels[%d]/I", maxchannels );
  waveforminfo->Branch( "activechannels", activechannels, branchinfo );
  waveforminfo->Branch( "samples_per_waveform", &samples_per_waveform, "samples_per_waveform/I" );

  // Get channel information
  bool atleastone = false;
  for (int i=0; i<maxchannels; i++) {
    TKVWaveformBuffer* ch_wfm = waveforms->getChannelBuffer(i);
    if ( ch_wfm->isallocated() ) {
      atleastone = true;
      activechannels[i] = 1;
      samples_per_waveform = ch_wfm->nsamples;
    }
    else {
      activechannels[i] = 0;
    }
  }
  waveforminfo->Fill();
  
  // Setup waveforms tree
  waveformdata = new TTree( "waveformdata", "Waveforms" );
  for (int i=0; i<maxchannels; i++ ) {

    waveforms_array[i] = new double[ samples_per_waveform ];
    memset( waveforms_array[i], 0, sizeof(double)*samples_per_waveform );

    sprintf( branchname, "ch%dwfms",i+1 );
    sprintf( branchinfo, "ch%dwfms[%d]/D", i+1, samples_per_waveform );
    waveformdata->Branch( branchname, waveforms_array[i], branchinfo );
  }
  // Save waveforms to tree
  appendWaveforms( waveforms );
#else
  std::cout << "Root not enabled. This class, TKVWaveformTree, does nothing." << std::endl;
#endif
}


TKVWaveformTree::~TKVWaveformTree() {
#ifdef ROOTENABLED
  delete waveforminfo;
  delete waveformdata;
  if ( waveforms_array!=NULL ) {
    for (int i=0; i<numchannels; i++) {
      delete [] waveforms_array[i];
    }
    delete [] waveforms_array;
    waveforms_array = NULL;
  }
#endif
  delete activechannels;
  
}

int TKVWaveformTree::appendWaveforms( TKVWaveformBufferCollection* waveforms ) {
#ifdef ROOTENABLED
  int iwfms = 0;
  bool morewfms = true;
  while ( morewfms ) {
    morewfms = false;

    for (int ch=0; ch<numchannels; ch++) {
      if ( activechannels[ch]==1 ) {
	TKVWaveformBuffer* ch_wfm = waveforms->getChannelBuffer(ch);
	if ( iwfms<ch_wfm->nstored )
	  memcpy( waveforms_array[ch], ch_wfm->volts[iwfms], sizeof(double)*ch_wfm->nsamples );
	if ( iwfms+1<ch_wfm->nstored )
	  morewfms = true;
      }
    }
    waveformdata->Fill();
    iwfms++;
  }
  return waveformdata->GetEntries();
#else
  return 0;
#endif
}

int TKVWaveformTree::entries() {
#ifdef ROOTENABLED
  if ( waveformdata )
    return waveformdata->GetEntries();
  else
    return 0;
#else
  return 0;
#endif
}
