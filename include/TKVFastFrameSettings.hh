#ifndef __TKVFastFrameSettings__
#define __TKVFastFrameSettings__

class TKVScope;

class TKVFastFrameSettings {
  friend class TKVScope;
  friend class TKVWaveformBuffer;
  
public:
  TKVFastFrameSettings();
  ~TKVFastFrameSettings();

  void updateParameters( char* scope_query_output );
  void print();

protected:

  bool activated;
  int numframes; 
  int framelength; // number of samples per frame

};

#endif
