#include "TKVWaveformBuffer.hh"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "TKVDataSettings.hh"
#include "TKVFastFrameSettings.hh"

TKVWaveformBuffer::TKVWaveformBuffer() {
  adc = NULL;
  t = NULL;
  volts = NULL;
  nframes = 0;
  nsamples = 0;
  allocated = false;
}


TKVWaveformBuffer::~TKVWaveformBuffer() {
  if ( allocated ) {
    destroyMemBlock();
  }
}


void TKVWaveformBuffer::configureMemBlock( int samples, int frames ) {
  nframes = frames;
  nsamples = samples;

  adc = new double*[nframes];
  volts = new double*[nframes];
  for ( int i=0; i<nframes; i++ ) {
    adc[i] = new double[nsamples];
    volts[i] = new double[nsamples];
  }
  t = new double[nsamples];
  allocated = true;
}

void TKVWaveformBuffer::destroyMemBlock() {
  for ( int i=0; i<nframes; i++ ) {
    delete [] adc[i];
    delete [] volts[i];
  }
  delete [] adc;
  delete [] volts;
  delete [] t;
  allocated = false;
  adc = NULL;
  volts = NULL;
  t = NULL;
  nframes = 0;
  nsamples = 0;
}

void TKVWaveformBuffer::extractFromBuffer( char* buff, TKVFastFrameSettings* ff, TKVDataSettings* data ) {
  if ( data->nsamples!=nsamples || data->framesinbuffer!=nframes || !allocated) {
    configureMemBlock( data->nsamples, ff->numframes );
  }

  // Parse Data
  // format: #<x><yyy><data><newline>
  // <x> is the size of <yyy> in bytes
  // <yyy> size of <data> in bytes
  // <newline> single byte new line character
  // 64bytes!

  char px[2];
  memcpy( px, buff+1, sizeof(char) );
  px[1]='\0';
  //std::cout << "px: " << px << std::endl;
  int x = std::atoi( px );
  char py[x+1];
  memcpy(py,buff+2,sizeof(char)*x);
  py[x]='\0';
  int databytes = std::atoi(py);

  if ( data->mode!="ASCII" ) {

    short wordbytes = data->bytesperword;
    long ndelivered = databytes/(wordbytes*nsamples); // what's in the buffer might be different than what was asked for
    char tracebuffer[nsamples*wordbytes+1]; // buffer for one waveform
    char tracepoint[wordbytes+1];

    // std::cout << "Parsing data!" << std::endl;
    // std::cout << "bytes per word: " << wordbytes << std::endl;
    // std::cout << "ndelivered: " << ndelivered << std::endl;
    // std::cout << "nframes: " << nframes << std::endl;
    // std::cout << "nsamples: " << nsamples << " per trace" << std::endl;
    // std::cout << "x=" << x << std::endl;
    // std::cout << "y=" << databytes << " total bytes of data" << std::endl;
    // std::cout << "bytes in char*: " << sizeof(char*) << std::endl;
    // std::cout << "bytes in char: " << sizeof(char) << std::endl;

    std::cout << "Parsing " << ndelivered << " delivered waveform(s) ..." << std::endl;
    
    for ( long iframe=0; iframe<ndelivered; iframe++ ) {
      //std::cout << "Frame " << iframe+1 << " of " << ndelivered << std::endl;
      int framepos = nsamples*wordbytes*iframe;
      //std::cout << "Frame position: " << framepos << std::endl;
      memcpy( tracebuffer, 
	      buff+(2+x+framepos),
	      sizeof(char)*nsamples*wordbytes );
      for (int isample=0; isample<nsamples; isample++) {
	memcpy( tracepoint, tracebuffer+(wordbytes*isample), sizeof(char)*wordbytes );
	tracepoint[wordbytes]='\0';
	adc[iframe][isample] = int( *tracepoint ); // is this machine invariant?
	volts[iframe][isample] = data->voltsperadc*double(adc[iframe][isample]-data->adcoffset);
	//std::cout << "Sample [" << iframe << ":" << isample << "] " << adc[iframe][isample] << std::endl;
	//std::cout << adc[iframe][isample] << " ";
	//std::cout << volts[iframe][isample] << " ";
      }
      //std::cout << std::endl;
      //std::cin.get();
    }

  }
  else if ( data->mode=="ASCII" ) {
    assert(false);
  }
  else {
    assert(false);
  }

}
