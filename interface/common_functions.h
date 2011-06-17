	#include <vector>
#include <iostream>
#include <string>

#include "TROOT.h"

#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TString.h"

#include "TMatrixDSym.h"

//------------------------------------------------------------------------------

namespace common_functions{

using namespace std;
  
void setTDRStyle(){
  TString tdr_style_file(".L $FWPATH");
  tdr_style_file+="/macros/TDR_Style.cxx";
  gROOT->ProcessLine(tdr_style_file.Data());
  gROOT->ProcessLine("TDR_Style()");
  }  
  
//------------------------------------------------------------------------------  

  
void setTDRStyle_mod(){
  TString tdr_style_file(".L $FWPATH");
  tdr_style_file+="/macros/tdrstyle_mod.C";
  gROOT->ProcessLine(tdr_style_file.Data());
  gROOT->ProcessLine("TDR_Style()");
  }  
  
//------------------------------------------------------------------------------ 

template <class T>
T* getObject(const char* objname, TString filename, bool close=true){  
  if (filename=="")
    return NULL;
  TFile ifile(filename);
  T* obj = (T*) ifile.Get(objname);
  if (close)
    ifile.Close();
  if (obj==NULL){
    cerr << "FATAL Could not read " << objname << " from " << filename.Data() << ".\n";
    exit (0);
    }
  return obj;
  }

//------------------------------------------------------------------------------  

template <>
TH1D* getObject<TH1D>(const char* objname, TString filename, bool close){
  if (filename=="")
    return NULL;  
  cerr << "Trying to read " << objname << " from " << filename.Data() << ".\n";
  TFile ifile(filename);
  TH1D* obj = (TH1D*) ifile.Get(objname);
  obj->SetDirectory(gROOT);
  if (close)
    ifile.Close();
  return obj;
  }
  
//------------------------------------------------------------------------------

template <class T>
vector<T*> getObjects(vector<TString> objnames, const char* filename){
  TFile ifile(filename);
  const unsigned int vsize=objnames.size();
  vector<T*> obj_vec(vsize);
  T* obj=0;
  
  cout << "---- RootFile " << filename << "\n";
  for (unsigned int iobj=0;iobj<vsize;iobj++){  
    cout << "    Getting  " << objnames[iobj].Data() << endl;
    obj = (T*) ifile.Get(objnames[iobj]);
    obj->SetDirectory(gROOT);
    obj_vec[iobj]=obj;
    
    }
  cout << "----\n";
  ifile.Close();
  return obj_vec;
  }

//------------------------------------------------------------------------------


bool are_compatible(double x,double ex,double y,double ey){
  
  double diff = TMath::Abs(x-y);
  double errsum = ex+ey;
  
  if (diff < errsum) return true;
  else return false;
  }

//------------------------------------------------------------------------------



class GraphContent{
  
public:
  
GraphContent(){};  


//---------------------------------------
GraphContent(TGraphErrors& g){
    m_name=g.GetName();
    double x,ex,y,ey;
    for (int ipoint=0;ipoint<g.GetN();++ipoint){
      g.GetPoint(ipoint,x,y);
      ey=g.GetErrorY(ipoint);
      ex=g.GetErrorX(ipoint);
      addPoint(x,ex,y,ey);
    }
  };  


//---------------------------------------
GraphContent(const char* name, GraphContent g){
    m_name=name;
//     cout << " [GraphContent] Name: " << m_name.Data() << endl;
    double x,ex,y,ey,eey;
    for (unsigned int ipoint=0;ipoint<g.size();++ipoint){
      g.getPoint(ipoint,x,ex,y,ey,eey);    
      addPoint(x,ex,y,ey,eey);
    }
  };  

//---------------------------------------
void addGraphContent(GraphContent& g){
  for (unsigned int i=0;i<g.size();++i)
    this->addPoint(g.xvals[i],g.exvals[i],
                   g.yvals[i],g.eyvals[i],
                   g.eeyvals[i]);
  }

//---------------------------------------

void addPoint(double x, double ex, double y, double ey, double eey=0){
    xvals.push_back(x);
    exvals.push_back(ex);
    yvals.push_back(y);
    eyvals.push_back(ey);
    eeyvals.push_back(eey);
    };

//---------------------------------------

unsigned int size(){return xvals.size();}; 
    
//---------------------------------------

void getPoint(int i, double &x, double &ex, double &y, double &ey, double &eey){
  x = xvals[i];
  ex = exvals[i];
  y = yvals[i];
  ey = eyvals[i];
  eey = eyvals[i];
  }    

//---------------------------------------

TGraphErrors* getGraph(){
    TGraphErrors* graph = new TGraphErrors(xvals.size());
    graph->SetName(getName());
//     std::cout << " [getGraph] The name is " << getName() <<endl;
    for (unsigned int ipoint=0;ipoint<xvals.size();ipoint++){
      graph->SetPoint(ipoint,xvals[ipoint],yvals[ipoint]);
      graph->SetPointError(ipoint,exvals[ipoint],eyvals[ipoint]);
      }
  return graph;
  }


//---------------------------------------

void decorrelate(){
  double first_err=0;
  double first_err_err=0;
  double x,ex,y,ey,eey;
  cout << "\n  [decorrelate] Begin decorrelation: \n";
  for (unsigned int ipoint=0;ipoint<size();++ipoint){
    getPoint(ipoint,x,ex,y,ey,eey);
    if (ipoint==0){
      first_err=ey;
      first_err_err=eey;
      }
    else{
        // Case one: the subtraction can be performed easily
        if (first_err<ey)
          ey=TMath::Sqrt(ey*ey-first_err*first_err);
        // Case two: they are compatible. Error is the resolution
        else if (are_compatible(ey,eey,first_err,first_err_err)){
          cout << "  [decorrelate] The errors are compatible within uncertainties. Using the average of uncertainties as error.\n";
          ey = 0.5*(first_err_err+eey);
          }
        // Case three: prompt an error and set to 0
        else{
          ey=0;
          cout << "\n\n ----------------------> Error occurred in decorrelation!\n\n";
          }
        }
    eyvals[ipoint]=ey;
    }
  }

//---------------------------------------

GraphContent divide(GraphContent& divisor){

  //cout << " [GraphContent::divide] Enter...\n";
  GraphContent dividend;
  //cout << " [GraphContent::divide] Sizes are - dividend=" << dividend.size() << " divisor=" << divisor.size() << " ...\n";
  
  double dummy;
  
  double dividend_x,dividend_ex,dividend_y,dividend_ey;
  double divisor_x,divisor_ex,divisor_y,divisor_ey;
  double x,ex,y,ey;
  
  //cout << " [GraphContent::divide] Cycle on content...\n";
  for (unsigned int ipoint=0;ipoint<std::min(this->size(),divisor.size());++ipoint){
    this->getPoint(ipoint,dividend_x,dividend_ex,dividend_y,dividend_ey,dummy);
    divisor.getPoint(ipoint,divisor_x,divisor_ex,divisor_y,divisor_ey,dummy);
    
    y = dividend_y/divisor_y;    
    ey = y*TMath::Sqrt(dividend_ey*dividend_ey/(dividend_y*dividend_y) + divisor_ey*divisor_ey/(divisor_y*divisor_y));
    x = (dividend_x+divisor_x)/2.;    
    ex = TMath::Sqrt(dividend_ex*dividend_ex + divisor_ex*divisor_ex)/2.;    
    
    dividend.addPoint(x,
                      ex,
                      y,
                      ey);
  }

  return dividend;
}
//---------------------------------------

void divide_inplace(GraphContent& divisor){

  *this = divide(divisor);
}
//---------------------------------------
TString getName(){return m_name;};
void setName(const char* name){m_name=name;};
//---------------------------------------

