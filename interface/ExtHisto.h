//ExtHisto Class: Holds TH1* for CanvasHolder + CanvasHolder DrawOptions, etc.
// [WARNING] Class does only hold a POINTER of a histogramm - Memory Management done by CanvasHolder

#pragma once

#include <vector>
#include <string>

#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLine.h>
#include <iostream>
class ExtHisto {
 public:
  ExtHisto();
  ExtHisto(TH1* inHisto);
  ExtHisto(TH1* inHisto, const char* inLegName, const char* inDrawOpt = "");


  void initHistVal();
  void formatHisto();
  void cloneHisto();

  TH1* theHisto;
  std::string theLegTitle;
  std::string theDrawOpt;
  int theLineColor;
  int theLineStyle;
  int theClones;
  int theMarkerStyle;
  int theMarkerSize;
  int theMarkerColor;
  
  //ClassDef(ExtHisto,0)

};


class extGraph {
 public:
  extGraph() {};
  extGraph(TGraph* inGraph, const char* inName, const char* inLegName, const char* inDrawOpt) {
    theGraph = inGraph;
    theName = inName;
    theLegName = inLegName;
    theDrawOpt = inDrawOpt;
  }
  
  int theLineColor;
  int theLineStyle;
  int theMarkerColor;
  int theMarkerStyle;
  double theMarkerSize;
  
  std::string theName;
  std::string theLegName;
  std::string theDrawOpt;
  TGraph *theGraph;
  

  void formatGraph() {
    theGraph->SetLineColor(theLineColor);
    theGraph->SetLineStyle(theLineStyle);
    theGraph->SetMarkerColor(theMarkerColor);
    theGraph->SetMarkerStyle(theMarkerStyle);
    theGraph->SetMarkerSize(theMarkerSize);
  }


};

class extTF1 {
 public:
  extTF1() {};
  extTF1(TF1* inTF1, const char* inLegName, const char* inDrawOpt) {
    theTF1 = inTF1; 
    theLegName = inLegName; 
    theDrawOpt = inDrawOpt; 
  }; 
  
  TF1* theTF1;
  std::string theLegName;
  std::string theDrawOpt;
  int theLineColor;
  int theLineStyle;


  void formatTF1() {
    theTF1->SetLineColor(theLineColor);
    theTF1->SetLineStyle(theLineStyle);
  }
};




class extLatex {
 public:

  extLatex(){
    theX = 0;
    theY = 0;
    theName = "";
    is_ndc = false;
    }

  extLatex(double inX,double inY, const char* inName, bool in_is_ndc=false) {
    theX = inX;
    theY = inY;
    theName = inName;
    is_ndc = in_is_ndc;
  }
  double theX;
  double theY;
  bool is_ndc;
  std::string theName;
};



class extLine {
 public:
//DP
  extLine(){
    theLine = 0;
    thePos         = 0;
    theOrientation = "";
    theLegEntry    = "";
    theWidth       = 0;
    theColor       = 0;
    theStyle       = 0;
    
  }
  
  extLine(const double& inPos,
	  const std::string &inOrientation,
	  const std::string &inLegEntry,
	  const int &inWidth,
	  const int &inColor, 
	  const int &inStyle
	  )  {
    
    theLine = 0;
    thePos         = inPos;
    theOrientation = inOrientation;
    theLegEntry    = inLegEntry;
    theWidth       = inWidth;
    theColor       = inColor;
    theStyle       = inStyle;
    
  }
  
  TLine * theLine;
  double thePos;
  std::string theLegEntry;
  std::string theOrientation;
  
  int theColor;
  int theStyle;	   
  int theWidth;	   


  void makeLine(double xmin, double xmax, double ymin, double ymax) {
    if(!theLine) {
      if(theOrientation == "H") {
	theLine = new TLine(xmin,thePos,xmax,thePos);
      } else if(theOrientation == "V") {
	theLine = new TLine(thePos,ymin,thePos,ymax);
      } else {
	theLine = new TLine(xmin,ymin,xmax,ymax);
      }
      theLine->SetLineColor(theColor);
      theLine->SetLineStyle(theStyle);
      theLine->SetLineWidth(theWidth);
    }
  }
};


