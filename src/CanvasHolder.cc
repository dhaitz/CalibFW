#include "CanvasHolder.h"

#include <ctime>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>

using namespace std;
// ---------------------------------------------------------------
// ----------------------- Canvas_Holder functions ---------------
// ---------------------------------------------------------------

string CanvasHolder::getRnd(){
  int r=rand()%10000;
  char buf[7];
  sprintf (buf,"_R%d",r);
  return string(buf);
}

// -----------------------------------------------------------------------------

CanvasHolder::CanvasHolder(){ 
  //cout<<"I am empty"<<endl;

  setStandardValues();
  theCanvasTitle=getRnd();
}

// -----------------------------------------------------------------------------

CanvasHolder::CanvasHolder(const char* name){

   setStandardValues();
   theCanvasTitle=getRnd();
   setCanvasTitle(name);

    }

// -----------------------------------------------------------------------------

CanvasHolder::CanvasHolder(const vector<std::string> &mitems,
			   const vector<std::string> &blacklist,
			   const vector<TH1*> &inhist){
  setStandardValues();
  
  for (vector<std::string>::const_iterator iter=mitems.begin();iter!=mitems.end();++iter){
    theMatchString+=(*iter);
  }
 
  cout<<"theMatchString: "<<theMatchString<<endl;
  for (vector<TH1*>::const_iterator iter=inhist.begin();iter!=inhist.end();++iter){
    if(iter==inhist.begin()) {theHistoType=(*iter)->IsA()->GetName();}
    //cout<<"type: "<<theHistoType<<" name: "<<(*iter)->GetName()<<endl;
    if (mitmatch((*iter)->GetName(),mitems,blacklist)) {
      if ( (*iter)->IsA()->GetName()!=theHistoType) {
	cerr<<"[ERROR] Histo-Type differs! Not adding "<<(*iter)->GetName()<<endl;
	cerr<<(*iter)->IsA()->GetName()<<"!="<<theHistoType<<endl;
	continue;
      }
      addHisto(*iter,doCutOut((*iter)->GetName(),mitems),"");
      cout<<"Match: "<<(*iter)->GetName()<<endl;
    }
  }

  // TODO: Titel setzen! Automatisch + Funktion
  if (ehistos.begin()!=ehistos.end())
    theCanvasTitle=ehistos.front().theHisto->GetName()+getRnd();    
  //cout<<"new Title:"<<theCanvasTitle<<endl;
}

// -----------------------------------------------------------------------------

CanvasHolder::CanvasHolder(const std::vector<std::string> &mitems,
			   const std::vector<std::string> &blacklist,
			   const std::map<std::string,TGraph*> &inmap){

  setStandardValues();
  
  for (vector<std::string>::const_iterator iter=mitems.begin();iter!=mitems.end();++iter){
    theMatchString+=(*iter);
  }
  theCanvasTitle = theMatchString;
  
  for (map<std::string,TGraph*>::const_iterator iter=inmap.begin();iter!=inmap.end();++iter){
    if (mitmatch(iter->first,mitems,blacklist)) {
      addGraph(iter->second,iter->first,doCutOut(iter->first,mitems),"L");
    }
  }
}

// -----------------------------------------------------------------------------
/**
Save the plot in the following graphical formats:
  - png
  - eps
and store it in a rootfile, opened according the tfile_opt, which is RECREATE 
by default.
**/
void CanvasHolder::saveAll(const char* tfile_opt){

    TFile ofile(getTitle(),tfile_opt);
    ofile.cd();
    write(&ofile);
    ofile.Close();

    TString formats[2]={"png","eps"};
    for (int i=0;i<2;++i)
        save(formats[i].Data());

    }

// -----------------------------------------------------------------------------

void CanvasHolder::addGraph(TGraph* inGraph, std::string inName, std::string inLegName, std::string inDrawOpt) {

  extGraph tmpGraph(inGraph,inName.c_str(),inLegName.c_str(),inDrawOpt.c_str());
  TString hname_s (inGraph->GetName());
   if (hname_s.Contains("data") or hname_s.Contains("Data")){
            tmpGraph.theMarkerColor = 1;
            tmpGraph.theLineColor = 1;
            tmpGraph.theMarkerStyle = 20;
            }
  else{
  int tmpColor = getNextColor();
  int tmpStyle = getNextStyle();
  tmpGraph.theLineColor = tmpColor;
  tmpGraph.theLineStyle = tmpStyle;
  tmpGraph.theMarkerColor = tmpColor;
  tmpGraph.theMarkerStyle = tmpStyle+20;
    }
  theGraphVec.push_back(tmpGraph); 

}

void CanvasHolder::addGraphFormated(TGraph* inGraph, std::string inName, std::string inLegName, std::string inDrawOpt) {
  extGraph tmpGraph(inGraph,inName.c_str(),inLegName.c_str(),inDrawOpt.c_str());

  cout << "Insertion of TGraph color = " << inGraph->GetLineColor() << std::endl;

  tmpGraph.theLineColor =  inGraph->GetLineColor();
  tmpGraph.theLineStyle = inGraph->GetLineStyle();
  tmpGraph.theMarkerColor = inGraph->GetMarkerColor();
  tmpGraph.theMarkerStyle = inGraph->GetMarkerStyle();
  tmpGraph.theMarkerSize  = inGraph->GetMarkerSize();
  theGraphVec.push_back(tmpGraph); 

}


CanvasHolder::CanvasHolder(const CanvasHolder &inCanvas) {
  setStandardValues(); //Copy Values from other CanvasHolder?
  theHistoType=inCanvas.getHistoType();
  CanvasHolder::add(inCanvas);
  theCanvasTitle = ehistos.front().theHisto->GetName()+getRnd();
  
}

void CanvasHolder::add(const CanvasHolder &inCanvas){  
  if (ehistos.begin()==ehistos.end()) theHistoType=inCanvas.getHistoType();
  if ( inCanvas.getHistoType()!=theHistoType) {
    cerr<<"[ERROR] Histo-Type differs! Not adding CanvasHolder!"<<endl;
    cerr<<inCanvas.getHistoType()<<"!="<<theHistoType<<endl;
    return;
  }
  inCanvas.getExtHistos(ehistos);
}

void CanvasHolder::show(){
  for (vector<ExtHisto>::const_iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    cout<<"[show] "<<(*iter).theLegTitle<<endl;
    TCanvas *CANNE=new TCanvas((*iter).theHisto->GetTitle(),(*iter).theHisto->GetTitle(),800,600);
    CANNE->cd();
    (*iter).theHisto->Draw();
  }
}

