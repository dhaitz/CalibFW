#pragma once

namespace Artus
{

//apply typeI-corrections to MET
class TypeIMETProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	TypeIMETProducer(bool EnableMetPhiCorrection, stringvector baseAlgos) : ZJetGlobalMetaDataProducerBase(), m_basealgorithms(baseAlgos), metphi(EnableMetPhiCorrection)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalsettings) const
	{

		for (unsigned int i = 0; i < m_basealgorithms.size(); i++)
		{
			std::string algoname_raw;
			KDataPFMET* rawmet;
			std::vector<std::string> algorithms;
			float sumEt_correction = 0;

			//check if CHS or no CHS
			if (std::string::npos == m_basealgorithms[i].find("chs"))
			{
				algoname_raw = m_basealgorithms[i] + "Jets";
				rawmet = event.m_pfMet;
			}
			else
			{
				algoname_raw = m_basealgorithms[i].substr(0, 5) + "JetsCHS";
				rawmet = event.m_pfMetChs;
			}

			std::string algoname_l1 = algoname_raw + "L1";

			algorithms.push_back(algoname_raw + "L1L2L3");
			//if data, add residuals:
			if (globalsettings.Global()->GetInputType())
				algorithms.push_back(algoname_raw + "L1L2L3Res");

			//iterate over the jet collection and sum up the differences between L1L2L3(res) and L1
			for (unsigned int j = 0; j < algorithms.size(); j++)
			{
				KDataLV correction;
				for (unsigned int i = 0; i < metaData.m_validPFJets.at(algorithms[j]).size(); ++ i)
				{
					KDataPFJet* corrjet = &metaData.m_validPFJets.at(algorithms[j]).at(i);

					if (corrjet->p4.Pt() > 10)
					{
						KDataPFJet* l1jet = &metaData.m_validPFJets.at(algoname_l1).at(i);
						correction.p4 +=  l1jet->p4 - corrjet->p4;
						sumEt_correction += correction.p4.Pt();
					}
				}

				KDataPFMET corrmet = * rawmet;
				corrmet.p4 += correction.p4;
				corrmet.p4.SetEta(0.0f);
				corrmet.sumEt = rawmet->sumEt + sumEt_correction;

				//apply MET-phi-corrections
				if (metphi)
				{
					double px = corrmet.p4.Px();
					double py = corrmet.p4.Py();
					const doublevector* m_metphi = &globalsettings.Global()->m_metphi;

					px = px - (m_metphi->at(0) + m_metphi->at(1) * event.m_vertexSummary->nVertices);
					py = py - (m_metphi->at(2) + m_metphi->at(3) * event.m_vertexSummary->nVertices);

					corrmet.p4.SetPt(sqrt(px * px + py * py));
					corrmet.p4.SetPhi(atan2(py, px));
				}

				metaData.m_MET[algorithms[j]] = corrmet;
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
	bool metphi;
};

}