  private:
  vector<double> xvals;
  vector<double> exvals;
  vector<double> yvals;
  vector<double> eyvals;
  vector<double> eeyvals;
  TString m_name;
};


//------------------------------------------------------------------------------    
    
class plot_style{

  public:
  plot_style(): marker_style(0),
                marker_size(1),
                line_size(1),
                color(0),
                fill_color(kWhite){};

  plot_style(const plot_style& style): marker_style(style.marker_style),
                                       marker_size(style.marker_size),
                                       line_size(style.line_size),
                                       color(style.color),
                                       fill_color(style.fill_color){};
                                
                
  plot_style(int the_marker_style,
             double the_marker_size,
             int the_line_size,
             int the_marker_color,
             int the_fill_color=kWhite):
             marker_style(the_marker_style),
             marker_size(the_marker_size),
             line_size(the_line_size),
             color(the_marker_color),
             fill_color(the_fill_color){};

//----------------
template <class T>  
T* setStyle(T* obj){
    obj->SetLineColor(color);
    obj->SetMarkerColor(color);
    obj->SetLineWidth(line_size);
    obj->SetMarkerSize(marker_size);
    obj->SetMarkerStyle(marker_style);
    obj->SetFillColor(fill_color);
    return obj;
    }
      
             
  private:
  int marker_style;
  double marker_size;
  int line_size;
  int color;
  int fill_color;
  
  };         

