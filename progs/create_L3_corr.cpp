#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

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


#include "PtBinWeighter.h"
#include "EventData.h"

#include "Utilities.h"
#include "PlotCommon.h"
#include "PlotCommon.h"

typedef std::vector<TF1> vTF1;
typedef std::vector< vdouble > vvdouble;


//------------------------------------------------------------------------------
/*
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
*/

double getX(TF1& func,double zval,double yval,double minXval,double maxXval);
double getY(TF1& func,double zval,double xval,double minYval,double maxYval);

class point;
typedef std::vector<point> Points;



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
double g_lumi=1.0f;
char lumi_str[100];

// yeah, i don't care either
int g_correction_level = 0;
InputTypeEnum g_input_type;
TString g_sCorrection_level = "";
TString g_sCorrectionAdd = "";

PlotEnv g_plotEnv;

// description defaults
    double info_x=.3;
    double info_y=.78;

    int g_LineCol;
int g_PointCol;
    
Intervals fill_intervals(vint edges);

TGraphErrors* histo2graph(TH1F* histo, double xmax,double ymax);
void formatHolder(CanvasHolder& h, const char* legSym="lf",  int size=1,int lines_width=2, int skip_colors=0, bool do_flag=false, int optStat = 0);
void formatHolderAlt(CanvasHolder& h);
void saveHolder(CanvasHolder &h, vString formats, bool make_log = false, TString sNamePostfix = "", TString sPrefix = "");

//------------------------------------------------------------------------------
double CalcHistoError( TH1D * pHist)
{
    if ( g_input_type == McInput )
    {
      return pHist->GetRMS() / ( TMath::Sqrt( pHist->GetSumOfWeights() * g_lumi));
    }
    if ( g_input_type == DataInput )
    {
      return pHist->GetMeanError();
    }      
}


void AddJetPoints ( 	boost::ptr_vector<DataHisto> & histDataResponse,
			boost::ptr_vector<DataHisto> & histDataJet1Pt,
			CanvasHolder & canvas,
			int iCutEntries = 0)
{	
	TString sCaption;
	TString sName;
        if ( g_input_type == McInput )
	{
            sCaption = "Binned MC";
	    sName = "BinMC";
	}
        if ( g_input_type == DataInput )
	{
            sCaption = "Binned Data";
	    sName = "BinData";
	}
	
        int i = 0;	
        TGraphErrors * p_dataCalibPoints = new TGraphErrors(histDataResponse.size());
		
	boost::ptr_vector< DataHisto >::iterator it_jet1pt = histDataJet1Pt.begin();
        for ( boost::ptr_vector< DataHisto >::iterator it = histDataResponse.begin();
                it != histDataResponse.end();
                ++it )
        {
	    std::cout << std::endl << "Adding " << i ;
	  
            p_dataCalibPoints->SetPoint(i, it_jet1pt->m_pHist->GetMean(), 1.0f / it->m_pHist->GetMean());
	    p_dataCalibPoints->SetPointError(i, 
					     CalcHistoError(it_jet1pt->m_pHist), 
					     /* calc using error propagation */
					     CalcHistoError( it->m_pHist) * 1.0f / (TMath::Power(it->m_pHist->GetMean(), 2.0)) );
	    //p_dataCalibPoints->SetPoint(i,( i + 1.0f) * 30.0f, 1.2f);
            i++;
	    it_jet1pt++;
	    
	    if (( histDataResponse.size() - i ) <= iCutEntries )
	      break;
        }
  
/*  
	// do the fit !
	TF1 * pDataFit = new TF1( "jecFit" + funcName, plot_function);
	pDataFit->SetParameter(0, 1.0f);
	pDataFit->SetParameter(1, 1.0f);
	pDataFit->SetParameter(2, 1.0f);
	pDataFit->SetParameter(3, 1.0f);
	pDataFit->SetParameter(4, 1.0f);
	
	pDataFit->SetLineColor(kRed);
	pDataFit->SetLineWidth(2);
	p_dataCalibPoints->Fit( pDataFit);
*/

	p_dataCalibPoints->SetLineColor(g_LineCol);
        p_dataCalibPoints->SetMarkerColor(g_PointCol);	
        p_dataCalibPoints->SetFillStyle(0);
	p_dataCalibPoints->SetLineWidth(2);
        p_dataCalibPoints->SetMarkerStyle(21);
	p_dataCalibPoints->SetName( sName);
	
	std::cout << std::endl << "Adding OBJ" << std::endl;
        canvas.addObjFormated(p_dataCalibPoints, sCaption,"P");        
}
			

