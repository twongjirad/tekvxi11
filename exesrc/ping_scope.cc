#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVDataSettings.hh"
#include <iostream>

#include "TApplication.h"

int main( int narg, char** argv ) {

  if (narg!=2) {
    std::cout << "usage: ping_scope [ip address]" << std::endl;
    return 0;
  }

  TApplication* app = new TApplication("app",0,0);

  std::string ipaddress = argv[1];

  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  man->openDevice( ipaddress.c_str() );
  TKVScope* tek1 = man->getScope( ipaddress.c_str() );
  tek1->idn();

  std::cout << "Finished." << std::endl;

  return 0;
}
