#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
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
#include "PlotCommon.h"

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
};

typedef std::vector<pt_interval> LocalIntervals;
//------------------------------------------------------------------------------
/*
class point {
public:
    point(double the_x, double the_y) {
        x=the_x;
        y=the_y;
    };
    double x;
    double y;
} ;
typedef std::vector<point> Points;*/
//------------------------------------------------------------------------------


double getX(TF1& func,double zval,double yval,double minXval,double maxXval);
double getY(TF1& func,double zval,double xval,double minYval,double maxYval);
class pt_interval;
class point;
typedef std::vector<point> Points;
void getResponses(vdouble& responses, Points& points,TFile* ifile,pt_interval interval,TString algoname);



//------------------------------------------------------------------------------

// Globals (Yes, I don't care)
double lumi=1;
char lumi_str[100];

// yeah, i don't care either
int g_correction_level = 0;
TString g_sCorrectionAdd = "";
TString g_sCorrection_level = "";
vString g_img_formats;
TString g_jetAppendix = "";

PlotEnv g_plotEnv;


LocalIntervals fill_intervals(vint edges);
void fill_holder(TString h_name_mc,
                 TString h_name_data,
                 CanvasHolder& h,
                 int rebin_factor,
                 TFile* ifileMc,
                 TFile* ifileData);

TGraphErrors* histo2graph(TH1F* histo, double xmax,double ymax);
void formatHolder(CanvasHolder& h, const char* legSym="lf",  int size=1,int lines_width=2, int skip_colors=0, bool do_flag=false);

void PlotNumberOfEvents( TString algoname, TFile *  ifile )
{
   TString treename= algoname+ g_jetAppendix + g_sCorrectionAdd + "_events";
    TTree* tree = RootFileHelper::SafeGet<TTree *>(ifile, treename.Data());

    if ( tree == NULL )
    {
    	std::cout << "Cant load " << treename.Data() << std::endl;
    	exit(0);
    }

    Long64_t cmsRun;

    int total_events=tree->GetEntries();
    int first_run=0;
    int last_run=0;


    tree->SetBranchAddress("cmsRun",&cmsRun);

    for (int ievt=0;ievt< tree->GetEntries();++ievt) 
    {

        tree->GetEntry(ievt);

        if (first_run == 0)
            first_run = cmsRun;

        if ( first_run > cmsRun)
            first_run = cmsRun;
        if ( last_run < cmsRun )
            last_run = cmsRun;
    }

    int tot_runs=last_run-first_run;
    TH1F nevts("nevts","nevts",tot_runs, first_run-1000.5,last_run+300.0);

// Make plots of the runs per events
    for (int ievt=0;ievt< tree->GetEntries();++ievt) 
    {
        tree->GetEntry(ievt);
        nevts.Fill( cmsRun );
    }

    nevts.ComputeIntegral();
    double *integral = nevts.GetIntegral();
    nevts.SetContent(integral);
    nevts.Scale(total_events);

    nevts.LabelsOption("av");

    nevts.SetFillColor(38);
    nevts.SetLineColor(38);

    CanvasHolder h_nevts( algoname+ "_number_of_events" + g_sCorrectionAdd );
    nevts.GetXaxis()->CenterTitle(1);
    nevts.GetYaxis()->CenterTitle(1);
    h_nevts.addObj(&nevts,"","Hist");

    h_nevts.setTitleY("Z(#rightarrow #mu #mu)+jet Events");
    h_nevts.setTitleX("Run Number");
    h_nevts.setLegDraw(0);
    h_nevts.setBoardersY(0.01,total_events*1.3);
    h_nevts.setBoardersX(first_run-1000.0,last_run+300);
    h_nevts.setOptTitle(false);
    h_nevts.setOptStat(false);

    h_nevts.addToCanrightmargin(.1);

    h_nevts.addLatex(.74,.93,"#sqrt{s}= 7 TeV",true);
    h_nevts.addLatex(.16,.93,lumi_str,true);
    saveHolder(h_nevts,g_img_formats, false, "", "", g_plotEnv);
}

//------------------------------------------------------------------------------

