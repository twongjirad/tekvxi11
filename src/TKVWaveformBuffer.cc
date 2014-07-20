#include "TKVWaveformBuffer.hh"

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
  if ( data->nsamples!=nsamples || ff->numframes!=nframes || !allocated) {
    configureMemBlock( data->nsamples, ff->numframes );
  }

  // Parse Data
  // format: #<x><yyy><data><newline>
  // <x> is the size of <yyy> in bytes
  // <yyy> size of <data> in bytes
  // <newline> single byte new line character
  
  int x = std::atoi( &buff[1] );
  char py[x];
  for (int i=0; i<x; i++)
    py[i] = buff[2+i];
  int databytes = std::atoi(py);

  if ( data->mode!="ASCII" ) {

    short wordbytes = data->bytesperword;
    long ntraces = databytes/wordbytes; // what's in the buffer might be different than what was asked for
    char* tracebuffer = new char[nsamples*wordbytes]; // buffer for one waveform
    char* tracepoint = new char[wordbytes];
    for ( int iframe=0; iframe<nframes; iframe++ ) {
      char* trace_start = buff+(2+x+sizeof(char*)*nsamples*iframe);
      memcpy( tracebuffer, trace_start, sizeof(char*)*nsamples*wordbytes );
      for (int isample=0; isample<nsamples; isample++) {
	memcpy( tracepoint, tracepoint+(wordbytes*sizeof(char*)*isample), sizeof(char*)*wordbytes );
	adc[iframe][isample] = int( *tracepoint ); // is this machine invariant?
      }
    }
  }
  else if ( data->mode=="ASCII" ) {
    assert(false);
  }
  else {
    assert(false);
  }

}