void PlotJetCorrection( TString algo, 
			boost::ptr_vector<DataHisto> & histDataResponse,
			boost::ptr_vector<DataHisto> & histDataJet1Pt,
			vString & img_formats, 
			TString sGlobalPrefix,
			TString the_info_string,
			TString plot_function, 
			TString funcName, 
			int iCutEntries = 0)
{
// JET Correction

	
	/*
        if ( g_correction_level == 2 )
            h_corr.addLatex(0.08,0.05, "Work in progress - L2 corrected Jets"  ,true);
        if ( g_correction_level == 0 )
            h_corr.addLatex(0.08,0.05, "Work in progress - uncorrected Jets"  ,true);
*/
//	h_corr.addLatex(0.08,0.01, "Fit function: " + plot_function, true);


}


void getResponses(vdouble& responses,
                  Points& points_zpt, // x value is Z.Pt
		  Points& points_jet1pt, // x value is jet1.Pt
                  TFile* ifile,
                  PtBin interval,
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
        if (interval.IsInBin(Z->Pt())) {

	    double corrPtJet1 = 1.0f;
            if ( g_correction_level == 2 )
            {
	      // use corrected jet !!
	      corrPtJet1 = l2corr;
            }

	    response = ( jet->Pt() * corrPtJet1 ) /Z->Pt();

            responses.push_back(response);
	    
            points_zpt.push_back(point(Z->Pt(),response));
	    points_jet1pt.push_back(point(jet->Pt() * corrPtJet1,response));
        }
    }
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
    g_lumi = p.getDouble(secname+".lumi");
    int pt_rebin_factor = p.getInt(secname+".pt_rebin_factor");
    int phi_rebin_factor = p.getInt(secname+".phi_rebin_factor");
    int mass_rebin_factor = p.getInt(secname+".mass_rebin_factor");
    int eta_rebin_factor = p.getInt(secname+".eta_rebin_factor");
    TString input_file = p.getString(secname+".input_file");

    TString info_string = p.getString(secname+".info_string");
    vString algos = p.getvString(secname+".algos");
    vString good_algos = p.getvString(secname+".good_algos");
    vint pt_bins = p.getvInt(secname+".pt_bins");
    vString img_formats= p.getvString(secname+".img_formats");
    double min_jes=p.getDouble(secname+".min_jes");
    double max_jes=p.getDouble(secname+".max_jes");
    double min_jer=p.getDouble(secname+".min_jer");
    double max_jer=p.getDouble(secname+".max_jer");
    
    if (p.getString(secname+".input_type") == "mc" )
      g_input_type = McInput;
    if (p.getString(secname+".input_type") == "data" )
      g_input_type = DataInput;
    
    g_plotEnv.LoadFromConfig( p);
    
   
// 0 = raw
// 2 = level2
// 3 = level3
    g_correction_level = p.getInt(secname+".correction_level");

    sprintf(lumi_str,"#scale[.8]{#int} L = %1.2f pb^{-1}",g_lumi);

//------------------------------------------------------------------------------

    TFile * ifile = new TFile (input_file);
    std::cout << "Input file : " << input_file << std::endl;

