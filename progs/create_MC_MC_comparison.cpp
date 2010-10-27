#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <stdlib.h>

#include "TROOT.h"
#include "TMinuit.h"
#include "TString.h"
#include "TF2.h"
#include "TH1F.h"
#include "TLine.h"
#include "TFile.h"
#include "TTree.h"
#include "TParticle.h"
#include "TAxis.h"
#include "TMath.h"
#include "TList.h"
#include "TGraphErrors.h"


#include "CanvasHolder.h"
#include "MinimalParser.h"



typedef std::vector<TF1> vTF1;
typedef std::vector< vdouble > vvdouble;



class  MyLikelihood {

public:

    MyLikelihood(vvdouble vresponses, vTF1 functions,int skip_bins=0) {
        m_vresponses=vresponses;
        m_functions=functions;
        m_skip_bins=skip_bins;
        m_minimum=0;
    };

    // use constructor to customize your function object
    double Evaluate(double *x, double *p) {
        // function implementation using class data members

        double nll=0;
        for (int ibin=m_skip_bins;ibin<m_vresponses.size();ibin++) {

            m_functions[ibin].SetParameter(3,x[0]);
            m_functions[ibin].SetParameter(4,x[1]);

            //          m_functions[ibin].Dump();

            vdouble responses(m_vresponses[ibin]);
            for (int iresp=0;iresp<responses.size();++iresp) {
                double partial_L=m_functions[ibin].Eval(responses[iresp]);
                // std::cout << "Partial L  = " << partial_L << std::endl;
                nll+= -1 * TMath::Log(partial_L);
            }// end loop on responses
        } // end loop on bins
        // std::cout << "nll val = " << nll << std::endl;

        //double ret_val=x[0]*x[0]+x[1]*x[1];
        //std::cout << "NLL val = " << ret_val << std::endl;

//         return ret_val;
        return nll-m_minimum;
    }

    void setMinimum(double min) {
        m_minimum=min;
    };

private:
    vvdouble m_vresponses;
    vTF1  m_functions;
    int   m_skip_bins;
    double m_minimum;
};

//------------------------------------------------------------------------------

class pt_interval {
public:
    pt_interval(double the_min, double the_max) {
        min=the_min;
        max=the_max;
    };
    bool contains(double n) {
        if (n<=max && n>min) {
            std::cout << min << " < " << n << " < " << max << "\n";
            return true;
        }
        return false;
    };
    TString id() {
        TString id("Pt");
        id+=(int)min;
        id+="to";
        id+=(int)max;
        return id;
    };
    TString good_id() {
        TString id("");
        id+=(int)min;
        id+=" < p_{T}^{Z} < ";
        id+=(int)max;
        return id;
    };
private:
    double min;
    double max;
} ;
typedef std::vector<pt_interval> Intervals;
//------------------------------------------------------------------------------

class point {
public:
    point(double the_x, double the_y) {
        x=the_x;
        y=the_y;
    };
    double x;
    double y;
} ;
typedef std::vector<point> Points;
//------------------------------------------------------------------------------


double getX(TF1& func,double zval,double yval,double minXval,double maxXval);
double getY(TF1& func,double zval,double xval,double minYval,double maxYval);
class pt_interval;
typedef std::vector<pt_interval> Intervals;
class point;
typedef std::vector<point> Points;
void getResponses(vdouble& responses, Points& points,TFile* ifile,pt_interval interval,TString algoname);



//------------------------------------------------------------------------------

// yeah, i don't care either
int g_correction_level = 0;
TString g_sCorrectionAdd = "";
TString g_sCorrection_level = "";
vString g_img_formats;
TString g_sCut = "";
TString g_sGlobalPrefix = "";

Intervals fill_intervals(vint edges);
void fill_holder(TString h_name_mc,
                 TString h_name_data,
                 CanvasHolder& h,
                 TFile* ifileMc,
                 TFile* ifileData);

TGraphErrors* histo2graph(TH1F* histo, double xmax,double ymax);
void formatHolder(CanvasHolder& h, const char* legSym="lf",  int size=1,int lines_width=2, int skip_colors=0, bool do_flag=false);
void saveHolder(CanvasHolder &h, vString formats, bool make_log = false, TString sNamePostfix = "");

//------------------------------------------------------------------------------

