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
  
template <class T>
T* getObject(const char* objname, const char* filename){
  TFile ifile(filename);
  T* obj = (T*) ifile.Get(objname);
  obj->SetDirectory(gROOT);
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

  for (unsigned int iobj=0;iobj<vsize;iobj++){  
    obj = (T*) ifile.Get(objnames[iobj]);
    obj->SetDirectory(gROOT);
    obj_vec[iobj]=obj;
    
    }
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
  double ey=0;
  
  //cout << " [GraphContent::divide] Cycle on content...\n";
  for (unsigned int ipoint=0;ipoint<this->size();++ipoint){
    this->getPoint(ipoint,dividend_x,dividend_ex,dividend_y,dividend_ey,dummy);
    divisor.getPoint(ipoint,divisor_x,divisor_ex,divisor_y,divisor_ey,dummy);
    ey = TMath::Sqrt(dividend_ey*dividend_ey + divisor_ey*divisor_ey);
    dividend.addPoint(dividend_x,dividend_ex,dividend_y/divisor_y,ey);
  }

  return dividend;
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
             int the_marker_size,
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
  int marker_size;
  int line_size;
  int color;
  int fill_color;
  
  };         

//------------------------------------------------------------------------------

TGraphErrors* make_band(TF1& f){
  
//   const int nbins=100;
//   TString bandname("band_");
//   bandname+=f.GetName();
//   TH1F *h =new  TH1F("band","band",nbins,f.GetXmin(),f.GetXmax());
//   h->SetMarkerStyle(0);
//   h->SetMarkerColor(f.GetLineColor());
//   h->SetLineColor(f.GetLineColor());
//   h->SetFillColor(f.GetLineColor());
//   h->SetFillStyle(3002);
//   const int npar = f.GetNpar();
//   for (int ibin=1;ibin<nbins-1;ibin++){
//     double bin_center=h->GetBinCenter(ibin);
//     
//     // Calculate error
//     double bin_error=0;
//     for (int ipar=0;ipar<npar;++ipar){
//       double par_error = f.GetParError(ipar);
//       bin_error += par_error * f.GradientPar(ipar,&bin_center);
//       } 
//     
//     h->SetBinContent(ibin,f.Eval(bin_center));
//     h->SetBinError(ibin,bin_error);
//     }
//   return h;
//   }

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
  
  for (int ipoint=0;ipoint<npoints;ipoint++){
    
    double point_error=0;
    for (int ipar=0;ipar<npar;++ipar){
      double par_error = f.GetParError(ipar);
      point_error += par_error * f.GradientPar(ipar,&point);
      }     
    g->SetPoint(ipoint,point,f.Eval(point));
    g->SetPointError(ipoint,0,point_error);
    
    point+=step;
  }
    
  return g;
  }



//------------------------------------------------------------------------------

}; // END OF NAMESPACE
