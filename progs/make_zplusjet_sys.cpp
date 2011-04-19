#include <iostream>

#include "TFile.h"
#include "TMath.h"
#include "TLegend.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TString.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "MinimalParser.h"


// Include the CMSStyle

using namespace std;
using namespace TMath;

void read_histos(TString& filename,
                 TString& data_h_name,
                 TH1D*& data_h,
                 TString& mc_h_name,
                 TH1D*& mc_h){

    cout << " o Reading from file " <<  filename.Data() << ": "
         << data_h_name.Data() << "," << mc_h_name.Data() << " \n";
  
  TFile file(filename);
  file.cd();
  data_h = new TH1D(*(TH1D*) file.Get(data_h_name));
  data_h->SetDirectory(gROOT);
  mc_h = new TH1D(*(TH1D*) file.Get(mc_h_name));
  mc_h->SetDirectory(gROOT);  
  file.Close();
  }
  
//------------------------------------------------------------------------------

void process_sys(TString sys_name,
                 TString sys_description,
                 TH1D stats_nominal_histo_data,
                 TH1D stats_varied_histo_data,
                 TH1D nominal_histo_data,
                 TH1D nominal_histo_mc,
                 TH1D varied_histo_data,
                 TH1D varied_histo_mc,
                 TF1& fit_f){
    
    sys_description="#scale[.8]{#splitline{"+sys_description;
    sys_description+="}{influence on jet response}}";
  
    cout << " o Processing Systematic "<< sys_name.Data() << " \n";
    
    fit_f.SetLineWidth(3);
    
    // Divide histos
    nominal_histo_data.Sumw2();
    nominal_histo_mc.Sumw2();
    nominal_histo_data.Divide(&nominal_histo_mc);        
    
    //varied_histo_data.Sumw2();
    //varied_histo_mc.Sumw2();
    varied_histo_data.Divide(&varied_histo_mc);
    
    for (int ibin=1;ibin<=nominal_histo_data.GetNbinsX();++ibin){
      double n0=stats_nominal_histo_data.GetBinContent(ibin);
      double nprime=stats_varied_histo_data.GetBinContent(ibin);
      
      varied_histo_data.SetBinError(ibin,0);
      
      double error=nominal_histo_data.GetBinError(ibin)*sqrt( 1 + n0 / nprime);
      
      cout << "delta, n0, nprime, error " 
           << nominal_histo_data.GetBinError(ibin) << " "
           << n0 << " " << nprime << " " << error << endl;
      nominal_histo_data.SetBinError(ibin,error);
      }

    // The histo to be plotted
    nominal_histo_data.Divide(&varied_histo_data);
    
    
    // Subtract one
    TH1D one_histo (nominal_histo_data);
    one_histo.SetName("one_histo");
    one_histo.Reset();
    for (int i=1;i<=one_histo.GetNbinsX();++i)
      one_histo.SetBinContent(i,1.0);
    nominal_histo_data.Add(&one_histo,-1);
    
    nominal_histo_data.Scale(100);    // PERCENT!
    
    nominal_histo_data.SetMarkerStyle(8);
    nominal_histo_data.SetMarkerSize(2.2);
    nominal_histo_data.SetMarkerColor(kBlue);
//     nominal_histo_data.SetLineWidth(2);
    nominal_histo_data.SetLineColor(kBlack);
    
    fit_f.SetLineColor(kRed);
    nominal_histo_data.Fit(&fit_f);

    TLatex lat(.25,.84,sys_description);
    lat.SetNDC();
    
    // Maquillage
    nominal_histo_data.GetXaxis()->SetTitle("P_{T}^{Z}");
    nominal_histo_data.GetYaxis()->SetTitle("(Varied - Nominal)/Nominal %");
    nominal_histo_data.GetYaxis()->SetTitleOffset(1.5);
    nominal_histo_data.GetYaxis()->SetRangeUser(-7.99,7.99);    
    
    TCanvas c;
    c.SetLeftMargin(.2);
    nominal_histo_data.Draw("PE");
    fit_f.Draw("SameL");
    lat.Draw("Same");
    c.Print(sys_name+".png");
    
    };
    
//------------------------------------------------------------------------------

