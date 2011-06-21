#pragma once

#include "TROOT.h"
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
#include "TProfile.h"
#include "Math/GenVector/VectorUtil.h"

/*
double ScalarProductTParticle( TParticle * p1, TParticle * p2 )
{
 return p1->Px()*p2->Px() + 
        p1->Py()*p2->Py() +
        p1->Pz()*p2->Pz(); 
}*/

class RootFileHelper
{
public:
/*
	(05:10:15 PM) Danilo Piparo: template <class T>
	T* getObject(const char* objname, const char* filename){
	  TFile ifile(filename);
	  T* obj = (T*) ifile.Get(objname);
	  obj->SetDirectory(gROOT);
	  ifile.Close();
	  return obj;
	  }
*/
	template < class T>
	static T SafeGet( TDirectory * pDir, std::string objName  )
	{
		T ob = (T)pDir->Get( objName.c_str() );

		if ( ob == NULL)
		{
			std::cout << std::endl << "Cant load " << objName << " from directory " << pDir->GetName() << std::endl;
			exit(0);
		}
		return ob;
	}

    static void SafeCd( TDirectory * pDir, std::string dirName )
    {
    	assert( pDir );

        if ( pDir->GetDirectory( dirName.c_str() ) == 0)
		{
			pDir->mkdir( dirName.c_str() );
		}
        pDir->cd( dirName.c_str() );
    }   
    static TH1D * GetStandaloneTH1D_1( std::string sName,
    		std::string sCaption,
    		int binCount,
    		double dCustomBins[255] )
		{
			return new TH1D(    sName.c_str(),
    					              sCaption.c_str(),
    					              binCount, &dCustomBins[0] );
		}

    static TH1D * GetStandaloneTH1D_2( std::string sName,
    		std::string sCaption,
    		int binCount,
    		double min, double max )
		{
			return new TH1D(    sName.c_str(),
    					              sCaption.c_str(),
    					              binCount, min, max );
		}


   static TProfile * GetStandaloneTProfile( std::string m_sName, std::string m_sCaption,
			int m_iBinXCount, double m_dBinXLower, double m_dBinXUpper )
		{
			return new TProfile(m_sName.c_str(), m_sCaption.c_str(),
    				m_iBinXCount, m_dBinXLower, m_dBinXUpper);
		}


    static TGraphErrors * GetStandaloneTGraphErrors( int i )
		{
			return new TGraphErrors(   i );
		}

    static TH2D * GetStandaloneTH2D_1( std::string m_sName, std::string m_sCaption,
			int m_iBinXCount, double m_dBinXLower, double m_dBinXUpper,
			int m_iBinYCount, double m_dBinYLower, double m_dBinYUpper )
		{
			return new TH2D(m_sName.c_str(), m_sCaption.c_str(),
    				m_iBinXCount, m_dBinXLower, m_dBinXUpper,
    				m_iBinYCount, m_dBinYLower, m_dBinYUpper);
		}

};

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
    
    //std::cout << "calc";
	double val = ROOT::Math::VectorUtil::DeltaR( v1, v2);
    //std::cout << "done";
    return val;
  }
};
