#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <stdlib.h>

#include <stdio.h>
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
TString g_sCorrection_level = "";
TString g_sCorrectionAdd = "";
std::string g_configFileName = "";
//InputTypeEnum g_input_type;



PlotEnv g_plotEnv;

// description defaults
    double info_x=.3;
    double info_y=.78;

Intervals fill_intervals(vint edges);

TGraphErrors* histo2graph(TH1F* histo, double xmax,double ymax);
void formatHolder(CanvasHolder& h, const char* legSym="lf",  int size=1,int lines_width=2, int skip_colors=0, bool do_flag=false, int optStat = 0);
void saveHolder(CanvasHolder &h, vString formats, bool make_log = false, TString sNamePostfix = "", TString sPrefix = "");

//------------------------------------------------------------------------------

double CalcHistoError( TH1D * pHist, InputTypeEnum inpEnum)
{
    if ( inpEnum == McInput )
    {
      if (  g_lumi < 0.1f )
	return pHist->GetRMS();
      else
	return pHist->GetRMS() / ( TMath::Sqrt( pHist->GetSumOfWeights() * g_lumi));
      
    }
    if ( inpEnum == DataInput )
    {
      return pHist->GetMeanError();
    }      
}

struct ResponseSourceHistos
{
public: 
  DataHisto * histDataResponse;
  DataHisto * histZPt;
  InputTypeEnum inputType;
};

class GraphFormating
{
public: 
  GraphFormating( Color_t line, Color_t marker )
  {
      lineColor = line;
      markerColor = marker;
  }
  
  TString algoCaption;
  Color_t lineColor;
  Color_t markerColor;
};