// jet, mus, Z -- eta, pt, phi
    Intervals mc_intervals (fill_intervals(pt_bins));

    vTF1 functions;

    TGraphErrors repsponse_mc(mc_intervals.size());
    TGraphErrors corr_mc_jetpt(mc_intervals.size());
    Points responses_points_all_bins_zpt;
    Points responses_points_all_bins_jetpt;
    
    Points jec_data_binned;
    
    int ibin=0;
    TString sGlobalPrefix = "L3_calc_";

    
    if ( g_correction_level == 3 )
	    g_sCorrection_level = "L3 corrected";
    if ( g_correction_level == 2)
    {
       g_sCorrectionAdd = "l2corr";
        g_sCorrection_level = "corrected for #eta dependence";
    }
    if ( g_correction_level == 0 )
      g_sCorrection_level = "uncorrected";
    

  Intervals intervals (fill_intervals(pt_bins));    
  for (int ialgo=0;ialgo<algos.size();++ialgo) {

	  TString algo(algos[ialgo]);
	  TString goodalgo(good_algos[ialgo]);

	  boost::ptr_vector<DataHisto> dataHistJet1Pt;
	  boost::ptr_vector<DataHisto> dataHistResponse;
	  
	  TString the_info_string(info_string);
	  the_info_string.ReplaceAll("__ALGO__",goodalgo);
	  the_info_string.ReplaceAll("__CORR__", g_sCorrection_level);

	
    CanvasHolder h_corr(algo+"_JetCorrection");
    
    
    for ( int count = 0 ; count < 2; count ++)
    {
	dataHistResponse.clear();
	dataHistJet1Pt.clear();
      
	  if ( count == 0 )
	  {
	    ifile = new TFile (p.getString(secname+".data_input_file"));
	    g_input_type = DataInput;
	    g_LineCol = kBlack;
	    g_PointCol = kBlack;	    
	  }
	  if ( count == 1 )
	  {
	    ifile = new TFile (p.getString(secname+".mc_input_file"));
	    g_input_type = McInput;
	    g_LineCol = kRed;
	    g_PointCol = kRed;
	  }

	  // get the response and jet1pt histos from the root file
	  for (Intervals::iterator interval=intervals.begin();
		  interval < intervals.end();++interval )
	  {	
	      TString quantity="jetresp";
	      
	      TString histName = RootNamer::GetHistoName(algo,
							quantity, 
							g_input_type,
							g_correction_level,
							&*interval);
	      std::cout << std::endl <<  histName.Data();
	      TH1D* respo = (TH1D*) ifile->Get( histName );
	      
	      if ( respo == NULL )
		handleError("create_L3_corr" , ("Can't load root histogram " + histName).Data() );

	      dataHistResponse.push_back( new DataHisto(interval->GetMin(), interval->GetMax(), respo) );
	      
	      quantity="jet1_pt";
	      histName = RootNamer::GetHistoName(algo,
						  quantity, 
						  g_input_type,
						  g_correction_level,
						  &*interval);
	      respo = (TH1D*) ifile->Get( histName );
	      if ( respo == NULL )
		handleError("create_L3_corr" , ("Can't load root histogram " + histName).Data() );

	      dataHistJet1Pt.push_back( new DataHisto(interval->GetMin(), interval-> GetMax(), respo) );
	  }
	  	 
	  AddJetPoints( dataHistResponse,  dataHistJet1Pt, h_corr, g_plotEnv.m_iSkipBinsEnd );
      }
  /*
	// do the fit !
	TF1 * pDataFit = new TF1( "jecFit" + funcName, plot_function);
	pDataFit->SetParameter(0, 1.0f);
	pDataFit->SetParameter(1, 1.0f);
	pDataFit->SetParameter(2, 1.0f);
	pDataFit->SetParameter(3, 1.0f);
	pDataFit->SetParameter(4, 1.0f);
	
	pDataFit->SetLineColor(kRed);
	pDataFit->SetLineWidth(2.0f);
	p_dataCalibPoints->Fit( pDataFit);
*/

        
	//std::cout << "ChiSquare : " << pDataFit->GetChisquare() << std::endl;	

        h_corr.setTitleY("Jet Energy Correction");
        h_corr.setTitleX("p_{T}^{jet} [GeV/c]");

        h_corr.setBoardersY(1.0, 1.5);
	h_corr.setBoardersX(0.11, 179.0);
	//h_corr.setBoardersX(0.0f, 170.0f );
        h_corr.setLegPos(.75,.75,.95,.87);
	
	
	h_corr.addLatex(info_x,info_y,the_info_string,true);		
	
        formatHolder(h_corr);

        if ( g_correction_level == 2 )
            saveHolder(h_corr,img_formats, false, "_l2", sGlobalPrefix );
        if ( g_correction_level == 0 )
            saveHolder(h_corr,img_formats, false, "_raw", sGlobalPrefix );

	/*
	PlotJetCorrection( algo, dataHistResponse, dataHistJet1Pt, img_formats, sGlobalPrefix,
			the_info_string,
			  "[0] + [1]/(x^[3]) + [2]/(x^[4])",
			   "-Danilo-Func", g_plotEnv.m_iSkipBinsEnd);*/
/*	PlotJetCorrection( algo, histData, corr_mc_jetpt, img_formats, sGlobalPrefix,
			the_info_string,
			  "[0] + [1]/((log(x)^[2]) + [3])",
			   "-AN-Func");
*/			   
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


//------------------------------------------------------------------------------
Intervals fill_intervals(vint edges) {

    Intervals intervals;
    for (int i=0;i<edges.size()-1;i++)
        intervals.push_back(PtBin(edges[i],edges[i+1]));
    return intervals;
};

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

//    if (not do_flag) h.setLineStyles(lines_styles);
//     h.setLineColors(colors);
//    h.setMarkerSizeGraph(markersize);
//    h.setLinesSize(lines_width);
    h.setLegDrawSymbol(legSym);
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
    h.setCanvasTitle(sPrefix + mod_name + sNamePostfix + g_plotEnv.m_sPlotFileNamePostfix);

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
	    std::cout << std::endl << "Saving plot" << std::endl;
            h.save(formats[i].Data());
        }
    }
}
