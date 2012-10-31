#include "Pipeline/Kappa/KappaEventProvider.h"
#include "ZJet/ZJetEventData.h"

namespace CalibFW
{

typedef KappaEventProvider<ZJetEventData> ZJetEventProvider;


/*
 * Template specialization for the ZJet related event data
 */
template<>
void KappaEventProvider<ZJetEventData>::WireEvent(bool phicorrection)
{
		m_event.m_vertexSummary = m_fi.Get<KVertexSummary>(
				"goodOfflinePrimaryVerticesSummary");
		m_event.m_beamSpot = m_fi.Get<KDataBeamSpot>("offlineBeamSpot");
		m_event.m_jetArea = m_fi.Get<KJetArea>("KT6Area");
		m_event.m_muons = m_fi.Get<KDataMuons>("muons");
		m_event.m_pfMet = m_fi.Get<KDataPFMET>("PFMET");
		if (phicorrection)
			m_event.m_pfMetChs = m_fi.Get<KDataPFMET>("PFMETCHSPhi");
		else
			m_event.m_pfMetChs = m_fi.Get<KDataPFMET>("PFMETCHS");
		m_event.m_pfMetL2L3 = m_fi.Get<KDataPFMET>("ak5PFMETL2L3");
		if (phicorrection)
			m_event.m_pfMetChsL2L3 = m_fi.Get<KDataPFMET>("ak5PFMETCHSL2L3phi");
		else
			m_event.m_pfMetChsL2L3 = m_fi.Get<KDataPFMET>("ak5PFMETCHSL2L3");
		InitPFJets(m_event, "AK5PFJets");
		InitPFJets(m_event, "AK5PFJetsCHS");

		InitPFJets(m_event, "AK7PFJets");
		InitPFJets(m_event, "AK7PFJetsCHS");

		InitCaloJets(m_event, "AK5CaloJets");
		InitCaloJets(m_event, "AK7CaloJets");

		if (m_fi.isMC())
		{
			// nice, we have the all-mighty TRUTH !
			InitGenJets(m_event, "AK5GenJets");
			InitGenJets(m_event, "AK7GenJets");
			m_event.m_particles = m_fi.Get<KGenParticles>("genParticles");
		}
		else
		{
			// we need to read the residual MET corrections for data
			m_event.m_pfMetL2L3Res = m_fi.Get<KDataPFMET>("ak5PFMETL2L3Res");
			if (phicorrection)
				m_event.m_pfMetChsL2L3Res = m_fi.Get<KDataPFMET>("ak5PFMETCHSL2L3ResPhi");
			else
				m_event.m_pfMetChsL2L3Res = m_fi.Get<KDataPFMET>("ak5PFMETCHSL2L3Res");
		}
}

}
