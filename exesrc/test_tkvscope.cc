#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVDataSettings.hh"
#include <iostream>

#include "TApplication.h"

int main( int narg, char** argv ) {

  TApplication* app = new TApplication("app",0,0);

  std::string ipaddress = "192.168.1.101";

  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  man->openDevice( ipaddress.c_str() );
  TKVScope* tek1 = man->getScope( ipaddress.c_str() );
  tek1->idn();

  tek1->readChannelSettings( 1 );
  tek1->readChannelSettings( 2 );
  tek1->readChannelSettings( 3 );
  tek1->readChannelSettings( 4 );
  for (int i=1; i<=4; i++)
    tek1->getChannelSettings(i)->print();

  //tek1->setChannelToRecord( 1, true );
  tek1->setChannelToRecord( 2, true );
  // tek1->setChannelToRecord( 3, true );
  // tek1->setChannelToRecord( 4, true );

  tek1->readHorizontalSettings();
  tek1->getHorizontalSettings()->print();

  tek1->readFastFrameSettings();
  tek1->getFastFrameSettings()->print();

  tek1->readDataSettings();
  tek1->getDataSettings()->print();

  tek1->acquireOneTrigger( 10000 );
  //tek1->acquireFastFrame( 10000, 20 );
  //tek1->getFastFrameSettings()->print();
  //tek1->getDataSettings()->print();

  std::cout << "Finished." << std::endl;
  std::cin.get();

  return 0;
}
