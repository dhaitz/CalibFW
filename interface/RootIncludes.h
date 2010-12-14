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
#include "Math/GenVector/VectorUtil.h"

double ScalarProductTParticle( TParticle * p1, TParticle * p2 )
{
 return p1->Px()*p2->Px() + 
        p1->Py()*p2->Py() +
        p1->Pz()*p2->Pz(); 
}

class DeltaHelper
{
public:  
  static double GetDeltaCenterZero ( TParticle * p1, TParticle * p2 )
  {
	TVector3 tZ (p1->Px(), p1->Py(), p1->Pz());
	TVector3 tJet ( p2->Px(), p2->Py(), p2->Pz());
	
	double delta = ROOT::Math::VectorUtil::DeltaPhi( tZ, tJet);
	if (delta > 0.0)
	    delta -= TMath::Pi();
	  else
	    delta += TMath::Pi();
		
	return delta;
  }

};


#endif

