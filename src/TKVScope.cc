#include "TKVScope.hh"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <time.h>
//#include <thread>
//#include <chrono>
#include <unistd.h> // for sleep function
#include <assert.h>

#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVDataSettings.hh"
#include "TKVWaveformBuffer.hh"
#include "TKVWaveformTree.hh"
#include "TKVRootDisplay.hh"

#ifdef ROOTENABLED
#include "TFile.h"
#endif

int TKVScope::sNumInstances = 0;

TKVScope::TKVScope( std::string ipaddress ) {
  m_ipaddress = ipaddress;
  m_scopenum = 0;
  m_iid = sNumInstances;
  sNumInstances++;
  fStatus = kClosedOK;
  std::stringstream ssn;
  ssn << "TKVScope_" << ipaddress << "_iid" << m_iid;
  m_nickname = ssn.str();
  m_clink = open_device( ipaddress );

  
  memset( m_channelSettings, 0, MAX_CHANNELS );
  m_horizontalSettings = NULL;
  m_fastframeSettings = NULL;
}

TKVScope::~TKVScope() {
  
}

CLINK* TKVScope::open_device( std::string ipaddress ) {
  CLINK* clnt = new CLINK;
  std::cout << "Attempting to open device at " << ipaddress << std::endl;
  int ret = vxi11_open_device( ipaddress.c_str(), clnt );
  if ( ret!=0 ) {
    std::cout << "Error: could not open device at " << ipaddress << std::endl;
    fStatus = kError;
    clnt = NULL;
  }

  fStatus = kOpenOK;
  return clnt;
}

CLINK* TKVScope::get_clink() {
  return m_clink;
}


bool TKVScope::isOK() {
  if ( fStatus==kOpenOK || fStatus==kClosedOK )
    return true;
  return false;
}

bool TKVScope::isOpen() {
  if ( fStatus==kOpenOK )
    return true;
  return false;
}

/* ------------------------------------------
   GET/SET
   ------------------------------------------ */
TKVTekChannelSettings* TKVScope::getChannelSettings(int ch) {
  if ( ch<=0 || ch>MAX_CHANNELS ) {
    std::cout << "Invalid channel number: range=[1," << MAX_CHANNELS << "]" << std::endl;
    return NULL;
  }
  return m_channelSettings[ch-1];
}

void TKVScope::setChannelToRecord( int ch, bool record ) {
  getChannelSettings( ch )->setToRecord( record );
}

/* ------------------------------------------
   SCOPE INTERFACE COMMANDS
   ------------------------------------------ */

int TKVScope::query( std::string command, char* buf, long buflen ) {
  int ret = vxi11_send( m_clink, command.c_str() );
  if ( ret<0 ) {
    std::cout << "Error sending " << command << std::endl;
    return -1;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, buflen );
  if ( bytes_ret>0 ) {
    return 0;
  }
  else if ( bytes_ret==-15 ) {
    std::cout << "Sent " << command << " *** [ NO RESPONSE ] ***" << std::endl;
    return -1;
  }
  else {
    std::cout << "Sent " << command << " *** [ ERROR ] ***" << std::endl;
    return -1;
  }

  return 0;
}

int TKVScope::sendcmd( std::string command ) {
  int ret = vxi11_send( m_clink, command.c_str() );
  if ( ret<0 ) {
    std::cout << "Error sending " << command << std::endl;
    return -1;
  }  
  return 0;
}

void TKVScope::waitforscope() {
  int err;
  clock_t init, final;
  init = clock();
  double sec_elapsed = 0;
  bool acqdone = false;
  char buf[BUF_LEN];
  while ( acqdone==false && sec_elapsed<3600 ) {
    //std::this_thread::sleep_for( std::chrono::seconds(1.0) );
    //sleep(1);
    usleep( 250000 ); // 250 milliseconds
    err = query( "*OPC?", buf );
    if ( err!=0 ) assert(false);
    final = clock()-init;
    sec_elapsed = (double)final/(double)CLOCKS_PER_SEC;
    //std::cout << "*OPC? check after " << sec_elapsed << ": " << buf << std::endl;
    if ( std::atoi(buf)==1 ) {
      acqdone=true;
    }
  }
}