void CanvasHolder::doSetError_1_N(const double &scale){
  if (scale==0) {
    cout<<"[ERROR ]  -  [SCALE is ZERO!]"<<endl;
    return;
  }
 
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    string type=ehistos.at(0).theHisto->IsA()->GetName();
    if (type=="TH1D") {
      unsigned int bins=(*iter).theHisto->GetNbinsX();
      for (unsigned int i=0;i<bins;++i)
	{
	  double value=(*iter).theHisto->GetBinContent(i);
	  double newErr=sqrt(1/(scale*value));
	  (*iter).theHisto->SetBinError(i,newErr);
	}
    }
  }
}

void CanvasHolder::addHisto(TH1* histo, const std::string legentry, std::string drawopt,double scale) {

  if (histo !=0 and fabs(histo->GetEntries())>0.0000000000001){
//     std::cout << "Creating ExtHisto with leg entry: " << legentry.c_str() << std::endl;
    TString hname(histo->GetName());
    TH1* histoc=(TH1*)(histo->Clone(hname+"_CHcloned"));
    if (histoc->GetSumw2N()==0)
        histoc->Sumw2();
    if (scale > 0){
//         std::cout << "scaling to " << scale << std::endl;
        histoc->Scale(scale);}
    ExtHisto tmpHisto(histoc,legentry.c_str(),drawopt.c_str());
    tmpHisto.theLineColor=getNextColor();
    tmpHisto.theLineStyle=getNextStyle();
    TString drawopt_str(drawopt);
    if (drawopt_str.Contains("P") or drawopt_str.Contains("p")){
//         std::cout << "Histograms with markers...\n";
        TString hname_s(hname);
        if (hname_s.Contains("data") or hname_s.Contains("Data")){
//             std::cout << "DEBUG: [CanvasHolder::addHisto] Adding a TH1F with black points! Name is "<< hname<< "\n";
            tmpHisto.theMarkerColor = 1;
            tmpHisto.theLineColor = 1;
            tmpHisto.theMarkerStyle = 20;
            }
        else{
        tmpHisto.theMarkerColor = tmpHisto.theLineColor;
        tmpHisto.theMarkerStyle = 20;//tmpHisto.theLineStyle+20+1;
            }
        }
    else{
        tmpHisto.theMarkerStyle=0;
        tmpHisto.theMarkerSize=0;
        }
    ehistos.push_back(tmpHisto);
    }
  else
    std::cout << "Histo is empty. *NOT* creating ExtHisto with leg entry: " << legentry.c_str() << std::endl;

}

void CanvasHolder::addHistoFormated(TH1* histo, const std::string legentry,std::string drawopt,double scale) {;
  if (histo !=0 and histo->GetEntries()>0.0000000000001){
//     std::cout << "Creating ExtHisto with leg entry: " << legentry.c_str() << std::endl;
    TString hname(histo->GetName());
    TH1* histoc=dynamic_cast<TH1*>(histo->Clone(hname+"_CHcloned"));
//     histoc->SetLineWidth(histo->GetLineWidth());
//     histoc->SetLineColor(histo->GetLineColor());
    if (histo->GetSumw2N()==0)
        histo->Sumw2();
    if (scale > 0){
        histo->Scale(scale);
        }
    ExtHisto tmpHisto(histoc,legentry.c_str(),drawopt.c_str());
    tmpHisto.theLineColor=histo->GetLineColor();
    TString drawopt_str(drawopt);
    ehistos.push_back(tmpHisto);
    }
  else
    std::cout << "Histo is empty. *NOT* creating ExtHisto with leg entry: " << legentry.c_str() << std::endl;
}

int CanvasHolder::getNextColor(){
  if(theNextColor == 5) theNextColor++; 
  if(theNextColor == 10) theNextColor+=30; 
  return theNextColor++; 
}; 
int CanvasHolder::getNextStyle(){
  if(theNextStyle == 5) theNextStyle++; 
  if(theNextStyle == 10) theNextStyle+=30; 
  return theNextStyle++; 
};