double getMeanAbsVal(TF1 plus_fit,TF1 minus_fit,double x){
//     cout << "[getMeanAbsVal] x= "<< x << "\n";
    double plus_val=Abs(plus_fit.Eval(x));
    double minus_val = Abs(minus_fit.Eval(x));
    // Specific for linear fits!!!
    if (plus_fit.GetNpar() == 1 and plus_fit.GetParameter(0) < plus_fit.GetParError(0)){
        plus_val=plus_fit.GetParError(0);
        std::cout << "Warning: fit function " <<  plus_fit.GetName() << " is a Pol0.\n"
                  << "The parameter value is smaller than its error.\n"
                  << "Assuming as value the error to be conservative!!\n\n";
      }

    if (minus_fit.GetNpar() == 1 and minus_fit.GetParameter(0) < minus_fit.GetParError(0)){
        minus_val=minus_fit.GetParError(0);
        std::cout << "Warning: fit function " <<  minus_fit.GetName() << " is a Pol0.\n"
                  << "The parameter value is smaller than its error.\n"
                  << "Assuming as value the error to be conservative!!\n\n";
      }

    return .5 * (plus_val+ minus_val);
}

//------------------------------------------------------------------------------

void computeErrors(TH1D*& jetresp,
                   TH1D*& jetresp_sys,
                   TH1D*& jetresp_statsys,
                   TH1D* nominal_data_h,
                   TF1 pt_ratio_plus_fit,TF1 pt_ratio_minus_fit,
                   TF1 deltaphi_plus_fit,TF1 deltaphi_minus_fit,
                   TF1 z_mass_plus_fit,  TF1 z_mass_minus_fit,
                   TF1 muons_pt_plus_fit,TF1 muons_pt_minus_fit){

  jetresp = new TH1D(*nominal_data_h);
  jetresp->SetName("jetresp");  
  
  jetresp_sys = new TH1D(*nominal_data_h);
  jetresp_sys->SetName("jetrespsys");
  
  jetresp_statsys = new TH1D(*nominal_data_h);
  jetresp_statsys->SetName("jetrespstatsys");
  
  cout << "\npippo\n";
  
  // take the mean of the effects
  for (int ibin=1;ibin <= nominal_data_h->GetNbinsX();ibin++){
    double bin_centre = nominal_data_h->GetBinCenter(ibin);
    double bin_val = nominal_data_h->GetBinContent(ibin);
    double bin_stat_e = nominal_data_h->GetBinError(ibin);
    double bin_sys_e = sqrt(pow(getMeanAbsVal(pt_ratio_plus_fit,pt_ratio_minus_fit,bin_centre),2)+
                            pow(getMeanAbsVal(deltaphi_plus_fit,deltaphi_plus_fit,bin_centre),2)+
                            pow(getMeanAbsVal(muons_pt_plus_fit,muons_pt_minus_fit,bin_centre),2)+
                            pow(getMeanAbsVal(z_mass_plus_fit,z_mass_minus_fit,bin_centre),2));
    
    // /100. is there since the fit is done on a percent plot!
    jetresp_sys->SetBinError(ibin,bin_sys_e*bin_val/100.);
    jetresp_statsys->SetBinError(ibin, bin_sys_e*bin_val/100. + bin_stat_e);
    jetresp->SetBinError(ibin,0);
    
    }

//   jetresp_sys->SetFillColor(15);
  jetresp_sys->SetFillColor(kRed-4);
//   jetresp_sys->SetFillStyle(3001);
//   jetresp_statsys->SetFillColor(18);
  jetresp_statsys->SetFillColor(kRed-10);
//   jetresp_statsys->SetFillStyle(3001);
  }

//------------------------------------------------------------------------------   