  //------------------------------------------------------------------------------

TGraphErrors* make_band(TF1& f, TMatrixDSym& cov){

  const int npoints=100;
  TString bandname("band_");
  bandname+=f.GetName();
  TGraphErrors *g =new  TGraphErrors(npoints);
  g->SetMarkerStyle(0);
  g->SetMarkerColor(f.GetLineColor());
  g->SetLineColor(f.GetLineColor());
  g->SetFillColor(f.GetLineColor());
  g->SetFillStyle(3002);
  const int npar = f.GetNpar();
  const double step = (f.GetXmax()-f.GetXmin())/npoints;
  double point = f.GetXmin();
 
  double point_error2=0;
  double gradi=0;
  double gradj=0;
  for (int ipoint=0;ipoint<npoints;ipoint++){
    point_error2=0;
    for (int ipar=0;ipar<npar;++ipar)
      for (int jpar=0;jpar<npar;++jpar){
        gradi=f.GradientPar(ipar,&point);
        gradj=f.GradientPar(jpar,&point);
        point_error2 += gradi* cov[ipar][jpar]*gradj;
        }
    g->SetPoint(ipoint,point,f.Eval(point));
    g->SetPointError(ipoint,0,TMath::Sqrt(point_error2));
    point+=step;
  }// end loop on reference points on function
    
  return g;
  }

//------------------------------------------------------------------------------

TGraph** make_bands_up_down(TF1& f,TMatrixDSym& cov ){

  TGraphErrors* ge = make_band(f,cov);

  const int npoints=ge->GetN();
  
  TGraph** up_down= new TGraph*[2];
  up_down[0]=new TGraph(npoints);up_down[0]->SetName("UPBand");
  up_down[1]=new TGraph(npoints);up_down[1]->SetName("DownBand");
  
  int linew=f.GetLineWidth()/2;
  if (linew==0)
    linew=1;
  
  for (int i=0;i<2;++i){
    up_down[i]->SetFillColor(0);
    up_down[i]->SetLineColor(ge->GetLineColor());
    up_down[i]->SetLineWidth(linew);
    up_down[i]->SetLineStyle(2);
    }
  
  double x,y,ey;
  for (int i=0;i<npoints;++i){
    
    ge->GetPoint(i,x,y);
    ey = ge->GetErrorY(i);
    
    up_down[0]->SetPoint(i,x,y-ey);
    up_down[1]->SetPoint(i,x,y+ey);
    }
  
  delete ge;
  

  return up_down;
  }


//------------------------------------------------------------------------------
void getMinMax(double& minx, double& maxx,
               double& miny, double& maxy,
               TH1* h1, TH1* h2, TH1* h3=0){
  
  maxy = std::max(h1->GetMaximum(), h2->GetMaximum());
  if (h3) maxy = std::max(maxy, h3->GetMaximum());

  miny = std::min(h1->GetMinimum(), h2->GetMinimum());
  if (h3) miny = std::min(miny, h3->GetMinimum());  
  
  const int nbins1=h1->GetNbinsX();
  const int nbins2=h2->GetNbinsX();
  
  minx = std::min(h1->GetBinLowEdge(1), h1->GetBinLowEdge(1));
  if (h3) minx = std::min(minx, h3->GetBinLowEdge(1));

  // Just use the last bin center
  maxx = std::min(h1->GetBinCenter(nbins1), h2->GetBinCenter(nbins2));
  if (h3) {
      const int nbins3=h3->GetNbinsX();
      maxx = std::max(maxx, h3->GetBinCenter(nbins3));
      }
  
  
/*  maxx = std::min(h1->GetBinLowEdge(nbins1-1)+h1->GetBinWidth(nbins1-1), h2->GetBinLowEdge(nbins2-1)+h2->GetBinWidth(nbins2-1));
  if (h3) {
      const int nbins3=h3->GetNbinsX();
      max = std::max(maxx, h3->GetBinLowEdge(nbins3-1)+h3->GetBinWidth(nbins3-1));
      }*/
      
  maxx*=1.2;
  maxy*=1.2;
  minx*=1.;
  miny*=.8;
  
  
  }

//------------------------------------------------------------------------------

}; // END OF NAMESPACE