void CanvasHolder::setDefaultStyle(){
  if(!theStyle) {
        //theStyle = new TStyle(getRnd().c_str(),getRnd().c_str());
        theStyle = new TStyle("cholder","cholder");
    }
// For the canvas:
  theStyle->SetCanvasBorderMode(0);
  theStyle->SetCanvasColor(kWhite);
  theStyle->SetCanvasDefH(600); //Height of canvas
  theStyle->SetCanvasDefW(600); //Width of canvas
  theStyle->SetCanvasDefX(0);   //POsition on screen
  theStyle->SetCanvasDefY(0);

// For the Pad:
  theStyle->SetPadBorderMode(0);
  // theStyle->SetPadBorderSize(Width_t size = 1);
  theStyle->SetPadColor(kWhite);

  theStyle->SetPadGridX(m_grid_x);
  theStyle->SetPadGridY(m_grid_y);
  theStyle->SetGridColor(0);
  theStyle->SetGridStyle(3);
  theStyle->SetGridWidth(1);
// Volker -->
  theStyle->SetPadTopMargin(0.05);
  theStyle->SetPadBottomMargin(0.13);
  theStyle->SetPadLeftMargin(0.14);
  theStyle->SetPadRightMargin(0.08);
// <--

// For the frame:
  theStyle->SetFrameBorderMode(0);
  theStyle->SetFrameBorderSize(1);
  theStyle->SetFrameFillColor(0);
  theStyle->SetFrameFillStyle(0);
  theStyle->SetFrameLineColor(1);
  theStyle->SetFrameLineStyle(1);
  theStyle->SetFrameLineWidth(1);

// For the histo:
  // theStyle->SetHistFillColor(1);
  // theStyle->SetHistFillStyle(0);
  theStyle->SetHistLineColor(1);
  theStyle->SetHistLineStyle(0);
  theStyle->SetHistLineWidth(2);
  // theStyle->SetLegoInnerR(Float_t rad = 0.5);
  // theStyle->SetNumberContours(Int_t number = 20);

  theStyle->SetEndErrorSize(2);
  //  theStyle->SetErrorMarker(20);
  theStyle->SetErrorX(0.);
  
  theStyle->SetMarkerStyle(20);

//For the fit/function:
//  theStyle->SetOptFit(1);
  theStyle->SetOptFit(0);
  theStyle->SetFitFormat("5.4g");
  theStyle->SetFuncColor(1);
  theStyle->SetFuncStyle(1);
  theStyle->SetFuncWidth(1);

//For the date:
  theStyle->SetOptDate(0);
  // theStyle->SetDateX(Float_t x = 0.01);
  // theStyle->SetDateY(Float_t y = 0.01);

// For the statistics box:
  theStyle->SetOptFile(0);
  theStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  theStyle->SetStatColor(kWhite);
  theStyle->SetStatFont(42);
  theStyle->SetStatFontSize(0.025);
  theStyle->SetStatTextColor(1);
  theStyle->SetStatFormat("6.4g");
  theStyle->SetStatBorderSize(1);
  theStyle->SetStatH(0.1);
  theStyle->SetStatW(0.15);
  // theStyle->SetStatStyle(Style_t style = 1001);
  // theStyle->SetStatX(Float_t x = 0);
  // theStyle->SetStatY(Float_t y = 0);

// Margins:
  theStyle->SetPadTopMargin(0.05);
  theStyle->SetPadBottomMargin(0.23);
  theStyle->SetPadLeftMargin(0.17);
  theStyle->SetPadRightMargin(0.05);

// For the Global title:

  theStyle->SetOptTitle(0);
  theStyle->SetTitleFont(42);
  theStyle->SetTitleColor(1);
  theStyle->SetTitleTextColor(1);
  theStyle->SetTitleFillColor(10);
  theStyle->SetTitleFontSize(0.05);
  // theStyle->SetTitleH(0); // Set the height of the title box
  // theStyle->SetTitleW(0); // Set the width of the title box
  // theStyle->SetTitleX(0); // Set the position of the title box
  // theStyle->SetTitleY(0.985); // Set the position of the title box
  // theStyle->SetTitleStyle(Style_t style = 1001);
  // theStyle->SetTitleBorderSize(2);

// For the axis titles:

  theStyle->SetTitleColor(1, "XYZ");
  theStyle->SetTitleFont(42, "XYZ");
//  theStyle->SetTitleSize(0.06, "XYZ");
  theStyle->SetTitleSize(0.05, "XYZ");
  // theStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // theStyle->SetTitleYSize(Float_t size = 0.02);
  theStyle->SetTitleXOffset(0.9);
  theStyle->SetTitleYOffset(1.1);//1.05
  // theStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

// For the axis labels:

  theStyle->SetLabelColor(1, "XYZ");
  theStyle->SetLabelFont(42, "XYZ");
  theStyle->SetLabelOffset(0.007, "XYZ");
//  theStyle->SetLabelSize(0.05, "XYZ");
  theStyle->SetLabelSize(0.045, "XYZ"); //Volker

// For the axis:

  theStyle->SetAxisColor(1, "XYZ");
  theStyle->SetStripDecimals(kTRUE);
  theStyle->SetTickLength(0.03, "XYZ");
  theStyle->SetNdivisions(510, "XYZ");
  theStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  theStyle->SetPadTickY(1);

// Change for log plots:
  theStyle->SetOptLogx(0);
  theStyle->SetOptLogy(0);
  theStyle->SetOptLogz(0);

// Postscript options:

//  theStyle->SetPaperSize(7.5,7.5);

  theStyle->SetPaperSize(15.,15.);

//  theStyle->SetPaperSize(20.,20.);

  // theStyle->SetLineScalePS(Float_t scale = 3);
  // theStyle->SetLineStyleString(Int_t i, const char* text);
  // theStyle->SetHeaderPS(const char* header);
  // theStyle->SetTitlePS(const char* pstitle);

  // theStyle->SetBarOffset(Float_t baroff = 0.5);
  // theStyle->SetBarWidth(Float_t barwidth = 0.5);
  // theStyle->SetPaintTextFormat(const char* format = "g");
  // theStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // theStyle->SetTimeOffset(Double_t toffset);
  // theStyle->SetHistMinimumZero(kTRUE);

  //theStyle->cd();

  //theStyle=theStyle;

//   if(!theStyle) {
//     theStyle = new TStyle(getRnd().c_str(),getRnd().c_str());
//   }
// 
//   theStyle->SetCanvasDefH(800);
//   theStyle->SetCanvasDefW(800);
// 
//   // Set pad grid and tick marks on opposite side of the axis
//   //theStyle->SetPadGridX(1);
//   //theStyle->SetPadGridY(1);
// 
//   theStyle->SetPadTickX(1);
//   theStyle->SetPadTickY(1);
// 
//   // Set background color
//   theStyle->SetCanvasColor(0);
//   theStyle->SetStatColor(kWhite);
// 
//   // Title config
//   theStyle->SetOptTitle(1);
//   //  theStyle->SetTitleW(0.41);
//   //  theStyle->SetTitleH(0.05);
// 
// //  theStyle->SetTitleX(0.16);
// //  theStyle->SetTitleY(0.93);
//   theStyle->SetTitleX(0.01);
//   theStyle->SetTitleY(0.99);
//   theStyle->SetTitleColor(1);
//   theStyle->SetTitleTextColor(1);
//   theStyle->SetTitleFillColor(0);
//   theStyle->SetTitleBorderSize(1);
// 
// 
// // Fonts
// 
// // Begin Michael
//   theStyle->SetStatFont(42);
//   theStyle->SetStatFontSize(0.025);
//   theStyle->SetTitleFont(42);
// 
//   theStyle->SetTitleFontSize(0.04);
// 
// //  theStyle->SetTitleFontSize(0.05);
// 
// //  theStyle->SetTitleFont(42, "XYZ");
// //  theStyle->SetLabelFont(42, "XYZ"); 
//   
// // End Michael

}

