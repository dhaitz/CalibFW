#pragma once

#include <iostream>
#include <iomanip>

#include <map>
#include <vector>
#include <string>


#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TFile.h>
#include <TColor.h>
#include <TClass.h>
#include <TLine.h>

#include <TLatex.h>

#include <TStyle.h>
#include "ExtHisto.h"

//need vector which can be initialized like a c-array: ;)
template <class T>
class myVector:public std::vector<T> {
 public:
  myVector(int size,const T *obj) {
    for (int i=0;i<size;++i){
      push_back(obj[i]);
    }
  }
};

class CanvasHolder:public TObject{
  
  class Diff_Histos {
  private:
    //there will be no private objects...
  public:
    Diff_Histos():eHistA(0),eHistB(0),use(true){}
      Diff_Histos(ExtHisto const &a,ExtHisto const &b):eHistA(a),eHistB(b),use(true){}
	ExtHisto eHistA;
	ExtHisto eHistB;
	bool use;
  };
 
 public:
  
  
  template <typename T>
    class SwitchVal {
  public:
    SwitchVal() {
      theState=false;  //da meckert der compiler - uninitialisiertusw... 
    }
    SwitchVal(T a) {
      theValue = a;
      theState = true;  //set sollte set machen... hmm
    }
    T getValue() {
      if(!theState) {
	std::cerr<<"Error: Variable not set!"<<std::endl;
      }
      return theValue;
    }
    
    inline void set(const T &val) {
      theValue = val;
      theState = true;
    }

    inline void set() {
      theState = true;
    }

    inline void unset() {
      theState=false;
    }
    
    bool getState() {
      return theState;
    }
    
  private:
    T theValue;
    bool theState;
  };
  
  CanvasHolder();

  CanvasHolder(const char* name);

  CanvasHolder(const std::vector<std::string> &mitems,
	       const std::vector<std::string> &blacklist,
	       const std::vector<TH1*> &inhist);

  CanvasHolder(const std::vector<std::string> &mitems,
	       const std::vector<std::string> &blacklist,
	       const std::map<std::string,TGraph*> &inmap);
  


  CanvasHolder(const CanvasHolder &inCan);

    ~CanvasHolder()
    {
    cleanExtHistos(ehistos);
    if (!theLeg) delete theLeg; 
    if (!theCan) delete theCan;
    if (!theStyle) delete theStyle;
  }

  /// save in various graphical formats and write on rootfile
  void saveAll(const char* tfile_opt="RECREATE");

  std::string getRnd();
  void addHisto(TH1* histo, const std::string legentry,std::string drawopt = "",double scale = -1);
  void addHisto(TH1* histo, const char* legentry,const char* drawopt = "",double scale = -1){
        addHisto(histo, std::string(legentry),std::string(drawopt),scale);
        }


  void add(const CanvasHolder &B);
  void show();
  void addHistoFormated(TH1* histo, const std::string legentry,std::string drawopt= "",double scale = -1);
  void setStandardValues();
  
  void addGraph(TGraph* inGraph, std::string inName, std::string inLegName, std::string inDrawOpt);
  void addGraphFormated(TGraph* inGraph, std::string inName, std::string inLegName, std::string inDrawOpt);
  
  bool draw(std::string opt = "");
  void write(TFile *file, std::string opt = "");
  void save(const std::string format, std::string opt = "", std::string sFolder = "");
  // -------------------------

  //DP
  /// Add a drawable object to the canvas without specifying the type
  void addObj(TObject* obj,const char* legname="",const char* drawopt="",double scale = -1);
  void addObjFormated(TObject* obj,const char* legname="",const char* drawopt="",double scale = -1);
  
  void cleanLegend(const std::map<std::string,std::string> &repmap);
  
  void divideHisto(const std::string &search);
  void divideHisto(const ExtHisto &a);
  //DP
  void exchangeAction(char action,CanvasHolder &B,const std::vector<std::string> &NoDiffIn){std::cout << "Not implemented\n";};
  void divide(CanvasHolder &B,const std::vector<std::string> &NoDiff){
    exchangeAction('d',B,NoDiff);
  }
  void multiply(CanvasHolder &B,const std::vector<std::string> &NoDiff){
    exchangeAction('m',B,NoDiff);
  };

