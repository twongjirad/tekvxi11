#include "TKVHodoView.hh"
#include "TCanvas.h"
#include "TEllipse.h"

const int TKVHodoView::HodoColorA[4][16] = { {kYellow, kYellow-4, kYellow-7, kYellow-9, kYellow, kYellow-4, kYellow-7, 
					      kYellow-9, kYellow+1, kYellow-4, kYellow-7, kYellow-9, kYellow, kYellow-4, kYellow-7, kYellow-9}, 
					     {kGreen,  kGreen-4,  kGreen-7,  kGreen-9,  kGreen,  kGreen-4,  kGreen-7,  kGreen-9,  kGreen+1, 
					      kGreen-4,  kGreen-7,  kGreen-9,  kGreen,  kGreen-4,  kGreen-7,  kGreen-9},
					     {kCyan,   kCyan-4,   kCyan-7,   kCyan-9,   kCyan,   kCyan-4,   kCyan-7,   kCyan-9,   
					      kCyan+1,   kCyan-4,   kCyan-7,   kCyan-9,   kCyan,   kCyan-4,   kCyan-7,   kCyan-9},
					     {kBlue,   kBlue-4,   kBlue-7,   kBlue-9,   kBlue,   kBlue-4,   kBlue-7,   kBlue-9,   
					      kBlue+1,   kBlue-4,   kBlue-7,   kBlue-9,   kBlue,   kBlue-4,   kBlue-7,   kBlue-9}    };
const int TKVHodoView::HodoColorB[4][16] = { {kYellow, kYellow-4, kYellow-7, kYellow-9, kYellow, kYellow-4, kYellow-7, kYellow-9, 
					      kYellow+1, kYellow-4, kYellow-7, kYellow-9, kYellow, kYellow-4, kYellow-7, kYellow-9}, 
					     {kGreen,  kGreen-4,  kGreen-7,  kGreen-9,  kGreen,  kGreen-4,  kGreen-7,  kGreen-9,  
					      kGreen+1,  kGreen-4,  kGreen-7,  kGreen-9,  kGreen,  kGreen-4,  kGreen-7,  kGreen-9},
					     {kCyan,   kCyan-4,   kCyan-7,   kCyan-9,   kCyan,   kCyan-4,   kCyan-7,   kCyan-9,   
					      kCyan+1,   kCyan-4,   kCyan-7,   kCyan-9,   kCyan,   kCyan-4,   kCyan-7,   kCyan-9},
					     {kBlue,   kBlue-4,   kBlue-7,   kBlue-9,   kBlue,   kBlue-4,   kBlue-7,   kBlue-9,   
					      kBlue+1,   kBlue-4,   kBlue-7,   kBlue-9,   kBlue,   kBlue-4,   kBlue-7,   kBlue-9}    };

TKVHodoView::TKVHodoView() {
  // Define the canvas
  HodoCan = new TCanvas("HodoCan","Hodscope Event View",300,600);
  HodoCan->Divide(1,2,0.01,0.01);
  // Define the ellipses

  for ( int stac = 0; stac < 4; ++stac ) {
    for ( int PMT = 0; PMT < 16; ++PMT ) {
      int i = PMT % 4;
      int j = (PMT - i)/4;
      // Channel A (North)
      double yA = chAstackOffsetY[stac] + double(i)*(1./8.) + 1./16.;
      double xA = chAstackOffsetX[stac] + double(3-j)*(1./8)+1./16.;
      xA = 1. - xA;
      HodoCan->cd(1);
      HodoGridA[stac][PMT] = new TEllipse(xA, yA, 0.05, 0.05);
      //if ( drawHits ) HodoGridA[stac][PMT]->Draw();
      // Channel B (South)
      double yB = chBstackOffsetY[stac] + double(i)*(1./8.) + 1./16.;
      double xB = chBstackOffsetX[stac] + double(3-j)*(1./8)+1./16.;
      //if ( drawHits ) HodoCan->cd(2);
      HodoGridB[stac][PMT] = new TEllipse(xB, yB, 0.05, 0.05);
      //if ( drawHits ) HodoGridB[stac][PMT]->Draw();
    }
  }
  
}

TKVHodoView::~TKVHodoView() {
  delete HodoCan;
  for (int stac=0; stac<4; stac++) {
    for (int pmt=0; pmt<16; pmt++) {
      delete HodoGridA[stac][pmt];
      delete HodoGridB[stac][pmt];
    }
  }
}


void TKVHodoView::ClearView() {
  // Clear Hodoscope display
  for ( int i = 0; i < 4; ++i ) {
    for ( int j = 0; j < 16; ++j ) {
      HodoGridA[i][j]->SetFillColor(HodoColorA[i][j]);
      HodoGridB[i][j]->SetFillColor(HodoColorB[i][j]);
    }
  }
  Draw();
}

void TKVHodoView::DrawHits( std::vector<HodoHit_t>& hits ) {
  for ( std::vector<HodoHit_t>::iterator it=hits.begin(); it!=hits.end(); it++ ) {
    if ( (*it).cbus==1 ) HodoGridA[ (*it).stac-1 ][ (*it).pmt ]->SetFillColor(2);
    if ( (*it).cbus==8 ) HodoGridB[ (*it).stac-1 ][ (*it).pmt ]->SetFillColor(2);
  }
  Draw();
}

void TKVHodoView::Draw() {
  HodoCan->cd(1);
  for ( int stac = 0; stac < 4; ++stac ) {
    for ( int PMT = 0; PMT < 16; ++PMT ) {
      HodoGridA[stac][PMT]->Draw();
      HodoGridB[stac][PMT]->Draw();
    }
  }
  HodoCan->cd(2);
  for ( int stac = 0; stac < 4; ++stac ) {
    for ( int PMT = 0; PMT < 16; ++PMT ) {
      HodoGridB[stac][PMT]->Draw();
    }
  }
  for (int c=1; c<=2; c++) {
    HodoCan->cd(c)->Modified(); 
    HodoCan->cd(c)->Update();
  }
  HodoCan->Draw();
}

