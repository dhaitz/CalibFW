#ifndef ___ROOTINCLUDES_H
#define ___ROOTINCLUDES_H

#include "TText.h"
#include "TTree.h"
#include "TParticle.h"
#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
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


  static double GetDeltaPhiCenterZero ( TVector3 & v1, TVector3 & v2)
  {
	  double delta = ROOT::Math::VectorUtil::DeltaPhi( v1, v2);
	/*if (delta > 0.0)
		delta -= TMath::Pi();
	  else
		delta += TMath::Pi();
*/
	return delta;
  }

  static TVector3 ConvertToVector ( TParticle * p1)
  {
	  return TVector3 (p1->Px(), p1->Py(), p1->Pz());
  }

  /* calcualtes the delta Phi between 2 particles
	  if the the particles are have the same direction in x-y-space. this returns 0
	  for 0 to Phi, this function returns a positiv DeltaPhi. For the region Pi to 2Pi, a negative
	  Delta Phi is returned.
	  The range of delta phi is [-Pi,Pi]
	  See also the class DrawDeltaPhiRange to get a plot of the range
  */
  static double GetDeltaPhiCenterZero ( TParticle * p1,
		  TParticle * p2 )
  {
	 TVector3 v1 = ConvertToVector(p1);
	 TVector3 v2 = ConvertToVector(p2);
	return DeltaHelper::GetDeltaPhiCenterZero( v1, v2);
  }

  static double GetDeltaR ( TParticle * p1,
		  TParticle * p2 )
  {
	 TVector3 v1 = ConvertToVector(p1);
	 TVector3 v2 = ConvertToVector(p2);
	return ROOT::Math::VectorUtil::DeltaR( v1, v2);
  }
};


#endif

