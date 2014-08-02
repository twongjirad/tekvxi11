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
  if ( allocated==true )
    destroyMemBlock();

  nframes = frames;
  nsamples = samples;

  adc = new int*[nframes];
  volts = new double*[nframes];
  for ( int i=0; i<nframes; i++ ) {
    adc[i] = new int[nsamples];
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

int TKVWaveformBuffer::extractFromBuffer( char* buff, TKVFastFrameSettings* ff, TKVDataSettings* data ) {

  if ( data->nsamples!=nsamples || ff->numframes!=nframes || !allocated) {
    // if the data format configuration has changed, we reconfigure the memory block
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
    char tracepoint[wordbytes];
    //char tracepoint[sizeof(int)];

    std::cout << "**** Parsing " << ndelivered << " delivered waveform(s) ****" << std::endl;
    nstored = ndelivered;

    std::cout << "Parsing data!" << std::endl;
    std::cout << "bytes per word: " << wordbytes << std::endl;
    std::cout << "ndelivered: " << ndelivered << std::endl;
    std::cout << "nframes: " << nframes << std::endl;
    std::cout << "nsamples: " << nsamples << " per trace" << std::endl;
    std::cout << "x=" << x << std::endl;
    std::cout << "y=" << databytes << " total bytes of data" << std::endl;
    // std::cout << "bytes in char*: " << sizeof(char*) << std::endl;
    // std::cout << "bytes in char: " << sizeof(char) << std::endl;
    std::cout << "bytes in int: " << sizeof(int) << std::endl;
    int complement = 1 << 8;

    for ( long iframe=0; iframe<ndelivered; iframe++ ) {
      //std::cout << "Frame " << iframe+1 << " of " << ndelivered << std::endl;
      int framepos = nsamples*wordbytes*iframe;
      //std::cout << "Frame position: " << framepos << std::endl;
      memcpy( tracebuffer, 
	      buff+(2+x+framepos),
	      sizeof(char)*nsamples*wordbytes );
      for (int isample=0; isample<nsamples; isample++) {
	//memset( tracepoint, 0, sizeof(int) );
	//memcpy( tracepoint+(sizeof(int)-wordbytes), tracebuffer+(wordbytes*isample), sizeof(char)*wordbytes );
	memcpy( tracepoint, tracebuffer+(wordbytes*isample), sizeof(char)*wordbytes );

	// adc[iframe][isample] = int(*tracepoint);
	// if ( adc[iframe][isample]<0 )
	//   adc[iframe][isample] += complement;

	int rawlow = int(tracepoint[0]);
	int lowbyte;
	if ( rawlow>0 )
	  lowbyte = rawlow;
	else {
	  lowbyte = complement+rawlow;
	}
	int highbyte = 0;
	if ( wordbytes>1 ) {
	  highbyte = int(tracepoint[1]);
	}
	adc[iframe][isample] = highbyte+lowbyte;

	volts[iframe][isample] = data->voltsperadc*double(adc[iframe][isample]-data->adcoffset);
	// if ( wordbytes>0 ) {
	//   std::cout << "rawlow=" << rawlow << " low=" << lowbyte 
	// 	    << " high=" << highbyte 
	// 	    << " adc=" << adc[iframe][isample] 
	// 	    << " v=" << volts[iframe][isample] 
	// 	    << " offset=" << data->adcoffset 
	// 	    << " compl=" << complement
	// 	    << std::endl;
	//   //std::cout << adc[iframe][isample] << " ";
	//   std::cin.get();
	// }
	// volts[iframe][isample] = double(adc[iframe][isample]-data->adcoffset);
	//std::cout << "Sample [" << iframe << ":" << isample << "] " << adc[iframe][isample] << std::endl;
	//std::cout << adc[iframe][isample] << " ";
	//std::cout << volts[iframe][isample] << " ";
      }
      //std::cout << std::endl;
      //std::cout << std::endl;
    }
    //std::cin.get();
  }
  else if ( data->mode=="ASCII" ) {
    //std::cout << "ASCII: " << buff << std::endl;
    char* pch = strtok( buff, ",\n");
    int samtot = 0;
    int isample=0;
    int iframe=0;
    while ( pch!=NULL ) {
      adc[iframe][isample] = std::atoi(pch);
      volts[iframe][isample] = data->voltsperadc*double(adc[iframe][isample]-data->adcoffset);
      //std::cout << adc[iframe][isample] << " ";
      //std::cout << volts[iframe][isample] << " ";
      isample++;
      samtot++;
      if ( isample==nsamples ) {
	isample=0;
	iframe++;
      }
      pch = strtok( NULL, ",\n");
    }
    std::cout << std::endl;
    std::cout << "Parsed " << iframe << " waveforms (" << samtot << " samples total)" << std::endl;
    nstored = iframe;
    //std::cin.get();
  }
  else {
    assert(false);
  }

  // TDC
  secs_per_sample = data->secspertdc;
  for (int isample=0; isample<nsamples; isample++) {
    t[isample] = isample*data->secspertdc;
  }

  return nstored;
  
}
