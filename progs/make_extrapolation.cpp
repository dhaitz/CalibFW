#include <iostream>

#include "common_functions.h"
#include "MinimalParser.h"

#include "TCanvas.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TF1.h"

using namespace common_functions;

//------------------------------------------------------------------------------

enum plot_styles_enum { 
     response_data,
     response_mc,
     balance_ratio,
     mpf_ratio};


plot_style plot_styles[4] = {plot_style(22,2,2,kBlue),
plot_style(23,2,2,kRed),
plot_style(21,2,2,kBlue),
plot_style(8,2,2,kRed)};

//------------------------------------------------------------------------------

class extrapolator {  
 
public:
extrapolator(TString pt_bin,
             TString prefix,
             TString ifilename_data,TString ifilename_mc,
             TString comment,
             TString cut_name,vdouble cut_values,
             vString balance_histo_names_data,
             vString mpf_histo_names_data,
             vString balance_histo_names_mc,
             vString mpf_histo_names_mc){
  
  // Adapt the names to the file structure
  for (unsigned int iname=0;iname< balance_histo_names_data.size(); iname++){
    balance_histo_names_data[iname].ReplaceAll("@PTBIN@",pt_bin);
    mpf_histo_names_data[iname].ReplaceAll("@PTBIN@",pt_bin);
    balance_histo_names_mc[iname].ReplaceAll("@PTBIN@",pt_bin);
    mpf_histo_names_mc[iname].ReplaceAll("@PTBIN@",pt_bin);
    }
  

  m_balance_histos_data = getObjects<TH1D>(balance_histo_names_data, ifilename_data);
  m_mpf_histos_data = getObjects<TH1D>(mpf_histo_names_data, ifilename_data);  
  m_balance_histos_mc = getObjects<TH1D>(balance_histo_names_mc, ifilename_mc);
  m_mpf_histos_mc = getObjects<TH1D>(mpf_histo_names_mc, ifilename_mc);    
 
  m_cut_values = cut_values;
  m_cut_name = cut_name; 
  
  m_comment = comment;
  m_prefix=prefix;
  m_pt_bin = pt_bin;
}

void extrapolate(){
  // Build extrapolations
  
  // DATA BALANCE
  GraphContent extr_balance_data = m_build_extr_graph("Data",m_cut_values,m_balance_histos_data);
  // MC BALANCE
  GraphContent extr_balance_mc = m_build_extr_graph("MC",m_cut_values,m_balance_histos_mc);
  // DATA MPF
  GraphContent extr_mpf_data = m_build_extr_graph("Data",m_cut_values,m_mpf_histos_data);
  // MC MPF
  GraphContent extr_mpf_mc = m_build_extr_graph("MC",m_cut_values,m_mpf_histos_mc);
  
  //Build ratios
  GraphContent extr_balance_ratio ("Balance",extr_balance_data.divide(extr_balance_mc));
  GraphContent extr_mpf_ratio ("MPF",extr_mpf_data.divide(extr_mpf_mc));
  
  // Do plots
  do_plot(m_pt_bin+"_response_balance_extrapolation_data_mc",
          "p_{T} balance",
          m_cut_name,
          "Jet Response",
          plot_styles[response_data].setStyle<TGraphErrors>(extr_balance_data.getGraph()),
          plot_styles[response_mc].setStyle<TGraphErrors>(extr_balance_mc.getGraph()),
          "[0]+[1]*x");

  do_plot(m_pt_bin+"_response_mpf_extrapolation_data_mc",
          "MPF",
          m_cut_name,
          "Jet Response",
          plot_styles[response_data].setStyle<TGraphErrors>(extr_mpf_data.getGraph()),
          plot_styles[response_mc].setStyle<TGraphErrors>(extr_mpf_mc.getGraph()),
          "[0]+[1]*x");          

  do_plot(m_pt_bin+"_data_over_mc",
          "Method",
          m_cut_name,
          "Data / MC",
          plot_styles[balance_ratio].setStyle<TGraphErrors>(extr_balance_ratio.getGraph()),
          plot_styles[mpf_ratio].setStyle<TGraphErrors>(extr_mpf_ratio.getGraph()),
          "[0]");     

     
          
}  
  
  
  
private:
vector<TH1D*> m_balance_histos_data;  
vector<TH1D*> m_mpf_histos_data;  
vector<TH1D*> m_balance_histos_mc;  
vector<TH1D*> m_mpf_histos_mc;  
vdouble m_cut_values;
TString m_cut_name;
TString m_comment;
TString m_pt_bin;
TString m_prefix;

//------------------------------------------------------------------------------
template <class T>
GraphContent m_build_extr_graph(const char* name,vector<double> cut_values,vector<T*> balance_histos, bool decorrelate=true){
  
  const unsigned int size = balance_histos.size();
  GraphContent extrapolated_graph;
  extrapolated_graph.setName(name);
  double center=0;
  double center_err=0;
  double center_err_err=0;
  
  for (unsigned int ipoint=0;ipoint<size;++ipoint){
    center=balance_histos[ipoint]->GetMean();
    center_err=balance_histos[ipoint]->GetMeanError();
    
    // Small study about the precision of RMS
    double sqrtNminus1=TMath::Sqrt(balance_histos[ipoint]->GetEntries()-1);    
    double center_err_err = balance_histos[ipoint]->GetRMSError()/sqrtNminus1;
    
    extrapolated_graph.addPoint(m_cut_values[ipoint],0,
                                center,center_err,center_err_err);
    }// end loop on points
  if (decorrelate)
    extrapolated_graph.decorrelate();
  return extrapolated_graph;
  };
        
//------------------------------------------------------------------------------    

void do_plot(TString name,
             TString leg_header,
             TString x_axis_title,
             TString y_axis_title,
             TGraphErrors* g1,
             TGraphErrors* g2,
             TString formula){
  

  // Get at first x max and x min, y max and y min
  double x1first,y1first,x2first,y2first, x1last,y1last,x2last,y2last;
  g1->GetPoint(0,x1first,y1first);
  g1->GetPoint(g1->GetN()-1,x1last,y1last);
  g2->GetPoint(0,x2first,y2first);
  g2->GetPoint(g2->GetN()-2,x2last,y2last);
  
  double x_max = max( max(x1first,x1last), max(x2first,x2last) );
  double y_min = min( min(y1first,y1last), min(y2first,y2last) );
  double y_max = max( max(y1first,y1last), max(y2first,y2last) );  

  // Get the TF1s for the extrapolations
  TF1 func1("interpolation1", formula, 0, x_max*1.2);
  func1.SetLineWidth(2);
  func1.SetLineColor(g1->GetLineColor());  
  
  TF1 func2("interpolation2", formula, 0, x_max*1.2);
  func2.SetLineWidth(2);
  func2.SetLineColor(g2->GetLineColor());
  
  for (int ipar=0;ipar<func1.GetNpar();ipar++){
    func1.SetParameter(ipar,1);
    func2.SetParameter(ipar,1);
    }
  
  g1->Fit(&func1);
  g2->Fit(&func2);
  
  // Dummy for x axis
  TH1F dummy_h("dummy","dummy",2,0,x_max*1.3);
  dummy_h.GetXaxis()->SetTitle(x_axis_title);
  dummy_h.GetYaxis()->SetTitle(y_axis_title);
  
  // Make the bands
  TGraphErrors* func1_band = make_band(func1);
  TGraphErrors* func2_band = make_band(func2);
  
  TCanvas c;
  dummy_h.Draw();
  g1->Draw("PESAME");
  g2->Draw("PESAME");
  func1.Draw("Same");
  func2.Draw("Same");
  func1_band->Draw("SameE3");
  func2_band->Draw("SameE3");
  
  // add a bit of space
  c.SetLeftMargin(c.GetLeftMargin()+0.02);
  c.SetTopMargin(c.GetTopMargin()+0.05);
  c.SetBottomMargin(c.GetBottomMargin()+0.02);
  
  // TLatex'es
  TLatex lumi_latex(.18, .93 , "#scale[.6]{#int L = 36.1 pb^{-1}}");
  lumi_latex.SetNDC();
  
  TLatex cem_latex(.78, .93 , "#scale[.6]{#sqrt{s}= 7 TeV}");
  cem_latex.SetNDC();  

  TLatex comment_latex(.22, .8 ,m_comment);
  comment_latex.SetNDC();

  TString ptbin_str(m_pt_bin);
  ptbin_str.ReplaceAll("Pt","#scale[.6]{");
  ptbin_str.ReplaceAll("to"," < p_{T}^{Z} < ");
  ptbin_str+="}";
  TLatex ptbin_latex(.22, .25 ,ptbin_str);
  ptbin_latex.SetNDC();  
  
  lumi_latex.Draw("Same");
  cem_latex.Draw("Same");
  comment_latex.Draw("Same");
  ptbin_latex.Draw("Same");

  // Make Legend
  TLegend legend(.65,.7,.88,.85);
  legend.SetFillColor(kWhite);
  legend.SetHeader(leg_header);
  legend.AddEntry(g1,g1->GetName());
  legend.AddEntry(g2,g2->GetName());  
  legend.Draw("Same");  
  
  dummy_h.GetYaxis()->SetRangeUser(y_min*0.7,y_max*1.3);
  
  c.Print(m_prefix+"_"+name+".png");

  };    

//------------------------------------------------------------------------------        
    
};

