
#include "Pipeline/Kappa/KappaEventProvider.h"
#include "ZJet/ZJetEventData.h"

namespace CalibFW
{

typedef KappaEventProvider< ZJetEventData > ZJetEventProvider;

template<>
void KappaEventProvider< ZJetEventData >::WireEvent()
{
		// open Kappa issue, disable the check and it will work
		m_event.m_vertexSummary = m_fi.Get<KVertexSummary> (
				"offlinePrimaryVerticesSummary", false);
		m_event.m_jetArea = m_fi.Get<KJetArea> ("KT6Area");

		//InitPFJets(m_event, "AK5PFJets");
		// dont load corrected jet here, we will do this offline
		InitPFJets(m_event, "AK5PFJets");
		InitPFJets(m_event, "AK7PFJets");

		if ( m_fi.isMC() )
		{
			// nice, we have the all-mighty TRUTH !
			InitGenJets(m_event, "AK5GenJets");
			InitGenJets(m_event, "AK7GenJets");
		}

		m_event.m_muons = m_fi.Get<KDataMuons> ("muons");
		m_event.m_pfMet = m_fi.Get<KDataPFMET> ("PFMET");

}
/*
class ZJetEventProvider: public KappaEventProvider< ZJetEventData >
{
public:
	ZJetEventProvider(FileInterface & fi, InputTypeEnum inpType) :
        KappaEventProvider< ZJetEventData >( fi, inpType )	{ }


    virtual void WireEvent()
    {

    }

};*/

}
