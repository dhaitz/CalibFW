#include <iostream>

#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TString.h"
#include "TLatex.h"
#include "TCanvas.h"

#include "MinimalParser.h"


// Include the CMSStyle

using namespace std;

void read_histos(TString& filename,
                 TString& data_h_name,
                 TH1D*& data_h,
                 TString& mc_h_name,
                 TH1D*& mc_h){

  TFile file(filename);
  file.cd();
  data_h = new TH1D(*(TH1D*) file.Get(data_h_name));
  data_h->SetDirectory(gROOT);
  mc_h = new TH1D(*(TH1D*) file.Get(mc_h_name));
  mc_h->SetDirectory(gROOT);  
  file.Close();
  }

void process_sys(TString sys_name,
                 TH1D nominal_histo_data,
                 TH1D nominal_histo_mc,
                 TH1D varied_histo_data,
                 TH1D varied_histo_mc,
                 TF1& fit_f){
    
    nominal_histo_data.Rebin(8);
    nominal_histo_mc.Rebin(8);
    varied_histo_data.Rebin(8);
    varied_histo_mc.Rebin(8);
    
    // Divide histos
    nominal_histo_data.Sumw2();
    nominal_histo_mc.Sumw2();
    nominal_histo_data.Divide(&nominal_histo_mc);

    varied_histo_data.Sumw2();
    varied_histo_mc.Sumw2();
    varied_histo_data.Divide(&varied_histo_mc);
        
    // The histo to be plotted
    nominal_histo_data.Divide(&varied_histo_data);
    
    // Subtract one
    TH1D one_histo (nominal_histo_data);
    one_histo.SetName("one_histo");
    one_histo.Reset();
    for (int i=0;i<one_histo.GetNbinsX();++i)
      one_histo.SetBinContent(i,1.0);
    nominal_histo_data.Add(&one_histo,-1);    
    
    nominal_histo_data.SetMarkerSize(2);
    nominal_histo_data.SetMarkerStyle(8);
    nominal_histo_data.SetLineWidth(2);
    nominal_histo_data.SetLineColor(0);
    
    fit_f.SetLineColor(kRed);
    nominal_histo_data.Fit(&fit_f);
    
    TLatex lat(.2,.8,sys_name);
    lat.SetNDC();
    
    TCanvas c;
    nominal_histo_data.Draw("");
    fit_f.Draw("SameL");
    lat.Draw("Same");
    c.Print(sys_name+".png");
    
    };

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

TString out_filename = p.getString(title+".out_filename");
vString image_formats = p.getvString(title+".image_formats");

//------------------------------------------------------------------------------
// Read the Histos and Process the systematics

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
             *nominal_data_h,*nominal_mc_h,
             *pt_ratio_plus_data_h,*pt_ratio_plus_mc_h,
             pt_ratio_plus_fit);            
            
TH1D *pt_ratio_minus_data_h,*pt_ratio_minus_mc_h;
read_histos(pt_ratio_minus_filename,
            data_histo_name,pt_ratio_minus_data_h,
            mc_histo_name,pt_ratio_minus_mc_h);

process_sys ("pt_ratio_variation_minus",
             *nominal_data_h,*nominal_mc_h,
             *pt_ratio_minus_data_h,*pt_ratio_minus_mc_h,
             pt_ratio_minus_fit);
            
// Muons pt cut            
TH1D *muons_pt_plus_data_h,*muons_pt_plus_mc_h;
read_histos(muons_pt_plus_filename,
            data_histo_name,muons_pt_plus_data_h,
            mc_histo_name,muons_pt_plus_mc_h);
            
process_sys ("muons_pt_variation_plus",
             *nominal_data_h,*nominal_mc_h,
             *muons_pt_plus_data_h,*muons_pt_plus_mc_h,
             muons_pt_plus_fit);          
            
TH1D *muons_pt_minus_data_h,*muons_pt_minus_mc_h;
read_histos(muons_pt_minus_filename,
            data_histo_name,muons_pt_minus_data_h,
            mc_histo_name,muons_pt_minus_mc_h);

process_sys ("muons_pt_variation_minus",
             *nominal_data_h,*nominal_mc_h,
             *muons_pt_minus_data_h,*muons_pt_minus_mc_h,
             muons_pt_minus_fit);             
             
// Z mass cut             
TH1D *z_mass_plus_data_h,*z_mass_plus_mc_h;
read_histos(z_mass_plus_filename,
            data_histo_name,z_mass_plus_data_h,
            mc_histo_name,z_mass_plus_mc_h);

process_sys ("z_mass_variation_plus",
             *nominal_data_h,*nominal_mc_h,
             *z_mass_plus_data_h,*z_mass_plus_mc_h,     
             z_mass_plus_fit);            
            
TH1D *z_mass_minus_data_h,*z_mass_minus_mc_h;
read_histos(z_mass_minus_filename,
            data_histo_name,z_mass_minus_data_h,
            mc_histo_name,z_mass_minus_mc_h);

process_sys ("z_mass_variation_minus",
             *nominal_data_h,*nominal_mc_h,
             *z_mass_minus_data_h,*z_mass_minus_mc_h,
             z_mass_minus_fit);        

// Deltaphi Cut             
TH1D *deltaphi_plus_data_h,*deltaphi_plus_mc_h;
read_histos(deltaphi_plus_filename,
            data_histo_name,deltaphi_plus_data_h,
            mc_histo_name,deltaphi_plus_mc_h);
            
process_sys ("deltaphi_variation_plus",
             *nominal_data_h,*nominal_mc_h,
             *deltaphi_plus_data_h,*deltaphi_plus_mc_h,
             deltaphi_plus_fit);                   
            
TH1D *deltaphi_minus_data_h,*deltaphi_minus_mc_h;
read_histos(deltaphi_minus_filename,
            data_histo_name,deltaphi_minus_data_h,
            mc_histo_name,deltaphi_minus_mc_h);            

process_sys ("deltaphi_variation_minus",
             *nominal_data_h,*nominal_mc_h,
             *deltaphi_minus_data_h,*deltaphi_minus_mc_h,
             deltaphi_minus_fit);              

//------------------------------------------------------------------------------

// Now combine the systematics

// Plot the response with the uncertainty band
             

             


}