void CanvasHolder::setStandardValues() {
  //cout<<"StyleName:"<<theStyle->GetName()<<endl;

  m_grid_x=m_grid_y=false;

// FixMe: Shoul be editable from cc
  theCanleftmargin   = 0.18;
  theCanrightmargin  = 0.03;
  theCantopmargin    = 0.10;
  theCanbottommargin = 0.10;


  theLogX=theLogY=theLogZ=false;
  theLeg=0;
  theCan=0;
  theStyle=0;

  //  theOptStat=111110;
  //  theOptStat=0000;
//  optstat=0;
  theOptTitle = 1;
  theOptFit = 0;
  theDoYperBinWidth=false;

  theDrawOpt = "";

  theLegW =  0.17; // WAS 33 DP
  theLegH =  0.20; // WAS 22 DP
  makeLegPos("UpperRight");
  


//   theLegX1 = 0.623116;
//   theLegY1 = 0.69459;
//   theLegX2 = 0.962312;
//   theLegY2 = 0.921466;

//0.624372,0.673647,0.962312,0.898778


  theDrawLegend = true;
  theLegDrawSymbol = "l";


  theCancelOutBigErr=false;


  theXaxisDec = theYaxisDec = kTRUE;
  theYaxisOff = 1.75;
  theXaxisOff = 1.00;

  theNextColor = 1;
  theNextStyle = 1;

  theYperBinWidth=false;
  
  theFont.set(42);

}



void CanvasHolder::doScaleHistos(const double &factor){
  for (vector<ExtHisto>::const_iterator iter=ehistos.begin();ehistos.end()!=iter;++iter){
    (*iter).theHisto->Scale(factor);
  }
}

void CanvasHolder::doScaleHistosToHisto(const char* name){

  TString name_s(name);
  if (not name_s.Contains("_CHcloned"))
    name_s+="_CHcloned";
  double scale = 0;
  for (vector<ExtHisto>::const_iterator iter=ehistos.begin();ehistos.end()!=iter;++iter)
    if (TString((*iter).theHisto->GetName()).Contains(name_s)){
        std::cout << "[CanvasHolder] Scaling according to " << name << std::endl;
        scale = (*iter).theHisto->Integral();
        }

  if (scale > 0.0000000000001)
    for (vector<ExtHisto>::const_iterator iter=ehistos.begin();ehistos.end()!=iter;++iter)
        (*iter).theHisto->Scale ( scale / (*iter).theHisto->Integral() );
  else{
    std::cout << "[CanvasHolder] I did not find " << name << " I am normalising all the histos to 1." <<std::endl;
    normalizeHistos();
    }


}


void CanvasHolder::setLegend(const std::vector<std::string> &inLeg){
  vector<std::string>::const_iterator LegIt=inLeg.begin();
  for (vector<ExtHisto>::iterator ita=ehistos.begin();ita!=ehistos.end();++ita){
    if (LegIt==inLeg.end()) break;
    //    cout<<"Legend:"<<(*LegIt)<<endl;
    (*ita).theLegTitle=*LegIt;
    //cout<<"++LegIt?"<<endl;
    ++LegIt;
  }
}

void CanvasHolder::prepareCanvas() {

  //CanvasTitle may not be empty, or root won't make a canvas, and our TStyle would
  //break, too

  setDefaultStyle();

  // Firefox color for TH2
  //  int colorsPa[22];
  for (int i=0;i<20;i++) colorsPa[i]=500+i; 
  theStyle->SetPalette(20,colorsPa);

  //cout<<"r: "<<r<<" buf:"<<buf<<endl;
  // initColorVec!
  theStyle->SetOptTitle(theOptTitle);
  theStyle->SetOptStat(theOptStat);
  theStyle->SetOptFit(theOptFit);
  theStyle->cd();

  theCan=new TCanvas(theCanvasTitle.c_str(),theCanvasTitle.c_str(),800,800);

  theCan->SetFillColor(0);
  theCan->SetBorderMode(0);
  theCan->SetBorderSize(2);
  theCan->SetTickx();
  theCan->SetTicky();

  theCan->SetFrameFillStyle(0);
  theCan->SetFrameBorderMode(0);
  theCan->SetFrameFillStyle(0);
  theCan->SetFrameBorderMode(0);

  theCan->SetLeftMargin(theCanleftmargin);
  theCan->SetRightMargin(theCanrightmargin);
  theCan->SetTopMargin(theCantopmargin);
  theCan->SetBottomMargin(theCanbottommargin);


  theCan->SetLogy(theLogY);
  theCan->SetLogx(theLogX);
  if (theHistoType=="TH2D") theCan->SetLogz(theLogZ);

  theCan->Update();

}


void CanvasHolder::normalizeHistos() {
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    doNormalizeHisto((*iter).theHisto);
  }
}

    

void CanvasHolder::doNormalizeHisto(TH1 *hist){
  double scale=1/hist->Integral();
  hist->Scale(scale);
}





