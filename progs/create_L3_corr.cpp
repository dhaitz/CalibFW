#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_vector.hpp>

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


    double min;
    double max;
} ;

class Intervals : public std::vector<pt_interval>
{
public:
    Double_t * getRootHistParams()
    {
        Double_t * val[ this->size() + 1 ];

        int i = 0;
        for ( Intervals::iterator it = this->begin();
                it != this->end();
                ++it )
        {
            val[ i ] = new Double_t( it->min ) ;
            i++;
        }
        val[ i ] = new Double_t(  this->back().min );

        return val[0];
    }
};
//------------------------------------------------------------------------------

class point {
public:
    point(double the_x, double the_y) {
        x=the_x;
        y=the_y;
    };
    double x;
    double y;
};
typedef std::vector<point> Points;
//------------------------------------------------------------------------------


double getX(TF1& func,double zval,double yval,double minXval,double maxXval);
double getY(TF1& func,double zval,double xval,double minYval,double maxYval);
class pt_interval;

class point;
typedef std::vector<point> Points;
void getResponses(vdouble& responses, Points& points,TFile* ifile,pt_interval interval,TString algoname);


class DataHisto
{
  public:
    DataHisto( double min, double max, TH1D * hist ) : 
      m_pHist( hist ),
      m_binMin(min),
      m_binMax(max)
    {
      
    }
    
    double GetBinCenter()
    {
	return (this->m_binMax+ this->m_binMin) / 2.0f;
    }
    
    double GetBinWidth()
    {
	return (this->m_binMax- this->m_binMin);
    }

    
    double m_binMin;
    double m_binMax;
    TH1D * m_pHist;
};


//------------------------------------------------------------------------------

// Globals (Yes, I don't care)
double lumi=1.0f;
char lumi_str[100];

// yeah, i don't care either
int g_correction_level = 0;
TString g_sCorrection_level = "";
TString g_sCorrectionAdd = "";

// description defaults
    double info_x=.3;
    double info_y=.78;

Intervals fill_intervals(vint edges);

TGraphErrors* histo2graph(TH1F* histo, double xmax,double ymax);
void formatHolder(CanvasHolder& h, const char* legSym="lf",  int size=1,int lines_width=2, int skip_colors=0, bool do_flag=false, int optStat = 0);
void formatHolderAlt(CanvasHolder& h);
void saveHolder(CanvasHolder &h, vString formats, bool make_log = false, TString sNamePostfix = "", TString sPrefix = "");

//------------------------------------------------------------------------------


void PlotJetCorrection( TString algo, boost::ptr_vector<DataHisto> & histData,
			TGraphErrors & corr_mc, vString & img_formats, TString sGlobalPrefix,
			TString the_info_string,
			TString plot_function, TString funcName)
{
// JET Correction
        CanvasHolder h_corr(algo+"_JetCorrection");

        int i = 0;
        TGraphErrors * p_dataCalibPoints = new TGraphErrors(histData.size());
        for ( boost::ptr_vector< DataHisto >::iterator it = histData.begin();
                it != histData.end();
                ++it )
        {
            p_dataCalibPoints->SetPoint(i, it->GetBinCenter(), 1.0f / it->m_pHist->GetMean());
	    p_dataCalibPoints->SetPointError(i, it->GetBinWidth() / 2.0f, it->m_pHist->GetMeanError());
	    //p_dataCalibPoints->SetPoint(i,( i + 1.0f) * 30.0f, 1.2f);
            i++;
        }
  
  
	// do the fit !
	TF1 * pDataFit = new TF1( "jecFit" + funcName, plot_function);
	pDataFit->SetParameter(0, 1.0f);
	pDataFit->SetParameter(1, 1.0f);
	pDataFit->SetParameter(2, 1.0f);
	pDataFit->SetParameter(3, 1.0f);
	pDataFit->SetParameter(4, 1.0f);
	
	p_dataCalibPoints->Fit( pDataFit);

        p_dataCalibPoints->SetLineColor(kBlack);
        p_dataCalibPoints->SetFillColor(kBlack);
        p_dataCalibPoints->SetMarkerColor(kBlack);
        p_dataCalibPoints->SetMarkerSize(1.0);
        p_dataCalibPoints->SetFillStyle(0);	
	
	std::cout << "ChiSquare : " << pDataFit->GetChisquare() << std::endl;
	

        h_corr.setTitleY("Jet Energy Correction");
        h_corr.setTitleX("p_{T}^{jet} [GeV/c]");

        h_corr.setBoardersY(1.0, 1.6);
	//h_corr.setBoardersX(0.0f, 170.0f );
        h_corr.setLegPos(.75,.75,.95,.87);

        corr_mc.SetLineColor(kRed);
        corr_mc.SetFillColor(kRed);
        corr_mc.SetMarkerColor(kBlack);
        corr_mc.SetMarkerSize(0.1);
        corr_mc.SetFillStyle(3002);
        h_corr.addObjFormated(&corr_mc,"Monte Carlo","L*");

        h_corr.addObjFormated(p_dataCalibPoints,"Binned Data","P");
        h_corr.addLatex(info_x,info_y,the_info_string,true);
	
	/*
        if ( g_correction_level == 2 )
            h_corr.addLatex(0.08,0.05, "Work in progress - L2 corrected Jets"  ,true);
        if ( g_correction_level == 0 )
            h_corr.addLatex(0.08,0.05, "Work in progress - uncorrected Jets"  ,true);
*/
//	h_corr.addLatex(0.08,0.01, "Fit function: " + plot_function, true);

        formatHolder(h_corr);
        h_corr.draw();
        h_corr.getCanvas()->cd();

        if ( g_correction_level == 2 )
            saveHolder(h_corr,img_formats, false, "_l2", sGlobalPrefix + funcName);
        if ( g_correction_level == 0 )
            saveHolder(h_corr,img_formats, false, "_raw", sGlobalPrefix + funcName);
}

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
    vint pt_data_bins = p.getvInt(secname+".pt_data_bins");
    vString img_formats= p.getvString(secname+".img_formats");
    double min_jes=p.getDouble(secname+".min_jes");
    double max_jes=p.getDouble(secname+".max_jes");
    double min_jer=p.getDouble(secname+".min_jer");
    double max_jer=p.getDouble(secname+".max_jer");

