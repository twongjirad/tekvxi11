#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include <iostream>
#include <assert.h>

TKVScopeManager* TKVScopeManager::gInstance = NULL;

TKVScopeManager::TKVScopeManager() {
  m_scopeDict.clear();
}

TKVScopeManager::~TKVScopeManager() {
  
}

TKVScopeManager* TKVScopeManager::getScopeManager() {
  if ( gInstance==NULL )
    gInstance = new TKVScopeManager();
  return gInstance;
}

void TKVScopeManager::openDevice( std::string ipaddress ) {
  ipDict_iter it = m_ipDict.find( ipaddress );
  if ( it==m_ipDict.end() ) {
    int devid = m_scopeDict.size();
    TKVScope* scope = new TKVScope( ipaddress );
    if ( scope->isOK() && scope->isOpen() ) {
      m_scopeDict[devid] = scope;
      m_ipDict[ipaddress] = devid;
    }
    else {
      delete scope;
      return;
    }
    std::cout << "Succesfully opened scope at " << ipaddress << ". Device ID=" << devid << std::endl;
  }
  else {
    std::cout << "Error. Scope at " << ipaddress << " already opened. Doing nothing." << std::endl;
    return;
  }
}


TKVScope* TKVScopeManager::getScope( int scopeID ) {
  scopeDict_iter it = m_scopeDict.find( scopeID );
  if ( it!=m_scopeDict.end() )
    return (*it).second;
  std::cout << "Could not find scope with ID=" << scopeID << " in Manager. Returning NULL." << std::endl;
  return NULL;
}

TKVScope* TKVScopeManager::getScope( std::string ip ) {
  ipDict_iter it = m_ipDict.find( ip );
  if ( it!=m_ipDict.end() ) {
    return getScope( m_ipDict[ip] );
  }
  std::cout << "Could not find scope with ip=" << ip << " in Manager. Returning NULL" << std::endl;
  return NULL;
}
