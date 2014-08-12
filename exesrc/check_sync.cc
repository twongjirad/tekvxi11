#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TKVWaveformTree.hh"

bool parse_args( int nargs, char** argv, std::map< std::string, std::string >& args ) {

  for (int i=1; i<nargs; i++) {
    std::string flag = argv[i];
    if ( flag=="-i1" && i+1<nargs)
      args["input1"] = std::string( argv[++i] );
    else if ( flag=="-i2" && i+1<nargs)
      args["input2"] = std::string( argv[++i] );
    else if ( flag=="-o" && i+1<nargs )
      args["output"] = std::string( argv[++i] );
    else if ( flag=="-ch1" && i+1<nargs )
      args["input1_channel"] = std::string( argv[++i] );
    else if ( flag=="-i2" && i+1<nargs)
      args["input2"] = std::string( argv[++i] );
    else if ( flag=="-o" && i+1<nargs )
      args["output"] = std::string( argv[++i] );
    else if ( flag=="-ch1" && i+1<nargs )
      args["input1_channel"] = std::string( argv[++i] );
    else if ( flag=="-ch2" && i+1<nargs )
      args["input2_channel"] = std::string( argv[++i] );
    else {
      std::cout << "unrecognized command: " << flag << std::endl;
      return false;
    }

  }

  std::string require[5] = { "input1", "input2", "input1_channel", "input2_channel", "output" };
  for (int i=0; i<5; i++) { 
    if ( args.find( require[i] )==args.end() ) {
      std::cout << "Missing required argument for " << require[i] << std::endl;
      return false;
    }
  }

  return true;
}

