#include "TKVWaveformBufferCollection.hh"
#include "TKVWaveformBuffer.hh"
#include <iostream>

TKVWaveformBufferCollection::TKVWaveformBufferCollection( std::string collection_name ) {
  m_collection_name = collection_name;
}

TKVWaveformBufferCollection::~TKVWaveformBufferCollection() {
  for ( TKVWaveformBufferIter it=begin(); it!=end(); it++ ) {
    delete (*it).second;
  }
  m_buffer_dict.clear();
}

int TKVWaveformBufferCollection::addChannelBuffer( int ch, TKVWaveformBuffer* wfm ) {
  m_buffer_dict[ch] = wfm;
  return m_buffer_dict.size();
}

TKVWaveformBuffer* TKVWaveformBufferCollection::getChannelBuffer( int ch ) {
  TKVWaveformBufferIter it = m_buffer_dict.find( ch );
  if ( it!=end() )
    return (*it).second;
  else
    return NULL;
}

bool TKVWaveformBufferCollection::doesChannelHaveBuffer( int ch ) {
  TKVWaveformBuffer* wfm = getChannelBuffer( ch );
  if ( !wfm ) 
    return false;
  else
    return true;
}
