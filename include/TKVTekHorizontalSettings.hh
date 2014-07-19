#ifndef __TKVTekHorizontalSettings__
#define __TKVTekHorizontalSettings__

class TKVScope;

class TKVTekHorizontalSettings {
  friend class TKVScope;

public:

  TKVTekHorizontalSettings( char* scope_output_string );
  ~TKVTekHorizontalSettings();

  void updateParameters( char* scope_output_string );

protected:
  
  

};

#endif

