#include <iostream>
#include <unistd.h>

#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVDataSettings.hh"
#include "TKVArduinoTrigger.hh"

#include "TApplication.h"

int main( int narg, char** argv ) {

  TApplication* app = new TApplication("app",0,0);

  // ------------------------------------------------------------------------
  // SETTINGS
  const int NUMSCOPES = 2;
  const int MAXCH = 4;
  std::string output_filenames[NUMSCOPES];
  output_filenames[0]="output_wfms_tektds";
  output_filenames[1]="output_wfms_tekdpo";
  std::string ips[NUMSCOPES];
  ips[0] = "192.168.1.3";   //TDS5054
  ips[1] = "192.168.1.101"; //DPO5054
  bool record_channel[2][4] = { {true,false,false,false},
				{false,true,false,false} };
  int ntottraces = 100;
  int nframes = 1;
  int nsamples_per_trace = 10000;
  bool use_fastframe = true;
  bool run_display = true;
  enum { ROOTOUT=0, BINARYOUT, ASCIIOUT };
  int output_format = ROOTOUT;
  // --------------------------------------------------------------------------

  TKVScope* tek[NUMSCOPES];
  long tot_acquired[NUMSCOPES] = {0, 0};

  std::cout << "Loading scopes.." << std::endl;
  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  for (int i=0; i<NUMSCOPES; i++) {
    man->openDevice( ips[i].c_str() );
    tek[i] = man->getScope( ips[i].c_str());
    tek[i]->idn();

    for (int ch=1; ch<=MAXCH; ch++) {
      tek[i]->readChannelSettings( ch );
      tek[i]->setChannelToRecord( ch, true );
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

  }    

  std::cout << "Loading Arduino Trigger..." << std::endl;
  TKVArduinoTrigger* trig = new TKVArduinoTrigger();
  if ( !trig->isconnected() ) {
    std::cout << "Could not connect arduino. quitting." << std::endl;
    delete trig;
    return -1;
  }
  std::cout << "Set veto mode." << std::endl;
  trig->setVetoMode();

  std::cout << "Staring acquisition loop ...." << std::endl;

  bool finished = false;
  while ( !finished ) {

    // acquisition loop.

    // first arm arduino to stop triggers from entering scopes
    std::cout << "Start veto ... ";
    trig->setONstate();

    std::cout << "Arming scopes ... " << std::endl;;
    // arm scopes
    for (int i=0; i<NUMSCOPES; i++) {
      if ( use_fastframe )
    	tek[i]->acquireFastFrame( nsamples_per_trace, nframes );
    }

    // stop veto of triggers
    sleep(3);

    std::cout << "Release veto ...";
    trig->setOFFstate();


    std::cout << "Collecting buffers ... " << std::endl;
    // collect buffers
    for (int i=0; i<NUMSCOPES; i++) {
      tek[i]->collectWaveforms();
    }

    //if ( run_display ) {
      // how to do this.
    // }
    std::string response;
    std::cout << "Acquired one buffer" << std::endl;
    std::cout << "Press [Enter] to continue, [q] to quit." << std::endl;
    std::cin >> response;
    if (response=="q")
      finished = true;
  }


  trig->setTriggerMode();
  trig->setOFFstate();

  std::cout << "Finished." << std::endl;

  return 0;
}

