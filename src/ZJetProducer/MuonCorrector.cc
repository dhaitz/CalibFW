#include "ZJetProducer/MuonCorrector.h"
#include <Math/PtEtaPhiM4D.h>
#include <Math/LorentzVector.h>

namespace Artus
{

MuonCorrector::MuonCorrector(std::string parameterfile, std::string parameterfileRunD,
							 bool smeared, bool deterministic, bool radiation)
	: startRunD(203770), m_smearing(smeared), m_deterministic(deterministic), m_radiationcorr(radiation),
	  m_parameterfile(parameterfile), m_parameterfileRunD(parameterfileRunD),
	  m_corrector(parameterfile), m_correctorRunD(parameterfileRunD)
{
	if (parameterfile == "")
		LOG_FATAL("Muon corrections enabled but no parameters given.");

	if (m_smearing && m_deterministic)
		LOG("Loading muon corrections (smeared, deterministic):");
	if (m_smearing && !m_deterministic)
		LOG("Loading muon corrections (smeared, random):");
	if (!m_smearing)
		LOG("Loading muon corrections (not smeared):");

	LOG("  " << parameterfile);
	if (parameterfileRunD != "")
		LOG("  " << parameterfileRunD);
}


bool MuonCorrector::PopulateGlobalProduct(ZJetEventData const& event,
		ZJetProduct& product,	ZJetPipelineSettings const& m_pipelineSettings) const
{
	bool isRunD = (m_pipelineSettings.Global()->GetInputType() == DataInput &&
				   event.m_eventproduct->nRun > startRunD);
	double corrPt = 0;
	double smearedPt = 0;
	if (isRunD)
	{
		if (m_parameterfileRunD == "")
			LOG_FATAL("Tried to correct muons in Run D, but no parameters given!");
	}

	// loop over valid muons an correct in place
	for (auto mu = product.m_listValidMuons.begin(); mu != product.m_listValidMuons.end(); mu++)
	{
		// double before = mu->p4.Pt();  // for log debug

		if (isRunD)
			corrPt = m_correctorRunD.getCorrectPt(mu->p4, mu->charge);
		else
			corrPt = m_corrector.getCorrectPt(mu->p4, mu->charge);

		mu->p4.SetPt(corrPt);

		if (m_smearing)
		{
			if (isRunD)
				smearedPt = m_correctorRunD.getSmearedPt(mu->p4, mu->charge, m_deterministic);
			else
				smearedPt = m_corrector.getSmearedPt(mu->p4, mu->charge, m_deterministic);

			mu->p4.SetPt(smearedPt);
		}

		if (m_radiationcorr)
		{
			mu->p4.SetPt(1.004 * mu->p4.Pt());
		}
		//LOG("Muon Correction: " << before << " -> " << corrPt << " -> " << smearedPt
		//	<< " (charge: " << int(mu->charge) <<")");
	}

	return true;
}

}
