#include "ris2.h"
// #include "src/picture.h"
#include <TAttMarker.h>

void bunch_example(){
  gROOT->Macro( "/home/mikhail/ris2/macro/style.cc" );
  std::string file_vf = "~/Flow/BM@N/vf.2024.02.12.root";
  using namespace ris2;
  auto container= Bunch<Correlation>{}
      .AddToBunch(
      std::string{"F1"},
      file_vf, 
      std::vector<std::string>{
          "proton/v1.F1_RESCALED(F3_RESCALED,Tpos_RESCALED).y1y1centrality",
          "proton/v1.F1_RESCALED(F3_RESCALED,Tneg_RESCALED).y1y1centrality",
    })
    .AddToBunch(
    std::string{"F2"},
    file_vf, 
    std::vector<std::string>{
          "proton/v1.F2_RESCALED.Tpos_RESCALED(F1_RESCALED,F3_RESCALED).y1y1centrality",
          "proton/v1.F2_RESCALED.Tneg_RESCALED(F1_RESCALED,F3_RESCALED).y1y1centrality",
    })
    .AddToBunch(
    std::string{"F3"},
    file_vf, 
    std::vector<std::string>{
          "proton/v1.F3_RESCALED(F1_RESCALED,Tpos_RESCALED).y1y1centrality",
          "proton/v1.F3_RESCALED(F1_RESCALED,Tneg_RESCALED).y1y1centrality",
    })
    .AddToBunch(
    std::string{"F3"},
    file_vf, 
    std::vector<std::string>{
          "proton/v1.F1_RESCALED(F3_RESCALED,Tpos_RESCALED).y1y1centrality",
          "proton/v1.F1_RESCALED(F3_RESCALED,Tneg_RESCALED).y1y1centrality",
          "proton/v1.F2_RESCALED.Tpos_RESCALED(F1_RESCALED,F3_RESCALED).y1y1centrality",
          "proton/v1.F2_RESCALED.Tneg_RESCALED(F1_RESCALED,F3_RESCALED).y1y1centrality",
          "proton/v1.F3_RESCALED(F1_RESCALED,Tpos_RESCALED).y1y1centrality",
          "proton/v1.F3_RESCALED(F1_RESCALED,Tneg_RESCALED).y1y1centrality",
    }
  )
  ;
  container.GetPalette().SetPalette( std::vector{
      Style().SetColor( kRed ).SetMarker(kFullCircle),
      Style().SetColor( kBlue+2 ).SetMarker(kFullCircle),
      Style().SetColor( kGreen+2 ).SetMarker(kFullCircle),
      Style().SetColor( kBlack ).SetMarker(-1),
  } );
  container.Perform( []( auto& obj ){
      obj
        .Rebin( std::vector<Qn::AxisD>{ {"centrality", 1, 10, 30}, { "trPt", 1, 0.2, 1.4 } } )
        .Project(std::vector<Qn::AxisD>{{"trProtonY", 6, -0.2, 1.0}});
  } );
  auto leg = container.MakeLegend( {0.25, 0.9, 0.55, 0.75} );
  auto plot = Plot( {1000, 1100} );
  plot.AddSubPlot( std::vector<double>{ 0.0, 0.0, 1.0, 1.0 } )
      .SetXAxis(Axis().SetTitle("y_{cm}").SetLo(-0.6).SetHi(1.4))
      .SetYAxis(Axis().SetTitle("v_{1}").SetLo(-0.1).SetHi(0.1))
      .AddToPlot( *container )
      .AddSystematics( *container )
      .AddLegend( leg );
  plot.Print( "/home/mikhail/ris2/macro/pictures/bunch_example.png" );
}