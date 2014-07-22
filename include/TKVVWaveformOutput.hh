#ifndef __TKVVWaveformOutput__
#define  __TKVVWaveformOutput__

#include <string>
class TKVWaveformBufferCollection;

class TKVVWaveformOutput {
  
public:

  TKVVWaveformOutput( std::string filename  );
  virtual ~TKVVWaveformOutput();

  virtual int appendWaveforms( TKVWaveformBufferCollection* wfms ) = 0;
  virtual void saveWaveforms() = 0;
  std::string getfilename() { return m_filename; };

protected:

  std::string m_filename;

};

#endif 
