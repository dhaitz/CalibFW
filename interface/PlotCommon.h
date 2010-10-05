#ifndef PLOTCOMMON_H
#define PLOTCOMMON_H


//#include "Singleton.h"
#include "MinimalParser.h"
#include "PtBinWeighter.h"



class PlotEnv// : Templates::Singleton<PlotEnv>
{
  //friend class Templates::Singleton<PlotEnv>;
  
public:
  void LoadFromConfig( MinimalParser p)
  {
    TString secname = "general";
    m_sPlotFileNamePostfix = p.getString(secname+".plot_file_postfix");
  }

    TString m_sPlotFileNamePostfix;
  
//  private:
//    PlotEnv();
};



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
 
 
