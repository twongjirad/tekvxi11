#ifndef __TKVHodoHit__
#define __TKVHodoHit__

typedef struct {
  int cbus;
  int stac;
  int pmt;
  int adc0;
  int adc1;
  int tdc;
  int tick;
  int deadtime;
} HodoHit_t;

#endif
