#include <iostream>
#include <iomanip>

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
#include "TKey.h"

#include "TMatrixTSym.h"

using namespace common_functions;

struct ks_report_element {
TString name;
double ks_pythia_val;
double ks_herwig_val;
};
typedef std::vector<ks_report_element> reportv;

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
    text += herwig_color + "]{P-val = " + herwig_ks + "}";
    }  
    
  text+="}}}";

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

int get_minimum_divisor(int val){
  bool found=false;
  int divisor=1;
  while (not found){
    divisor++;
    found = (val%divisor == 0);
    }
  return divisor;
  }

//------------------------------------------------------------------------------

ks_report_element drawHistos(TString name,
                TString x_axis_name,
                TString y_axis_name,
                TString legend_header,
                TString comment,
                TH1D* datah, TString data_name,
                TH1D* pythiah, TString pythia_name,
                TH1D* herwigh= NULL, TString herwig_name="",
                bool norm_all = false){
  
  if (norm_all){
    int divisor = get_minimum_divisor(datah->GetNbinsX());
    datah->Rebin(divisor);    
    datah->Scale(1./36.,"width");
    //datah->Scale(1.,"width");
    pythiah->Rebin(divisor); 
//     pythiah->Scale(1./pythiah->GetSumOfWeights(),"width");
    pythiah->Scale(1.,"width");
    if(herwigh){
      herwigh->Rebin(divisor);
//       herwigh->Scale(1./herwigh->GetSumOfWeights(),"width");
      herwigh->Scale(1.,"width");      
      }
    }
  
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
                             herwigh,herwig_name,
                             "ur");
  
  // Set First bins to 0
  if (not norm_all)
  for (int i=1;datah->GetBinCenter(i)<20;++i){
    datah->SetBinContent(i,0);
    datah->SetBinError(i,0);
    pythiah->SetBinContent(i,0);
    pythiah->SetBinError(i,0);
    if (herwigh!=0){
          herwigh->SetBinContent(i,0);
          herwigh->SetBinError(i,0);
          }
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
  pythiah->Draw("HistESame");
  if (herwigh!=NULL) herwigh->Draw("HistESame");
  
  add_topline(c);
  
  leg->Draw();
  
  commentlatex.Draw();
  
  ks_summary->Draw();
    
  c.Print(name+".png");
  
  delete ks_summary;
  delete leg;

  ks_report_element rep_el;
  rep_el.name = name;
  rep_el.ks_pythia_val = ks_pythia;
  rep_el.ks_herwig_val = ks_herwig;
  return rep_el;
  
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
  int skip_bins=0; // for the first bin!0-15 for example
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
TString get_plot_nick_name(TString& plot_name){
  TString nick(plot_name);
  nick.ReplaceAll("_Zplusjet_data_hist","");  
  return nick;
  }
//------------------------------------------------------------------------------
TString mcfyname(TString& data_name){
  TString mc_name(data_name);
  mc_name.ReplaceAll("_data_","_mc_");  
  mc_name.ReplaceAll("L3Res","L3");  
  return mc_name;
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

  
reportv report;

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
  
  //----------------
  
  TString data_correction_level(correction_level);
  if (data_correction_level.Contains("L1L2L3")){
    cout << "\n\n *** Adjusting correction level on data to L1L2L3Res...\n";
      data_correction_level = "L1L2L3Res";
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
  
  report.push_back(drawHistos(plot_hdr+"calo_PF_ratio_vs_pf_pt",
             "p_{T}^{PF}",
             "<p_{T}^{Calo}/p_{T}^{PF}>",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));
             
  histo_name_data="calo_avg_pf_avg_ratio_vs_z_pt_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="calo_avg_pf_avg_ratio_vs_z_pt_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  report.push_back(drawHistos(plot_hdr+"calo_avg_PF_avg_ratio_vs_z_pt",
             "p_{T}^{Z}",
             "<p_{T}^{Calo}>/<p_{T}^{PF}>",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));             
         
             
  //----------------
  // PF Jets Quality plots
  histo_name_data="jet1_chargedhadronenergy_fraction_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jet1_chargedhadronenergy_fraction_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  report.push_back(drawHistos(plot_hdr+"charged_hadron_energy",
             "p_{T}^{Z}",
             "Charged Hadron Energy Fraction",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));    
  
  histo_name_data="jet1_neutralhadronenergy_fraction_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jet1_neutralhadronenergy_fraction_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  report.push_back(drawHistos(plot_hdr+"neutral_hadron_energy",
             "p_{T}^{Z}",
             "Neutral Hadron Energy Fraction",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));               
  
  histo_name_data="jet1_photonenergy_fraction_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jet1_photonenergy_fraction_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  report.push_back(drawHistos(plot_hdr+"photon_hadron_energy",
             "p_{T}^{Z}",
             "Photon Energy Fraction",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));              
    
  //----------
  // The Balances now!
  histo_name_data="jetresp_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="jetresp_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  report.push_back(drawHistos(plot_hdr+"zpj_balance",
             "p_{T}^{Z}",
             "Z+jet Balance",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));  
  
  histo_name_data="mpfresp_";
  histo_name_data+=algorithm+"PFJets"+data_correction_level+"_Zplusjet_data_hist";
  
  histo_name_mc="mpfresp_";
  histo_name_mc+=algorithm+"PFJets"+correction_level+"_Zplusjet_mc_hist";
  
  report.push_back(drawHistos(plot_hdr+"zpj_mpf",
             "p_{T}^{Z}",
             "Z+jet MPF",
             "Sample:",
             comment,
             getObject<TH1D>(histo_name_data,data_ifile_name),"Data",
             getObject<TH1D>(histo_name_mc,pythia_ifile_name),"Pythia Z2",
             getObject<TH1D>(histo_name_mc,herwig_ifile_name),"Herwig++"));              

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
               "Data/Pythia Z2",
               comment,
               getObject<TH1D>(histo_name_data_bal,data_ifile_name),
               getObject<TH1D>(histo_name_data_mpf,data_ifile_name),
               getObject<TH1D>(histo_name_mc_bal,pythia_ifile_name),
               getObject<TH1D>(histo_name_mc_mpf,pythia_ifile_name));   

  make_jme1010(plot_hdr+"balance_data_over_herwig_mc",
               "p_{T}^{Z}",
               "Data/Herwig",
               comment,
               getObject<TH1D>(histo_name_data_bal,data_ifile_name),
               getObject<TH1D>(histo_name_data_mpf,data_ifile_name),
               getObject<TH1D>(histo_name_mc_bal,herwig_ifile_name),
               getObject<TH1D>(histo_name_mc_mpf,herwig_ifile_name));                
               
  
}// End loop on sectionnames in cfg

