#include <iostream>
#include <unistd.h>

#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVDataSettings.hh"
#include "TKVArduinoTrigger.hh"
#include "TKVWaveformTree.hh"
#include "TKVRootDisplay.hh"

#include "TApplication.h"

int VERBOSE = 0; //GROSS

int main( int narg, char** argv ) {

  TApplication* app = new TApplication("app",0,0);

  // ------------------------------------------------------------------------
  // SETTINGS
  const int NUMSCOPES = 2;
  const int MAXCH = 4;
  std::string output_filenames[NUMSCOPES];
  output_filenames[0]="output_wfms_tekdpo_arduino_test";
  output_filenames[1]="output_wfms_tektds_arduino_test";
  std::string ips[NUMSCOPES];
  ips[0] = "192.168.1.101"; //DPO5054
  ips[1] = "192.168.1.3";  //TDS554
  bool use_arduino_trigger = false; // True for SiPM; False for LED
  bool record_scopes[NUMSCOPES] = {true, false }; //LED
  //bool record_scopes[NUMSCOPES] = {true, true }; //SiPM
  bool record_channel[2][4] = { {false,true,false,false},
  				{false,false,false,false} }; //LED
  //bool record_channel[2][4] = { {false,false,false,false},
  //				{true,true,true,true} }; //SiPM
  int ntottraces = 40000;
  int nframes = 1000; //LED
  //int nframes = 10; //SiPM
  int nsamples_per_trace = 1250; //LED
  //int nsamples_per_trace = 500; //SiPM
  bool use_fastframe = true;
  bool run_display = false;
  enum { ROOTOUT=0, BINARYOUT, ASCIIOUT };
  int output_format = ROOTOUT;
  // --------------------------------------------------------------------------
  // Output
  if ( output_format!=ROOTOUT ) {
    std::cout << "Haven't implemented output formats other than ROOT TTree." << std::endl;
    return -1;
  }

  // --------------------------------------------------------------------------

  TKVScope* tek[NUMSCOPES];
  TKVRootDisplay* display[NUMSCOPES];
  TKVWaveformTree* root_output[NUMSCOPES];
  long tot_acquired[NUMSCOPES] = {0, 0};

  std::cout << "Loading scopes.." << std::endl;
  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  for (int i=0; i<NUMSCOPES; i++) {
    if ( !record_scopes[i] )
      continue;
    man->openDevice( ips[i].c_str() );
    tek[i] = man->getScope( ips[i].c_str());
    tek[i]->idn();

    for (int ch=1; ch<=MAXCH; ch++) {
      tek[i]->readChannelSettings( ch );
      tek[i]->setChannelToRecord( ch, true );
      tek[i]->getChannelSettings(ch)->print();
    }
    tek[i]->readHorizontalSettings();
    tek[i]->readFastFrameSettings();
    tek[i]->readDataSettings();

    // switch ( output_format ) {
    // case BINARYOUT:
    //   tek[i]->setOutputFilename( output_filenames[i]+".bin" );
    //   tek[i]->setOutputFormatAsBinary();
    //   break;
    // case ASCIOUT:
    //   tek[i]->setOutputFilename( output_filenames[i]+".dat" );
    //   tek[i]->setOutputFormatAsASCII();
    // default:
    //   tek[i]->setOutputFilename( output_filenames[i]+".root" );
    //   tek[i]->setOutputFormatAsROOTfile();
    //   break;
    // };

    display[i] =  new TKVRootDisplay( ips[i] );
    root_output[i] =  new TKVWaveformTree( output_filenames[i]+".root" );
    root_output[i]->setupForOutput();
  }    

  std::cout << "Loading Arduino Trigger..." << std::endl;
  TKVArduinoTrigger* trig = NULL;
  if ( use_arduino_trigger )  {
    trig = new TKVArduinoTrigger();
    if ( !trig->isconnected() ) {
      std::cout << "Could not connect arduino. quitting." << std::endl;
      delete trig;
      return -1;
    }
    std::cout << "Set veto mode." << std::endl;
    //trig->setVetoMode(); 
  }

  std::cout << "Staring acquisition loop ...." << std::endl;

  bool finished = false;
  int totwaveforms = 0;

  while ( !finished ) {

    // acquisition loop.

    // first arm arduino to stop triggers from entering scopes
    if ( use_arduino_trigger ) {
      std::cout << "Start veto ... ";
      trig->setONstate();
    }

    std::cout << "Arming scopes ... " << std::endl;;
    // arm scopes
    for (int i=0; i<NUMSCOPES; i++) {
      if ( !record_scopes[i] )
	continue;

      if ( use_fastframe )
    	tek[i]->acquireFastFrame( nsamples_per_trace, nframes );
      else
	tek[i]->acquireOneTrigger( nsamples_per_trace );
    }

    // stop veto of triggers
    sleep(1);

    if ( use_arduino_trigger ) {
      std::cout << "Release veto ...";
      trig->setOFFstate();
    }


    std::cout << "Collecting buffers ... " << std::endl;
    int nwaveforms = 0;
    // collect buffers
    for (int i=0; i<NUMSCOPES; i++) {
      if ( !record_scopes[i] ) 
	continue;
       tot_acquired[i] += tek[i]->collectWaveforms();
    }


    // arm scopes
    for (int i=0; i<NUMSCOPES; i++) {
      if ( !record_scopes[i] )
	continue;

      // for (int ch=1; ch<=MAXCH; ch++)
      // 	if ( tek[i]->getChannelSettings(ch)->willRecord() )
      // 	  tek[i]->getChannelSettings(ch)->print();
      std::cout << "Settings for " << ips[i] << std::endl;
      tek[i]->getDataSettings()->print();
      tek[i]->getFastFrameSettings()->print();
    }

    // append waveform to trees
    for (int i=0; i<NUMSCOPES; i++) {
      if ( !record_scopes[i] )
	continue;
      root_output[i]->appendWaveforms( tek[i]->getChannelBuffers() );
    }
    
    if ( run_display ) {
      std::cout << "** Displaying Traces **" << std::endl;
      // how to do this?
      for (int i=0; i<NUMSCOPES; i++) {
	if ( !record_scopes[i] )
	  continue;
	//root_output[i]->appendWaveforms( tek[i]->getChannelBuffers() );
	display[i]->display( root_output[i] );
      }
  
      std::string response;
      std::cout << "Acquired one buffer" << std::endl;
      std::cout << "Press [c] to continue, [q] to quit." << std::endl;
      std::cin >> response;
      if (response=="q")
	finished = true;
    }
    else {
      // batch mode
      finished = true;
      for (int i=0; i<NUMSCOPES; i++) {
	if ( record_scopes[i] && tot_acquired[i]<ntottraces ) {
	  finished = false;
	  std::cout << "\nAcquired " << tot_acquired[i] << " waveforms so far.\n";
	}
	else {
	  std::cout << "Scope on " << ips[i] << ": collected " << tot_acquired[i] << " of " << ntottraces << " waveforms" << std::endl;
	}
      }
    }
  }//end of acquisition loop


  if ( use_arduino_trigger ) {
    trig->setTriggerMode();
    trig->setOFFstate();
  }

  for (int i=0; i<NUMSCOPES; i++) {
    if ( !record_scopes[i] )
      continue;
    root_output[i]->saveWaveforms();
    std::cout << "Finished. Scope on " << ips[i] << " collected " << tot_acquired[i] << " waveforms" << std::endl;
  }

  if ( use_arduino_trigger ) {
    trig->setOFFstate();  
    delete trig;
  }
  

  return 0;
}

