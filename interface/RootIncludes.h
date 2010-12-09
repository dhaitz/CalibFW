#ifndef ___ROOTINCLUDES_H
#define ___ROOTINCLUDES_H

#include "TText.h"
#include "TTree.h"
#include "TParticle.h"
#include "TChain.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TParticle.h"
#include "TGraphPolar.h"
#include "TLine.h"
#include "TStyle.h"
#include "TMath.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TObjString.h"
#include "TVector3.h"

double ScalarProductTParticle( TParticle * p1, TParticle * p2 )
{
 return p1->Energy() * p2->Energy() - 
	p1->Px()*p2->Px() - 
	p1->Py()*p2->Py() - 
	p1->Pz()*p2->Pz(); 
}

#endif