void CanvasHolder::formatHistos() {

  // get Min and Max for X and Y axis from the histograms
  double tmpMinX = 0;
  double tmpMaxX = 0;
  double tmpMinY = 0;
  double tmpMaxY = 0;

  bool getMinMaxStartVal = true;
  for (vector<ExtHisto>::iterator iter=ehistos.begin();
       iter!=ehistos.end();++iter){
    
    if(getMinMaxStartVal) {
      tmpMaxY = (*iter).theHisto->GetMaximum()*m_maxY_s;
      tmpMinY = (*iter).theHisto->GetMinimum()*m_minY_s;
      tmpMinX = (*iter).theHisto->GetXaxis()->GetXmin();
      tmpMaxX = (*iter).theHisto->GetXaxis()->GetXmax();
      getMinMaxStartVal=false;
    }else {

      tmpMaxY = max((*iter).theHisto->GetMaximum(),tmpMaxY*m_maxY_s);
      tmpMinY = min((*iter).theHisto->GetMinimum(),tmpMinY*m_minY_s);
      tmpMinX = min((*iter).theHisto->GetXaxis()->GetXmin(),tmpMinX);
      tmpMaxX = max((*iter).theHisto->GetXaxis()->GetXmax(),tmpMinX);
    }

  }



  // get Min and Max for X and Y axis from the graphs
  
  for(std::vector<extGraph>::iterator iter=theGraphVec.begin();
      iter!=theGraphVec.end();++iter) {
    TGraph *tmp = (*iter).theGraph;
    
    if(getMinMaxStartVal) {
      tmpMinX =tmp->GetHistogram()->GetXaxis()->GetXmin();
      tmpMaxX =tmp->GetHistogram()->GetXaxis()->GetXmax();
      tmpMinY =tmp->GetHistogram()->GetYaxis()->GetXmin()*m_minY_s;
      tmpMaxY =tmp->GetHistogram()->GetYaxis()->GetXmax()*m_maxY_s;
      getMinMaxStartVal=false;
    } else {
      tmpMinX =min(tmp->GetHistogram()->GetXaxis()->GetXmin(),tmpMinX);
      tmpMaxX =max(tmp->GetHistogram()->GetXaxis()->GetXmax(),tmpMaxX);
      tmpMinY =min(tmp->GetHistogram()->GetYaxis()->GetXmin(),tmpMinY*m_minY_s);
      tmpMaxY =max(tmp->GetHistogram()->GetYaxis()->GetXmax(),tmpMaxY*m_maxY_s);
      
    }
  }
  
    
  // set this variable if x axis should be set
  theUserSetXRange = false;
  if (theMinX.getState()&&theMaxX.getState())  theUserSetXRange = true;

  // set this variable if y axis should be set
  theUserSetYRange = false;
  if (theMinY.getState()&&theMaxY.getState())  theUserSetYRange = true;


  if(!theMinY.getState()) theMinY.set(tmpMinY);
  if(!theMaxY.getState()) theMaxY.set(tmpMaxY);


  // are currently only needed for TLines
  if(!theMinX.getState()) theMinX.set(tmpMinX);
  if(!theMaxX.getState()) theMaxX.set(tmpMaxX);


  // TODO: Log Y Check should be implemented also for TGraph!
  
  if( theLogY && (theMinY.getValue() <= 0)) {
    SwitchVal<double> minYLog(5);
    minYLog.unset();
    for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
      int nbinsX=(*iter).theHisto->GetNbinsX();
      for(int i = 1; i < nbinsX; i++) {
	double binContent = (*iter).theHisto->GetBinContent(i);
	if(binContent > 0 ) {
	  if( minYLog.getState() ) {
	    minYLog.set(min(binContent,minYLog.getValue()));
	  } else {
	    minYLog.set(binContent);
	  }
	}
      }
    }
    if(minYLog.getState() ) {
      std::cout<<"MinY changed for LogScale"<<std::endl;
      theMinY.set(minYLog.getValue());
    } else {
      std::cout<<"All Y entries <= 0! Could not changed for LogScale"<<std::endl;
    }
  }

  
  if( !theUserSetYRange) {
    if(theMinY.getValue() > 0) {
      theMinY.set(theMinY.getValue()*0.8);
    } else {
      theMinY.set(theMinY.getValue()*1.2);
    }
    
    if( theMaxY.getValue() > 0 ) {
      theMaxY.set(theMaxY.getValue()*1.2);
        } else {
      theMaxY.set(theMaxY.getValue()*0.8);
    }
  }


  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    formatHisto((*iter).theHisto);  
    (*iter).formatHisto();
  
    if (theUserSetXRange) (*iter).theHisto->GetXaxis()->SetRangeUser(theMinX.getValue(),theMaxX.getValue()); 
    //  if (theUserSetXRange) inHisto->SetAxisRange(theMinX.getValue(),theMaxX.getValue(),"X"); 
    //  if (theUserSetXRange) inHisto->GetXaxis()->SetLimits(theMinX.getValue(),theMaxX.getValue()); 
    
 //    (*iter).theHisto->GetYaxis()->SetRangeUser(theMinY.getValue(),theMaxY.getValue());
//     if (theUserSetXRange) (*iter).theHisto->GetXaxis()->SetRangeUser(theMinX.getValue(),theMaxX.getValue()); 
    
//     (*iter).theHisto->GetXaxis()->SetTitleOffset(theXaxisOff);
//     (*iter).theHisto->GetXaxis()->SetDecimals(theXaxisDec);
//     (*iter).theHisto->GetYaxis()->SetTitleOffset(theYaxisOff);
//     (*iter).theHisto->GetYaxis()->SetDecimals(theYaxisDec);
    
//     if(theTitle.getState()) (*iter).theHisto->SetTitle(theTitle.getValue().c_str());
//     if(theTitleX.getState()) (*iter).theHisto->GetXaxis()->SetTitle(theTitleX.getValue().c_str());
//     if(theTitleY.getState()) (*iter).theHisto->GetYaxis()->SetTitle(theTitleY.getValue().c_str());
    
  }
  
  for(std::vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter) {
  //   (*iter).theGraph->GetHistogram()->GetXaxis()->SetRangeUser(theMinX.getValue(),theMaxX.getValue());
//     (*iter).theGraph->GetHistogram()->GetYaxis()->SetRangeUser(theMinY.getValue(),theMaxY.getValue());
//     if(theTitle.getState()) (*iter).theGraph->GetHistogram()->SetTitle(theTitle.getValue().c_str());
//     if(theTitleX.getState()) (*iter).theGraph->GetHistogram()->GetXaxis()->SetTitle(theTitleX.getValue().c_str());
//     if(theTitleY.getState()) (*iter).theGraph->GetHistogram()->GetYaxis()->SetTitle(theTitleY.getValue().c_str());
    
    formatHisto((*iter).theGraph->GetHistogram()); 
    (*iter).formatGraph();
    // if (theUserSetXRange) (*iter).theHisto->GetXaxis()->SetRangeUser(theMinX.getValue(),theMaxX.getValue()); 
    //  if (theUserSetXRange) inHisto->SetAxisRange(theMinX.getValue(),theMaxX.getValue(),"X"); 
    if (theUserSetXRange) (*iter).theGraph->GetHistogram()->GetXaxis()->SetLimits(theMinX.getValue(),theMaxX.getValue()); 

   
    
  }
  
}



void CanvasHolder::formatHisto(TH1* inHisto) {
//  inHisto->UseCurrentStyle();
  inHisto->GetYaxis()->SetRangeUser(theMinY.getValue(),theMaxY.getValue());
  
  inHisto->GetXaxis()->SetTitleOffset(theXaxisOff);
  inHisto->GetXaxis()->SetDecimals(theXaxisDec);
  inHisto->GetYaxis()->SetTitleOffset(theYaxisOff);
  inHisto->GetYaxis()->SetDecimals(theYaxisDec);
  
  if(theFont.getState()) {
    inHisto->SetTitleFont(theFont.getValue());
    inHisto->SetTitleFont(theFont.getValue(),"XYZ");
    inHisto->SetLabelFont(theFont.getValue(),"XYZ");
  }
  if(theTitle.getState()) inHisto->SetTitle(theTitle.getValue().c_str());
  if(theTitleX.getState()) inHisto->GetXaxis()->SetTitle(theTitleX.getValue().c_str());
  if(theTitleY.getState()) inHisto->GetYaxis()->SetTitle(theTitleY.getValue().c_str());
  


  
}


