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
		// open Kappa issue, disable the check and it will work
		m_event.m_vertexSummary = m_fi.Get<KVertexSummary> (
				"goodOfflinePrimaryVerticesSummary", false);
		m_event.m_beamSpot = m_fi.Get<KDataBeamSpot> ("offlineBeamSpot", false);
		m_event.m_jetArea = m_fi.Get<KJetArea> ("KT6Area");
		m_event.m_muons = m_fi.Get<KDataMuons> ("muons");
		m_event.m_pfMet = m_fi.Get<KDataPFMET> ("PFMET", false);
		m_event.m_pfMetChs = m_fi.Get<KDataPFMET> ("PFMETCHS", false);
		m_event.m_pfMetL1 = m_fi.Get<KDataPFMET> ("ak5PFMETL1", false);
		m_event.m_pfMetChsL1 = m_fi.Get<KDataPFMET> ("ak5PFMETCHSL1", false);
		m_event.m_pfMetL2L3 = m_fi.Get<KDataPFMET> ("ak5PFMETL2L3", false);
		m_event.m_pfMetL1L2L3 = m_fi.Get<KDataPFMET> ("ak5PFMETL1L2L3", false);
		m_event.m_pfMetChsL1L2L3 = m_fi.Get<KDataPFMET> ("ak5PFMETCHSL1L2L3", false);
		if (phicorrection)
			m_event.m_pfMetChsL2L3 = m_fi.Get<KDataPFMET> ("ak5PFMETCHSL2L3phi", false); //phi-corrected
		else
			m_event.m_pfMetChsL2L3 = m_fi.Get<KDataPFMET> ("ak5PFMETCHSL2L3", false);
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
			m_event.m_particles = m_fi.Get<KPartons> ("genParticles", false);
		}
		else
		{
			// we need to read the residual MET corrections for data
			m_event.m_pfMetL2L3Res = m_fi.Get<KDataPFMET> ("ak5PFMETL2L3Res", false);
			if (phicorrection)
				m_event.m_pfMetChsL2L3Res = m_fi.Get<KDataPFMET> ("ak5PFMETCHSL2L3Resphi", false); //phi-corrected
			else
				m_event.m_pfMetChsL2L3Res = m_fi.Get<KDataPFMET> ("ak5PFMETCHSL2L3Res", false);
		}
}

}
