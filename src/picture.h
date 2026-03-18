#ifndef PICTURE_H
#define PICTURE_H


#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <random>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include <TPad.h>
#include <TVirtualPad.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TF1.h>
#include <TLegend.h>
#include <TH2.h>
#include <TFile.h>
#include <TLine.h>

#include "wrap.h"
#include "bunch.h"

namespace ris2 {
/// @brief Class used to configure the axis of the plotted picture. Supports chaining for convinience
struct Axis{
  Axis() = default;
  /// @brief Sets the label of the axis
  Axis& SetTitle( std::string title ){ title_ = std::move(title); return *this; }
  /// @brief Sets the upper value of the axis
  Axis& SetHi( double hi ){ hi_ = hi; return *this; }
  /// @brief Sets the lower value of the axis
  Axis& SetLo( double lo ){ lo_ = lo; return *this; }
  Axis& SetLog( bool value = true ){ log_ = value; return *this; }
  std::string title_{};
  double hi_{};
  double lo_{};
  bool log_{false};
};

/// @brief Class used to add text to the plot
struct Text{
  Text() = default;
  /// @brief Sets the size of the text
  Text& SetSize(double size){ size_ = size; return *this; }
  /// @brief Sets the text to be placed
  Text& SetText(std::string text){ text_ = std::move(text); return *this; }
  /// @brief Sets the position of the text. {x1, y1} --- position of the  top right point
  Text& SetPosition(std::array<double, 2> pos){ position_ = std::move(pos); return *this; }
  Text& SetFont(int font) { font_ = font; return *this; }
  double size_;
  std::string text_;
  std::array<double, 2> position_;
  int font_;
};

class FileManager{
public:
  FileManager() = default;
  TFile* AddFile( std::string file_name ){
    files_.push( std::unique_ptr<TFile>{TFile::Open( file_name.c_str() )} );
    return files_.top().get();
  }
private:
  std::stack< std::unique_ptr<TFile> > files_{};
};

/// @brief Class handling the sub-plots of the picture
class Picture{
public:
  /// @brief Default constructor
  Picture() : graph_stack_{ std::make_unique<TMultiGraph>() } {}
  /// @brief Constructor specializing the position of the sub-plot
  /// @param aspect_ratio {x1, y1, x2, y2} is the relative position of the bottom-left (x1, y1) and top-right (x2, y2) points of the sub-plot
  Picture(const std::vector<double>& aspect_ratio ) : graph_stack_{ std::make_unique<TMultiGraph>() } {
    auto rd = std::random_device{};
    auto re = std::mt19937{rd()};
    auto distr = std::uniform_int_distribution<size_t>{ 0, std::numeric_limits<size_t>::max() };
    auto name = std::string{"canv_" }.append( std::to_string(distr(re)) );
    pad_ = new TPad( name.c_str(), "", 
                                    aspect_ratio.at(0), 
                                    aspect_ratio.at(1), 
                                    aspect_ratio.at(2), 
                                    aspect_ratio.at(3) );
  }
  /// @brief Copy constructor
  Picture( Picture&& ) = default;
  /// @brief Move constructor
  Picture( const Picture& ) = delete;
  template<typename T>
  /// @brief Adds the Wrap<T> to the plot
  Picture& AddToPlot( Wrap<T>& wrap ){ 
    auto style = wrap.GetStyle();
    auto graph =  wrap.ReleaseResult();
    if( !graph ) return *this;
    std::string option = style.marker_ >= 0 ? "P" : "L";
    option.append( wrap.GetOption() );
    if( style.marker_ >= 0 )
      graph_stack_->Add( graph, option.c_str() ); 
    if( style.marker_ < 0 )
      graph_stack_->Add( graph, option.c_str() ); 
    return *this; 
  }
  /// @brief Adds the TH2 object to the plot
  Picture& AddToPlot( const std::string& file_name, const std::string& histo2d_name ){
    file_histo2d_ = std::make_unique<TFile>( file_name.c_str() );
    file_histo2d_->GetObject(histo2d_name.c_str(), histo2d_);
    return *this;
  }
  /// @brief Adds the systematical errors of the Wrap<T> to the plot
  template<typename T>
  Picture& AddSystematics(Wrap<T>& wrap){
    auto style = wrap.GetStyle();
    auto graph =  wrap.ReleaseResult();
    if( !graph ) return *this;
    if( style.marker_ >= 0 )
      graph_stack_->Add( wrap.ReleaseSystematics(), "2" ); 
    if( style.marker_ < 0 )
      graph_stack_->Add( wrap.ReleaseSystematics(), "2" ); 
    return *this; 
  }
  /// @brief Adds the vector of the Wrap<T> to the plot
  template<typename T>
  Picture& AddToPlot( std::vector<Wrap<T>>& bunch ){
    std::for_each( bunch.begin(), bunch.end(), [this]( Wrap<T>& obj ){ 
      AddToPlot( obj ); 
    } );    
    return *this;
  }
  /// @brief Adds the systematic errors of vector of the Wrap<T> to the plot
  template<typename T>
  Picture& AddSystematics( std::vector<Wrap<T>>& bunch ){
    std::for_each( bunch.begin(), bunch.end(), [this]( Wrap<T>& obj ){ 
      AddSystematics(obj);
    } );    
    return *this;
  }
  /// @brief Delayed initialization of the sub-plot
  /// @param aspect_ratio {x1, y1, x2, y2} is the relative position of the bottom-left (x1, y1) and top-right (x2, y2) points of the sub-plot
  Picture& SetResolution( std::vector<double> aspect_ratio ){
    auto rd = std::random_device{};
    auto re = std::mt19937{rd()};
    auto distr = std::uniform_int_distribution<size_t>{ 0, std::numeric_limits<size_t>::max() };
    auto name = std::string{"pad_" }.append( std::to_string(distr(re)) );
    pad_ = new TPad( name.c_str(), "", 
                              aspect_ratio.at(0), 
                              aspect_ratio.at(1), 
                              aspect_ratio.at(2), 
                              aspect_ratio.at(3) );
    return *this;
  }
  /// @brief Customizing the X-axis
  Picture& SetXAxis( Axis axis ){ x_axis_ = std::move(axis); return *this; }
  /// @brief Customizing the Y-axis
  Picture& SetYAxis( Axis axis ){ y_axis_ = std::move(axis); return *this; }
  /// @brief Customizing the Y-axis
  Picture& SetZAxis( Axis axis ){ z_axis_ = std::move(axis); return *this; }
  /// @brief Adds text to the sub-plot
  Picture& AddText( Text text ) {
    texts_.emplace_back(std::move(text));
    return *this;
  }
  /// @brief Adds legend to the sub-plot
  Picture& AddLegend( TLegend* leg ){
    legends_.emplace_back( leg );
    return *this;
  }
  /// @brief Adds TF1* object to the sub-plot
  Picture& AddFunction( TF1* func ){
    functions_.emplace_back(func);
    return *this;
  }
  Picture& AddLine( TLine* line ){
    lines_.emplace_back(line);
    return *this;
  }
  Picture& AddImage( TASImage* img ){
    images_.emplace_back(img);
    return *this;
  }
  /// @brief Plots all of the added objects
  TPad* Print(){
    pad_->cd();
    Draw2D();
    Draw1D();
    DrawText();
    DrawLegends();
    DrawFunctions();
    DrawLines();
    DrawImages();
    if( x_axis_.log_ )
      pad_->SetLogx();
    if( y_axis_.log_ )
      pad_->SetLogy();
    if( z_axis_.log_ )
      pad_->SetLogz();
    return pad_;
  }
private:
  void Draw2D(){
    if( !histo2d_ )
      return;
    auto axis_titles = std::string{";"}.append( x_axis_.title_ ).append(";").append(y_axis_.title_).append(";").append(z_axis_.title_);
    histo2d_->SetTitle(axis_titles.c_str());
    histo2d_->Draw("colz");
    SetLimits2D();
  }
  void SetLimits2D(){
    if( x_axis_.hi_ > x_axis_.lo_ ){
      histo2d_->GetXaxis()->SetRangeUser(x_axis_.lo_, x_axis_.hi_);
      histo2d_->Draw("colz");
    }
    if( y_axis_.hi_ > y_axis_.lo_ ){
      histo2d_->GetYaxis()->SetRangeUser(y_axis_.lo_, y_axis_.hi_);
      histo2d_->Draw("colz");
    }
    if( z_axis_.hi_ > z_axis_.lo_ ){
      histo2d_->SetMinimum(z_axis_.lo_);
      histo2d_->SetMaximum(z_axis_.hi_);
      histo2d_->Draw("colz");
    }
  }
  void Draw1D(){
    if( !graph_stack_->GetListOfGraphs() )
      return;
    auto axis_titles = std::string{";"}.append( x_axis_.title_ ).append(";").append(y_axis_.title_);
    graph_stack_->SetTitle(axis_titles.c_str());
    graph_stack_->Draw(!histo2d_ ? "APL" : "APL+SAME");
    SetLimits1D();
  }
  void SetLimits1D(){
    if( histo2d_ )
      return;
    if( x_axis_.hi_ > x_axis_.lo_ ){
      graph_stack_->GetHistogram()->GetXaxis()->SetLimits(x_axis_.lo_, x_axis_.hi_);
      graph_stack_->GetHistogram()->Draw("APL");
    }
    if( y_axis_.hi_ > y_axis_.lo_ ){
      graph_stack_->GetHistogram()->GetYaxis()->SetRangeUser(y_axis_.lo_, y_axis_.hi_);
      graph_stack_->Draw("APL");
    }
  }
  void DrawText(){
    vec_tlatexs_.reserve( texts_.size() );
    for( auto text : texts_ ){
      vec_tlatexs_.emplace_back( new TLatex( text.position_.at(0), text.position_.at(1), text.text_.c_str() ) );
      vec_tlatexs_.back()->SetNDC();
      vec_tlatexs_.back()->SetTextSize( text.size_ );
      vec_tlatexs_.back()->SetTextFont(text.font_);
      vec_tlatexs_.back()->Draw("same");
    }
  }
  void DrawLegends(){
    for( auto& leg : legends_ ){
      leg->SetFillStyle(0);
      leg->Draw( "same" );
    }
  }
  void DrawFunctions(){
    // functions_.emplace_back(new TF1( "zero", "0", x_axis_.lo_, x_axis_.hi_ ));
    for( auto& f : functions_ )
      f->Draw("same");
  }
  void DrawLines(){
    // functions_.emplace_back(new TF1( "zero", "0", x_axis_.lo_, x_axis_.hi_ ));
    for( auto& l : lines_ )
      l->Draw("same");
  }
  void DrawImages(){
    for ( auto& img : images_ ){
      img->Draw("same");
    }
  }
  