int main(int argc, char **argv) {


    if (argc != 2 and argc !=3) {
        std::cout << "\nUsage:"
                  << argv[0] << " configfile [verbose def =0]\n"
                  << std::endl;
        return 1;
    }

// ###########################################################
// #############     Config         ##########################
// ###########################################################

    MinimalParser p(argv[1]);

    p.setVerbose(false);

    if (argc==3) {
        int verbosityi=atoi(argv[2]);
        if (verbosityi==1) p.setVerbose(true);
    }

    TString secname("general");
//
// Section general
//
   TString mc_input_file = p.getString(secname+".mc_input_file");
    TString data_input_file = p.getString(secname+".data_input_file");
    TString info_string = p.getString(secname+".info_string");
    g_sCut = p.getString(secname+".cut");
    vString algos = p.getvString(secname+".algos");
    vString good_algos = p.getvString(secname+".good_algos");
    vint pt_bins = p.getvInt(secname+".pt_bins");
    g_img_formats= p.getvString(secname+".img_formats");
    vint runs_with_events = p.getvInt(secname+".runs_with_evts");
    g_sGlobalPrefix = p.getString(secname + ".global_prefix" );
    
// 0 = raw
// 2 = level2
// 3 = level3
    g_correction_level = p.getInt(secname+".correction_level");

    
    
    if ( g_correction_level == 2)
    {
      g_sCorrectionAdd = "_l2corr";
        g_sCorrection_level = "corrected for #eta dependence";
    }
    

//------------------------------------------------------------------------------

    TFile* ifileMc = new TFile (mc_input_file);
    TFile* ifileData = new TFile (data_input_file);


    std::cout << "MC file : " << mc_input_file << std::endl;
    std::cout << "Data file : " << data_input_file << std::endl;


// jet, mus, Z -- eta, pt, phi

// description defaults
    double info_x=.3;
    double info_y=.78;



    for (int ialgo=0;ialgo<algos.size();++ialgo) {

        TString algo(algos[ialgo]);
        TString goodalgo(good_algos[ialgo]);

        TString the_info_string(info_string);
        the_info_string.ReplaceAll("__ALGO__",goodalgo);    	
        the_info_string.ReplaceAll("__CORR__", g_sCorrection_level);

        TString quantity;

//------------------------------------------------------------------------------
        // Eta jet
        quantity="jet1_eta_";
        CanvasHolder h_eta_jet(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_eta_jet,
                    ifileMc, ifileData);

        h_eta_jet.setTitleY("dN_{Events}/d#eta");
        h_eta_jet.setTitleX("#eta^{jet}");

        h_eta_jet.addLatex(info_x,info_y,the_info_string,true);
        h_eta_jet.scaleBoardersY(1,2);
        h_eta_jet.setBoardersX(-1.4,1.4);
//        h_eta_jet.setBoardersY(0.001,69.0);

        formatHolder(h_eta_jet);
        saveHolder(h_eta_jet,g_img_formats);

        // Phi jet
        quantity="jet1_phi_";
        CanvasHolder h_phi_jet(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_phi_jet,
                    ifileMc, ifileData);

        h_phi_jet.setTitleY("dN_{Events}/d#phi");
        h_phi_jet.setTitleX("#phi^{jet}");
        h_phi_jet.setBoardersX(-3.14,3.14);
//        h_phi_jet.setBoardersY(0.001,34.9);

        h_phi_jet.addLatex(info_x,info_y,the_info_string,true);
        h_phi_jet.scaleBoardersY(1,2);
        formatHolder(h_phi_jet);
        saveHolder(h_phi_jet,g_img_formats);

        // Pt jet
        quantity="jet1_pt_";
        CanvasHolder h_pt_jet(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_pt_jet,
                    ifileMc, ifileData);

        h_pt_jet.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_jet.setTitleX("p_{T}^{jet}[GeV]");
        h_pt_jet.setBoardersX(0,189);
//        h_pt_jet.setBoardersY(0.0099,100.0);
        //h_pt_jet.setLogY();


        h_pt_jet.addLatex(info_x,info_y,the_info_string,true);
        //h_pt_jet.scaleBoardersY(1,2.5);
        formatHolder(h_pt_jet);
        saveHolder(h_pt_jet,g_img_formats);
        /*
            // Pt jet l2 corr
            quantity="jet1_pt_";
            CanvasHolder h_pt_jet_l2(quantity+algo+"l2");
            fill_holder(quantity+algo+"Jets_Zplusjet_mc_hist",
                        quantity+algo+"Jets_Zplusjet_data_l2corr_hist",
                        h_pt_jet_l2,
                        pt_rebin_factor,
                        ifileMc, ifileData);

            h_pt_jet_l2.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
            h_pt_jet_l2.setTitleX("p_{T}^{jet}[GeV]");
            h_pt_jet_l2.setBoardersX(0,190);
            h_pt_jet_l2.setLogY();


            h_pt_jet_l2.addLatex(info_x,info_y,the_info_string,true);
            h_pt_jet_l2.scaleBoardersY(1,2.5);
            formatHolder(h_pt_jet_l2);
            saveHolder(h_pt_jet_l2,img_formats);
        */

//------------------------------------------------------------------------------

        // Eta jet2
        quantity="jet2_eta_";
        CanvasHolder h_eta_jet2(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_eta_jet2,
                    ifileMc, ifileData);

        h_eta_jet2.setTitleY("dN_{Events}/d#eta");
        h_eta_jet2.setTitleX("#eta^{jet2}");

        h_eta_jet2.addLatex(info_x,info_y,the_info_string,true);
        h_eta_jet2.scaleBoardersY(1,2);
        h_eta_jet2.setBoardersX(-1.4,1.4);
//        h_eta_jet2.setBoardersY(0.001,19.99);

        formatHolder(h_eta_jet2);
        saveHolder(h_eta_jet2,g_img_formats);

        // Phi jet2
        quantity="jet2_phi_";
        CanvasHolder h_phi_jet2(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_phi_jet2,
                    ifileMc, ifileData);

        h_phi_jet2.setTitleY("dN_{Events}/d#phi");
        h_phi_jet2.setTitleX("#phi^{jet2}");
        h_phi_jet2.setBoardersX(-3.14,3.14);
//        h_phi_jet2.setBoardersY(0.001,19.99);

        h_phi_jet2.addLatex(info_x,info_y,the_info_string,true);
        h_phi_jet2.scaleBoardersY(1,2);
        formatHolder(h_phi_jet2);
        saveHolder(h_phi_jet2,g_img_formats);

        // Pt jet2
        quantity="jet2_pt_";
        CanvasHolder h_pt_jet2(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_pt_jet2,
                    ifileMc, ifileData);

        h_pt_jet2.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_jet2.setTitleX("p_{T}^{jet2}[GeV]");
        h_pt_jet2.setBoardersX(0,189);
//        h_pt_jet2.setBoardersY(0.0099,100.0);
//        h_pt_jet2.setLogY();


        h_pt_jet2.addLatex(info_x,info_y,the_info_string,true);
        //h_pt_jet2.scaleBoardersY(1,2.5);
        formatHolder(h_pt_jet2);
        saveHolder(h_pt_jet2,g_img_formats);

//------------------------------------------------------------------------------

        // Eta Z
        quantity="zEta_";
        CanvasHolder h_eta_z(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_eta_z,
                    ifileMc, ifileData);

        h_eta_z.setTitleY("dN_{Events}/d#eta");
        h_eta_z.setTitleX("#eta^{Z}");

        h_eta_z.addLatex(info_x,info_y,the_info_string,true);
//        h_eta_z.setBoardersY(0.001,74.9);
        h_eta_z.scaleBoardersY(1,2.3);

        formatHolder(h_eta_z);
        saveHolder(h_eta_z,g_img_formats);

        // Phi z
        quantity="zPhi_";
        CanvasHolder h_phi_z(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_phi_z,
                    ifileMc, ifileData);

        h_phi_z.setTitleY("dN_{Events}/d#phi");
        h_phi_z.setTitleX("#phi^{Z}");
        h_phi_z.setBoardersX(-3.14,3.14);
        //h_phi_z.setBoardersY(0.001,74.9);

        h_phi_z.addLatex(info_x,info_y,the_info_string,true);
        h_phi_z.scaleBoardersY(1,2);

        formatHolder(h_phi_z);
        saveHolder(h_phi_z,g_img_formats);

        // Pt z
        quantity="zPt_";
        CanvasHolder h_pt_z(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_pt_z,
                    ifileMc, ifileData);

        h_pt_z.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_z.setTitleX("p_{T}^{Z}[GeV]");
        h_pt_z.setBoardersX(0,189.0);
//	h_pt_z.setLogY();
        h_pt_z.addLatex(info_x,info_y,the_info_string,true);
        //h_pt_z.scaleBoardersY(1,1.9);

        formatHolder(h_pt_z);
        saveHolder(h_pt_z,g_img_formats);

        // z mass
        quantity="zmass_";
        CanvasHolder h_mass_z(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_mass_z,
                    ifileMc, ifileData);

        h_mass_z.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_mass_z.setTitleX("M_{Z}[GeV]");
        h_mass_z.setBoardersX(60.0,120.0);
//        h_mass_z.setBoardersY(0.01,8.9);

        h_mass_z.addLatex(info_x,info_y,the_info_string,true);
        //h_mass_z.scaleBoardersY(1,1.3);

        formatHolder(h_mass_z);
        saveHolder(h_mass_z,g_img_formats);

// COMBINED MUS

        // Eta mus
        quantity="mus_eta_";
        CanvasHolder h_eta_mus(quantity+algo);
        fill_holder(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd + g_sCut + "_hist",
                    h_eta_mus,
                    ifileMc, ifileData);

        h_eta_mus.setTitleY("dN_{Events}/d#eta");
        h_eta_mus.setTitleX("#eta^{#mu}");

        h_eta_mus.setBoardersX(-2.5,2.5);
	
    } // end loop on algos
}

