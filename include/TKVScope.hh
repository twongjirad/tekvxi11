/*
 * Class provides interface to scope
 */

#ifndef __TKVScope__
#define __TKVScope__

#include <string>
#include "vxi11_user.h"

#define BUF_LEN 100000
#define MAX_CHANNELS 4

class TKVTekChannelSettings;    // vertical scale settings per channel
class TKVTekHorizontalSettings; // information about time axis

class TKVScope {
  friend class TKVScopeManager;

protected:
  // Only the TKVScopeManager is allowed to make new instances
  TKVScope( std::string ipaddress );
  ~TKVScope();

public:

  bool isOK();
  bool isOpen();
  void idn();
  void getChannelSettings(int ch);
  void getHorizontalSettings();

protected:

  CLINK* open_device( std::string ipaddress );
  CLINK* get_clink();

  std::string m_ipaddress;
  int m_scopenum;
  int m_iid;
  std::string m_nickname;
  CLINK* m_clink;

  TKVTekChannelSettings* m_channelSettings[MAX_CHANNELS];
  TKVTekHorizontalSettings* m_horizontalSettings;

  enum { kError=-1, kOpenOK, kClosedOK };
  int fStatus;

private:
  static int sNumInstances;

};


#endif
