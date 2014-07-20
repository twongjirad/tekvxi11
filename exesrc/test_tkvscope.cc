#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include "TKVFastFrameSettings.hh"
#include "TKVTekChannelSettings.hh"
#include "TKVTekHorizontalSettings.hh"
#include "TKVDataSettings.hh"
#include <iostream>

int main( int narg, char** argv ) {

  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  man->openDevice( "192.168.1.3" );
  TKVScope* tek1 = man->getScope( "192.168.1.3" );
  tek1->idn();

  tek1->readChannelSettings( 1 );
  tek1->readChannelSettings( 2 );
  tek1->readChannelSettings( 3 );
  tek1->readChannelSettings( 4 );
  for (int i=1; i<=4; i++)
    tek1->getChannelSettings(i)->print();

  tek1->readHorizontalSettings();
  tek1->getHorizontalSettings()->print();

  tek1->readFastFrameSettings();
  tek1->getFastFrameSettings()->print();

  tek1->readDataSettings();
  tek1->getDataSettings()->print();
  

  std::cout << "Finished." << std::endl;
  std::cin.get();

  return 0;
}
