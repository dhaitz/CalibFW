#include <iostream>

#include "common_functions.h"
#include "MinimalParser.h"

#include "TCanvas.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"

#include "TMatrixTSym.h"

using namespace common_functions;

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

enum plot_styles_enum { 
     data,
     pythia_h,
     herwig_h,
     balance_g,
     mpf_g};


plot_style plot_styles[5] = {plot_style(8,1.5,2,1),
                             plot_style(0,0,2,kRed), // No points for them
                             plot_style(0,0,2,kBlue),// No points for them
                             plot_style(21,1.5,2,kBlue),
                             plot_style(8,1.5,2,kRed)};  

//------------------------------------------------------------------------------

TLegend* buildLegend(TString header,
                     TObject* obj1, TString obj1_name,
                     TObject* obj2, TString obj2_name,
                     TObject* obj3, TString obj3_name,
                     TString position){

  
  TLegend* l = 0;
  if (position.Contains("ur"))
    l = new TLegend(.65,.7,.85,.85);
  if (position.Contains("ul"))
    l = new TLegend(.2,.7,.35,.85);
  if (position.Contains("br"))
    l = new TLegend(.7,.2,.85,.35);  
  if (position.Contains("bl"))
    l = new TLegend(.2,.2,.35,.35);  
  
  l->SetFillColor(kWhite);
  l->SetBorderSize(0);
  
  l->SetHeader(header);
  l->AddEntry(obj1,obj1_name);
  l->AddEntry(obj2,obj2_name);
  if (obj3) l->AddEntry(obj3,obj3_name);
  
  
  return l;
  
  };

//------------------------------------------------------------------------------  
TString make_comment(TString& correction_level,TString& algorithm){

  TString comment("#scale[.6]{#splitline{");
  
  // Beautiful algo
  if (algorithm.Contains("ak"))
    comment+="Anti-Kt ";
  else if (algorithm.Contains("kt"))
    comment+="Kt ";
  else
    comment+=algorithm;
  
  // Beautiful R
  if (algorithm.Contains("4"))
    comment+="R=0.4 ";
  else if (algorithm.Contains("5"))
    comment+="R=0.5 ";
  else if (algorithm.Contains("6"))
    comment+="R=0.6 ";
  else if (algorithm.Contains("7"))
    comment+="R=0.7 ";  
  
  comment+="Particle Flow}{";
  
  if (correction_level.Contains("L1L2L3"))
    comment+="Fully Corrected Jets";
  else if (correction_level.Contains("L1"))
    comment+="Jets corrected for PU offset";
  
  comment+="}}";//close scale and splitline
  
  return comment;

  }  
  
//------------------------------------------------------------------------------
void add_topline(TCanvas& c){

  c.cd();
  
  // add a bit of space
  c.SetLeftMargin(c.GetLeftMargin()+0.02);
  c.SetTopMargin(c.GetTopMargin()+0.05);
  c.SetBottomMargin(c.GetBottomMargin()+0.02);
  
  TLatex lumi_latex(.18, .93 , "#scale[.6]{#int L = 36 pb^{-1}}");
  lumi_latex.SetNDC();
  lumi_latex.DrawClone();
  
  TLatex cem_latex(.78, .93 , "#scale[.6]{#sqrt{s}= 7 TeV}");
  cem_latex.SetNDC();    
  cem_latex.DrawClone();
  
  
  }

//------------------------------------------------------------------------------

TLatex* make_ks_summary(TString position,
                        TH1D* pythiah,double ks_pythia,
                        TH1D* herwigh=NULL,double ks_herwig=0){

  TString pythia_color("");
  pythia_color += pythiah->GetLineColor();
  
  TString pythia_ks("");
  pythia_ks += ks_pythia;
  pythia_ks = pythia_ks(0,5);
    
  TString text("#scale[.6]{#splitline{Kolmogorov:}{#splitline{#color[");
  text += pythia_color + "]{P-val = " + pythia_ks + "}}{";
  
  if(herwigh){
    TString herwig_color("");
    TString herwig_ks("");    
    herwig_color += herwigh->GetLineColor();
    herwig_ks += ks_herwig;
    herwig_ks = herwig_ks(0,5);    
    text += "#color[";
    text += herwig_color + "]{P-val = " + herwig_ks + "}}";
    }  
  else
    text+="}}";
    
  text+="}";

  TLatex* latex = new TLatex(0,0,text.Data());
  latex->SetNDC();
  if (position.Contains("ur")){
    latex->SetX(.7);
    latex->SetY(.7);
    }

  if (position.Contains("ul")){
    latex->SetX(.25);
    latex->SetY(.7);
    }

  if (position.Contains("br")){
    latex->SetX(.7);
    latex->SetY(.25);
    }

  if (position.Contains("bl")){
    latex->SetX(.25);
    latex->SetY(.25);
    }
  
  return latex;
  
  }

//------------------------------------------------------------------------------

