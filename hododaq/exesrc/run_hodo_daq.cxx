#include <iostream>
#include <unistd.h>

#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVDataSettings.hh"
#include "TKVHodoTree.hh"
#include "TKVRootDisplay.hh"
#include "TKVHodoView.hh"
#include "TKVHodoInterface.hh"

#include "TApplication.h"

int main( int narg, char** argv ) {

  TApplication* app = new TApplication("app",0,0);

  // ------------------------------------------------------------------------
  // SETTINGS
  const int MAXCH = 4;
  std::string output_filenames;
  output_filenames = "output_wfms_hodo_test";
  std::string ips = "192.168.1.3";  //TDS554;
  bool record_channel[4] = {false,true,false,false};

  int ntottraces = 10;
  int nframes = 1; //LED
  int nsamples_per_trace = 1250; //LED
  //int nsamples_per_trace = 500; //SiPM
  bool use_fastframe = false; /// DONT USE FASTFRAME
  bool run_display = true;
  bool pause_display = false;
  int HODO_MAX_ATTEMPTS = 100000;
  enum { ROOTOUT=0, BINARYOUT, ASCIIOUT };
  int output_format = ROOTOUT;
  // --------------------------------------------------------------------------
  // Output
  if ( output_format!=ROOTOUT ) {
    std::cout << "Haven't implemented output formats other than ROOT TTree." << std::endl;
    return -1;
  }

  // --------------------------------------------------------------------------

  TKVScope* tek;
  TKVRootDisplay* wfmdisplay;
  TKVHodoView* hododisplay;
  TKVHodoTree* root_output;
  long tot_acquired = 0;

  std::cout << "Loading scopes.." << std::endl;
  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  man->openDevice( ips.c_str() );
  tek = man->getScope( ips.c_str());
  tek->idn();

  // configure channels
  for (int ch=1; ch<=MAXCH; ch++) {
    tek->readChannelSettings( ch );
    tek->setChannelToRecord( ch, true );
    tek->getChannelSettings(ch)->print();
  }
  tek->readHorizontalSettings();
  tek->readFastFrameSettings();
  tek->readDataSettings();

  wfmdisplay =  new TKVRootDisplay( ips );
  hododisplay = new TKVHodoView();
  root_output =  new TKVHodoTree( output_filenames+".root" );
  root_output->setupForOutput();
  if ( run_display )
    hododisplay->Draw();
  wfmdisplay->setbatchmode(true);

  std::cout << "Loading HodoScope" << std::endl;
  TKVHodoInterface& hodo = TKVHodoInterface::Hodo();
  hodo.Initialize();
  std::cout << "Pausing to let Hodoscope initialize" << std::endl;
  usleep( 1000 );
  std::cout << "OK long enough." << std::endl;

  std::cout << "Staring acquisition loop ...." << std::endl;

  bool finished = false;
  int totwaveforms = 0;
  int hodo_attempts = 0;
  while ( !finished ) {

    // acquisition loop.
    // time_t acqstart(0), acqend(0);
    // time(&acqstart);
    // time(&acqend);
    // while ( acqend-acqstart < HodoWaitTime ) {

    // arm scope
    if ( use_fastframe )
      tek->acquireFastFrame( nsamples_per_trace, nframes );
    else
      tek->acquireOneTrigger( nsamples_per_trace );

    std::cout << "Wait for hodotrigger" << std::endl;
    std::vector<HodoHit_t> hits;
    int nHits = 0;
    while ( nHits==0 && hodo_attempts<HODO_MAX_ATTEMPTS ) {
      nHits = hodo.GetOneEvent(hits);
      if ( nHits>0 ) {
	std::cout << "Received " << nHits << " hodoscope hits" << std::endl;
	hodo_attempts = 0;
      }
      else {
	usleep(100);
	//std::cout << "Received no hodoscope hits" << std::endl;
	hodo_attempts++;
	// if ( hodo_attempts>0 && hodo_attempts%10000 )
	//   std::cout << "still waiting on hodoscope hits" << std::endl;
      }
    }
    if ( hodo_attempts>=HODO_MAX_ATTEMPTS ) {
      std::cout << "Hodo Attempts Maxed out." << std::endl;
      break;
    }
    std::cout << "Collecting scope buffers ... " << std::endl;
    int nwaveforms = 0;
    // collect buffers
    tot_acquired += tek->collectWaveforms();

    // append waveform to trees
    root_output->appendWaveforms( tek->getChannelBuffers() );
    root_output->StoreHits( hits );
    
    if ( run_display ) {
      std::cout << "** Displaying Traces **" << std::endl;
      wfmdisplay->display( root_output );
      
      std::cout << "** Displaying Hodoscope hits **" << std::endl;
      hododisplay->ClearView();
      hododisplay->DrawHits( hits );
  
      if ( pause_display ) {
	std::string response;
	std::cout << "Acquired one set of hodohits/waveform buffer" << std::endl;
	std::cout << "Press [c] to continue, [q] to quit." << std::endl;
	std::cin >> response;
	if (response=="q")
	  finished = true;
      }
    }// end of display mode

    // are we done?
    finished = true;
    if ( tot_acquired<ntottraces ) {
      finished = false;
      std::cout << "\nAcquired " << tot_acquired << " waveforms so far.\n";
    }
    else {
      std::cout << "Scope on " << ips << ": collected " << tot_acquired << " of " << ntottraces << " waveforms" << std::endl;
    }

  }//end of acquisition loop
  
  std::cout << "Save the Data!" << std::endl;
  root_output->saveWaveforms();
  std::cout << "Finished. Scope on " << ips << " collected " << tot_acquired << " waveforms" << std::endl;


  return 0;
}