// Fill Report histos and print report...
TH1F* ks_pythia_h = plot_styles[pythia_h].setStyle<TH1F>(new TH1F("ks_pythia","ks_pythia;Kolmogorov-Smirnov outcome;dN/dTest",100,0,1));
TH1F* ks_herwig_h = plot_styles[herwig_h].setStyle<TH1F>(new TH1F("ks_herwig","ks_herwig;Kolmogorov-Smirnov outcome;dN/dTest",100,0,1));


std::cout <<  setprecision(3) // for the precision of the KS
          << "\n\nPlot Name\t\t\t\t\tPythia\t\tHerwig++" << endl
          << "-------------------------------------------------------------------------\n";
for (reportv::iterator it=report.begin();it<report.end();++it){
  int string_lenght=it->name.Length();
  std::cout << it->name.Data();
  for (int i=0;i<50-string_lenght;++i)
    std::cout << " ";

  cout << it->ks_pythia_val << "\t\t" << it->ks_herwig_val << endl;
  
  ks_pythia_h->Fill(it->ks_pythia_val);
  ks_herwig_h->Fill(it->ks_herwig_val);
  }

// Make plot for the ks_vals
TLegend leg (.2,.7,.45,.9);
leg.SetFillColor(0);
leg.SetBorderSize(0);
leg.AddEntry(ks_pythia_h,"Pythia Z2");
leg.AddEntry(ks_herwig_h,"Herwig++");
leg.SetHeader("Monte Carlo");
TCanvas c;c.cd();

ks_herwig_h->DrawNormalized("hist");
ks_pythia_h->DrawNormalized("Same");
leg.Draw();

c.Print("KS_summary.png");
  
//------------------------------------------------------------------------------

// Build list of all histos in NoBinning_incut dir in files

TString title(argv[2]);

MinimalParser p(cfg_name);
p.setVerbose(false);

TString data_ifile_name (p.getString(title+".data_ifile_name"));
TString pythia_ifile_name (p.getString(title+".pythia_ifile_name"));
TString herwig_ifile_name (p.getString(title+".herwig_ifile_name"));
bool do_mega_comparison (p.getInt(title+".do_mega_comparison"));

if(do_mega_comparison){
  TString dirname("NoBinning_incut");

    
  TFile ifile(data_ifile_name);
  TList* NoBinning_incut_keys = ((TDirectoryFile*)ifile.Get(dirname))->GetListOfKeys();

  for (int ikey=0;ikey<NoBinning_incut_keys->GetSize();++ikey){
    TString keyname (NoBinning_incut_keys->At(ikey)->GetName());
    TString keytype (((TKey*)(NoBinning_incut_keys->At(ikey)))->GetClassName());
  //   cout << keyname.Data() << " " << keytype.Data() << endl;
    TString nick;
    TString mc_keyname;
    TString y_axis_title;
    if (keytype == "TH1D"){
      nick=get_plot_nick_name(keyname);
      mc_keyname = mcfyname(keyname);
      y_axis_title="1/N #cdot dN/d";
      drawHistos(nick,
              nick,
              y_axis_title+nick,
              "Sample:",
              "#scale[.6]{Automatic check}",
              getObject<TH1D>(dirname+"/"+keyname,data_ifile_name),"Data",
              getObject<TH1D>(dirname+"/"+mc_keyname,pythia_ifile_name),"Pythia Z2",
              getObject<TH1D>(dirname+"/"+mc_keyname,herwig_ifile_name),"Herwig++",
              true);
      }
    }
  }
//------------------------------------------------------------------------------

}// end of prog