void TKVScope::idn() {
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  int err = query( "*IDN?", buf );
  std::cout << buf << std::endl;
  return ;
}

void TKVScope::readChannelSettings(int ch) {
  if ( !isOpen() ) 
    return;

  if ( ch<0 || ch>MAX_CHANNELS ) {
    std::cout << "Invalid channel number: range=[1," << MAX_CHANNELS << "]" << std::endl;
    return;
  }

  int err;
  char cmd[256];
  char zch[10];
  char buf[BUF_LEN];
  memset(cmd,0,256);
  memset(buf,0,BUF_LEN);
  sprintf( zch, "CH%d", ch );
  std::string strch = zch;
  
  // bandwidth
  TKVTekChannelSettings* chdata = new TKVTekChannelSettings( ch );
  err = query( strch+":BAN?", buf );
  chdata->bandwidth = std::atof( buf );

  // coupling
  err = query( strch+":COUP?", buf );
  if ( strcmp( buf , "DC" )==0 )
    chdata->coupling = TKVTekChannelSettings::DC;
  else
    chdata->coupling = TKVTekChannelSettings::AC;

  // offset
  err = query( strch+":OFFS?", buf );
  chdata->offset = std::atof( buf );
  
  // position
  err = query( strch+":POS?", buf );
  chdata->position = std::atof( buf );

  // scale
  err = query( strch+":SCA?", buf );
  chdata->scale = std::atof( buf );

  // termination
  err = query( strch+":TER?", buf );
  chdata->termination = std::atof( buf );

  // on
  err = query( "SELect:"+strch+"?", buf );
  chdata->state = std::atoi(buf);

  m_channelSettings[ch-1] = chdata;
  //   else
  //     m_channelSettings[ch-1]->updateParameters( buf );
  // }
  // else if ( bytes_ret==-15 ) 
  //   std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
  return ;  
}