// 0 = raw
// 2 = level2
// 3 = level3
    g_correction_level = p.getInt(secname+".correction_level");

    sprintf(lumi_str,"#scale[.8]{#int} L = %1.2f pb^{-1}",lumi);

//------------------------------------------------------------------------------

    TFile* ifileMc = new TFile (mc_input_file);
    TFile* ifileData = new TFile (data_input_file);

    std::cout << "MC file : " << mc_input_file << std::endl;
    std::cout << "Data file : " << data_input_file << std::endl;

// jet, mus, Z -- eta, pt, phi
    Intervals intervals (fill_intervals(pt_bins));
    Intervals data_intervals (fill_intervals(pt_data_bins));

    vTF1 functions;
    vvdouble vresponses;

    TGraphErrors repsponse_mc(intervals.size());
    TGraphErrors corr_mc(intervals.size());
    Points responses_points_all_bins;
    int ibin=0;
    TString sGlobalPrefix = "L3_calc_";

    
    if ( g_correction_level == 3 )
	    g_sCorrection_level = "L3 corrected";
    if ( g_correction_level == 2)
    {
       g_sCorrectionAdd = "_l2corr";
        g_sCorrection_level = "corrected for #eta dependence";
    }
    if ( g_correction_level == 0 )
	    g_sCorrection_level = "uncorrected";
    
    for (int ialgo=0;ialgo<algos.size();++ialgo) {

        TString algo(algos[ialgo]);
        TString goodalgo(good_algos[ialgo]);

        TString the_info_string(info_string);
        the_info_string.ReplaceAll("__ALGO__",goodalgo);
    	the_info_string.ReplaceAll("__CORR__", g_sCorrection_level);
	
        TString quantity;
        boost::ptr_vector< DataHisto > histData;

        for (Intervals::iterator interval=intervals.begin();
                interval < intervals.end();++interval )
        {
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
            
            std::cout << ( quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd +"_"+ interval->id()+  "_hist");
            TH1D* respo = (TH1D*) ifileMc->Get(quantity+algo+"Jets_Zplusjet_mc" + g_sCorrectionAdd +"_"+ interval->id()+  "_hist");
//            respo->Rebin(pt_rebin_factor);
            respo->SetFillColor(kRed-9);
            respo->SetLineColor(kRed-9);
            respo->SetLineStyle(1);
            respo->Scale(1./respo->Integral());

            // Fill The response histo
            TString zpt_name("zPt_");
            zpt_name+=algo+"Jets_Zplusjet_mc"+g_sCorrectionAdd + "_"+interval->id() + "_hist";
            std::cout << zpt_name << " in usage" << std::endl;
            TH1D* zpt = (TH1D*) ifileMc->Get(zpt_name);
	    
            TString jet1_pt_name("jet1_pt_");
            jet1_pt_name+=algo+"Jets_Zplusjet_mc"+g_sCorrectionAdd+"_"+interval->id() + "_hist";
            std::cout << zpt_name << " in usage" << std::endl;
            TH1D* jet1_pt = (TH1D*) ifileMc->Get(jet1_pt_name);
	    

	      /// smal hack to circumvemt wrong MC data, TODO REMOVE !
	      if ( TMath::Abs( jet1_pt->GetMean() ) > 0.01 )
	      {
	      repsponse_mc.SetPoint(ibin,zpt->GetMean(),respo->GetMean());
	      repsponse_mc.SetPointError(ibin,zpt->GetMeanError(),respo->GetRMS());

		  corr_mc.SetPoint(ibin,jet1_pt->GetMean(),1.0f/respo->GetMean());
		  std::cout << "Added Point to MC Correction x: " << jet1_pt->GetMean() << " y: " << 1.0f/respo->GetMean() << std::endl;

	      ibin++;

	      }
	      // todo is this correct
	      //corr_mc.SetPointError(ibin,zpt->GetMeanError(),respo->GetRMS());

	      // End fill The response histo


            double maximum=0.03;

            h_resp.setLegPos(.77,.78,.95,.87);

            h_resp.addObjFormated(respo,"Monte Carlo","Hist");

            // fit section

            TF1 the_gaus(interval->id()+"the_gaus",
                         "[2]/(sqrt(2*TMath::Pi())*[1]*[4]) * exp(-0.5*((x-[0]*[3])/([1]*[4]))**2)",
                         0.,2.);
            the_gaus.SetParNames("Mean","Sigma","Norm","JES","JER");
            the_gaus.SetParameters(.8,.5,1);

//             the_gaus.FixParameter(2,1);

            the_gaus.FixParameter(4,1);
            the_gaus.FixParameter(3,1);
            the_gaus.SetLineStyle(2);
            respo->Fit(&the_gaus,"L");
            the_gaus.FixParameter(0,the_gaus.GetParameter(0));
            the_gaus.FixParameter(1,the_gaus.GetParameter(1));
            the_gaus.FixParameter(2,the_gaus.GetParameter(2));
            functions.push_back(the_gaus);

            // end fit section
            h_resp.addObjFormated(&the_gaus,"Gaussian Fit","L");
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
            h_resp.setBoardersY(0.0001,0.319);
            h_resp.setBoardersX(0,1.98);
        }

        int iCount = 0;
        for (Intervals::iterator interval=data_intervals.begin();
                interval != data_intervals.end();
                ++interval )
        {
            std::cout << "Generating binned_data_histo for " << interval->id() << std::endl;

            CanvasHolder * c_dataBinned = new CanvasHolder(algo+ "_data_binned" + interval->id() + "_canvas");
            TH1D * bHist = new TH1D(algo+ "_data_binned" + interval->id(),
                                    algo+ "_data_binned" + interval->id(),
                                    10,
                                    0.1f, 2.0f );

            c_dataBinned->setTitleY("Number of Events");
            c_dataBinned->setTitleX("Jet Response");

	    c_dataBinned->setBoardersX(0.0,2.0);
            c_dataBinned->setBoardersY(0.0,12.0);
            c_dataBinned->setLegPos(.75,.75,.95,.87);

            for (int i=0;i<responses_points_all_bins.size();++i)
            {
                if ( interval->contains(responses_points_all_bins[i].x))
                {
                    // fill Histo
                    std::cout << "Filling x: " << responses_points_all_bins[i].x << " y: " << responses_points_all_bins[i].y << std::endl;
                    bHist->Fill( responses_points_all_bins[i].y);
                }
            }
	    //bHist->SetFillColor(kRed-9);
	    //bHist->SetLineColor(kRed-9);
	    //bHist->SetLineStyle(1);
	    
	    c_dataBinned->setOptStat( kFALSE );
            c_dataBinned->addObjFormated( bHist, "Data", "Hist" );
            histData.push_back( new DataHisto( interval->min,
					interval->max,
					      bHist ));

	    c_dataBinned->addLatex(0.55,.93,interval->good_id(),true);
            formatHolderAlt(*c_dataBinned);
	    //c_dataBinned->setOptStat( 1101 );
            c_dataBinned->draw();
	    if ( g_correction_level == 0 )	    
	      saveHolder(*c_dataBinned,img_formats, false, "_raw", sGlobalPrefix);
	    if ( g_correction_level == 2 )	    
	      saveHolder(*c_dataBinned,img_formats, false, "_l2", sGlobalPrefix);
	    
            iCount++;

	    std::cout << "For Data: BinCenter = " << ( interval->max + interval->min ) / 2.0f << std::endl;
	    std::cout << "Min " << interval->min << " Max " << interval->max << std::endl;
            std::cout << "Mean" << bHist->GetMean() << "  RMS " << bHist->GetRMS() << std::endl;
            std::cout << "done" << std::endl;
        }


        // Prepare the response/MC plot for the response
        TGraph repsponse_data(responses_points_all_bins.size());
        for (int i=0;i<responses_points_all_bins.size();++i)
        {
            // std::cout << "PUNTI DEI DATI " << responses_points_all_bins[i].x << " " << responses_points_all_bins[i].y<<std::endl;
            repsponse_data.SetPoint(i,responses_points_all_bins[i].x,responses_points_all_bins[i].y);

        }
        repsponse_data.SetFillColor(kWhite);
        repsponse_data.SetMarkerStyle(22);
        repsponse_data.SetMarkerSize(1);
        repsponse_data.SetName("data");

        repsponse_data.Print();

        for (int i=0;i<2;++i) // to remove high pt bins
        {
            repsponse_mc.RemovePoint(repsponse_mc.GetN()-1);
            corr_mc.RemovePoint(corr_mc.GetN()-1);
        }

        repsponse_mc.Print();

        CanvasHolder h_response(algo+"_JetResponse");

        h_response.setTitleY("Jet Response");
        h_response.setTitleX("p_{T}^{Z} [GeV/c]");

        h_response.setBoardersY(0,2);
        h_response.setLegPos(.75,.75,.95,.87);

        repsponse_mc.SetLineColor(kRed);
        repsponse_mc.SetFillColor(kRed);
        repsponse_mc.SetMarkerColor(kBlack);
        repsponse_mc.SetMarkerSize(0.1);
        repsponse_mc.SetFillStyle(3002);
        h_response.addObjFormated(&repsponse_mc,"Monte Carlo","CE3");
        h_response.addObj(&repsponse_data,"Single events","P");
        h_response.addLatex(info_x,info_y,the_info_string,true);
/*
        if ( g_correction_level == 2 )
            h_response.addLatex(0.08,0.05, "Work in progress - L2 corrected Jets"  ,true);
        if ( g_correction_level == 0 )
            h_response.addLatex(0.08,0.05, "Work in progress - uncorrected Jets"  ,true);
*/

        formatHolder(h_response);
        h_response.draw();
        h_response.getCanvas()->cd();

        if ( g_correction_level == 2 )
            saveHolder(h_response,img_formats, false, "_l2", sGlobalPrefix);
        if ( g_correction_level == 0 )
            saveHolder(h_response,img_formats, false, "_raw", sGlobalPrefix);

  
	PlotJetCorrection( algo, histData, corr_mc, img_formats, sGlobalPrefix,
			the_info_string,
			  "[0] + [1]/(x^[3]) + [2]/(x^[4])",
			   "-Danilo-Func");
	PlotJetCorrection( algo, histData, corr_mc, img_formats, sGlobalPrefix,
			the_info_string,
			  "[0] + [1]/((log(x)^[2]) + [3])",
			   "-AN-Func");
			   
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
    TString treename=algoname+"Jets_Zplusjet_data_events";
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

void formatHolder(CanvasHolder& h, const char* legSym, int markersize, int lines_width, int skip_colors, bool do_flag, int optStat) {
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

void formatHolderAlt(CanvasHolder& h) { 
    //h.setLineColors(colors);
    //h.setMarkerSizeGraph(markersize);
    //h.setLinesSize(lines_width);
    //h.setLegDrawSymbol(legSym);
    h.setOptStat(0);
    h.addToCantopmargin(+0.0);
    h.addToCanleftmargin(+0.08);
    h.addToCanbottommargin(+0.05);
    h.setOptTitle(false);

    h.addLatex(.84,.93,"#sqrt{s}= 7 TeV",true);
    h.addLatex(.26,.93,lumi_str,true);
}


//------------------------------------------------------------------------------

void saveHolder(CanvasHolder &h,
                vString formats,
                bool make_log,
                TString sNamePostfix,
                TString sPrefix)
{
    TString mod_name(h.getTitle());
    h.setCanvasTitle(sPrefix + mod_name + sNamePostfix);

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
    else
    {
//      h.save("png");
        for (int i=0;i<formats.size();++i)
        {
//         h.draw();
            h.save(formats[i].Data());
        }
    }
}
