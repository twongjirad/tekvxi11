#include "TKVScopeManager.hh"
#include "TKVScope.hh"
#include <iostream>

int main( int narg, char** argv ) {

  TKVScopeManager* man = TKVScopeManager::getScopeManager();
  man->openDevice( "192.168.1.3" );
  TKVScope* tek1 = man->getScope( "192.168.1.3" );
  tek1->idn();

  tek1->getChannelSettings( 1 );
  tek1->getChannelSettings( 2 );
  tek1->getChannelSettings( 3 );
  tek1->getChannelSettings( 4 );

  tek1->getHorizontalSettings();

  std::cout << "Finished." << std::endl;
  std::cin.get();

  return 0;
}
