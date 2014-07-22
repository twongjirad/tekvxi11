#ifndef __TKVWaveformBufferCollection__
#define __TKVWaveformBufferCollection__

#include <map>
#include <string>

class TKVWaveformBuffer;

typedef std::map< int, TKVWaveformBuffer* >::iterator TKVWaveformBufferIter;

class TKVWaveformBufferCollection {

public:
  TKVWaveformBufferCollection( std::string collection_name );
  ~TKVWaveformBufferCollection();


  int addChannelBuffer( int ch, TKVWaveformBuffer* wfms );
  TKVWaveformBuffer* getChannelBuffer( int ch );
  bool doesChannelHaveBuffer( int ch );
  std::string getCollectionName() { return m_collection_name; };
  int numBuffers() { return m_buffer_dict.size(); };
  
  TKVWaveformBufferIter begin() { return m_buffer_dict.begin(); };
  TKVWaveformBufferIter end() { return m_buffer_dict.end(); };

protected:
  std::string m_collection_name;
  std::map< int, TKVWaveformBuffer* > m_buffer_dict;

  

};

#endif