void CanvasHolder::getExtHistos(std::vector<ExtHisto> &eHistVec) const {
  //cout<<" [getExtHistos] adding: "<<ehistos.size()<<" ";
  for (vector<ExtHisto>::const_iterator ita=ehistos.begin();ita!=ehistos.end();++ita){
    ExtHisto tmp(*ita);
    tmp.cloneHisto();
    eHistVec.push_back(tmp);
  }
}


void CanvasHolder::setLineColors(const std::vector<int> &vcolor) {
  std::vector<int>::const_iterator coliter = vcolor.begin();
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    if(coliter != vcolor.end() ) {
      TString name((*iter).theHisto->GetName());
      if (name.Contains("data") or name.Contains("Data") ){
        (*iter).theLineColor=1;
        continue;
        }
      (*iter).theLineColor=*coliter;
      (*iter).theMarkerColor=*coliter;
      coliter++;
    } else {
      (*iter).theLineColor=1;
    }
  }
  for(std::vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter) {
    if(coliter != vcolor.end() ) {
      TString name((*iter).theGraph->GetName());
//       std::cout << "DEBUG: the graph name " << name.Data() << std::endl;
      if (name.Contains("data") or name.Contains("Data") ){
        (*iter).theLineColor=1;
        continue;
        }
      (*iter).theLineColor=*coliter;
      (*iter).theMarkerColor=*coliter;
      coliter++;
    } else {
      (*iter).theLineColor=1;
      }
    }
  
  
}

void CanvasHolder::setLineColors(){
  int counter=0;
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theLineColor=++counter;
  }
  
}

void CanvasHolder::setLineStyles(){
  int counter=0;
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theLineStyle=++counter;
  }
}


void CanvasHolder::setLineSize(const int &insize) {
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theHisto->SetLineWidth(insize);
  }
}
      
// To be fixed!!!
// Not so much internal variables - direct modification of Graphs!

void CanvasHolder::setLineSizeGraph(const int &insize) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theGraph->SetLineWidth(insize);
  }
}


void CanvasHolder::setLineSizeTF1(const int &insize) {
  for(std::vector<extTF1>::iterator iter=theTF1Vec.begin();
      iter!=theTF1Vec.end(); ++iter) {
    (*iter).theTF1->SetLineWidth(insize);
  }
}




void CanvasHolder::setLineStylesGraph(const int &instyle) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theLineStyle = instyle;
  }
}
void CanvasHolder::setLineColorsGraph(const int &instyle) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theLineColor = instyle;
  }
}

void CanvasHolder::setMarkerStylesGraph(const int &instyle) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theMarkerStyle = instyle;
  }
}
void CanvasHolder::setMarkerColorsGraph(const int &instyle) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theMarkerColor = instyle;
  }
}
void CanvasHolder::setMarkerSizeGraph(const double &instyle) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theMarkerSize = instyle;
  }
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter)
      (*iter).theMarkerSize=(int)instyle;
}

void CanvasHolder::setDrawOptGraph(std::string instring) {
  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theDrawOpt = instring;
  }    
}






void CanvasHolder::divideHisto(const ExtHisto &divHist) {
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theHisto->Divide(divHist.theHisto);
  }
  theCanvasTitle+="_DivideBy"+divHist.theLegTitle;
}


void CanvasHolder::divideHisto(const std::string &search) {
  //dangerous: Was bei mehreren matches? ungeschickt...

  ExtHisto a;
  bool foundHisto = false;
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter) {
    if( (*iter).theLegTitle.find(search) != std::string::npos) {
      a.theHisto=(*iter).theHisto;
      a.theLegTitle=((*iter).theLegTitle);
      foundHisto = true;
       break;
    }
  }
  if(foundHisto) {
    divideHisto(a);
  } else {
    std::cout<<"Warning: No histo found for "<<search<<std::endl;
  }
}


void CanvasHolder::cleanExtHistos(vector<ExtHisto> &candidate){
  for (vector<ExtHisto>::iterator iter=candidate.begin();iter!=candidate.end();++iter){
/*      std::cout << "[CanvasHolder::cleanExtHistos] about to delete: " << (*iter).theHisto; 
      if ((*iter).theHisto->IsZombie())
          std::cout << ". A zombie";
      std::cout << " with name " << (*iter).theHisto->GetName() << std::endl;*/
      
      delete (*iter).theHisto;
//       std::cout << "Deleted...\n";
      
  }
  candidate.clear();
}

void CanvasHolder::setLineColors(const int &incolor) {
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theLineColor=incolor;
  }
}

void CanvasHolder::setLineStyles(const std::vector<int> &vstyle) {
  std::vector<int>::const_iterator styeliter = vstyle.begin();
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    TString name((*iter).theHisto->GetName());
    if(styeliter != vstyle.end() ) {
//       std::cout << "DEBUG: the histo name " << name.Data() << std::endl;
      if (name.Contains("data") or name.Contains("Data") ){
        (*iter).theLineStyle=1;
        continue;
        }    
      (*iter).theLineStyle=*styeliter;
      styeliter++;
    } else {
      (*iter).theLineStyle=1;
    }
  }
//   styeliter = vstyle.begin();

  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    TString name((*iter).theGraph->GetName());
    if(styeliter != vstyle.end() ) {
//       std::cout << "DEBUG: the histo name " << name.Data() << std::endl;
        if (name.Contains("data") or name.Contains("Data") ){
            (*iter).theLineStyle=1;
            continue;
            }    
        (*iter).theLineStyle=*styeliter;
        styeliter++;
        } else {
        (*iter).theLineStyle=1;
        }
    }
  
}

void CanvasHolder::setLineStyles(const int &instyle) {
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theLineStyle=instyle;
  }
}


void CanvasHolder::cleanLegend(const std::map<std::string,std::string> &repmap) {
  for (vector<ExtHisto>::iterator iter=ehistos.begin();iter!=ehistos.end();++iter){
    (*iter).theLegTitle=doStringRep((*iter).theLegTitle,repmap);

  }

  for (vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter){
    (*iter).theLegName = doStringRep((*iter).theLegName,repmap);
  }    
}



