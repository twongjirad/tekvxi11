#ifndef __TKVTekHorizontalSettings__
#define __TKVTekHorizontalSettings__

#include <string>

class TKVScope;

class TKVTekHorizontalSettings {
  friend class TKVScope;

public:

  TKVTekHorizontalSettings( char* scope_output_string );
  ~TKVTekHorizontalSettings();

  void updateParameters( char* scope_output_string );
  void print();

protected:
  
  double interpratio;
  double scale; // secs per division
  double position; // in divisions?
  double samplerate;
  unsigned long recordlength;
  std::string units;
  

};

#endif

