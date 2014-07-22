/*
 * Class provides interface to scope
 */

#ifndef __TKVScope__
#define __TKVScope__

#include <string>
#include "vxi11_user.h"

#define BUF_LEN 200000
#define DATA_BUF_LEN 10000000
#define MAX_CHANNELS 4

class TKVTekChannelSettings;       // vertical scale settings per channel
class TKVTekHorizontalSettings;    // information about time axis
class TKVFastFrameSettings;        // information for fast frame acquisition mode
class TKVDataSettings;             // information on the format the waveforms received
class TKVWaveformBuffer;           // class parses/stores data taken from scope buffer
class TKVWaveformBufferCollection; // container class for waveform buffer objects. hashed using channl number.

class TKVScope {
  friend class TKVScopeManager;

protected:
  // Only the TKVScopeManager is allowed to make new instances
  TKVScope( std::string ipaddress );
  ~TKVScope();

public:

  bool isOK();
  bool isOpen();
  void setChannelToRecord( int ch, bool record=true );
  void idn();
  void readChannelSettings(int ch);
  void readHorizontalSettings();
  void readFastFrameSettings();
  void readDataSettings();
  void writeFastFrameSettings();
  void writeDataSettings();
  void acquireOneTrigger( int nsamples );
  void acquireFastFrame( int nsamples, int nframes );
  int collectWaveforms();
  void acqBinary() { fBinaryMode = true; }; // default
  void acqASCII() { fBinaryMode = false; };// not recommended

  TKVTekChannelSettings* getChannelSettings(int ch);
  TKVTekHorizontalSettings* getHorizontalSettings() { return m_horizontalSettings; };
  TKVFastFrameSettings* getFastFrameSettings() { return m_fastframeSettings; };
  TKVDataSettings* getDataSettings() { return m_dataSettings; };
  TKVWaveformBufferCollection* getChannelBuffers() { return m_channelBuffers; };

protected:

  CLINK* open_device( std::string ipaddress );
  CLINK* get_clink();
  int query( std::string string, char* buff, long buflen=BUF_LEN, int pause=0 );
  int sendcmd( std::string command );
  void waitforscope();
  void close_device();

  std::string m_ipaddress;
  int m_scopenum;
  int m_iid;
  std::string m_nickname;
  CLINK* m_clink;
  bool fBinaryMode;

  TKVTekChannelSettings* m_channelSettings[MAX_CHANNELS];
  TKVTekHorizontalSettings* m_horizontalSettings;
  TKVFastFrameSettings* m_fastframeSettings;
  TKVDataSettings* m_dataSettings;
  TKVWaveformBufferCollection* m_channelBuffers;

  enum { kError=-1, kOpenOK, kClosedOK };
  int fStatus;

private:
  static int sNumInstances;

};


#endif
