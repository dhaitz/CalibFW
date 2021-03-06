#pragma once

/*
Apply typeI-corrections to MET: Iterate over jets and subtract L2L3(Res)
https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMetAnalysis#Type_I_Correction
*/
namespace Artus
{


class TypeIMETProducer: public ZJetGlobalProductProducerBase
{
public:

	TypeIMETProducer(bool EnableMetPhiCorrection, stringvector baseAlgos, bool rc, bool isData, bool flavourCorrections) :
		ZJetGlobalProductProducerBase(), m_basealgorithms(baseAlgos),
		m_metphi(EnableMetPhiCorrection), m_rc(rc), m_isData(isData),
		m_flavourCorrections(flavourCorrections), jet_min_pt(10.)
	{
		if (m_rc)
			LOG("Using RC Offset to calculate type-I corrections")

			for (unsigned int i = 0; i < m_basealgorithms.size(); i++)
			{
				std::string algoname_raw, algoname_l1;
				std::vector<std::string> algorithms;

				if (std::string::npos == m_basealgorithms[i].find("chs"))
				{
					m_isCHS.push_back(false);
					algoname_raw = m_basealgorithms[i] + "Jets";
				}
				else
				{
					m_isCHS.push_back(true);
					algoname_raw = m_basealgorithms[i].substr(0, 5) + "JetsCHS";
				}

				if (m_rc)
					m_l1algorithms.emplace_back(algoname_raw + "RC");
				else
					m_l1algorithms.emplace_back(algoname_raw + "L1");

				algorithms.emplace_back(algoname_raw + "L1L2L3");
				//if data, add residuals:
				if (m_isData)
					algorithms.emplace_back(algoname_raw + "L1L2L3Res");
				else if (m_flavourCorrections)
				{
					algorithms.emplace_back(algoname_raw + "L1L2L3L5Algo");
					algorithms.emplace_back(algoname_raw + "L1L2L3L5Phys");
				}

				m_algorithms.emplace_back(algorithms);
			}
	}

	virtual bool PopulateGlobalProduct(ZJetEventData const& event,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalsettings) const
	{

		for (unsigned int i = 0; i < m_algorithms.size(); i++)
		{
			KDataPFMET* rawmet;
			float sumEt_correction = 0;

			//check if CHS or no CHS
			if (m_isCHS[i])
				rawmet = event.m_pfMet;
			else
				rawmet = event.m_pfMetChs;

			//iterate over the jet collection and sum up the differences between L1L2L3(res) and L1
			for (unsigned int j = 0; j < m_algorithms[i].size(); j++)
			{

				KDataLV correction;
				for (unsigned int k = 0; k < product.m_validPFJets.at(m_algorithms[i][j]).size(); ++ k)
				{
					KDataPFJet* corrjet = &product.m_validPFJets.at(m_algorithms[i][j]).at(k);

					if (corrjet->p4.Pt() > jet_min_pt)
					{
						KDataPFJet* l1jet = &product.m_validPFJets.at(m_l1algorithms[i]).at(k);
						correction.p4 +=  l1jet->p4 - corrjet->p4;
						sumEt_correction += correction.p4.Pt();
					}
				}

				KDataPFMET corrmet = * rawmet;
				corrmet.p4 += correction.p4;

				//add p4 from MuscleFit
				corrmet.p4 += product.muondiff.p4;

				//eta of MET is always zero
				corrmet.p4.SetEta(0.0f);
				corrmet.sumEt = rawmet->sumEt + sumEt_correction;

				//apply MET-phi-corrections
				if (m_metphi)
				{
					double px = corrmet.p4.Px();
					double py = corrmet.p4.Py();
					const doublevector* metphi = &globalsettings.Global()->m_metphi;

					px = px - (metphi->at(0) + metphi->at(1) * event.m_vertexSummary->nVertices);
					py = py - (metphi->at(2) + metphi->at(3) * event.m_vertexSummary->nVertices);

					corrmet.p4.SetPt(sqrt(px * px + py * py));
					corrmet.p4.SetPhi(atan2(py, px));
				}

				product.m_MET[m_algorithms[i][j]] = corrmet;
			}
		}
		return true;
	}

	static std::string Name()
	{
		return "typeImet_producer";
	}



private:
	std::vector<std::string> m_basealgorithms;
	const bool m_metphi;
	const bool m_rc;
	const bool m_isData;
	const bool m_flavourCorrections;
	const float jet_min_pt;
	std::vector<std::vector<std::string>> m_algorithms;
	std::vector<std::string> m_l1algorithms;
	std::vector<bool> m_isCHS;
};

}