int main(int argc, char **argv) {
	TString fileName("Datapoints_");
	fstream txtfile;

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

    g_plotEnv.LoadFromConfig( p );
    
    p.setVerbose(false);

    if (argc==3) {
        int verbosityi=atoi(argv[2]);
        if (verbosityi==1) p.setVerbose(true);
    }

    TString secname("general");
//
// Section general
//
    g_jetAppendix = p.getString(secname+".jet_appendix");
    lumi = p.getDouble(secname+".lumi");
    int pt_rebin_factor = p.getInt(secname+".pt_rebin_factor");
    int phi_rebin_factor = p.getInt(secname+".phi_rebin_factor");
    int mass_rebin_factor = p.getInt(secname+".mass_rebin_factor");
    int eta_rebin_factor = p.getInt(secname+".eta_rebin_factor");
    TString mc_input_file = p.getString(secname+".mc_input_file");
    TString data_input_file = p.getString(secname+".data_input_file");
    TString info_string = p.getString(secname+".info_string");
    vString algos = p.getvString(secname+".algos");
    vString good_algos = p.getvString(secname+".good_algos");
    vint pt_bins = p.getvInt(secname+".pt_bins");
    g_img_formats= p.getvString(secname+".img_formats");
    vint runs_with_events = p.getvInt(secname+".runs_with_evts");
    double min_jes=p.getDouble(secname+".min_jes");
    double max_jes=p.getDouble(secname+".max_jes");
    double min_jer=p.getDouble(secname+".min_jer");
    double max_jer=p.getDouble(secname+".max_jer");
    TString outputtype=p.getString(secname+"outputtype");
    bool txtout=("outputtype"=="txt");

    TString baseFolder("NoBinning_incut/");
// 0 = raw
// 2 = level2
// 3 = level3
    g_correction_level = p.getInt(secname+".correction_level");

    
    
    if ( g_correction_level == 2)
    {
      g_sCorrectionAdd = "_l2corr";
        g_sCorrection_level = "corrected for #eta dependence";
    }
    
    sprintf(lumi_str,"#scale[.8]{#int} L = %1.0f pb^{-1}",lumi);

//------------------------------------------------------------------------------

    TFile* ifileMc = new TFile (mc_input_file);
    TFile* ifileData = new TFile (data_input_file);


    std::cout << "MC file : " << mc_input_file << std::endl;
    std::cout << "Data file : " << data_input_file << std::endl;

	TString sOutputFolder = ("plot_out/" + g_plotEnv.m_sConfigFileName + "/");


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

        PlotNumberOfEvents( algo, ifileData );

//------------------------------------------------------------------------------

        // Eta jet
        quantity="jet1_eta_";
        CanvasHolder h_eta_jet(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix +"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix +"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_eta_jet,
                    eta_rebin_factor,
                    ifileMc, ifileData);

        h_eta_jet.setTitleY("dN_{Events}/d#eta");
        h_eta_jet.setTitleX("#eta^{jet}");

        h_eta_jet.addLatex(info_x,info_y,the_info_string,true);
        h_eta_jet.scaleBoardersY(1,2);
        h_eta_jet.setBoardersX(-1.4,1.4);
        //h_eta_jet.setBoardersY(0.001,129.0);
	h_eta_jet.scaleBoardersY(1.0,1.2);

        formatHolder(h_eta_jet);
        saveHolder(h_eta_jet,g_img_formats, false, "", "", g_plotEnv);

        // Phi jet
        quantity="jet1_phi_";
        CanvasHolder h_phi_jet(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_phi_jet,
                    phi_rebin_factor,
                    ifileMc, ifileData);

        h_phi_jet.setTitleY("dN_{Events}/d#phi");
        h_phi_jet.setTitleX("#phi^{jet}");
        h_phi_jet.setBoardersX(-3.14,3.14);
	h_phi_jet.scaleBoardersY(1.0,1.3);

        h_phi_jet.addLatex(info_x,info_y,the_info_string,true);

        formatHolder(h_phi_jet);
        saveHolder(h_phi_jet,g_img_formats, false, "", "", g_plotEnv);

        // Pt jet
        quantity="jet1_pt_";
        CanvasHolder h_pt_jet(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_pt_jet,
                    pt_rebin_factor,
                    ifileMc, ifileData);

        h_pt_jet.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_jet.setTitleX("p_{T}^{jet}[GeV]");
        h_pt_jet.setBoardersX(0,189);
        h_pt_jet.setBoardersY(0.0099,950.0);
        h_pt_jet.setLogY();


        h_pt_jet.addLatex(info_x,info_y,the_info_string,true);
        h_pt_jet.scaleBoardersY(1,2.5);
        formatHolder(h_pt_jet);
        saveHolder(h_pt_jet,g_img_formats, false, "", "", g_plotEnv);
        /*
            // Pt jet l2 corr
            quantity="jet1_pt_";
            CanvasHolder h_pt_jet_l2(quantity+algo+"l2");
            fill_holder(quantity+algo+g_jetAppendix+"_Zplusjet_mc_hist",
                        quantity+algo+g_jetAppendix+"_Zplusjet_data_l2corr_hist",
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
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_eta_jet2,
                    eta_rebin_factor,
                    ifileMc, ifileData);

        h_eta_jet2.setTitleY("dN_{Events}/d#eta");
        h_eta_jet2.setTitleX("#eta^{jet2}");

        h_eta_jet2.addLatex(info_x,info_y,the_info_string,true);
        h_eta_jet2.scaleBoardersY(1,2);
        h_eta_jet2.setBoardersX(-1.4,1.4);
        //h_eta_jet2.setBoardersY(0.001,79.99);
	h_eta_jet2.scaleBoardersY(1.0,1.2);

        formatHolder(h_eta_jet2);
        saveHolder(h_eta_jet2,g_img_formats, false, "", "", g_plotEnv);

        // Phi jet2
        quantity="jet2_phi_";
        CanvasHolder h_phi_jet2(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_phi_jet2,
                    phi_rebin_factor,
                    ifileMc, ifileData);

        h_phi_jet2.setTitleY("dN_{Events}/d#phi");
        h_phi_jet2.setTitleX("#phi^{jet2}");
        h_phi_jet2.setBoardersX(-3.14,3.14);
        //h_phi_jet2.setBoardersY(0.001,64.99);


        h_phi_jet2.addLatex(info_x,info_y,the_info_string,true);
        h_phi_jet2.scaleBoardersY(1,1.5);
        formatHolder(h_phi_jet2);
        saveHolder(h_phi_jet2,g_img_formats, false, "", "", g_plotEnv);

        // Pt jet2
        quantity="jet2_pt_";
        CanvasHolder h_pt_jet2(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_pt_jet2,
                    pt_rebin_factor,
                    ifileMc, ifileData);

        h_pt_jet2.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_jet2.setTitleX("p_{T}^{jet2}[GeV]");
        h_pt_jet2.setBoardersX(0,189);
        h_pt_jet2.setBoardersY(0.0099,950.0);
        h_pt_jet2.setLogY();


        h_pt_jet2.addLatex(info_x,info_y,the_info_string,true);
        h_pt_jet2.scaleBoardersY(1,1.4);
        formatHolder(h_pt_jet2);
        saveHolder(h_pt_jet2,g_img_formats, false, "", "", g_plotEnv);

//------------------------------------------------------------------------------

        // Eta Z
        quantity="z_eta_";
        CanvasHolder h_eta_z(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_eta_z,
                    eta_rebin_factor,
                    ifileMc, ifileData);

        h_eta_z.setTitleY("dN_{Events}/d#eta");
        h_eta_z.setTitleX("#eta^{Z}");

        h_eta_z.addLatex(info_x,info_y,the_info_string,true);
        //h_eta_z.setBoardersY(0.001,124.9);
        h_eta_z.scaleBoardersY(1,1.5);

        formatHolder(h_eta_z);
        saveHolder(h_eta_z,g_img_formats, false, "", "", g_plotEnv);

        // Phi z
        quantity="z_phi_";
        CanvasHolder h_phi_z(quantity+algo);
        fill_holder(baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder + quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_phi_z,
                    phi_rebin_factor,
                    ifileMc, ifileData);

        h_phi_z.setTitleY("dN_{Events}/d#phi");
        h_phi_z.setTitleX("#phi^{Z}");
        h_phi_z.setBoardersX(-3.14,3.14);
        //h_phi_z.setBoardersY(0.001,74.9);

        h_phi_z.addLatex(info_x,info_y,the_info_string,true);
        h_phi_z.scaleBoardersY(1.0,1.4);

        formatHolder(h_phi_z);
        saveHolder(h_phi_z,g_img_formats, false, "", "", g_plotEnv);

        // Pt z
        quantity="z_pt_";
        CanvasHolder h_pt_z(quantity+algo);
        fill_holder(baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_pt_z,
                    pt_rebin_factor,
                    ifileMc, ifileData);

        h_pt_z.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_z.setTitleX("p_{T}^{Z}[GeV]");
        h_pt_z.setBoardersX(0,189.0);
        //h_pt_z.setBoardersY(0.0001,7.59);

        h_pt_z.addLatex(info_x,info_y,the_info_string,true);
        h_pt_z.scaleBoardersY(1.0,1.1);

        formatHolder(h_pt_z);
        saveHolder(h_pt_z,g_img_formats, false, "", "", g_plotEnv);

        // z mass
        quantity="zmass_";
        CanvasHolder h_mass_z(quantity+algo);
        fill_holder(baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_mass_z,
                    mass_rebin_factor,
                    ifileMc, ifileData);

        h_mass_z.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_mass_z.setTitleX("M_{Z}[GeV]");
        h_mass_z.setBoardersX(60.0,120.0);
        //h_mass_z.setBoardersY(0.01,74.9);

        h_mass_z.addLatex(info_x,info_y,the_info_string,true);
        h_mass_z.scaleBoardersY(1,1.3);

        formatHolder(h_mass_z);
        saveHolder(h_mass_z,g_img_formats, false, "", "", g_plotEnv);

// COMBINED MUS

        // Eta mus
        quantity="mus_eta_";
        CanvasHolder h_eta_mus(quantity+algo);
        fill_holder(baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_eta_mus,
                    eta_rebin_factor,
                    ifileMc, ifileData);

        h_eta_mus.setTitleY("dN_{Events}/d#eta");
        h_eta_mus.setTitleX("#eta^{#mu}");

        h_eta_mus.setBoardersX(-2.5,2.5);
        //h_eta_mus.setBoardersY(0.001,379.9);

        h_eta_mus.addLatex(info_x,info_y,the_info_string,true);
        h_eta_mus.scaleBoardersY(1,1.4);

        formatHolder(h_eta_mus);
        saveHolder(h_eta_mus,g_img_formats, false, "", "", g_plotEnv);

        // Phi mus
        quantity="mus_phi_";
        CanvasHolder h_phi_mus(quantity+algo);
        fill_holder(baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_phi_mus,
                    phi_rebin_factor,
                    ifileMc, ifileData);

        h_phi_mus.setTitleY("dN_{Events}/d#phi");
        h_phi_mus.setTitleX("#phi^{#mu}");
        h_phi_mus.setBoardersX(-3.14,3.14);
        //h_phi_mus.setBoardersY(0.01,199.9);

        h_phi_mus.addLatex(info_x,info_y,the_info_string,true);
        h_phi_mus.scaleBoardersY(1,2);

        formatHolder(h_phi_mus);
        saveHolder(h_phi_mus,g_img_formats, false, "", "", g_plotEnv);

        // Pt mus
        quantity="mus_pt_";
        CanvasHolder h_pt_mus(quantity+algo);
        fill_holder(baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_hist",
        		baseFolder +quantity+algo+g_jetAppendix+"_Zplusjet_data" + g_sCorrectionAdd + "_hist",
                    h_pt_mus,
                    pt_rebin_factor,
                    ifileMc, ifileData);

        h_pt_mus.setTitleY("dN_{Events}/dp_{T} [GeV^{-1}]");
        h_pt_mus.setTitleX("p_{T}^{#mu}[GeV]");
        h_pt_mus.setBoardersX(0,189);
        //h_pt_mus.setBoardersY(0.01,16.49);

        h_pt_mus.addLatex(info_x,info_y,the_info_string,true);
        h_pt_mus.scaleBoardersY(1,1.1);

        formatHolder(h_pt_mus);
        saveHolder(h_pt_mus,g_img_formats, false, "", "", g_plotEnv);

//------------------------------------------------------------------------------
// Il reame della response.. Specchio Specchio delle mie brame

        LocalIntervals intervals (fill_intervals(pt_bins));

        vTF1 functions;
        vvdouble vresponses;

        TGraphErrors repsponse_mc(intervals.size());
        Points responses_points_all_bins;
        int ibin=0;

        for (LocalIntervals::iterator interval=intervals.begin();
                interval < intervals.end();++interval ) {

        	TString folderName;

        	folderName = interval->id() + "_incut/";
            // Get responses
            vdouble responses;
            TFile data_ifile(data_input_file);
//             getResponses(responses,responses_points_all_bins,ifile,*interval,algo);
            getResponses(responses,responses_points_all_bins,&data_ifile,*interval,algo);
            data_ifile.Close();
            vresponses.push_back(responses);

            // Jet Response Histo
            quantity="jetresp_";
            CanvasHolder h_resp(quantity+algo+"_"+interval->id());

            TH1D* respo =
            		RootFileHelper::SafeGet<TH1D *>(ifileMc,  (folderName + quantity+algo+g_jetAppendix+"_Zplusjet_mc" +
													g_sCorrectionAdd + "_" + interval->id()+  "_hist").Data());
            respo->Rebin(pt_rebin_factor);
            respo->SetFillColor(kRed-9);
            respo->SetLineColor(kRed-9);
            respo->SetLineStyle(1);
            respo->Scale(1./respo->Integral());

            // Fill The response histo
            TString zpt_name("z_pt_");
            zpt_name+=algo+g_jetAppendix+"_Zplusjet_mc" + g_sCorrectionAdd + "_" + interval->id()+"_hist";

            TH1D* zpt = RootFileHelper::SafeGet<TH1D *>(ifileMc, ( folderName + zpt_name).Data());

            repsponse_mc.SetPoint(ibin,zpt->GetMean(),respo->GetMean());
            repsponse_mc.SetPointError(ibin,zpt->GetMeanError(),respo->GetRMS());
            ibin++;
            // End fill The response histo

            double maximum=0.03;

            h_resp.setLegPos(.77,.78,.95,.87);

            h_resp.addObjFormated(respo,"Monte Carlo","Hist");

            // fit section

            TF1 *  the_gaus = new TF1(interval->id()+"the_gaus",
                         "[2]/(sqrt(2*TMath::Pi())*[1]*[4]) * exp(-0.5*((x-[0]*[3])/([1]*[4]))**2)",
                         0.,2.);
            the_gaus->SetParNames("Mean","Sigma","Norm","JES","JER");
            the_gaus->SetParameters(.8,.5,1);

//             the_gaus.FixParameter(2,1);

            the_gaus->FixParameter(4,1);
            the_gaus->FixParameter(3,1);
            the_gaus->SetLineStyle(1);
            the_gaus->SetLineColor( kBlack );
            the_gaus->SetFillColor( kBlack );
            the_gaus->SetMarkerColor(kBlack);
            respo->Fit(the_gaus,"L");
            if (txtout){
	            std::cout << "Gauss: Mean " << the_gaus->GetParameter(0) 
    	                  << " Sigma " << the_gaus->GetParameter(1) 
        	              << " Norm " << the_gaus->GetParameter(2) 
            	          << "\n";
           	}
            the_gaus->FixParameter(0,the_gaus->GetParameter(0));
            the_gaus->FixParameter(1,the_gaus->GetParameter(1));
            the_gaus->FixParameter(2,the_gaus->GetParameter(2));

            the_gaus->SetLineWidth( 4 );

            functions.push_back(*the_gaus);

            // end fit section
            
            h_resp.addObjFormated(the_gaus,"Gaussian Fit","L");

            h_resp.setTitleY("Arb. Units");
            h_resp.setTitleX("Jet Response");

            h_resp.addLatex(info_x,info_y,the_info_string,true);
            h_resp.addLatex(0.55,.93,interval->good_id(),true);

            //FAKE
            TH1F fake("fake","fake",100,-0,-0.00000001);
            fake.SetLineColor(kBlue);
            fake.SetLineWidth(4);
            fake.SetBinContent(1,0.0000000000001);
            h_resp.addObjFormated(&fake,"Single Events","L");

            formatHolder(h_resp);

//             h_resp.normalizeHistos();
            h_resp.setBoardersY(0.0001,0.28);
            h_resp.setBoardersX(0,1.98);
            h_resp.draw();

            for (vdouble::iterator response=responses.begin();
                    response < responses.end();response++ ) {
//                 std::cout << "Response is " << *response <<std::endl;
                TLine* l= new TLine(*response,0,*response,maximum);
                l->SetLineColor(kBlue);
                l->SetLineWidth(4);
                h_resp.getCanvas()->cd();
                l->Draw("same");
            }

            h_resp.getCanvas()->Print(sOutputFolder + quantity+algo+"_"+interval->id()+".png");
            h_resp.getCanvas()->Print(sOutputFolder + quantity+algo+"_"+interval->id()+".pdf");
//             saveHolder(h_resp,img_formats);
        }

        // Prepare the response/MC plot for the response
        TGraph repsponse_data(responses_points_all_bins.size());
        fileName = "Datapoints_ak5Calo.txt";
        if (txtout) {
        txtfile.open(fileName, std::ios::out | std::ios::trunc);
        txtfile << "#Response (default: anti-kt 0.5 Algorithm, CaloJets)"
                << "\n#  i    x           y\n" << std::fixed;
        txtfile.precision(6);
        }                                                                                                                                                
        for (int i=0;i<responses_points_all_bins.size();++i) {
            repsponse_data.SetPoint(i,responses_points_all_bins[i].x,responses_points_all_bins[i].y);
            // std::cout << "Punkt " << i << ": " << responses_points_all_bins[i].x << ", " << responses_points_all_bins[i].y << std::endl;
			if (txtout) {
	            txtfile << std::setw(4) << i 
    	                << std::setw(12) << responses_points_all_bins[i].x 
        	            << std::setw(12) << responses_points_all_bins[i].y 
            	        << std::endl;
           	}
       }

        //repsponse_data.SetMarkerSize(0.5);
        repsponse_data.SetName("data");
        repsponse_data.SetFillColor(kWhite);
        repsponse_data.SetMarkerStyle(kFullDotMedium);
        repsponse_data.Print();
/*
        for (int i=0;i<4;++i) // to remove high pt bins
            repsponse_mc.RemovePoint(repsponse_mc.GetN()-1);*/
        repsponse_mc.Print();

        CanvasHolder h_response(algo+"_JetResponse");

        h_response.setTitleY("Jet Response");
        h_response.setTitleX("p_{T}^{Z} [GeV]");

        h_response.setBoardersY(0.001,2);
        h_response.setLegPos(.75,.75,.95,.87);

        repsponse_mc.SetLineColor(kRed);
        repsponse_mc.SetFillColor(kRed);
        repsponse_mc.SetMarkerColor(kRed);
        repsponse_mc.SetMarkerSize(0.5);
        repsponse_mc.SetFillStyle(3002);
        h_response.addObjFormated(&repsponse_mc,"Monte Carlo","LE3");
        h_response.addObjFormated(&repsponse_data,"Single events","P");
        h_response.addLatex(info_x,info_y,the_info_string,true);

/*        if ( g_correction_level == 2 )
            h_response.addLatex(0.08,0.05, "Work in progress - L2 corrected Jets"  ,true);
        if ( g_correction_level == 0 )
            h_response.addLatex(0.08,0.05, "Work in progress - uncorrected Jets"  ,true);
*/

        formatHolder(h_response);
        h_response.draw();
        h_response.getCanvas()->cd();

        if ( g_correction_level == 2 )
            saveHolder(h_response,g_img_formats, false, "_l2", "" , g_plotEnv);
        if ( g_correction_level == 0 )
            saveHolder(h_response,g_img_formats, false, "_raw", "" , g_plotEnv);

        // Prepare the likelihood --------------------------------------------------

        MyLikelihood nll (vresponses,functions,1);

        TF2 nll_func("nll_func",&nll,&MyLikelihood::Evaluate,.5,1.5,.5,1.5,0,"MyLikelihood","Evaluate");

        double jes,jer;
        nll_func.GetMinimumXY(jes,jer);
        gMinuit->SetPrintLevel(1);
        Double_t p=100;
        Int_t one=1;
        gMinuit->mnexcm("MIGRAD",&p,one,one);
        gMinuit->mnmnos();

        double minimum=nll_func.Eval(jes,jer);

        nll.setMinimum(minimum);


        // The point where the 1 and 2 sigma contour extremes will be located
        double jes_p1s,jes_p2s,jer_p1s,jer_p2s,
        jes_m1s,jes_m2s,jer_m1s,jer_m2s;

        jes_m1s = getX(nll_func,.5,jer,0,jes);
        jes_p1s = getX(nll_func,.5,jer,jes,2);

        jer_m1s = getY(nll_func,.5,jes,0,jer);
        jer_p1s = getY(nll_func,.5,jes,jer,2);


        jes_m2s = getX(nll_func,2.,jer,0,jes);
        jes_p2s = getX(nll_func,2.,jer,jes,10);

        jer_m2s = getY(nll_func,2.,jes,0,jer);
        jer_p2s = getY(nll_func,2.,jes,jer,10);

        std::cout << " jes = " << jes << " + " << jes_p1s-jes << " - " << jes-jes_m1s << std::endl;
        std::cout << " jer = " << jer << " + " << jer_p1s-jer << " - " << jer-jer_m1s << std::endl;

        std::cout << " jes = " << jes << " + " << jes_p2s << " - " << jes_m2s << std::endl;
        std::cout << " jer = " << jer << " + " << jer_p2s << " - " << jer_m2s << std::endl;

        // Draw the Whole story

        int nbins=300;

        TH2F nll_func_h("nll_func_h","nll_func_h;Jet Energy Scale_{Data}/Jet Energy Scale_{MC};Resolution_{Data}/Resolution_{MC}",nbins,min_jes,max_jes,nbins,min_jer,max_jer);
        for (int i=1;i<=nbins;++i)
            for (int j=1;j<=nbins;++j) {
                double val =nll_func.Eval(min_jes+i*(max_jes-min_jes)/nbins,min_jer+j*(max_jer-min_jer)/nbins);
                if (val>2)val=-1;
                nll_func_h.SetBinContent(i,j,val);
            }
        nll_func_h.GetZaxis()->SetRangeUser(0,2);

        gStyle->SetPalette(1);
        TCanvas c("mycan","mycan",800,600);
        c.cd();

        nll_func_h.Draw("COLZ");
        c.SetBottomMargin(.15);
        c.SetRightMargin(.15);
        c.SetLeftMargin(.15);
        c.Print(sOutputFolder + algo+"_funzione.png");
        c.Print(sOutputFolder + algo+"_funzione.pdf");

        // Contours ----------------------------------------------------------------

        double ncontours[2];
        ncontours[0] = .5;
        ncontours[1] =  2;
        nll_func_h.SetContour(2, ncontours);

        nll_func_h.Draw("CONT Z LIST");
        c.Update(); // Needed to force the plotting and retrieve the contours in TGraphs

        // Get Contours
        TObjArray *conts = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");

        // TGraph vector
        std::vector<TGraph> contours;

        for (int i = 0; i < conts->GetSize(); i++) {
            TList* contLevel = (TList*)conts->At(i);
            for (int j = 0; j < contLevel->GetSize(); j++) {
                TGraph contour (*(TGraph*)contLevel->At(j));
                contours.push_back(contour);
            }
        }

//     std::vector<TGraph> contours;
//     // NEW METHOD
//     Int_t zero=0;gMinuit->mncomd("SET ERR .5",zero);
//     contours[0]=*(TGraph*)gMinuit->Contour(100);
// //    gMinuit->mncomd("SET ERR .5",zero);
// //    contours[1]=*(TGraph*)gMinuit->Contour(20);
//     contours[0]=contours[1];

        CanvasHolder h_contours(algo+"_Contours");

        TGraph centre(1);
        centre.SetPoint(0,jes,jer);
        centre.SetMarkerStyle(20);
        centre.SetMarkerSize(5);
        h_contours.addObjFormated(&centre,"","P");

        TGraph fail(2);
        fail.SetPoint(0,-1,-4);
        fail.SetPoint(1,4,-4);
        h_contours.addObjFormated(&fail,"","P");

        h_contours.setTitleX("s_{Data}/s_{MC}");
        h_contours.setBoardersX(min_jes+0.0001,max_jes-0.0001);
        h_contours.setTitleY("r_{Data}/r_{MC}");
        h_contours.setBoardersY(min_jer+0.0001,1.3*max_jer-0.0001);

        h_contours.addLatex(info_x,info_y,the_info_string,true);
        h_contours.addLatex(jes-0.01,jer_p1s+0.01,"1-#sigma");
        h_contours.addLatex(jes-0.01,jer_p2s+0.01,"2-#sigma");

        //Let's write the result on the plot
        char result[255];
        sprintf(result,"#scale[.8]{#splitline{s_{Data}/s_{MC} = %1.2f^{+%1.2f}_{-%1.2f}}{r_{Data}/r_{MC} = %1.2f^{+%1.2f}_{-%1.2f}}}",
                jes,jes_p1s-jes,jes-jes_m1s,  jer,jer_p1s-jer,jer-jer_m1s);

        //std::cout << result << std::endl;
        std::cout << "\nJet Energy Scale wrt Monte Carlo &=& "<<jes<<"^{+"<<jes_p1s-jes<<"}_{-"<<jes-jes_m1s<<"} \\\\" << std::endl;
        std::cout << "Jet Energy Resolution wrt Monte Carlo &=& "<<jer<<"^{+"<<jer_p1s-jer<<"}_{-"<<jer-jer_m1s<<"} \\\\" << std::endl;

        char resultL[255];
        sprintf(resultL,"\nJet Energy Scale wrt Monte Carlo &=&  %1.2f^{+%1.2f}_{-%1.2f}\nJet Energy Resolution wrt Monte Carlo &=& %1.2f^{+%1.2f}_{-%1.2f}\n\n",
                jes,jes_p1s-jes,jes-jes_m1s,  jer,jer_p1s-jer,jer-jer_m1s);

        std::cout << resultL;

        std::cout << "\n\nRESULT\n\n" << resultL << std::endl;

        h_contours.addLatex(info_x,info_y*0.85,result,true);



        // info
	/*
        if (g_correction_level == 2 )
            h_contours.addLatex(0.08,0.05, "Work in progress - L2 corrected Jets"  ,true);

        if (g_correction_level == 0 )
            h_contours.addLatex(0.08,0.05, "Work in progress - uncorrected Jets"  ,true);
*/

        formatHolder(h_contours);
        h_contours.draw();

        TCanvas* final=h_contours.getCanvas();
        final->cd();

        contours[0].SetLineColor(kRed);
        contours[0].SetLineStyle(2);
        contours[1].SetLineColor(kBlue);
        contours[1].SetLineStyle(2);
        contours[0].Draw("SameL");
        contours[1].Draw("SameL");

        TLine l(0,1,0,2);
        l.SetLineStyle(1);
        l.DrawLine(jes,min_jer,jes,jer);
        l.DrawLine(min_jes,jer,jes,jer);
        l.SetLineStyle(2);
        l.DrawLine(jes_p1s,min_jer,jes_p1s,jer);
        l.DrawLine(jes_m1s,min_jer,jes_m1s,jer);
        l.DrawLine(min_jes,jer_p1s,jes,jer_p1s);
        l.DrawLine(min_jes,jer_m1s,jes,jer_m1s);

        TString postFix;

        if ( g_correction_level == 0 )
            postFix = "_raw";
        if ( g_correction_level == 2 )
            postFix = "_l2";

        final->Print( sOutputFolder + algo + postFix + "_Contours.png");
        final->Print( sOutputFolder + algo + postFix + "_Contours.pdf");
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
	//ak5PFJets_events
    TString treename=algoname+"Jets" + g_sCorrectionAdd + "_events";
    std::cout << "Generating Response for " << treename << std::endl;

    TTree* tree = RootFileHelper::SafeGet<TTree *>(ifile,  treename.Data());

    TParticle* Z=new TParticle();
    TParticle* jet=new TParticle();
    Double_t l2corr;

    tree->SetBranchAddress("Z",&Z);
    tree->SetBranchAddress("jet1",&jet);
    tree->SetBranchAddress("l2corrJet",&l2corr);

    double response;
    for (int ievt=0;ievt< tree->GetEntries();++ievt) 
    {
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
LocalIntervals fill_intervals(vint edges) {

    LocalIntervals intervals;
    for (int i=0;i<edges.size()-1;i++)
        intervals.push_back(pt_interval(edges[i],edges[i+1]));
    return intervals;
};

//------------------------------------------------------------------------------
void fill_holder(TString h_name_mc,
                 TString h_name_data,
                 CanvasHolder& h,
                 int rebin_factor,
                 TFile* ifileMc,
                 TFile* ifileData) {

    std::cout <<"in func\n" << h_name_mc.Data() << " " << h_name_data.Data() << std::endl ;

    //TH1D* hmc =( TH1D*)ifileMc->Get(h_name_mc);

    TH1D* hmc = RootFileHelper::SafeGet< TH1D *>( ifileMc, h_name_mc.Data());
    TH1D* hdata =RootFileHelper::SafeGet< TH1D *>( ifileData, h_name_data.Data());


    hmc->Sumw2();
    hmc->Rebin(rebin_factor);
    hmc->SetFillColor(kRed-9);
    hmc->SetLineColor(kRed-9);
    hmc->SetLineStyle(1);
    // Normalise MC
//    hmc->Scale(1./hmc->Integral());
//    double nevts=hmc->Integral();

    // Scale to data
    hmc->Scale(lumi);

    hdata->SetMarkerStyle(20);

    hdata->Sumw2();
    hdata->Rebin(rebin_factor);

    hmc->Scale(1,"width");
    hdata->Scale(1,"width");

    h.setLegPos(.75,.75,.95,.87);

    h.addObjFormated(hmc,"Monte Carlo","Hist");
    h.addObj(hdata,"Data","PE");
//     h.normalizeHistos();

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

    h.addLatex(.26,.93,lumi_str,true);

}