void drawHistos(TString name,
                TString x_axis_name,
                TString y_axis_name,
                TString legend_header,
                TString comment,
                TH1D* datah, TString data_name,
                TH1D* pythiah, TString pythia_name,
                TH1D* herwigh= NULL, TString herwig_name=""
                ){
  
  // Title offset
  datah->GetYaxis()->SetTitleOffset(1.5);
  
  // GetMaxMin
  double minx,maxx,miny,maxy;
  getMinMax(minx,maxx,miny,maxy,
            datah,pythiah,herwigh);            
  
            
  datah->GetYaxis()->SetRangeUser(miny,maxy); 
  datah->GetXaxis()->SetRangeUser(minx,maxx);           
            
  // Set The Styles
  datah = plot_styles[data].setStyle<TH1D>(datah);
  pythiah = plot_styles[pythia_h].setStyle<TH1D>(pythiah);
  if (herwigh!=NULL) herwigh = plot_styles[herwig_h].setStyle<TH1D>(herwigh);
  
  // Make the first drawn beautiful
  datah->GetXaxis()->SetTitle(x_axis_name);
  datah->GetYaxis()->SetTitle(y_axis_name);
  
  // The Legend
  TLegend* leg = buildLegend(legend_header,
                             datah,data_name,
                             pythiah,pythia_name,
                             herwigh,data_name,
                             "ur");
  
  // Set First bin to 0
  datah->SetBinContent(1,0);
  datah->SetBinError(1,0);
  pythiah->SetBinContent(1,0);
  pythiah->SetBinError(1,0);
  if (herwigh!=0){
        herwigh->SetBinContent(1,0);
        herwigh->SetBinError(1,0);
        }
 
  // KS tests
  double ks_pythia = datah->KolmogorovTest(pythiah,"D");
  double ks_herwig = 0;
  if (herwigh!=0)
    ks_herwig =  datah->KolmogorovTest(herwigh,"D");
 
  // Build the KS summary in TLatex
  TLatex* ks_summary = make_ks_summary("br",
                                        pythiah,ks_pythia,
                                        herwigh,ks_herwig);
  
                                        
  // Latex for the comment
  TLatex commentlatex(.2,.8,comment);
  commentlatex.SetNDC();
      
  // The Draw Opts                             
  TCanvas c;
  c.cd();
  
//   c.SetLogx();
  

  datah->Draw("PE1");
  pythiah->Draw("HistSame");
  if (herwigh!=NULL) herwigh->Draw("HistSame");
  
  add_topline(c);
  
  leg->Draw();
  
  commentlatex.Draw();
  
  ks_summary->Draw();
    
  c.Print(name+".png");
  
  delete ks_summary;
  delete leg;
  
  
  }

//------------------------------------------------------------------------------

void make_jme1010(TString name,
                  TString x_axis_name,
                  TString y_axis_name,
                  TString comment,
                  TH1D* data_bal,
                  TH1D* data_mpf,
                  TH1D* mc_bal,
                  TH1D* mc_mpf){

  // Title offset
  data_bal->GetYaxis()->SetTitleOffset(1.5);         
  data_bal->GetYaxis()->SetRangeUser(.9,1.1); 
               
  // Set The Styles
  data_bal = plot_styles[balance_g].setStyle<TH1D>(data_bal);
  data_mpf = plot_styles[mpf_g].setStyle<TH1D>(data_mpf);
    
  // The Legend
  TLegend* leg = buildLegend("Z+jet",
                             data_bal,"p_{T} Balance",
                             data_mpf,"MPF",
                             0,"",
                             "ur");
  // Divide the histos!
  data_bal->Divide(mc_bal);
  data_mpf->Divide(mc_mpf);
  
  // Build a mega TGraphErrors with all points and errors!
  int skip_bins=1; // for the first bin!0-15 for example
  const int nbins=data_bal->GetNbinsX();
  TGraphErrors megag((nbins-skip_bins)*2);
  
  int graph_point=0;
  for (int ibin=1+skip_bins;ibin<=nbins;ibin++){
    megag.SetPoint(graph_point,data_bal->GetBinCenter(ibin),data_bal->GetBinContent(ibin));
    megag.SetPointError(graph_point,0,data_bal->GetBinError(ibin));

    megag.SetPoint(nbins+graph_point-skip_bins,data_mpf->GetBinCenter(ibin),data_mpf->GetBinContent(ibin));
    megag.SetPointError(nbins+graph_point-skip_bins,0,data_mpf->GetBinError(ibin));    
    
    graph_point++;
    
    }
 
  megag.Print();
  
  TGraphErrors megag_dummy(megag);
  megag_dummy.SetPoint(0,-100,0);
  megag_dummy.SetPoint(1,400,2);
  
  // Make the first drawn beautiful
  megag_dummy.GetXaxis()->SetTitle(x_axis_name);
  megag_dummy.GetYaxis()->SetTitle(y_axis_name);  
  megag_dummy.GetXaxis()->SetMoreLogLabels();
  
  // Perform the fit!
  TF1 fitf("FittingFunction", "[0]+x*[1]", 20, 350);
  fitf.SetLineWidth(2);
  TFitResultPtr fitresp = megag.Fit(&fitf,"LVSR");
  
  // The band(s)
  TMatrixDSym cov((*fitresp).GetCovarianceMatrix());
  TGraphErrors* yellowband = make_band(fitf,cov);
  yellowband->SetFillColor(kYellow);
  yellowband->SetFillStyle(1);
  
  TGraph** updownband = make_bands_up_down(fitf,cov);
  
  // Latex for the comment
  TLatex commentlatex(.2,.8,comment);
  commentlatex.SetNDC();  
  
  // Draw Everything
                       
  TCanvas c;
  c.cd();
  
  c.SetLogx();  
  
  megag_dummy.Draw("APE");
  megag.Draw("SamePE");  // To be sure of the procedure  
  yellowband->Draw("E4");
  fitf.Draw("CSame");
  updownband[0]->Draw("SameC");
  updownband[1]->Draw("SameC");
  data_bal->Draw("PE1Same");
  data_mpf->Draw("PE1Same");
  add_topline(c);
  megag_dummy.GetYaxis()->SetRangeUser(.891,1.09);
  megag_dummy.GetXaxis()->SetRangeUser(20,400);
  leg->Draw();
  
  commentlatex.Draw();
    
  c.Print(name+".png");
  
  delete leg;
  
  
  }
                  

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

  TString algorithm (p.getString(title+".algorithm"));
  TString correction_level (p.getString(title+".correction_level"));
  
  TString data_ifile_name (p.getString(title+".data_ifile_name"));
  TString pythia_ifile_name (p.getString(title+".pythia_ifile_name"));
  TString herwig_ifile_name (p.getString(title+".herwig_ifile_name"));
  
