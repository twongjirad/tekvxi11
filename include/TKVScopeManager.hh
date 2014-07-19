/*
 * Singleton container and Factory for TKVScope instances
 */
#ifndef __TKVScopeManager__
#define __TKVScopeManager__

#include <map>
#include <string>

class TKVScope;

class TKVScopeManager {
  
private:
  TKVScopeManager();
  ~TKVScopeManager();


  static TKVScopeManager* gInstance;
  std::map< int, TKVScope* > m_scopeDict;
  std::map< std::string, int > m_ipDict;

  typedef std::map< int, TKVScope* >::iterator scopeDict_iter;
  typedef std::map< std::string, int >::iterator ipDict_iter;

public:

  static TKVScopeManager* getScopeManager();
  void openDevice( std::string ipaddress );
  TKVScope* getScope( std::string ipaddress );
  TKVScope* getScope( int scopeID );

};

#endif
