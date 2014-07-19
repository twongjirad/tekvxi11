#include "TKVScope.hh"
#include <sstream>
#include <iostream>

#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVFastFrameSettings.hh"

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
   SCOPE INTERFACE COMMANDS
   ------------------------------------------ */

void TKVScope::idn() {
  char cmd[256];
  memset(cmd,0,256);
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  sprintf( cmd, "*IDN?" );
  int ret = vxi11_send( m_clink, cmd );
  if ( ret<0 ) {
    std::cout << "Error sending *IDN?" << std::endl;
    return;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
  if ( bytes_ret>0 )
    std::cout << std::string(buf) << std::endl;
  else if ( bytes_ret==-15 ) 
    std::cout << "Sent *IDN? *** [ NO RESPONSE ] ***" << std::endl;
  return ;
}

void TKVScope::getChannelSettings(int ch) {
  if ( !isOpen() ) 
    return;

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
    m_channelSettings[ch-1]->print();
  }
  else if ( bytes_ret==-15 ) 
    std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
  return ;  
}

void TKVScope::getHorizontalSettings() {
  if ( !isOpen() ) 
    return;

  char cmd[256];
  memset(cmd,0,256);
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  sprintf( cmd, "HOR?");
  int ret = vxi11_send( m_clink, cmd );
  if ( ret<0 ) {
    std::cout << "Error sending: " << cmd << std::endl;
    return;
  }
  long bytes_ret = vxi11_receive( m_clink, buf, BUF_LEN );
  if ( bytes_ret>0 ) {
    if ( m_channelSettings==NULL )
      m_horizontalSettings = new TKVTekHorizontalSettings( buf );
    else
      m_horizontalSettings->updateParameters( buf );
    //m_horizontalSettings->print();
  }
  else if ( bytes_ret==-15 ) 
    std::cout << "Sent " << cmd << " *** [ NO RESPONSE ] ***" << std::endl;
  return ;  
}