//------------------------------------------------------------------------------

double getX(TF1& func,double zval,double yval,double minXval,double maxXval) {

    double epsilon = 0.0001;
    double x=minXval+epsilon;
    double fprec=func.Eval(minXval,yval);
    while (x<maxXval) {
        if ((zval-fprec)*(zval-func.Eval(x,yval)) < 0 )
            return x-epsilon/2;
        x+=epsilon;
    }
    return 0;
}

//------------------------------------------------------------------------------

double getY(TF1& func,double zval,double xval,double minYval,double maxYval) {

    double epsilon = 0.0001;
    double y=minYval+epsilon;
    double fprec=func.Eval(xval,minYval);
    while (y<maxYval) {
        if ((zval-fprec)*(zval-func.Eval(xval,y)) < 0 )
            return y-epsilon/2;
        y+=epsilon;
    }
    return 0;
}

//------------------------------------------------------------------------------
void getResponses(vdouble& responses,
                  Points& points,
                  TFile* ifile,
                  pt_interval interval,
                  TString algoname)
{
    TString treename=algoname+"Jets_Zplusjet_data" + g_sCorrectionAdd + "_events";
    std::cout << "Generating Response for " << treename << std::endl;

    TTree* tree = (TTree*) ifile->Get(treename);
    TParticle* Z=new TParticle();
    TParticle* jet=new TParticle();
    Double_t l2corr;

    tree->SetBranchAddress("Z",&Z);
    tree->SetBranchAddress("jet1",&jet);
    tree->SetBranchAddress("l2corrJet",&l2corr);

    double response;
    for (int ievt=0;ievt< tree->GetEntries();++ievt) {
        tree->GetEntry(ievt);
        if (interval.contains(Z->Pt())) {

            if ( g_correction_level == 2 )
            {
                // use corrected jet !!
                response = ( jet->Pt() * l2corr ) /Z->Pt();
            }
            // don't use correction, raw
            if ( g_correction_level == 0 )
            {
                response = ( jet->Pt() ) /Z->Pt();
            }

            responses.push_back(response);
            points.push_back(point(Z->Pt(),response));
        }
    }
}


