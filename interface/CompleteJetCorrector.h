

#include "RootIncludes.h"

#include "JetCorrectorParameters.h"
#include "FactorizedJetCorrector.h"


#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>

#include "EventData.h"


class CorrBase
{
  public:
    CorrBase()
    {
      
    }
    
    virtual void Correct( EventResult * evRes )
    {
      
    }
};

class TF1Corr : public CorrBase
{
public:
    TF1Corr( TString formula, std::vector<double> params)
    {
          m_corrFormula.reset( new TF1( "l3_corr", formula ));
    
	  int i = 0;
	  BOOST_FOREACH( double d, params)
	  {
	    m_corrFormula->SetParameter(i, d);
	    i++;
	  }
    }
    
    virtual void Correct( EventResult * evRes )
    {
	bool beforel2;
	bool beforel3;
        
	for ( int i = 0; i < 3; i++ )
	{
	  // Important: use l2 corrected jets for input to l3 correct !!!
	  beforel2 = evRes->m_bUseL2;
	  beforel3 = evRes->m_bUseL3;
	  
	  evRes->m_bUseL2 = true;
	  evRes->m_bUseL3 = false;
	  
	  evRes->m_l3CorrPtJets[i] = m_corrFormula->Eval( evRes->GetCorrectedJetPt(i) );
	  evRes->m_bUseL2 = beforel2;
	  evRes->m_bUseL3 = beforel3;
	}
    }

    boost::scoped_ptr<TF1> m_corrFormula;
};

class LFileCorr : public CorrBase
{
public:  
    void LoadCorrectionFile( TString algoName, vString algoFileMapping )
    {
        bool bFound = false;
        TString sData;

        BOOST_FOREACH( TString e, algoFileMapping )
        {
            TObjArray * parts = e.Tokenize(":");
            TString sAlgName = ((TObjString*)parts->At(0))->GetString();
            sData = ((TObjString*)parts->At(1))->GetString();

            if (sAlgName == algoName)
            {
                bFound = true;
                break;
            }
        }

        std::vector<JetCorrectorParameters>  vParam;

        if ( ! bFound )
        {
            std::cout << "No JetEnergyCorrections data for " << algoName << " found" << std::endl;
            exit( 10 );
        }

        std::cout << "Loading JetEnergyCorrections " << sData << std::endl;
        vParam.push_back( JetCorrectorParameters(sData.Data()) );
        m_JEC.reset( new FactorizedJetCorrector(vParam) );
    }

    boost::scoped_ptr<FactorizedJetCorrector> m_JEC;
};

class L2Corr : public LFileCorr
{
public:
    L2Corr( TString algoName, vString algoFileMapping )
    {
      LoadCorrectionFile( algoName, algoFileMapping );
    }
    
    virtual void Correct( EventResult * evRes )
    {
        for ( int i = 0; i < 3; i++ )
	{	  
	  m_JEC->setJetEta(evRes->m_pData->jets[i]->Eta());
	  m_JEC->setJetPt(evRes->m_pData->jets[i]->Pt());
	  evRes->m_l2CorrPtJets[i] = m_JEC->getCorrection();
	}
    }
};

class L3Corr : public LFileCorr
{
public:
    L3Corr( TString algoName, vString algoFileMapping )
    {
      LoadCorrectionFile( algoName, algoFileMapping );
    }
    
    virtual void Correct( EventResult * evRes )
    {
	bool beforel2;
	bool beforel3;
        
	for ( int i = 0; i < 3; i++ )
	{
	  // Important: use l2 corrected jets for input to l3 correct !!!
	  beforel2 = evRes->m_bUseL2;
	  beforel3 = evRes->m_bUseL3;
	  
	  evRes->m_bUseL2 = true;
	  evRes->m_bUseL3 = false;
	  
	  m_JEC->setJetEta( evRes->m_pData->jets[i]->Eta() );
	  m_JEC->setJetPt( evRes->GetCorrectedJetPt(i) );

	  evRes->m_l3CorrPtJets[i] = m_JEC->getCorrection();
	  
	  evRes->m_bUseL2 = beforel2;
	  evRes->m_bUseL3 = beforel3;
	}	
    }
};

class CompleteJetCorrector
{
public:
    void AddCorrection( CorrBase * pCorr )
    {
       m_corrs.push_back( pCorr);
    }

    // calculate correction data, but don't overwrite data
    void CalcCorrectionForEvent( EventResult * ev )
    {
      
      for ( boost::ptr_vector< CorrBase >::iterator it = m_corrs.begin();
	    !(it == m_corrs.end());
	    it++ )
      {
	  it->Correct( ev );
      }
    }
    
  private:
    boost::ptr_vector<CorrBase> m_corrs;
};