void TKVScope::readHorizontalSettings() {
  if ( !isOpen() ) 
    return;

  char cmd[256];
  memset(cmd,0,256);
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  sprintf( cmd, "HOR:MAI?");
  int ret = vxi11_send( m_clink, cmd );
  if ( ret<0 ) {
    std::cout << "Error sending: " << cmd << std::endl;
    return;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
  if ( bytes_ret>0 ) {
    if ( m_horizontalSettings==NULL )
      m_horizontalSettings = new TKVTekHorizontalSettings( buf );
    else
      m_horizontalSettings->updateParameters( buf );
  }
  else if ( bytes_ret==-15 )  {
    std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
    return;
  }

  // also get record length
  sprintf( cmd, "HOR:RESO?");
  ret = vxi11_send( m_clink, cmd );
  if ( ret<0 ) {
    std::cout << "Error sending: " << cmd << std::endl;
    return;
  }
  bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
  if ( bytes_ret>0 ) {
    m_horizontalSettings->recordlength = std::atof( buf );
  }
  else if ( bytes_ret==-15 )  {
    std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
    m_horizontalSettings->recordlength = -1;
  }
  return;  
}

void TKVScope::readFastFrameSettings() {
  if ( !isOpen() ) 
    return;

  char cmd[256];
  memset(cmd,0,256);
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  sprintf( cmd, "HOR:FAST?");
  int ret = vxi11_send( m_clink, cmd );
  if ( ret<0 ) {
    std::cout << "Error sending: " << cmd << std::endl;
    return;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
  if ( bytes_ret>0 ) {
    if ( m_fastframeSettings==NULL )
      m_fastframeSettings = new TKVFastFrameSettings( buf );
    else
      m_fastframeSettings->updateParameters( buf );
  }
  else if ( bytes_ret==-15 ) 
    std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
  return ;  
}


void TKVScope::readDataSettings() {
  if ( !isOpen() ) 
    return;

  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  int err;

  bool success = true;
  TKVDataSettings* data = new TKVDataSettings();

  // Volts per ADC
  err = query( "WFMPre:YMUlt?", buf );
  if ( err<0 ) {
    delete data;
    return;
  }
  else {
    data->voltsperadc = std::atof( buf );
  }

  // ADC Offset
  memset(buf,0,BUF_LEN);
  err = query("WFMPre:YOFf?",buf);
  if ( err<0 ) {
    delete data;
    return;
  }
  else {
    data->adcoffset = std::atoi( buf );
  }

  // Bytes per word (or point)
  memset(buf,0,BUF_LEN);
  err = query("WFMPre:BYT_Nr?",buf);
  if ( err<0 ) {
    delete data;
    return;
  }
  else {
    data->bytesperword = std::atoi( buf );
  }

  // Bytes per word (or point)
  memset(buf,0,BUF_LEN);
  err = query("WFMPre:NR_Pt?",buf);
  if ( err<0 ) {
    delete data;
    return;
  }
  else {
    data->nsamples = std::atoi( buf );
  }

  // Data mode
  memset(buf,0,BUF_LEN);
  err = query("DATa:ENCdg?",buf);
  if ( err<0 ) {
    delete data;
    return;
  }
  else {
    char encode[50];
    sscanf( buf, "%s", encode );
    data->mode = std::string(encode);
  }  

  // Secs per Sample
  memset(buf,0,BUF_LEN);
  err = query("WFMOutpre:XINcr?",buf);
  if ( err<0 ) {
    delete data;
    return;
  }
  else {
    data->secspertdc = std::atof( buf );
  } 

  // frames in the data block
  memset(buf,0,BUF_LEN);
  err = query("WFMOutpre:NR_FR?",buf);
  if ( err<0) {
    delete data;
    return;
  }
  else
    data->framesinbuffer = std::atoi(buf);

  m_dataSettings = data;
}

void TKVScope::writeFastFrameSettings() {
  if ( !isOpen() ) 
    return;
  
  if ( m_fastframeSettings==NULL ) {
    std::cout << "fast frame settings haven't been created." << std::endl;
    return;
  }

  int err;
  char command[50];
  char buf[BUF_LEN];

  // Number of samples
  sprintf( command, "HOR:FAST:LEN %d", m_fastframeSettings->framelength );
  err = sendcmd( command );
  
  // Number of fast frames
  sprintf( command, "HOR:FAST:COUN %d", m_fastframeSettings->numframes );
  err = sendcmd( command );
  
  return ;  
}

/* ---------------------------------------------
   Acquisition Routines
   ------------------------------------------- */

void TKVScope::acquireOneTrigger() {
  int err = 0;
  // Turn off Fast Frame
  err = sendcmd( "HOR:FAST:STATE 0" );
  if ( err!=0 ) assert(false);

  // Set encoding
  err = sendcmd( "DATa:ENCg RIBinary" );
  if ( err!=0 ) assert(false);

  // Tell scope to enter into single sequence mode
  err = sendcmd( "ACQuire:STOPAfter SEQuence" );
  if ( err!=0 ) assert(false);

  // Tell scope to acquire
  std::cout << "Staring single acquisition" << std::endl;
  err = sendcmd( "ACQ:STATE RUN" );
  if ( err!=0 ) assert(false);

  clock_t init, final;
  init = clock();
  double sec_elapsed = 0;
  bool acqdone = false;
  char buf[BUF_LEN];
  while ( acqdone==false && sec_elapsed<3600 ) {
    //std::this_thread::sleep_for( std::chrono::seconds(1.0) );
    //sleep(1);
    usleep( 250000 ); // 250 milliseconds
    err = query( "*OPC?", buf );
    if ( err!=0 ) assert(false);
    final = clock()-init;
    sec_elapsed = (double)final/(double)CLOCKS_PER_SEC;
    std::cout << "*OPC? check after " << sec_elapsed << ": " << buf << std::endl;
    if ( std::atoi(buf)==1 ) {
      acqdone=true;
    }
  }

  // Sync the scope settings
  readDataSettings();
  readFastFrameSettings();
  
  // Grab the traces
  char* databuffer = new char[DATA_BUF_LEN];
  TKVWaveformBuffer** waveforms = new TKVWaveformBuffer*[MAX_CHANNELS];
  for (int ch=0; ch<MAX_CHANNELS; ch++) {
    if ( m_channelSettings[ch]->record ) {
      waveforms[ch] = new TKVWaveformBuffer();
      char command[50];
      sprintf(command, "DATa:SOUrce CH%d", ch+1 );
      err = sendcmd( command );

      err = query("CURVe?", databuffer, DATA_BUF_LEN );
      waitforscope();

      waveforms[ch]->extractFromBuffer( databuffer, m_fastframeSettings, m_dataSettings );
    }
    else
      waveforms[ch] = NULL;
  }

#ifdef ROOTENABLED
  // Convert raw data to ROOT TTree format
  TFile* out = new TFile("testone.root", "RECREATE" );
  TKVWaveformTree* wfmtree = new TKVWaveformTree( waveforms, MAX_CHANNELS ); 
  std::cout << "Stored " << wfmtree->entries() << " waveforms per channel" << std::endl;
  TKVRootDisplay* canvas = new TKVRootDisplay();
  canvas->display( wfmtree, 0 );
  out->Write();
  delete out;
  //delete wfmtree;
#endif

}

void TKVScope::acquireFastFrame( int nsamples, int nframes ) {

  if ( !m_fastframeSettings )
    readFastFrameSettings();

  int err;
 
  m_fastframeSettings->activated = true;
  m_fastframeSettings->numframes = nframes;
  m_fastframeSettings->framelength = nsamples;

  // push settings onto scope
  writeFastFrameSettings();

  // Set Fast Frame to be ON
  err = sendcmd( "HOR:FAST:STATE ON" );

  // Set encoding
  err = sendcmd( "DATa:ENCg RIBinary" );
  if ( err!=0 ) assert(false);

  // Tell scope to enter into single sequence mode
  err = sendcmd( "ACQuire:STOPAfter SEQuence" );
  if ( err!=0 ) assert(false);

  // Acquire!
  std::cout << "Staring single acquisition" << std::endl;
  err = sendcmd( "ACQ:STATE RUN" );
  if ( err!=0 ) assert(false);

  waitforscope();

  // Sync the scope settings
  readDataSettings();
  readFastFrameSettings();

    // Grab the traces
  char* databuffer = new char[DATA_BUF_LEN];
  TKVWaveformBuffer** waveforms = new TKVWaveformBuffer*[MAX_CHANNELS];
  for (int ch=0; ch<MAX_CHANNELS; ch++) {
    if ( m_channelSettings[ch]->record ) {
      waveforms[ch] = new TKVWaveformBuffer();
      char command[50];
      sprintf(command, "DATa:SOUrce CH%d", ch+1 );
      err = sendcmd( command );

      err = query("CURVe?", databuffer );
      waitforscope();

      waveforms[ch]->extractFromBuffer( databuffer, m_fastframeSettings, m_dataSettings );
    }
    else
      waveforms[ch] = NULL;
  }

#ifdef ROOTENABLED
  // Convert raw data to ROOT TTree format
  TFile* out = new TFile("testsinglefastframe.root", "RECREATE" );
  TKVWaveformTree* wfmtree = new TKVWaveformTree( waveforms, MAX_CHANNELS ); 
  std::cout << "Stored " << wfmtree->entries() << " waveforms per channel" << std::endl;
  TKVRootDisplay* canvas = new TKVRootDisplay();
  canvas->display( wfmtree, 0 );
  out->Write();
  delete out;
  //delete wfmtree;
#endif

}


