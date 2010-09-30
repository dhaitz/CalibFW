

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

class L2Corr : public CorrBase
{
public:
    L2Corr( TString algoName, vString algoFileMapping )
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
    
    virtual void Correct( EventResult * evRes )
    {
        for ( int i = 0; i < 3; i++ )
	{
	  m_JEC->setJetEta(evRes->m_pData->jets[i]->Eta());
	  m_JEC->setJetPt(evRes->m_pData->jets[i]->Pt());
	  evRes->m_l2CorrPtJets[i] = m_JEC->getCorrection();
	}
    }

    boost::scoped_ptr<FactorizedJetCorrector> m_JEC;
};

class CompleteJetCorrector
{
public:
    void AddCorrection( CorrBase * pCorr )
    {
       //m_corrs.push_back( pCorr);
       m_corr = pCorr;
    }

    // calculate correction data, but don't overwrite data
    void CalcCorrectionForEvent( EventResult * ev )
    {
      m_corr->Correct( ev );
      /*
      for ( boost::ptr_vector< CorrBase >::iterator it = m_corrs.begin();
	    !(it == m_corrs.end());
	    it++ )
      {
	  it->Correct( ev );
      }*/
    }
    CorrBase * m_corr;
  //private:
  //  boost::ptr_vector<CorrBase> m_corrs;
};