  Axis x_axis_;
  Axis y_axis_;
  Axis z_axis_;
  TPad* pad_{};
  std::unique_ptr<TFile> file_histo2d_{};
  TH2* histo2d_{};
  std::unique_ptr<TMultiGraph> graph_stack_{};
  std::vector<Text> texts_{};
  std::vector< std::unique_ptr<TLatex> > vec_tlatexs_{};
  std::vector< std::unique_ptr<TF1> > functions_{};
  std::vector< std::unique_ptr<TLine> > lines_{};
  std::vector<std::unique_ptr<TLegend>> legends_;
  std::vector<std::unique_ptr<TASImage>> images_;
};

/// @brief Class for managing the sub-plots and save the plotted data to a file.
/// Contains the vector<Picture>. On a call of Plot::Print() plots all the underlying sub-plots and saves the canvas to the file.
class Plot{
public:
  /// @brief Constructor initializing the canvas
  Plot( std::vector<size_t> resolution ){
    auto rd = std::random_device{};
    auto re = std::mt19937{rd()};
    auto distr = std::uniform_int_distribution<size_t>{ 0, std::numeric_limits<size_t>::max() };
    auto name = std::string{"canv_" }.append( std::to_string(distr(re)) );
    canvas_.reset( new TCanvas(name.c_str(), "", resolution.at(0), resolution.at(1)) );
  }
  /// @brief Default destructor
  ~Plot() = default;
  /// @brief Adds the sub-plot to the plot
  template<typename... Args>
  Picture& AddSubPlot( Args... args ){ 
    plots_.emplace_back(args...); return plots_.back(); 
  }
  /// @brief Returns the last sub-plot created
  Picture& LastSubPlot(){
    if( plots_.empty() ){
      plots_.emplace_back( std::vector<double>{0., 0., 1., 1.} );
    }
    return plots_.back(); 
  }
  /// @brief Returns the sub-plot via the index
  Picture& GetSubPlot( size_t idx ){ return plots_.at(idx); }
  template<typename T>
  /// @brief Automatically plots the ratio plot
  Plot& AddRatioPlot( RatioBuilder<T>& ratio, 
    std::vector<double> result_plot = { 0.0, 0.35, 1.0, 1.0 },
    std::vector<double> ratio_plot = { 0.0, 0.0, 1.0, .35 }
  ){
      plots_.emplace_back( result_plot );
      plots_.back().AddToPlot( ratio.GetResultWraps() );
      plots_.emplace_back( ratio_plot );
      plots_.back().AddToPlot( ratio.GetRatioWraps() );
      return *this;
  }
  /// @brief Draws all the sub-plots created and saves result to the file
  /// @param save_name path to the file to save results in
  Plot& Print( const std::string& save_name ){
    canvas_->cd();
    std::vector<TPad*> pads;
    pads.reserve(plots_.size());
    for( auto& plot : plots_ ){
      pads.push_back(plot.Print());
    }
    canvas_->cd();
    for( auto pad : pads ){
      pad->Draw();
    }
    canvas_->Draw();
    canvas_->Print( save_name.c_str() );
    return *this;
  }
  Plot& cd() { canvas_->cd(); return *this; }
protected:
  std::vector<Picture> plots_;
  std::unique_ptr<TCanvas> canvas_{};
};

}
#endif // PICTURE_H