//   bool do_herwig (p.getInt(title+".do_herwig"));
  
  //----------------
  
  TString data_correction_level(correction_level);
  if (data_correction_level.Contains("L1L2L3")){
    cout << "Adjusting correction level on data to L1L2L3Res...\n";
//     data_correction_level = "L1L2L3Res";
    }
    
  
  //----------------

  // Build header for Plots
  TString plot_hdr(algorithm+"_"+correction_level+"_");

  //----------------
  // PF Ratios Plots

  TString histo_name_data("calo_pf_avg_ratio_vs_pf_pt_");
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  TString histo_name_mc("calo_pf_avg_ratio_vs_pf_pt_");
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  TString comment (make_comment(correction_level,algorithm));
  
  drawHistos(plot_hdr+"calo_PF_ratio_vs_pf_pt",
             "p_{T}^{PF}",
             "<p_{T}^{Calo}/p_{T}^{PF}>",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");
  
  histo_name_data="calo_avg_pf_avg_ratio_vs_z_pt_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="calo_avg_pf_avg_ratio_vs_z_pt_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  drawHistos(plot_hdr+"calo_avg_PF_avg_ratio_vs_z_pt",
             "p_{T}^{Z}",
             "<p_{T}^{Calo}>/<p_{T}^{PF}>",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");             
             
  //----------------
  // PF Jets Quality plots
  histo_name_data="jet1_chargedhadronenergy_fraction_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jet1_chargedhadronenergy_fraction_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  drawHistos(plot_hdr+"charged_hadron_energy",
             "p_{T}^{Z}",
             "Charged Hadron Energy Fraction",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");    

  histo_name_data="jet1_neutralhadronenergy_fraction_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jet1_neutralhadronenergy_fraction_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  drawHistos(plot_hdr+"neutral_hadron_energy",
             "p_{T}^{Z}",
             "Neutral Hadron Energy Fraction",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");               

  histo_name_data="jet1_photonenergy_fraction_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jet1_photonenergy_fraction_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  drawHistos(plot_hdr+"photon_hadron_energy",
             "p_{T}^{Z}",
             "Photon Energy Fraction",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");              
  
  //----------
  // The Balances now!
  histo_name_data="jetresp_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jetresp_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  drawHistos(plot_hdr+"zpj_balance",
             "p_{T}^{Z}",
             "Z+jet Balance",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");  

  histo_name_data="mpfresp_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="mpfresp_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  drawHistos(plot_hdr+"zpj_mpf",
             "p_{T}^{Z}",
             "Z+jet MPF",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++");              
 
  //----------
  // Specific Plots: "Response" Data/MC logX fit Fig9 JME-10-10

  TString histo_name_data_bal="jetresp_";
  histo_name_data_bal+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";  

  TString histo_name_data_mpf(histo_name_data_bal);
  histo_name_data_mpf.ReplaceAll("jetresp","mpfresp");
  
  TString histo_name_mc_bal="jetresp_";
  histo_name_mc_bal+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";  

  TString histo_name_mc_mpf(histo_name_mc_bal);
  histo_name_mc_mpf.ReplaceAll("jetresp","mpfresp");  
  
  make_jme1010(plot_hdr+"balance_data_over_pythia_mc",
               "p_{T}^{Z}",
               "Data/MC",
               comment,
               getObject<TH1D>(histo_name_data_bal,data_ifile_name),
               getObject<TH1D>(histo_name_data_mpf,data_ifile_name),
               getObject<TH1D>(histo_name_mc_bal,pythia_ifile_name),
               getObject<TH1D>(histo_name_mc_mpf,pythia_ifile_name));   
 
  
}// End loop on sectionnames in cfg
  
  
}// end of prog
                    