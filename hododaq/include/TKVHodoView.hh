#ifndef __TKVHODOVIEW__
#define __TKVHODOVIEW__

#include "TKVHodoHit.hh"
#include <vector>

class TCanvas;
class TEllipse;

class TKVHodoView {

public:
  TKVHodoView();
  virtual ~TKVHodoView();

  void ClearView();
  void DrawHits( std::vector<HodoHit_t>& hits );
  void Draw();

protected:
  static const int HodoColorA[4][16];
  static const int HodoColorB[4][16];
  const double chAstackOffsetX[4] = { 0.0, 0.5, 0.5, 0.0 };
  const double chAstackOffsetY[4] = { 0.5, 0.0, 0.5, 0.0 };
  const double chBstackOffsetX[4] = { 0.5, 0.0, 0.0, 0.5 };
  const double chBstackOffsetY[4] = { 0.5, 0.5, 0.0, 0.0 };
  TEllipse *HodoGridA[4][16]; // North
  TEllipse *HodoGridB[4][16]; // South

  TCanvas* HodoCan;


};

#endif