int main( int narg, char** argv ) {


#ifdef ROOTENABLED

  if ( narg!=4 ) {
    std::cout << "usage: check_sync -i [input1] -ch1 [input 1 test channel] -i [input2] -ch2 [input 2 test channel] -o [output]" << std::endl;
  }

  // internal params
  double tolerance = 6e-3;
  double max_chi2 = 1.1;
  int verbosity = 1;				
  bool use_adc_check = false;

  std::map< std::string, std::string > args;
  bool parseok = parse_args( narg, argv, args );

  if ( !parseok )
    return -1;

  TKVWaveformTree* input1 = new TKVWaveformTree( args["input1"] );
  input1->setupForInput();
  TKVWaveformTree* input2 = new TKVWaveformTree( args["input2"] );
  input2->setupForInput();
  int inputch1 = std::atoi( args["input1_channel"].c_str() );
  int inputch2 = std::atoi( args["input2_channel"].c_str() );
  
  TFile* output = new TFile( args["output"].c_str(), "NEW" );
  TTree* matchtree = new TTree("matchflag", "Contains results of sync test" );
  int synced;
  int shim1, shim2;
  int offset = 0;
  double chi2 = 0;
  matchtree->Branch( "synced", &synced, "synced/I" );
  matchtree->Branch( "tdcoffset", &offset, "tdcoffset/I" );
  matchtree->Branch( "shim1", &shim1, "shim1/I" );
  matchtree->Branch( "shim2", &shim2, "shim2/I" );
  matchtree->Branch( "chi2", &chi2, "chi2/D" );

  int entries1 = input1->entries();
  int entries2 = input2->entries();
  int entries = 0;

  if ( entries1!=entries2 ) {
    std::cout << "Number of entries do not match! Proceeding anyway, but be careful with results." << std::endl;
  }
  if ( entries1>entries2 )
    entries = entries1;
  else
    entries = entries2;
  
  shim1 = 0;
  shim2 = 0;
  synced = 1;

  entries = 1000;

  for (int entry=0; entry<entries; entry++) {

    unsigned long bytes1 = input1->waveformdata->GetEntry(entry);
    unsigned long bytes2 = input2->waveformdata->GetEntry(entry);
    if (bytes1==0 || bytes2==0 ) {
      std::cout << "reach end of tree for either file: bytes1=" << bytes1 << ", bytes2=" << bytes2 << std::endl;
      synced = 0;
      matchtree->Fill();
      continue;
    }

    bool matches = true;
    int push[2] = {0,-1};
    bool maxed[2] = {false, false};
    double biggest_diff = 0;
    chi2 = 0;
    do {

      for (int j=0; j<2; j++) {

	matches = true;

	// ADC-by-ADC check
	if ( use_adc_check ) {
	  for (int i=0; i<input1->samples_per_waveform; i++) {
	    int tdc1 = i+offset+push[j];
	    int tdc2 = i;
	    if ( tdc1>=input1->samples_per_waveform )
	      tdc1 = input1->samples_per_waveform-1;
	    if ( tdc1<0 )
	      tdc1 = 0;
	    double diff = std::fabs( input1->waveforms_array[inputch1][tdc1] - input2->waveforms_array[inputch2][tdc2] );
	    if ( diff>biggest_diff )
	      biggest_diff = diff;
	    if ( verbosity>1 ) {
	      std::cout << "j=" << j << ", offset=" << offset << ", push=" << push[j] << " (" << push[0]-push[1] << ") :: ";
	      std::cout << "| W1[" << inputch1 << "][" << tdc1 << "]=" << input1->waveforms_array[inputch1][tdc1] 
			<< " - W2[" << inputch2 << "][" << tdc2 << "]=" << input2->waveforms_array[inputch2][tdc2] 
			<< "| = " <<  diff
			<< std::endl;
	    }
	    if ( diff>tolerance ) {
	      matches = false;
	      biggest_diff = diff;
	      break;
	    }
	  }
	}///end of adc check
	else {
	  // CHI-SQUARED CHECK
	  double waveform1[input1->samples_per_waveform];
	  int start = offset+push[j];
	  if ( start<0 )
	    start = 0;
	  if ( start>=input1->samples_per_waveform ) {
	    start = input1->samples_per_waveform-1;
	  }
	  memset( &waveform1[0], 0, sizeof(double)*(start) );
	  memcpy( &waveform1[0]+start, input1->waveforms_array[inputch1], sizeof(double)*(input1->samples_per_waveform-start) );

	  chi2 = 0;
	  int nbins = 0;
	  for (int i=start; i<input1->samples_per_waveform; i++) {
	    double diff = std::fabs( waveform1[i] - input2->waveforms_array[inputch2][i] );
	    chi2 += diff/tolerance;
	    nbins++;
	  }
	  chi2 /= double(nbins-1);
	  if ( chi2>max_chi2 )
	    matches = false;
	  if ( verbosity>1 ) {
	    std::cout << "entry=" << entry << ": chi2=" << chi2 << " j=" << j << " start=" << start << std::endl; 
	    if ( !matches )
	      std::cin.get();
	  }
	
	}//end of chi2 check
      
	if ( verbosity>1 ) {
	  std::cout << "matches=" << matches << std::endl;
	  std::cin.get();
	}
	if ( matches ) {
	  break; // out of j loop
	}
	else {
	  if ( j==0 )
	    push[j]++;
	  else
	    push[j]--;
	}
      
	if ( use_adc_check ) {
	  if ( offset+push[j]>=input1->samples_per_waveform || offset+push[j]<0 )
	    maxed[j] = true;
	}
	else {
	  if ( abs(push[j])>-5 )
	    maxed[j] = true;
	}
      }//end of j loop
      
    } while ( !matches && (!maxed[0] || !maxed[1]) );
    
    if ( matches ) {
      synced = 1;
      if ( abs(push[0]-push[1])%2==1 )
	offset += push[0];
      else
	offset += push[1];
    }
    else {
      synced = 0;
    }
    
    matchtree->Fill();
    if ( verbosity>0 ) {
      std::cout << "entry " << entry << ": synced=" << synced << " offset=" << offset << " largest diff=" << biggest_diff << " chi2=" << chi2 << std::endl;
      if ( !matches )
	std::cin.get();
    }

  }//end of entry loop
  
  output->Write();

#else
  std::cout << "Entire program does nothing! Requires ROOT" << std::endl;
#endif

  return 0;
}