//------------------------------------------------------------------------------
Intervals fill_intervals(vint edges) {

    Intervals intervals;
    for (int i=0;i<edges.size()-1;i++)
        intervals.push_back(pt_interval(edges[i],edges[i+1]));
    return intervals;
};

//------------------------------------------------------------------------------
void fill_holder(TString h_name_mc,
                 TString h_name_data,
                 CanvasHolder& h,
                 TFile* ifileMc,
                 TFile* ifileData) {

    std::cout <<"in func\n" << h_name_mc.Data() << " " << h_name_data.Data() << std::endl ;

    TH1D* hmc =( TH1D*)ifileMc->Get(h_name_mc);
    TH1D* hdata =( TH1D*)ifileData->Get(h_name_data);

    // already created in resp_cuts
//    hmc->Sumw2();

    hmc->SetFillColor(kAzure-3);
    hmc->SetLineColor(kAzure-3);
    hmc->SetLineStyle(1);

    // already created in resp_cuts
    //hdata->Sumw2();
    std::cout << "Sum Of Weights PtHatBinned: " << hmc->GetSumOfWeights() << std::endl;
    std::cout << "Sum Of Weights Weighted: " << hdata->GetSumOfWeights() << std::endl;

    
    hmc->Scale(1.0f/ hmc->GetSumOfWeights(),"width");
    hdata->Scale(1.0f/ hdata->GetSumOfWeights(),"width");
   

    
    h.setLegPos(.75,.75,.95,.87);

    h.addObjFormated(hmc,"Fall10 Z2 PtHat binned","Hist");
    h.addObj(hdata,"Fall10 Z2 weighted Events","PE");
}