// Plots the response of one or more algos into a CanvasHolder
void PlotResponse( std::vector<GraphFormating> & algoForamting,
		    std::vector<boost::ptr_vector<ResponseSourceHistos> > & histSource,
		    vString & img_formats, 
		    TString sGlobalPrefix,
		    TString the_info_string,
		    int iCutEntries = 0)
{
// JET Correction
        CanvasHolder h_corr(sGlobalPrefix+"_JetResponse");

	std::vector<boost::ptr_vector<ResponseSourceHistos> >::iterator itAlgos;
	std::vector< GraphFormating >::iterator itAlgoFormat = algoForamting.begin();
	
	for( itAlgos = histSource.begin();
	      itAlgos != histSource.end();
	      itAlgos ++)
	{
	    std::cout << "Plotting " << itAlgoFormat->algoCaption.Data() << std::endl;
	  
	    TGraphErrors * p_dataCalibPoints = new TGraphErrors(itAlgos->size() - iCutEntries);
	    //p_dataCalibPoints->SetName( sGlobalPrefix +  );
	    int i = 0;
	    
	    for ( boost::ptr_vector< ResponseSourceHistos >::iterator it = itAlgos->begin();
		    it != itAlgos->end();
		    ++it )
	    {
		p_dataCalibPoints->SetPoint(i, it->histZPt->m_pHist->GetMean(), it->histDataResponse->m_pHist->GetMean());
		p_dataCalibPoints->SetPointError(i, 
						CalcHistoError( it->histZPt->m_pHist, it->inputType ), 
						CalcHistoError( it->histDataResponse->m_pHist, it->inputType ));
		i++;
/*		if (( it->size() - i ) <= iCutEntries )
		  break;*/
	    }
	  
	    p_dataCalibPoints->SetLineColor(itAlgoFormat->lineColor);
	    p_dataCalibPoints->SetMarkerColor(itAlgoFormat->markerColor);
    //        p_dataCalibPoints->SetMarkerSize(1.0);
	    p_dataCalibPoints->SetFillStyle(0);
	    p_dataCalibPoints->SetMarkerStyle(21);
	    
	    std::cout << "Adding to canvas " << itAlgoFormat->algoCaption.Data() << std::endl;
	    h_corr.addObjFormated(p_dataCalibPoints, itAlgoFormat->algoCaption.Data() ,"LP");
	    
	    itAlgoFormat++;
	}
  
	std::cout << "done adding plots"<< std::endl;		
	
        h_corr.setTitleY("Jet Response");
        h_corr.setTitleX("p_{T}^{Z} [GeV/c]");

        h_corr.setBoardersY(0.11,1.89);
	h_corr.setBoardersX(0.11, 189.0);
	//h_corr.setBoardersX(0.0f, 170.0f );
        h_corr.setLegPos(.75,.75,.95,.87);

        TString sCaption;
/*
        if ( g_input_type == McInput )
            sCaption = "Binned MC";
        if ( g_input_type == DataInput )
            sCaption = "Binned Data";*/
        
        h_corr.addLatex(info_x,info_y,the_info_string,true);
	
	/*
        if ( g_correction_level == 2 )
            h_corr.addLatex(0.08,0.05, "Work in progress - L2 corrected Jets"  ,true);
        if ( g_correction_level == 0 )
            h_corr.addLatex(0.08,0.05, "Work in progress - uncorrected Jets"  ,true);
*/
//	h_corr.addLatex(0.08,0.01, "Fit function: " + plot_function, true);

        formatHolder(h_corr);
               
	//h_corr.draw();
	//h_corr.getCanvas()->cd();

        if ( g_correction_level == 3 )
            saveHolder(h_corr,img_formats, false, "_l3", sGlobalPrefix);
        if ( g_correction_level == 2 )
            saveHolder(h_corr,img_formats, false, "_l2", sGlobalPrefix );
        if ( g_correction_level == 0 )
            saveHolder(h_corr,img_formats, false, "_raw", sGlobalPrefix );
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

    g_configFileName = argv[1];
    
    size_t lastSlash = g_configFileName.rfind("/");
    std::cout << "/ pos " << lastSlash;
    if (lastSlash != std::string::npos )
    {
      g_configFileName = g_configFileName.substr( lastSlash + 1, std::string::npos);
    }
    
    
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

    vString source_files = p.getvString( secname+ ".source_files");
    vint source_number = p.getvInt( secname+ ".source_number");
    
    TString info_string = p.getString(secname+".info_string");
    TString sGlobalPrefix = p.getString(secname+".global_prefix");
    vString algos = p.getvString(secname+".algos");
    vString good_algos = p.getvString(secname+".good_algos");
    vString source_type = p.getvString(secname+".source_type");
    vint pt_bins = p.getvInt(secname+".pt_bins");
    vString img_formats= p.getvString(secname+".img_formats");
    double min_jes=p.getDouble(secname+".min_jes");
    double max_jes=p.getDouble(secname+".max_jes");
    double min_jer=p.getDouble(secname+".min_jer");
    double max_jer=p.getDouble(secname+".max_jer");
       
    /*
    if (p.getString(secname+".input_type") == "mc" )
      g_input_type = McInput;
    if (p.getString(secname+".input_type") == "data" )
      g_input_type = DataInput;*/
    
    g_plotEnv.LoadFromConfig( p);
       
// 0 = raw
// 2 = level2
// 3 = level3
    g_correction_level = p.getInt(secname+".correction_level");

    if (g_lumi < .1f )
      sprintf(lumi_str,"");
    else
      sprintf(lumi_str,"#scale[.8]{#int} L = %1.2f pb^{-1}",g_lumi);

//------------------------------------------------------------------------------

    std::vector<TFile *> ifiles;
    
    /*
    TFile * mc_ifile = new TFile (mc_input_file);
    TFile * data_ifile = new TFile (data_input_file);    
*/
    for (vString::iterator f = source_files.begin();
	    f < source_files.end(); ++f )
    {
      ifiles.push_back( new TFile( *f ) );
    }

// jet, mus, Z -- eta, pt, phi
    Intervals mc_intervals (fill_intervals(pt_bins));

    vTF1 functions;

    TGraphErrors repsponse_mc(mc_intervals.size());
    TGraphErrors corr_mc_jetpt(mc_intervals.size());
    Points responses_points_all_bins_zpt;
    Points responses_points_all_bins_jetpt;
    
    Points jec_data_binned;
    
    int ibin=0;


    
    if ( g_correction_level == 3 )
    {
	g_sCorrection_level = "corrected for #eta and Pt dependence";
	g_sCorrectionAdd = "l3corr"; 
    }
    if ( g_correction_level == 2)
    {
        g_sCorrectionAdd = "l2corr";
        g_sCorrection_level = "corrected for #eta dependence";
    }
    if ( g_correction_level == 0 )
    {
	g_sCorrectionAdd = "";
        g_sCorrection_level = "uncorrected";
    }

    std::vector< boost::ptr_vector< ResponseSourceHistos> > respHistos;
    std::vector<GraphFormating> algoStyle;
    
    std::cout <<  algos.size() << std::endl;

    	TString the_info_string(info_string);
        the_info_string.ReplaceAll("__CORR__", g_sCorrection_level);

    algoStyle.push_back( GraphFormating( kRed, kRed) );
    algoStyle.push_back( GraphFormating( kBlue, kBlue) );	
    algoStyle.push_back( GraphFormating( 8, 8) );	
    algoStyle.push_back( GraphFormating( kOrange, kOrange) );
    algoStyle.push_back( GraphFormating( kBlack, kBlack) );	
	
    Intervals intervals (fill_intervals(pt_bins));    
    for (int ialgo=0;ialgo<algos.size();++ialgo) {
        TString algo(algos[ialgo]);
        TString goodalgo(good_algos[ialgo]);
	std::cout << "Filling hist for algo " << goodalgo << std::endl;
	std::cout << source_type[ialgo];
	
	TFile *  sourceFile = NULL;
	InputTypeEnum local_input_type;
	
	sourceFile = ifiles[ source_number[ ialgo ]];
	if ( source_type[ialgo] == "mc")
	{
	  local_input_type = McInput;
	}
	if ( source_type[ialgo] == "data")
	{
	  local_input_type = DataInput;
	}

	
	respHistos.push_back( boost::ptr_vector<ResponseSourceHistos>() );

	algoStyle[ialgo].algoCaption = goodalgo;
	
	// get the response and jet1pt histos from the root file
        for (Intervals::iterator interval=intervals.begin();
                interval < intervals.end();++interval )
        {	
	    
	  
	    ResponseSourceHistos * pHistos = new ResponseSourceHistos;
	    pHistos->inputType = local_input_type;
            TString quantity="jetresp";
	    
	    TString histName = RootNamer::GetHistoName(algo,
						      quantity, 
						      local_input_type,
						      g_correction_level,
						      &*interval);
	    std::cout <<  histName.Data() << std::endl;
	    TH1D* respo = (TH1D*) sourceFile->Get( histName );
	    
	    if ( respo == NULL )
	      handleError("." , ("Can't load root histogram " + histName).Data() );

	    pHistos->histDataResponse = new DataHisto(interval->GetMin(), interval-> GetMax(), respo);

	    
	    quantity="zPt";
	    histName = RootNamer::GetHistoName(algo,
						quantity, 
						local_input_type,
						g_correction_level,
						&*interval);
	    respo = (TH1D*) sourceFile->Get( histName );
	    if ( respo == NULL )
	      handleError("." , ("Can't load root histogram " + histName).Data() );

	    pHistos->histZPt = new DataHisto(interval->GetMin(), interval-> GetMax(), respo);
	    respHistos.back().push_back( pHistos );
	}
	
    } 
    
    PlotResponse( algoStyle, 
		  respHistos,
		  img_formats, sGlobalPrefix,
		    the_info_string, g_plotEnv.m_iSkipBinsEnd);
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

//------------------------------------------------------------------------------

void saveHolder(CanvasHolder &h,
                vString formats,
                bool make_log,
                TString sNamePostfix,
                TString sPrefix)
{
    TString mod_name(h.getTitle());
    h.setCanvasTitle(mod_name + sNamePostfix + g_plotEnv.m_sPlotFileNamePostfix);

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
	// make needed folder
      
//      h.save("png");
        for (int i=0;i<formats.size();++i)
        {
//         h.draw();
	    system ( ("mkdir plot_out/" + g_configFileName).c_str() );
            h.save(formats[i].Data(), "" , "plot_out/" + g_configFileName + "/");
        }
    }
}
