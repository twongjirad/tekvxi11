#include <iostream>
#include <vector>
#include <map>
#include <string>

#ifdef ROOTENABLED
#include "TApplication.h"
#include "TKVWaveformTree.hh"
#include "TKVRootDisplay.hh"
#endif

int main( int narg, char** argv ) {

#ifndef ROOTENABLED
  std::cout << "This program relies on ROOT being available." << std::endl;
  return 0;
#endif
  
  if ( narg<=1 ) {
    std::cout << "usage: tkv_viewer [input root file 1] [input root file 2] ..." << std::endl;
  }

  TApplication* app = new TApplication("app",0,0);

  std::vector< std::string > inputs;
  for (int i=1; i<narg; i++) {
    inputs.push_back( argv[i] );
  }

#ifdef ROOTENABLED
  std::map< std::string, TKVWaveformTree* > inputData;
  std::vector< TKVWaveformTree* > treeData;
  for ( std::vector< std::string >::iterator it=inputs.begin(); it!=inputs.end(); it++ ) {
    TKVWaveformTree* data = new TKVWaveformTree( *it );
    data->setupForInput();
    inputData[*it] = data;
    treeData.push_back( data );
  }

  // for ( std::vector< std::string >::iterator it=inputs.begin(); it!=inputs.end(); it++ ) {
  //   std::cout << "========================================================" << std::endl;
  //   std::cout << "DISPLAYING DATA FOR " << *it << std::endl;
  //   std::cout << "--------------------------------------------------------" << std::endl;
  //   TKVRootDisplay display( *it );
  //   display.display( inputData[*it] );
  //   //display.display( inputData[*it] );
  // }

  std::cout << "========================================================" << std::endl;
  std::cout << "DISPLAYING DATA" << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;
  TKVRootDisplay display( "my waveforms" );
  display.display( treeData );

#endif

  return 0;
};
