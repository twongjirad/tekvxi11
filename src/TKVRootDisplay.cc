#include "TKVRootDisplay.hh"

#include <iostream>
#include <cstdlib>
#ifdef ROOTENABLED
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#endif
#include "TKVWaveformTree.hh"

TKVRootDisplay::TKVRootDisplay( std::string label ) {
  lastwfm = -1;
  nchannels = -1;
  m_label = label;
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

bool TKVRootDisplay::isnumber( std::string str ) {
  std::string::iterator it;
  for ( it=str.begin(); it!=str.end(); it++ ) {
    std::cout << *it << ": " << std::isdigit(*it ) << std::endl;
    if ( !std::isdigit( *it ) )
      return false;
  }
  return true;
}

void TKVRootDisplay::display( TKVWaveformTree* wfmdata, int wfm_num ) {
  if ( wfm_num==-1 ) {
    lastwfm++;
  }
  else
    lastwfm = wfm_num;

#ifdef ROOTENABLED


  nchannels=wfmdata->numchannels;
  std::cout << "displaying " << nchannels << " nchannels" << std::endl;
  delete [] wfms;
  wfms = new TGraph[nchannels];
  
  if ( !canvas ) {
    std::string canvname = "cWaveforms";
    if ( m_label!="" )
      canvname = "cWaveforms_"+m_label;
    canvas = new TCanvas(canvname.c_str(),canvname.c_str(),600,200*nchannels);
    canvas->Divide(1,nchannels);
  }
  canvas->Draw();

  bool quitdisplay = false;

  while ( !quitdisplay ) {

    if ( lastwfm<0 )
      lastwfm = 0;
    if ( lastwfm>=wfmdata->waveformdata->GetEntries() )
      lastwfm = wfmdata->waveformdata->GetEntries()-1;
    wfmdata->waveformdata->GetEntry( lastwfm );
    int ngraphs = 0;
    for (int ich=0; ich<nchannels; ich++) {
      canvas->cd(ich+1);
      if ( wfmdata->activechannels[ich]==1 ) {
	ngraphs++;
	wfms[ich].Set( wfmdata->samples_per_waveform );
	char wfmname[20];
	sprintf(wfmname,"CH%d: WFM ##%d", ich+1, lastwfm );
	wfms[ich].SetTitle( wfmname );
	for (int i=0; i<wfmdata->samples_per_waveform; i++) {
	  wfms[ich].SetPoint( i, i, wfmdata->waveforms_array[ich][i] );
	} 
	wfms[ich].Draw("ALP");
      }
    }
    canvas->Update();

    std::string response;
    bool validresponse;
    do {
      validresponse = true;
      response = "";
      std::cout << "Diplaying waveform #" << lastwfm 
		<< " of " << wfmdata->waveformdata->GetEntries()-1 
		<< " [" << wfmdata->waveformdata->GetEntries() << " total]" << std::endl;
      std::cout << "What next?"  << std::endl;
      std::cout << "  [q] quit display :(" << std::endl;
      std::cout << "  [n] next waveform(s)" << std::endl;
      std::cout << "  [p] previous waveform(s)" << std::endl;
      std::cout << "  [#] enter number to go to waveform" << std::endl;
      std::cin >> response;
      //usleep(10000);
      response = response.substr( 0, response.find(' \n'));
      //response = "n";
      //if(lastwfm==1) lastwfm = 39199;
      if ( response!="q" &&
	   response!="n" &&
	   response!="p" &&
	   !isnumber( response ) ) {
	validresponse = false;
	std::cout << "Not a valid response." << std::endl;
      }
    } while (!validresponse);
    if ( response=="q" ) {
      quitdisplay = true;
      std::cout << "quitting display" << std::endl;
    }
    else if ( response=="n" ) {
      lastwfm++;
      quitdisplay = false;
    }
    else if ( response=="p" ) {
      lastwfm--;
      quitdisplay = false;
    }
    else {
      lastwfm=std::atoi( response.c_str() );
      quitdisplay = false;
    }
  }// end of quit display loop
#endif
}

void TKVRootDisplay::display( std::vector< TKVWaveformTree* >& wfmdata, int wfm_num ) {
  if ( wfm_num==-1 ) {
    lastwfm++;
  }
  else
    lastwfm = wfm_num;

#ifdef ROOTENABLED

  int numtrees = wfmdata.size();
  
  // count channels; make map of channel to data element
  std::map< int, int > treemap;
  std::map< int, int > chmap;
  nchannels = 0;
  for (int i=0; i<numtrees; i++) {
    for (int ch=nchannels; ch<nchannels+wfmdata.at(i)->numchannels; ch++) {
      treemap[ch] = i;
      chmap[ch] = ch-nchannels;
    }
    nchannels+=wfmdata.at(i)->numchannels;
  }
  
  int chpertree = nchannels/numtrees;
  if ( nchannels%numtrees!=0 )
    chpertree++;

  std::cout << "displaying " << nchannels << " nchannels" << std::endl;
  delete [] wfms;
  wfms = new TGraph[nchannels];
  
  if ( !canvas ) {
    std::string canvname = "cWaveforms";
    if ( m_label!="" )
      canvname = "cWaveforms_"+m_label;
    canvas = new TCanvas(canvname.c_str(),canvname.c_str(),600*numtrees,200*chpertree);
    canvas->Divide(numtrees,chpertree);
  }
  canvas->Draw();

  bool quitdisplay = false;

  while ( !quitdisplay ) {

    // keep wfm index in bounds
    if ( lastwfm<0 )
      lastwfm = 0;
    // get entry
    for (int i=0; i<numtrees; i++) {
      if ( lastwfm< wfmdata.at(i)->waveformdata->GetEntries() )
	wfmdata.at(i)->waveformdata->GetEntry( lastwfm );
      else
	wfmdata.at(i)->waveformdata->GetEntry( wfmdata.at(i)->waveformdata->GetEntries()-1 );
    }

    int ngraphs = 0;
    for (int ich=0; ich<nchannels; ich++) {
      canvas->cd(ich+1);
      if ( wfmdata.at( treemap[ich] )->activechannels[chmap[ich]]==1 ) {
	ngraphs++;
	wfms[ich].Set( wfmdata[treemap[ich]]->samples_per_waveform );
	char wfmname[20];
	sprintf(wfmname,"Tree %d CH%d: WFM ##%d", treemap[ich], chmap[ich]+1, lastwfm );
	wfms[ich].SetTitle( wfmname );
	for (int i=0; i<wfmdata.at(treemap[ich])->samples_per_waveform; i++) {
	  wfms[ich].SetPoint( i, i, wfmdata.at(treemap[ich])->waveforms_array[chmap[ich]][i] );
	} 
	wfms[ich].Draw("ALP");
      }
    }
    canvas->Update();

    std::string response;
    bool validresponse;
    do {
      validresponse = true;
      response = "";
      std::cout << "Diplaying waveform #" << lastwfm 
		<< " of " << wfmdata.at(0)->waveformdata->GetEntries()-1 
		<< " [" << wfmdata.at(0)->waveformdata->GetEntries() << " total]" << std::endl;
      std::cout << "What next?"  << std::endl;
      std::cout << "  [q] quit display :(" << std::endl;
      std::cout << "  [n] next waveform(s)" << std::endl;
      std::cout << "  [p] previous waveform(s)" << std::endl;
      std::cout << "  [#] enter number to go to waveform" << std::endl;
      std::cin >> response;
      //usleep(10000);
      response = response.substr( 0, response.find(' \n'));
      //response = "n";
      //if(lastwfm==1) lastwfm = 39199;
      if ( response!="q" &&
	   response!="n" &&
	   response!="p" &&
	   !isnumber( response ) ) {
	validresponse = false;
	std::cout << "Not a valid response." << std::endl;
      }
    } while (!validresponse);
    if ( response=="q" ) {
      quitdisplay = true;
      std::cout << "quitting display" << std::endl;
    }
    else if ( response=="n" ) {
      lastwfm++;
      quitdisplay = false;
    }
    else if ( response=="p" ) {
      lastwfm--;
      quitdisplay = false;
    }
    else {
      lastwfm=std::atoi( response.c_str() );
      quitdisplay = false;
    }
  }// end of quit display loop
#endif
}