  void setLineColors(const std::vector<int> &vcolor);
  void setLineColors(const int &incolor);
  void setLineColors();
  void setLineStyles(const std::vector<int> &vstyle);
  void setLineStyles(const int &instyle);
  void setLineStyles();
  void setLineSize(const int &insize);

  void getExtHistos(std::vector<ExtHisto> &HistVec) const ;

  void setOptStat(const Int_t &opt){theOptStat=opt;}
  void setOptTitle(const Int_t &opt){theOptTitle=opt;}
  void setOptFit(const Int_t &opt){theOptFit=opt;}


  void setLegend(const std::vector<std::string> &inLeg);
  void setLegDraw(bool gna=true){theDrawLegend=gna;}
  void setLegPos(double x1,double y1,double x2,double y2) {theLegX1=x1;theLegX2=x2;theLegY1=y1;theLegY2=y2;}
  void setLegTitle(const std::string &inLegTitle) {theLegTitle.set(inLegTitle);};
  void setLegTitle(const char* inLegTitle) {theLegTitle.set(inLegTitle);};
  void setLegNCol(const int inLegNCol) {theLegNCol.set(inLegNCol);};
  void setLegMargin(const double inLegMargin) {theLegMargin.set(inLegMargin);};
  void setLegBorderSize(const double inLegBorder) {theLegBorder.set(inLegBorder);};


  void setLegDrawSymbol(std::string inLegDrawSymbol) { theLegDrawSymbol = inLegDrawSymbol;}

  void setLogX(bool flag = true){theLogX=flag;}
  void setLogY(bool flag = true){theLogY=flag;}
  void setLogZ(bool flag = true){theLogZ=flag;}
    
  void setTitle(const char* title){theTitle.set(title);}

  const char* getTitle(){return theCanvasTitle.c_str();}

  void setCanvasTitle(const char* title) {theCanvasTitle=title;};
  void addToCanvasTitle(std::string title) {theCanvasTitle=theCanvasTitle+title;};

  void setTitleX(const std::string &titx){theTitleX.set(titx);}
  void setTitleY(const std::string &tity){theTitleY.set(tity);}
  void setTitleZ(const std::string &titz){theTitleZ.set(titz);}

  void setTitleX(const char* titx){theTitleX.set(titx);}
  void setTitleY(const char* tity){theTitleY.set(tity);}
  void setTitleZ(const char* titz){theTitleZ.set(titz);}

  void setBinning(const std::string &bintype){
    std::vector<double> binning;
    if (bintype=="QCD"){
      fillQcdBinning(binning);
    }
    else{
      std::cerr<<"No Such Binning: "<<bintype<<std::endl;
      return;
    }
    
    doNewBinning(binning);
  }
  void setBinning(const std::vector<double> &binning){
    doNewBinning(binning);
  }
  
  //DP
  void setYperBinWidth(bool b=true){std::cout << "Not implemented\n";};
  void setCancelOutBigErr(bool b=true){if (b) doTheCancelOutBigErr();}
  


  void setBoardersX(const double min,const double max){
    theMinX.set(min);
    theMaxX.set(max);
  }

  void setBoardersY(const double min,const double max){
    theMinY.set(min);
    theMaxY.set(max);
  }

  void scaleBoardersY(const double min_s,const double max_s){
    m_minY_s=min_s;
    m_maxY_s=max_s;
  }

  void setBoarders(const double xmin,const double ymin,
                   const double xmax,const double ymax){
    theMinX.set(xmin);
    theMaxX.set(xmax);
    theMinY.set(ymin);
    theMaxY.set(ymax);
  }

  std::string getHistoType() const {return theHistoType;}

  void normalizeHistos();
  void scaleHistos(const double &factor){doScaleHistos(factor);}

  void scaleHistosToHisto (const char* name){doScaleHistosToHisto(name);}
  void doScaleHistosToHisto (const char* name);

