#include "TKVWaveformTree.hh"

#include "TKVWaveformBuffer.hh"
#include "TKVWaveformBufferCollection.hh"
#ifdef ROOTENABLED
#include "TTree.h"
#include "TFile.h"
#endif
#include <iostream>
#include <assert.h>

TKVWaveformTree::TKVWaveformTree( std::string filename ) 
  : TKVVWaveformOutput( filename ) {

  waveforms_array = NULL;
  activechannels = NULL;
  samples_per_waveform = 1;
  fMode = UNSPECIFIED;

#ifdef ROOTENABLED
  waveforminfo = NULL;
  waveformdata = NULL;
  m_outfile = NULL;
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
  delete m_outfile;
}


void TKVWaveformTree::setupTrees( TKVWaveformBufferCollection* waveforms ) {

  if ( fMode!=WRITE ) {
    std::cout << "Need to setup this instance to be in write mode using setupForOutput()" << std::endl;
    assert(false);
    return;
  }

  char branchinfo[100];
  char branchname[100];
  
#ifdef ROOTENABLED
  // create file
  m_outfile = new TFile( getfilename().c_str(), "NEW" );
  if ( !m_outfile )
    assert(false);

  // Setup in File's directory
  m_outfile->cd();

  // Setup Variables
  numchannels = waveforms->numBuffers();
  waveforms_array = new double*[numchannels];
  activechannels = new int[numchannels];
  secs_per_sample = new double[numchannels];

  // Setup information tree
  waveforminfo = new TTree( "waveforminfo", "Info about waveforms stored in file" );
  waveforminfo->Branch( "numchannels", &numchannels, "numchannels/I" );
  sprintf( branchinfo, "activechannels[%d]/I", numchannels );
  waveforminfo->Branch( "activechannels", activechannels, branchinfo );
  waveforminfo->Branch( "samples_per_waveform", &samples_per_waveform, "samples_per_waveform/I" );
  sprintf( branchinfo, "secs_per_sample[%d]/D", numchannels );
  waveforminfo->Branch( "secs_per_sample", secs_per_sample, branchinfo );

  // Get channel information
  bool atleastone = false;
  for (int i=0; i<numchannels; i++) {
    TKVWaveformBuffer* ch_wfm = waveforms->getChannelBuffer(i);
    if ( ch_wfm->isallocated() ) {
      atleastone = true;
      activechannels[i] = 1;
      samples_per_waveform = ch_wfm->nsamples;
      secs_per_sample[i] = ch_wfm->secs_per_sample;
    }
    else {
      activechannels[i] = 0;
    }
  }
  waveforminfo->Fill();
  
  // Setup waveforms tree
  waveformdata = new TTree( "waveformdata", "Waveforms" );

//   sprintf( branchinfo, "secs_per_sample[%d]/D", numchannels );
//   sprintf( branchname, "secs_per_sample");
//   waveformdata->Branch( branchname, secs_per_sample, branchinfo );
  for (int i=0; i<numchannels; i++ ) {

    waveforms_array[i] = new double[ samples_per_waveform ];
    memset( waveforms_array[i], 0, sizeof(double)*samples_per_waveform );

    sprintf( branchname, "ch%dwfms",i+1 );
    sprintf( branchinfo, "ch%dwfms[%d]/D", i+1, samples_per_waveform );
    waveformdata->Branch( branchname, waveforms_array[i], branchinfo );
    
  }
  fMode = WRITE;

#else
  std::cout << "ROOT not enabled. This class, TKVWaveformTree, does nothing." << std::endl;
#endif
}



int TKVWaveformTree::appendWaveforms( TKVWaveformBufferCollection* waveforms ) {
#ifdef ROOTENABLED
  if ( !waveformdata || !waveforminfo )
    setupTrees( waveforms );

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

void TKVWaveformTree::saveWaveforms() {
  m_outfile->cd();
  waveformdata->Write();
  waveforminfo->Write();
}

void TKVWaveformTree::setupForOutput() {
  if ( fMode!=UNSPECIFIED ) {
    std::cout << "Already specified mode for this instance!" << std::endl;
    return;
  }
  fMode = WRITE;
}

void TKVWaveformTree::setupForInput() {
  if ( fMode!=UNSPECIFIED ) {
    std::cout << "Already specified mode for this instance!" << std::endl;
    return;
  }
  fMode = READ;
  setupInputTrees();
}

void TKVWaveformTree::setupInputTrees() {
  if ( fMode!=READ ) {
    std::cout << "TKVWaveformTree instance not declared as input/read mode!" << std::endl;
    return;
  }

#ifdef ROOTENABLED

  char branchinfo[100];
  char branchname[100];

  if ( m_outfile || waveforminfo || waveformdata ) {
    std::cout << "Already setup interface to data for " << getfilename() << std::endl;
    return;
  }

  // open file
  m_outfile = new TFile( getfilename().c_str() );

  // declare trees
  waveforminfo = (TTree*)m_outfile->Get( "waveforminfo");
  waveformdata = (TTree*)m_outfile->Get( "waveformdata" );

  // get number of channels
  waveforminfo->SetBranchAddress( "numchannels", &numchannels );
  waveforminfo->SetBranchAddress( "samples_per_waveform", &samples_per_waveform );
  waveforminfo->GetEntry(0);
  std::cout << "Reading data with " << numchannels << " channels with " << samples_per_waveform << " samples per waveform" << std::endl;

  // setup variables
  waveforms_array = new double*[numchannels];
  activechannels = new int[numchannels];
  waveforminfo->SetBranchAddress( "activechannels", activechannels );
  for (int i=0; i<numchannels; i++ ) {

    waveforms_array[i] = new double[ samples_per_waveform ];
    memset( waveforms_array[i], 0, sizeof(double)*samples_per_waveform );
    
    sprintf( branchname, "ch%dwfms",i+1 );
    sprintf( branchinfo, "ch%dwfms[%d]/D", i+1, samples_per_waveform );
    waveformdata->SetBranchAddress( branchname, waveforms_array[i] );
  }

  waveforminfo->GetEntry(0);
  waveformdata->GetEntry(0);
#endif

}

