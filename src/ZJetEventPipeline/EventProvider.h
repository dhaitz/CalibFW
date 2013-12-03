#include "EventPipeline/KappaEventProvider.h"
#include "ZJetEventPipeline/EventData.h"

namespace Artus
{

typedef KappaEventProvider<ZJetEventData> ZJetEventProvider;

/* Template specialization for the ZJet related event data
 */

template<>
void KappaEventProvider<ZJetEventData>::WireEvent(bool phicorrection, bool tagged)
{
	m_event.m_vertexSummary = m_fi.Get<KVertexSummary>(
								  "goodOfflinePrimaryVerticesSummary");
	m_event.m_beamSpot = m_fi.Get<KDataBeamSpot>("offlineBeamSpot");
	m_event.m_jetArea = m_fi.Get<KJetArea>("KT6Area");
	m_event.m_muons = m_fi.Get<KDataMuons>("muons");
	m_event.m_pfMet = m_fi.Get<KDataPFMET>("PFMET");
	m_event.m_pfMetChs = m_fi.Get<KDataPFMET>("PFMETCHS");


	m_event.m_electrons = m_fi.Get<KDataElectrons>("Electrons");

	InitPFJets(m_event, "AK5PFJets");
	InitPFJets(m_event, "AK5PFJetsCHS");

	if (tagged)
	{
		m_event.m_pfTaggedJets["AK5PFTaggedJets"] = m_fi.Get<KDataPFTaggedJets>("AK5PFTaggedJets");
		m_event.m_pfTaggedJets["AK5PFTaggedJetsCHS"] = m_fi.Get<KDataPFTaggedJets>("AK5PFTaggedJetsCHS");
	}

	//InitPFJets(m_event, "AK7PFJets");
	//InitPFJets(m_event, "AK7PFJetsCHS");

	InitCaloJets(m_event, "AK5CaloJets");
	InitCaloJets(m_event, "AK7CaloJets");

	if (m_fi.isMC())
	{
		//InitGenJets(m_event, "AK5GenJets");
		InitGenJets(m_event, "AK7GenJets");

		if (tagged)
			m_event.m_genJets["AK5GenJets"] = m_fi.Get<KDataLVs>("AK5GenJetsNoNu");
		else
			m_event.m_genJets["AK5GenJets"] = m_fi.Get<KDataLVs>("AK5GenJets");

		m_event.m_particles = m_fi.Get<KGenParticles>("genParticles");
	}

	m_event.m_filtermetadata = m_fi.GetMeta<KFilterMetadata>("KFilterMetadata");
	m_event.m_filter = m_fi.Get<KFilterSummary>("filterSummary");
}

}