  void divideHistosByHisto (const char* name, bool add_to_legend=false);

  void setError_1_N(const double &n){doSetError_1_N(n);}

//   void addHLine(const double& inPos,
// 		const std::string &inLegEntry = "",
// 		const int &inWidth = 1,
// 		const int &inColor = 1, 
// 		const int &inStyle = 1) {
//     theLines.push_back(extLine(inPos,"H",inLegEntry,inWidth,inColor,inStyle));
//   }

  void addHLine(const double& inPos,
        const char* inLegEntry = "",
        const int &inWidth = 1,
        const int &inColor = 1, 
        const int &inStyle = 1) {
    theLines.push_back(extLine(inPos,"H",inLegEntry,inWidth,inColor,inStyle));
  }

//   void addVLine(const double& inPos,
// 		const std::string &inLegEntry = "",
// 		const int &inWidth = 1,
// 		const int &inColor = 1, 
// 		const int &inStyle = 1) {
//     theLines.push_back(extLine(inPos,"V",inLegEntry,inWidth,inColor,inStyle));
//   }

  void addVLine(const double& inPos,
        const char* inLegEntry = "",
        const int &inWidth = 1,
        const int &inColor = 1, 
        const int &inStyle = 1) {
    theLines.push_back(extLine(inPos,"V",inLegEntry,inWidth,inColor,inStyle));
  }
  
  void setLineStylesGraph(const int &instyle);
  void setLineColorsGraph(const int &instyle);
  void setLineSizeGraph(const int &insize);

  void setLinesSize(const int &insize){
    setLineSize(insize);
    setLineSizeGraph(insize);
    setLineSizeTF1(insize);}

  void setMarkerStylesGraph(const int &instyle);
  void setMarkerColorsGraph(const int &instyle);
  void setDrawOptGraph(std::string instring);
  void setMarkerSizeGraph(const double &instyle);
  
  void setLineSizeTF1(const int &insize);

  void setFont(const int &inFont) {theFont.set(inFont);};
  void setYaxisOff(const double &inYaxisOff) {theYaxisOff = inYaxisOff;};
  void setXaxisOff(const double &inXaxisOff) {theXaxisOff = inXaxisOff;};

  void setLegW(const double &inVal) {theLegW = inVal;};
  void setLegH(const double &inVal) {theLegH = inVal;};
  void addLegW(const double &inVal) {theLegW = theLegW + inVal;};
  void addLegH(const double &inVal) {theLegH = theLegH + inVal;};

  

  void setLegPos(std::string inPos) {theLegPos.set(inPos);};
  const char* getLegPos(){if (theLegPos.getState()) return theLegPos.getValue().c_str();else return "";};
  void makeLegPos( const std::string &inVal);


  void addToCanleftmargin(const double &inVal) {theCanleftmargin+=inVal;};
  void addToCanrightmargin(const double &inVal) {theCanrightmargin+=inVal;};
  void addToCantopmargin(const double &inVal) {theCantopmargin+=inVal;};
  void addToCanbottommargin(const double &inVal) {theCanbottommargin+=inVal;};

  void setDrawOpt(const std::string &inDrawOpt) {theDrawOpt = inDrawOpt;};


  void addTF1(TF1* inTF1, std::string inLegName, std::string inDrawOpt = "") {
    extTF1 tmp(inTF1,inLegName.c_str(),inDrawOpt.c_str());
    tmp.theLineColor = getNextColor();
    tmp.theLineStyle = getNextStyle();
    tmp.theDrawOpt = inDrawOpt;

    //tmp.formatTF1()

 //   tmp.theTF1->SetLineColor( tmp.theLineColor);
 //   tmp.theTF1->SetLineStyle( tmp.theLineStyle);
 //   tmp.theTF1->SetLineWidth(2);
    

    theTF1Vec.push_back(tmp);
  }

  void addTF1Formated(TF1* inTF1, std::string inLegName, std::string inDrawOpt) {
    extTF1 tmp(inTF1,inLegName.c_str(),inDrawOpt.c_str());

    theTF1Vec.push_back(tmp);
  }

