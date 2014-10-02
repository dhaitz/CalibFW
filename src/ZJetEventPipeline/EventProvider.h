#include "EventPipeline/KappaEventProvider.h"
#include "ZJetEventPipeline/EventData.h"

namespace Artus
{

typedef KappaEventProvider<ZJetEventData> ZJetEventProvider;

/* Template specialization for the ZJet related event data
 */

template<>
void KappaEventProvider<ZJetEventData>::WireEvent(boost::property_tree::ptree propTree)
{
	m_event.m_vertexSummary = m_fi.Get<KVertexSummary>(
								  "goodOfflinePrimaryVerticesSummary");
	m_event.m_beamSpot = m_fi.Get<KDataBeamSpot>("offlineBeamSpot");
	m_event.m_jetArea = m_fi.Get<KJetArea>("KT6Area");
	if (propTree.get<std::string>("muons") != "")
		m_event.m_muons = m_fi.Get<KDataMuons>("muons");
	//m_event.m_electrons = m_fi.Get<KDataElectrons>("calelectrons");
	m_event.m_electrons = m_fi.Get<KDataElectrons>(propTree.get<std::string>("electrons"));
	m_event.m_pfMet = m_fi.Get<KDataPFMET>("PFMET");
	m_event.m_pfMetChs = m_fi.Get<KDataPFMET>("PFMETCHS");

	if (propTree.get<bool>("Tagged"))
	{
		m_event.m_pfTaggedJets["AK5PFTaggedJets"] = m_fi.Get<KDataPFTaggedJets>("AK5PFTaggedJets");
		m_event.m_pfTaggedJets["AK5PFTaggedJetsCHS"] = m_fi.Get<KDataPFTaggedJets>("AK5PFTaggedJetsCHS");
	}
	else
	{
		InitPFJets(m_event, "AK5PFJets");
		InitPFJets(m_event, "AK5PFJetsCHS");
	}

	if (m_fi.isMC())
	{
		//InitGenJets(m_event, "AK5GenJets");
		//InitGenJets(m_event, "AK7GenJets");

		m_event.m_genJets["AK5GenJets"] = m_fi.Get<KDataLVs>(propTree.get<std::string>("AK5GenJets"));
		m_event.m_particles = m_fi.Get<KGenParticles>("genParticles");

		if (propTree.get<std::string>("LHE") != "")
			m_event.m_lhe = m_fi.Get<KGenParticles>("LHE");
	}

	m_event.m_filterproduct = m_fi.GetMeta<KFilterMetadata>("KFilterMetadata");
	m_event.m_taggerproduct = m_fi.GetMeta<KTaggerMetadata>("KTaggerMetadata");
	m_event.m_filter = m_fi.Get<KFilterSummary>("filterSummary");
}

}