void CanvasHolder::makeLegPos( const std::string &inVal) {
  if(inVal == "UpperRight") {
    theLegX2 = 1-theCanrightmargin - 0.01;
    theLegX1 = theLegX2 - theLegW - 0.01;
    theLegY2 = 1-theCantopmargin - 0.025;
    theLegY1 = theLegY2 - theLegH - 0.025;
    
  } else if (inVal == "LowerRight") {
    
    theLegX2 = 1-theCanrightmargin - 0.01;
    theLegX1 = theLegX2 - theLegW - 0.01;
    theLegY1 = theCanbottommargin + 0.025;
    theLegY2 = theLegY1 + theLegH + 0.025;
    
  } else if (inVal == "UpperLeft") {
    
      theLegX1 = theCanleftmargin + 0.01;
      theLegX2 = theLegX1 + theLegW + 0.01;
      theLegY2 = 1-theCantopmargin - 0.025;
      theLegY1 = theLegY2 - theLegH - 0.025;
    
  } else if (inVal == "LowerLeft") {
    theLegX1 = theCanleftmargin + 0.01;
    theLegX2 = theLegX1 + theLegW + 0.01;
    theLegY1 = theCanbottommargin + 0.025;
    theLegY2 = theLegY1 + theLegH + 0.025;
  } else {
    std::cerr<<"CanvasHolder::setLegPos: No Settings for position "<<inVal<<" available"<<std::endl;
  }
}



void CanvasHolder::drawPad(TVirtualPad *a, std::string PadDrawOpt){
  cout<<"[DRAW] "<<theCanvasTitle<<endl;
  formatHistos();
  if(theLegPos.getState() )  makeLegPos(theLegPos.getValue());

  theLeg = new TLegend(theLegX1,theLegY1,theLegX2,theLegY2,NULL,"brNDC");
  theLeg->SetBorderSize(1);
  theLeg->SetLineColor(1);
  theLeg->SetLineStyle(1);
  theLeg->SetLineWidth(1);
  theLeg->SetFillColor(0);
  theLeg->SetFillStyle(1001);
  theLeg->SetBorderSize(0);
  theLeg->SetMargin(.35);
  
  if(theLegTitle.getState() ) {
    theLeg->SetHeader(theLegTitle.getValue().c_str());
  }
  if(theLegNCol.getState() ) {
    theLeg->SetNColumns(theLegNCol.getValue());
  }

  if(theLegMargin.getState() ) {
    theLeg->SetMargin(theLegMargin.getValue());
  }

  if(theLegBorder.getState() ) {
    theLeg->SetBorderSize((int)theLegBorder.getValue());
  }
  if(theFont.getState()) {
    theLeg->SetTextFont(theFont.getValue());
  }

  bool isFirstDraw = true;

  for (vector<ExtHisto>::const_iterator iter=ehistos.begin();iter!=ehistos.end();++iter) {
    //std::cout << "Adding histo: " << (*iter).theHisto->GetName() << std::endl;
    if((*iter).theLegTitle != "") {
      //std::cout << "Adding histo which has a leg title: " << (*iter).theHisto->GetName() << std::endl;
      TString the_name((*iter).theHisto->GetName());
      if (the_name.Contains("Data") or the_name.Contains("data"))
          theLeg->AddEntry((*iter).theHisto,(*iter).theLegTitle.c_str(),"p");
//       else if (the_name.Contains("template") or the_name.Contains("Template"))
//          theLeg->AddEntry((*iter).theHisto,(*iter).theLegTitle.c_str(),);
      else
          theLeg->AddEntry((*iter).theHisto,(*iter).theLegTitle.c_str(),theLegDrawSymbol.c_str());
    }
     if(isFirstDraw ) {
      (*iter).theHisto->Draw( ((*iter).theDrawOpt+PadDrawOpt).c_str() );
      isFirstDraw = false;
    } else {
      (*iter).theHisto->Draw( ((*iter).theDrawOpt+PadDrawOpt+"same").c_str());
    }
  }

  for(std::vector<extGraph>::iterator iter=theGraphVec.begin();iter!=theGraphVec.end();++iter) {
    if((*iter).theLegName != "") {
      std::cout << "Adding Graph: " << (*iter).theGraph->GetName() << std::endl;
      TString the_name((*iter).theGraph->GetName());
      theLeg->AddEntry((*iter).theGraph,(*iter).theLegName.c_str(),"p");
/*      if (the_name.Contains("Data") or the_name.Contains("data"))
          theLeg->AddEntry((*iter).theGraph,(*iter).theLegName.c_str(),"p");
//       else if (the_name.Contains("template") or the_name.Contains("Template"))
//          theLeg->AddEntry((*iter).theHisto,(*iter).theLegTitle.c_str(),);
      else*/
      //  theLeg->AddEntry((*iter).theGraph,(*iter).theLegName.c_str(),theLegDrawSymbol.c_str());
    }
    if(isFirstDraw) {
      (*iter).theGraph->Draw(std::string("A"+ (*iter).theDrawOpt).c_str());
      std::cout << "Draw Opt: " << (*iter).theDrawOpt << std::endl;
      isFirstDraw=false;
    } 
    else 
    {
      (*iter).theGraph->Draw(std::string((*iter).theDrawOpt + "same").c_str());
    }
  }
  
  for(std::vector<extLine>::iterator iter=theLines.begin(); iter!=theLines.end();++iter) {  
    (*iter).makeLine(theMinX.getValue(),theMaxX.getValue(),theMinY.getValue(),theMaxY.getValue());
    if((*iter).theLegEntry != "") {
      theLeg->AddEntry((*iter).theLine,(*iter).theLegEntry.c_str(),"l");//theLegDrawSymbol.c_str());
    }
    (*iter).theLine->Draw("same");
  }


    

  for(std::vector<extTF1>::iterator iter=theTF1Vec.begin();
      iter!=theTF1Vec.end(); ++iter) {
    theLeg->AddEntry((*iter).theTF1,(*iter).theLegName.c_str(),(*iter).theDrawOpt.c_str());
    (*iter).formatTF1();
    if(isFirstDraw) {
      (*iter).theTF1->Draw("L");
      isFirstDraw=false;
    }else {
      (*iter).theTF1->Draw("LSAME");
    }
  }


  if (theDrawLegend) theLeg->Draw("NDC");

  TLatex *T1 = new TLatex;
  T1->SetTextSize(0.03);
  for(std::vector<extLatex>::iterator iter = theLatexVec.begin();
      iter!=theLatexVec.end(); ++iter) {
    T1->SetNDC((*iter).is_ndc);
    T1->DrawLatex((*iter).theX,(*iter).theY,(*iter).theName.c_str());
  }
}