  void addLatex(double inX, double inY, const char* inName, bool is_ndc=false) {
    extLatex tmp(inX,inY,inName,is_ndc);
    theLatexVec.push_back(tmp);
  }


  // DP
 TStyle* getStyle(){return theStyle;};

 TH1* getHisto(unsigned int index){if(index >= ehistos.size() or index<0)return 0;else return ehistos[index].theHisto;};

 void setGrid(bool val=true){m_grid_x=m_grid_y=val;};
 void setGridX(bool val=true){m_grid_x=val;};
 void setGridY(bool val=true){m_grid_y=val;};

 TCanvas* getCanvas(){return theCan;};

 private:

  double m_minY_s;
  double m_maxY_s;

  bool m_grid_x;
  bool m_grid_y;

  void setDefaultStyle();
  void doSetError_1_N(const double &n);

  bool mitmatch(const std::string &hina,const std::vector<std::string> &items,const std::vector<std::string> &blacklist);

  std::vector<ExtHisto> ehistos;
  void doNewBinning(const std::vector<double> &binning){std::cout << "Not implemented!\n";};
  void reBinHisto(TH1D *&Histo,const std::vector<double> &binning);
  void doScaleHistos(const double &wert);
  void doYperBinWidth();
  void doTheCancelOutBigErr(){std::cout << "Not implemented!\n";};


  void fillQcdBinning(std::vector<double> &binnings){std::cout << "Not implemented!\n";};
 

  // Canvas
  TCanvas *theCan;
  TStyle *theStyle;
  std::string theMatchString;
  std::string theCanvasTitle;
  bool theLogX,theLogY,theLogZ;
  Int_t theOptStat,theOptTitle,theOptFit;
  double theCanleftmargin, theCanrightmargin, theCantopmargin,theCanbottommargin;

  // Legend
  TLegend *theLeg;
  double theLegX1,theLegY1,theLegX2,theLegY2;
  bool theDrawLegend;
  SwitchVal<std::string> theLegTitle;
  SwitchVal<int> theLegNCol;
  SwitchVal<double> theLegMargin;
  SwitchVal<double> theLegBorder;

  std::string theLegDrawSymbol;
  double theLegW, theLegH;
  SwitchVal<std::string> theLegPos;


  // Histogramms
  SwitchVal<double> theMinX,theMaxX;
  SwitchVal<double> theMinY,theMaxY;
  SwitchVal<std::string> theTitle,theTitleX,theTitleY,theTitleZ;
  std::string theHistoType;
  bool theXaxisDec, theYaxisDec,theDoYperBinWidth, theCancelOutBigErr, theYperBinWidth;
  double theYaxisOff,theXaxisOff;
  std::string theDrawOpt;


  bool theUserSetXRange, theUserSetYRange;

  // Color and Style
  std::vector<int> fillColors;
  SwitchVal<int> theFont;
  int colorsPa[22];


  // Lines

//  std::vector<TLine> theLines;
//  std::map<TLine*,std::string> theLines;
  std::vector<extLine> theHLines;
  std::vector<extLine> theVLines;

   std::vector<extLine> theLines;
   


  std::vector<extGraph> theGraphVec;

  std::vector<extTF1> theTF1Vec;
  std::vector<extLatex> theLatexVec;

  void doNormalizeHisto(TH1 *hist);
  
  void formatHistos();
  void formatHisto(TH1* inHisto);
  void setLineColor();

  int theNextColor; 
  int theNextStyle; 
  int getNextColor();
  int getNextStyle();


  void prepareCanvas();
  void drawPad(TVirtualPad *a,std::string PadDrawOpt="");
  void cleanExtHistos(std::vector <ExtHisto>  &cand);
  std::string doCutOut(const std::string &instring,
		       const std::vector<std::string> &cutIt);
  std::string doStringRep(const std::string &instring, 
			  const std::string &cutIt,
			  const std::string &rep);
  
  std::string doStringRep(std::string instring, 
			  const std::map< std::string , std::string > &repmap);

  
    // For Cint
    ClassDef(CanvasHolder,1) 

};
