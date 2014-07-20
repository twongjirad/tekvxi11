#include "TKVScope.hh"
#include <sstream>
#include <iostream>
#include <cstdlib>

#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVDataSettings.hh"

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

int TKVScope::query( std::string command, char* buf ) {
  int ret = vxi11_send( m_clink, command.c_str() );
  if ( ret<0 ) {
    std::cout << "Error sending " << command << std::endl;
    return -1;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
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

void TKVScope::idn() {
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  int err = query( "*IDN?", buf );
  return ;
}

void TKVScope::readChannelSettings(int ch) {
  if ( !isOpen() ) 
    return;

  if ( ch<0 || ch>MAX_CHANNELS ) {
    std::cout << "Invalid channel number: range=[1," << MAX_CHANNELS << "]" << std::endl;
    return;
  }

  char cmd[256];
  memset(cmd,0,256);
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  sprintf( cmd, "CH%d?", ch );
  int ret = vxi11_send( m_clink, cmd );
  if ( ret<0 ) {
    std::cout << "Error sending: " << cmd << std::endl;
    return;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
  if ( bytes_ret>0 ) {
    if ( m_channelSettings[ch-1]==NULL )
      m_channelSettings[ch-1] = new TKVTekChannelSettings( ch, buf );
    else
      m_channelSettings[ch-1]->updateParameters( buf );
  }
  else if ( bytes_ret==-15 ) 
    std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
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

  m_dataSettings = data;
}


void TKVScope::acquireOneTrigger() {
  // int err = 0;
  // // Turn off Fast Frame
  // err = sendcmd( "HOR:FAST:STATE 0" );

  // // Channels that are on set at scope
}