bool CanvasHolder::draw(std::string opt){

  // scheint nicht mehr noetig zu sein
  // entfernt bei Umstellung auf TGraph
//   if( ehistos.begin() == ehistos.end() ) {
//     std::cout<<"Could not draw histos for "<<theCanvasTitle<<". No histos!"<<std::endl;
//     return false;
//   }
  prepareCanvas();
  theDrawOpt = opt + theDrawOpt;;
  drawPad(theCan->cd(),theDrawOpt);
  return true;
}



void CanvasHolder::write(TFile *file, std::string opt){
  if( CanvasHolder::draw(opt) ) {
    theCan->Update();
    theCan->Write();
  }
}


void CanvasHolder::save(const std::string format, std::string opt, std::string sFolder){
  if( CanvasHolder::draw(opt) ) {
    theCan->Update();
    std::string tmpString = sFolder + theCanvasTitle+"."+format;
    theCan->SaveAs(tmpString.c_str());
  }
}


/// String manipulation

std::string CanvasHolder::doStringRep(const std::string &instring, 
				    const std::string &cutIt,
				    const std::string &rep) {
  std::string retstring=instring;
  long unsigned int pos=retstring.find(cutIt);
  if (pos!=std::string::npos) {
    std::string temp1=retstring.replace(pos,cutIt.length(),rep);
    retstring=temp1;
  }
  return retstring;
}




std::string CanvasHolder::doCutOut(const std::string &instring,const std::vector<string> &cutIt){
  std::string retstring=instring;
  for (vector<string>::const_iterator iter=cutIt.begin();iter!=cutIt.end();++iter){
    retstring=doStringRep(retstring,*iter,"");
  }
  return retstring;
}

std::string CanvasHolder::doStringRep(std::string instring, 
				    const map<string,string> &repmap) {
  for( map<string,string>::const_iterator iter=repmap.begin();
       iter!=repmap.end();++iter) {
    instring = doStringRep(instring,iter->first,iter->second);
  }
  return instring;
}

bool CanvasHolder::mitmatch(const string &hina,const vector<string> &items,const vector<string> &blacklist){
  bool retval=true;
  //cout<<"mitmatch - ";
  for (vector<string>::const_iterator iter=items.begin();iter!=items.end();++iter){
    if ((hina.find(*iter)!=string::npos)&&retval) {
      retval=true;
      for (vector<string>::const_iterator iterb=blacklist.begin();iterb!=blacklist.end();++iterb){
	if ((hina.find(*iterb)!=string::npos)) {
	  retval=false;
	}
      }
    }
    else {
      retval=false;
    }
    if (!retval) break;
  }
  //cout<<"return mitmatch: "<<retval<<endl;
  return retval;
}

//------------------------------------------------------------------------------
//DP
void CanvasHolder::addObj(TObject* obj,const char* legname,const char* drawopt, double scale){

    if (obj->InheritsFrom("TH1")){
        TH1* h=dynamic_cast<TH1*>(obj->Clone());
        addHisto(h,legname,drawopt,scale);
        delete h;
        }
    else if(obj->InheritsFrom("TGraph")){
        TGraph* h=dynamic_cast<TGraph*>(obj->Clone());
        addGraph(h,getRnd(),legname,drawopt);
        }
    else if(obj->InheritsFrom("TF1")){
        TF1* h=dynamic_cast<TF1*>(obj->Clone());
        addTF1(h,legname,drawopt);
        }
    else{
        try{
        throw -1;
        }
        catch (int e){
            std::cout << "Unknown Class "<< obj->IsA()->GetName() << std::endl;
            }
        }
    }
//------------------------------------------------------------------------------
//DP
void CanvasHolder::addObjFormated(TObject* obj,const char* legname,const char* drawopt, double scale){

    if (obj->InheritsFrom("TH1")){
        TH1* h=dynamic_cast<TH1*>(obj->Clone());
        addHistoFormated(h,legname,drawopt,scale);
        delete h;
        }
    else if(obj->InheritsFrom("TGraph")){
        TGraph* h=dynamic_cast<TGraph*>(obj->Clone());
        addGraphFormated(h,getRnd(),legname,drawopt);
        }
    else if(obj->InheritsFrom("TF1")){
        TF1* h=dynamic_cast<TF1*>(obj->Clone());
        addTF1Formated(h,legname,drawopt);
        }
    else{
        try{
        throw -1;
        }
        catch (int e){
            std::cout << "Unknown Class "<< obj->IsA()->GetName() << std::endl;
            }
        }
    }
//------------------------------------------------------------------------------

void CanvasHolder::divideHistosByHisto (const char* name, bool add_to_legend){

    TString mod_name(name);
    if (not mod_name.Contains("_CHcloned"))
        mod_name+="_CHcloned";
    // Find the divider!
    TH1* divider=NULL;
    int divider_index=-1;
    for (vector<ExtHisto>::const_iterator iter=ehistos.begin();iter!=ehistos.end();++iter) {
        divider_index++;
        TString divider_name((*iter).theHisto->GetName());

//         std::cout << "Divider's Name = " << divider_name.Data() << std::endl;
        if (divider_name == mod_name){
            divider = (TH1*)(*iter).theHisto->Clone("thedivider");
            if (divider->GetSumw2N() == 0)
                divider->Sumw2();

            break;
            }
        }// end loop to find the divider

    if (divider != NULL){
        
        ehistos.erase(ehistos.begin()+divider_index);
        
        TString hline_leg_entry("");
        if (add_to_legend)
            hline_leg_entry = ehistos[divider_index].theLegTitle.c_str();
        addHLine(1,hline_leg_entry,2);

        for (vector<ExtHisto>::const_iterator iter=ehistos.begin();iter!=ehistos.end();++iter) {
            if ((*iter).theHisto->GetSumw2N() == 0)
                (*iter).theHisto->Sumw2();
            TString dividend_name((*iter).theHisto->GetName());
            if (not (dividend_name.Contains("Template") or dividend_name.Contains("template")))
                (*iter).theHisto->Divide(divider);
            else
                std::cout << "NOT DIVIDING " << dividend_name.Data() << std::endl;
            }
        delete divider;

        }
    else
        std::cerr << "ERROR: Could not find the divider called " << name << std::endl;

    }

//------------------------------------------------------------------------------



/// DP To build the cint dictionaries
// ClassImp(CanvasHolder)