//------------------------------------------------------------------------------
TGraphErrors* histo2graph(TH1F* histo, double xmax,double ymax) {

    int ibin=1;
    int npoints=0;
    std::cout << "\nTreating histo " << histo->GetName() << std::endl;
    while (histo->GetBinContent(ibin) < ymax and ibin < histo->GetNbinsX() and histo->GetBinCenter(ibin)<xmax) {
//         std::cout << " - Bin Center = " << histo->GetBinCenter(ibin) << std::endl
//                   << "  + Content = " << histo->GetBinContent(ibin) << std::endl
//                   << "  + Upper Limit = " << ymax << std::endl;
        npoints++;
        ibin++;
    }

    TGraphErrors* g = new TGraphErrors(npoints);
    g->SetName(histo->GetName());
    g->SetTitle(histo->GetTitle());

    for (int i=1;i<=npoints;i++) {
        g->SetPoint(i-1,histo->GetBinCenter(i),histo->GetBinContent(i));
//         g->SetPointError(i-1,0,histo->GetBinError(i));
    }

// g->Print();

    return g;
}

//------------------------------------------------------------------------------

void formatHolder(CanvasHolder& h, const char* legSym, int markersize, int lines_width, int skip_colors, bool do_flag) {
    std::vector<int> colors;
    std::vector<int> lines_styles;
    for (int i=0;i<skip_colors;++i) {
        colors.push_back(kGray+i*2);
        lines_styles.push_back(1);
    }

// Old Flag Colours
    if (do_flag) {
        colors.clear();
        colors.push_back(kRed);
        colors.push_back(kOrange+7);
//     colors.push_back(kYellow);
        colors.push_back(kGreen-3);
        colors.push_back(kMagenta+1);
        colors.push_back(kAzure+1);
        colors.push_back(kBlue);
        colors.push_back(kViolet);
        colors.push_back(kGreen+2);
        colors.push_back(kPink+1);
    }

    if (not do_flag) h.setLineStyles(lines_styles);
//     h.setLineColors(colors);
    h.setMarkerSizeGraph(markersize);
    h.setLinesSize(lines_width);
    h.setLegDrawSymbol(legSym);
    h.setOptStat(0);
    h.addToCantopmargin(+0.0);
    h.addToCanleftmargin(+0.08);
    h.addToCanbottommargin(+0.05);
    h.setOptTitle(false);

    h.addLatex(.84,.93,"#sqrt{s}= 7 TeV",true);

}

//------------------------------------------------------------------------------

void saveHolder(CanvasHolder &h,vString formats, bool make_log, TString sNamePostfix) {

    TString mod_name(h.getTitle());
    h.setCanvasTitle( g_sGlobalPrefix + mod_name + sNamePostfix);

    if (make_log) {
        TString can_name(h.getTitle());
        std::cout << "DEBUG: log scale for the canvas " << (can_name+"_log_y").Data() << std::endl;
        h.setCanvasTitle(can_name+"_log_y");
        h.setLogY();
        for (int i=0;i<formats.size();++i) {
//             h.draw();
            h.save(formats[i].Data());
        }
    }
    else {
        for (int i=0;i<formats.size();++i)
//         h.draw();
            h.save(formats[i].Data());
    }
}
