// ---------------------------------------------------------------
// -----------------------      ExtHisto functions ---------------
// ---------------------------------------------------------------

#include "ExtHisto.h"
#include <vector>
#include <string>

#include <TH1.h>
#include <iostream>
#include <iomanip>

using namespace std;



void ExtHisto::initHistVal(){
  theClones=0;
  theLineColor=theHisto->GetLineColor();
  theLineStyle=theHisto->GetLineStyle();
}

ExtHisto::ExtHisto():theHisto(0){
  std::cout << "To be implemented...\n"; //DP
}

ExtHisto::ExtHisto(TH1* inHisto):theHisto(inHisto) {
  initHistVal();
  theLegTitle=theHisto->GetTitle();
}

ExtHisto::ExtHisto(TH1* inHisto,const char* inLegTitle,const char* inDrawOpt)
  //:theHisto(inHisto),theLegTitle(inLegTitle),theDrawOpt(inDrawOpt) {
  {
  //std::cout << "[ExtHisto::ExtHisto] Constructing\n";
  //std::cout << "[ExtHisto::ExtHisto] Assigning TH1* \n";
  theHisto=inHisto;
  theLegTitle=inLegTitle;
  //std::cout << "[ExtHisto::ExtHisto] Assigned legTitle --|"<< inLegTitle <<"|--\n";

  //std::cout << "[ExtHisto::ExtHisto] Assigning DrawOpt \n";
  theDrawOpt=inDrawOpt;
  //std::cout << "[ExtHisto::ExtHisto] Init.. \n";
  initHistVal();
}


void ExtHisto::formatHisto() {
  theHisto->SetLineColor(theLineColor);
  theHisto->SetLineStyle(theLineStyle);
}

void ExtHisto::cloneHisto(){
  if (theClones) cout<<"[CLONE HISTO] histo already cloned in this instance! --> "<<theClones<<endl;
  theHisto=dynamic_cast<TH1*>(theHisto->Clone());
}