//------------------------------------------------------------------------------

int main(int argc, char** argv){

setTDRStyle();
gStyle->SetOptStat(0);
gStyle->SetOptFit(0);

if (argc<3){
    cout << "Usage: " << argv[0] << " cfgfile.py section_name\n";
    return -1;
    }
    
TString cfg_name(argv[1]);

for (int iarg=2;iarg<argc;iarg++){

  TString title(argv[iarg]);

  //----------------

  MinimalParser p(cfg_name);
  p.setVerbose(false);

  TString filename_data (p.getString(title+".filename_data"));
  TString filename_mc (p.getString(title+".filename_mc"));

  TString comment (p.getString(title+".comment"));

  vdouble cut_vals (p.getvDouble(title+".cut_vals"));
  TString cut_name (p.getString(title+".cut_name"));

  vString  balance_histo_names_data(p.getvString(title+".balance_histo_names_data"));
  vString mpf_histo_names_data(p.getvString(title+".mpf_histo_names_data"));

  vString balance_histo_names_mc(p.getvString(title+".balance_histo_names_mc"));
  vString mpf_histo_names_mc(p.getvString(title+".mpf_histo_names_mc"));

  vString pt_bins(p.getvString(title+".pt_bins"));
    
  //----------------

  vString::iterator ibin;
  for (ibin=pt_bins.begin();ibin!=pt_bins.end();ibin++){

    extrapolator the_extrapolator(*ibin,
                                  title,
                                  filename_data,filename_mc,
                                  comment,
                                  cut_name, cut_vals,
                                  balance_histo_names_data,mpf_histo_names_data,
                                  balance_histo_names_mc,mpf_histo_names_mc);
    the_extrapolator.extrapolate();
    } // end loop on pt bins

  } 

}












