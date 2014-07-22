#ifndef __TKVVWaveformWriter__
#define  __TKVVWaveformWriter__


class TKVVWaveformWriter {
  
public:

  TKVVWaveformWriter( std::string filename );
  virtual ~TKVVWaveformWriter();

  void appendWaveforms( TKVWaveformBuffer* wfms );

};

#endif 
