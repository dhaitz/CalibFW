#ifndef PLOTCOMMON_H
#define PLOTCOMMON_H

#include <exception>

//#include "Singleton.h"
#include "EventData.h"
#include "MinimalParser.h"
#include "PtBinWeighter.h"




class PlotEnv// : Templates::Singleton<PlotEnv>
{
  //friend class Templates::Singleton<PlotEnv>;
/*	template <class T>
	T* getObject(const char* objname, const char* filename){
	  TFile ifile(filename);
	  T* obj = (T*) ifile.Get(objname);
	  obj->SetDirectory(gROOT);
	  ifile.Close();
	  return obj;
	  }
	*/
public:
  void LoadFromConfig( MinimalParser p)
  {
    TString secname = "general";
    m_sPlotFileNamePostfix = p.getString(secname+".plot_file_postfix");
    m_iSkipBinsEnd = p.getInt(secname+".skip_bins_end");
    
    std::string sFileName = p.getConfigFileName().Data();
    
    size_t lastSlash = sFileName.rfind("/");
    std::cout << "/ pos " << lastSlash;
    if (lastSlash != std::string::npos )
    {
      m_sConfigFileName = sFileName.substr( lastSlash + 1, std::string::npos);
    }

  }

    TString m_sPlotFileNamePostfix;
    int m_iSkipBinsEnd;
    TString m_sConfigFileName;
    
//  private:
//    PlotEnv();
};


void saveHolder(CanvasHolder &h,
                vString formats,
                bool make_log,
                TString sNamePostfix,
                TString sPrefix,
		PlotEnv & env
 	      )
{
    TString mod_name(h.getTitle());
    h.setCanvasTitle(mod_name + sNamePostfix + env.m_sPlotFileNamePostfix);

    if (make_log) {
        TString can_name(h.getTitle());
        std::cout << "DEBUG: log scale for the canvas " << (can_name+"_log_y").Data() << std::endl;
        h.setCanvasTitle(can_name+"_log_y");
        h.setLogY();
    }

    for ( unsigned int i=0;i<formats.size();++i)
	{
//         h.draw();
		system ( "mkdir plot_out");
		system ( ("mkdir plot_out/" + env.m_sConfigFileName).Data() );
		h.save(formats[i].Data(), "" , ("plot_out/" + env.m_sConfigFileName + "/").Data());
	}
}


double CalcHistoError( TH1D * pHist, 
		       InputTypeEnum inpEnum,
		       double fScaleToLumi)
{
    if ( inpEnum == McInput )
    {
     // if (  fScaleToLumi < 0.1f )
	return pHist->GetMeanError();
//      else
//	return pHist->GetRMS() / ( TMath::Sqrt( pHist->GetSumOfWeights() * fScaleToLumi));
      
    }
    if ( inpEnum == DataInput )
    {
      return pHist->GetMeanError();
    }      
    throw "This InputTypeEnum is not supported";
}

class Intervals : public std::vector<PtBin>
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
            val[ i ] = new Double_t( it->GetMin() ) ;
            i++;
        }
        val[ i ] = new Double_t(  this->back().GetMin() );

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


#endif
 
 
