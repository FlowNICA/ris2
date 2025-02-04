//
// Created by mikhail on 3/5/21.
//
// #include "src/picture.h"
#include <vector>
void style2D(){
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetFrameLineWidth(4);
  gStyle->SetMarkerSize(3);
  gStyle->SetLineWidth(4);

  gStyle->SetTitleSize(0.05, "X");
  gStyle->SetTitleSize(0.05, "Y");
  gStyle->SetTitleSize(0.01, "Z");

  gStyle->SetNdivisions(505, "X");
  gStyle->SetLabelSize(0.05, "X");
  gStyle->SetLabelSize(0.05, "Y");
  gStyle->SetLabelSize(0.05, "Z");

  gStyle->SetLabelOffset(0.003, "X");
  gStyle->SetLabelOffset(0.01, "Y");
  gStyle->SetLabelOffset(0.05, "Z");

  gStyle->SetTitleOffset(1.0, "X");
  gStyle->SetTitleOffset(0.0, "Y");
  gStyle->SetTitleOffset(0.05, "Z");

  gStyle->SetPalette(kViridis);

  gStyle->SetOptStat(0);
}