int main(int argc, char** argv) {

if (argc!=3){
    cout << "Usage: " << argv[0] << " cfgfile.py section_name\n";
    return -1;
    }
    
TString cfg_name(argv[1]);
TString title(argv[2]);

// gROOT->ProcessLine(".L /usr/users/piparo/CalibFW/macros/TDR_Style.cxx");
TString tdr_style_file(".L $FWPATH");
tdr_style_file+="/macros/TDR_Style.cxx";
gROOT->ProcessLine(tdr_style_file.Data());
gROOT->ProcessLine("TDR_Style()");

gStyle->SetOptStat(0);
gStyle->SetStatW(.30);
gStyle->SetFitFormat("5.2g");

//------------------------------------------------------------------------------

MinimalParser p(cfg_name);
p.setVerbose(false);

TString nominal_filename (p.getString(title+".nominal_filename"));

double fit_upper = p.getDouble(title+".fit_upper");                
double fit_lower = p.getDouble(title+".fit_lower");

TString data_histo_name = p.getString(title+".data_histo_name");
TString mc_histo_name = p.getString(title+".mc_histo_name");

TString pt_ratio_plus_filename = p.getString(title+".pt_ratio_plus_filename");
TString pt_ratio_minus_filename = p.getString(title+".pt_ratio_minus_filename");
TString pt_ratio_fit_func = p.getString(title+".pt_ratio_fit_func");
TF1 pt_ratio_plus_fit("pt_ratio_plus_fit",pt_ratio_fit_func,fit_lower,fit_upper);
TF1 pt_ratio_minus_fit("pt_ratio_minus_fit",pt_ratio_fit_func,fit_lower,fit_upper);

TString muons_pt_plus_filename = p.getString(title+".muons_pt_plus_filename");                                                                                                                  
TString muons_pt_minus_filename = p.getString(title+".muons_pt_minus_filename");
TString muons_pt_fit_func = p.getString(title+".muons_pt_fit_func");
TF1 muons_pt_plus_fit("muons_pt_plus_fit",muons_pt_fit_func,fit_lower,fit_upper);
TF1 muons_pt_minus_fit("muons_pt_minus_fit",muons_pt_fit_func,fit_lower,fit_upper);

TString z_mass_plus_filename = p.getString(title+".z_mass_plus_filename");
TString z_mass_minus_filename = p.getString(title+".z_mass_minus_filename");
TString z_mass_fit_func = p.getString(title+".z_mass_fit_func");
TF1 z_mass_plus_fit("z_mass_plus_fit",z_mass_fit_func,fit_lower,fit_upper);
TF1 z_mass_minus_fit("z_mass_minus_fit",z_mass_fit_func,fit_lower,fit_upper);

TString deltaphi_plus_filename = p.getString(title+".deltaphi_plus_filename");
TString deltaphi_minus_filename = p.getString(title+".deltaphi_minus_filename");                                                                                                                 
TString deltaphi_fit_func = p.getString(title+".deltaphi_fit_func");
TF1 deltaphi_plus_fit("deltaphi_plus_fit",deltaphi_fit_func,fit_lower,fit_upper);
TF1 deltaphi_minus_fit("deltaphi_minus_fit",deltaphi_fit_func,fit_lower,fit_upper);

TString z_pt_histo_name = p.getString(title+".z_pt_histo_name");

TString out_filename = p.getString(title+".out_filename");
vString image_formats = p.getvString(title+".image_formats");

//------------------------------------------------------------------------------
// Read the Histos and Process the systematics

// Read the Zpt histos
TFile file1(nominal_filename);
file1.cd();
TH1D* stats_nominal_histo_data = new TH1D(*(TH1D*) file1.Get(z_pt_histo_name));
stats_nominal_histo_data->SetDirectory(gROOT);
file1.Close();

TFile file2(nominal_filename);
file2.cd();
TH1D* stats_varied_histo_data = new TH1D(*(TH1D*) file2.Get(z_pt_histo_name));
stats_varied_histo_data->SetDirectory(gROOT);
file2.Close();

// Nominal
TH1D *nominal_data_h,*nominal_mc_h;
read_histos(nominal_filename,
            data_histo_name,nominal_data_h,
            mc_histo_name,nominal_mc_h);

// Pt ratio cut 
TH1D *pt_ratio_plus_data_h,*pt_ratio_plus_mc_h;
read_histos(pt_ratio_plus_filename,
            data_histo_name,pt_ratio_plus_data_h,
            mc_histo_name,pt_ratio_plus_mc_h);

process_sys ("pt_ratio_variation_plus",
             "P_{T}^{Jet2}/P_{T}^{Z} cut upward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,
             *nominal_data_h,*nominal_mc_h,
             *pt_ratio_plus_data_h,*pt_ratio_plus_mc_h,
             pt_ratio_plus_fit);            
            
TH1D *pt_ratio_minus_data_h,*pt_ratio_minus_mc_h;
read_histos(pt_ratio_minus_filename,
            data_histo_name,pt_ratio_minus_data_h,
            mc_histo_name,pt_ratio_minus_mc_h);

process_sys ("pt_ratio_variation_minus",
             "P_{T}^{Jet2}/P_{T}^{Z} cut downward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,
             *nominal_data_h,*nominal_mc_h,
             *pt_ratio_minus_data_h,*pt_ratio_minus_mc_h,
             pt_ratio_minus_fit);
            
// Muons pt cut            
TH1D *muons_pt_plus_data_h,*muons_pt_plus_mc_h;
read_histos(muons_pt_plus_filename,
            data_histo_name,muons_pt_plus_data_h,
            mc_histo_name,muons_pt_plus_mc_h);
            
process_sys ("muons_pt_variation_plus",
             "P_{T}^{#mu} cut upward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,             
             *nominal_data_h,*nominal_mc_h,
             *muons_pt_plus_data_h,*muons_pt_plus_mc_h,
             muons_pt_plus_fit);          
            
TH1D *muons_pt_minus_data_h,*muons_pt_minus_mc_h;
read_histos(muons_pt_minus_filename,
            data_histo_name,muons_pt_minus_data_h,
            mc_histo_name,muons_pt_minus_mc_h);

process_sys ("muons_pt_variation_minus",
             "P_{T}^{#mu} cut downward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,             
             *nominal_data_h,*nominal_mc_h,
             *muons_pt_minus_data_h,*muons_pt_minus_mc_h,
             muons_pt_minus_fit);             
             
// Z mass cut             
TH1D *z_mass_plus_data_h,*z_mass_plus_mc_h;
read_histos(z_mass_plus_filename,
            data_histo_name,z_mass_plus_data_h,
            mc_histo_name,z_mass_plus_mc_h);

process_sys ("z_mass_variation_plus",
             "M_{Z} window cut upward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,             
             *nominal_data_h,*nominal_mc_h,
             *z_mass_plus_data_h,*z_mass_plus_mc_h,     
             z_mass_plus_fit);            
            
TH1D *z_mass_minus_data_h,*z_mass_minus_mc_h;
read_histos(z_mass_minus_filename,
            data_histo_name,z_mass_minus_data_h,
            mc_histo_name,z_mass_minus_mc_h);

process_sys ("z_mass_variation_minus",
             "M_{Z} window cut downward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,             
             *nominal_data_h,*nominal_mc_h,
             *z_mass_minus_data_h,*z_mass_minus_mc_h,
             z_mass_minus_fit);        

// Deltaphi Cut             
TH1D *deltaphi_plus_data_h,*deltaphi_plus_mc_h;
read_histos(deltaphi_plus_filename,
            data_histo_name,deltaphi_plus_data_h,
            mc_histo_name,deltaphi_plus_mc_h);
            
process_sys ("deltaphi_variation_plus",
             "#Delta#Phi cut upward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,             
             *nominal_data_h,*nominal_mc_h,
             *deltaphi_plus_data_h,*deltaphi_plus_mc_h,
             deltaphi_plus_fit);                   
            
TH1D *deltaphi_minus_data_h,*deltaphi_minus_mc_h;
read_histos(deltaphi_minus_filename,
            data_histo_name,deltaphi_minus_data_h,
            mc_histo_name,deltaphi_minus_mc_h);            

process_sys ("deltaphi_variation_minus",
             "#Delta#Phi cut downward variation",
             *stats_nominal_histo_data,*stats_varied_histo_data,             
             *nominal_data_h,*nominal_mc_h,
             *deltaphi_minus_data_h,*deltaphi_minus_mc_h,
             deltaphi_minus_fit);              

//------------------------------------------------------------------------------

// Now combine the systematics and statistical errors
TH1D *jetresp,*jetresp_sys,*jetresp_statsys;

computeErrors(jetresp,
              jetresp_sys,
              jetresp_statsys,
              nominal_data_h,
              pt_ratio_plus_fit,pt_ratio_minus_fit,
              deltaphi_plus_fit,deltaphi_minus_fit,
              z_mass_plus_fit,z_mass_minus_fit,
              muons_pt_plus_fit,muons_pt_minus_fit);

TLegend legend(.65,.25,.85,.4);
// legend.SetHeader("Jet Response");
legend.SetBorderSize(0);
legend.SetFillColor(kWhite);
legend.AddEntry(jetresp_sys,"Sys.","lf");
legend.AddEntry(jetresp_statsys,"Sys. + Stat.","lf");
              
TLatex latex(.25,.8,"#splitline{Anti-Kt 0.5 PF Jets}{Corrected for #eta dependence}");
latex.SetNDC();

TCanvas c;
c.cd();
jetresp_statsys->GetXaxis()->SetTitle("P_{T}^{Z} [GeV]");
jetresp_statsys->GetYaxis()->SetTitle("Jet Response");
jetresp_statsys->GetYaxis()->SetRangeUser(.66,1.04);
jetresp_statsys->GetXaxis()->SetRangeUser(0.,230.);
jetresp_statsys->Draw("E3");
jetresp_sys->Draw("SameE3");
jetresp->Draw("SameL");
legend.Draw("Same");
c.Print("ResponseBandPlot.png");

// Plot the response with the uncertainty band

}

