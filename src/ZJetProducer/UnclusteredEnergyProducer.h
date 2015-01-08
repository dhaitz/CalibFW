#pragma once

/*
This producer calculates the unclustered energy as the difference of all jets, Z and MET.
TODO this is just for testing purposes, not official results
*/

namespace Artus
{
class UnclusteredEnergyProducer: public ZJetGlobalProductProducerBase
{
public:

	UnclusteredEnergyProducer(stringvector baseAlgos) : ZJetGlobalProductProducerBase(), m_basealgorithms(baseAlgos)
	{}

	virtual bool PopulateGlobalProduct(ZJetEventData const& event,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalsettings) const
	{
		for (unsigned int i = 0; i < m_basealgorithms.size(); i++)
		{
			std::string algoname_raw;
			KDataPFMET* rawmet;
			KDataLV* ue;

			//check if CHS or no CHS
			if (std::string::npos == m_basealgorithms[i].find("chs"))
			{
				algoname_raw = m_basealgorithms[i] + "Jets";
				rawmet = event.m_pfMet;
				ue = &product.UE;
			}
			else
			{
				algoname_raw = m_basealgorithms[i].substr(0, 5) + "JetsCHS";
				rawmet = event.m_pfMetChs;
				ue = &product.UEchs;
			}

			// UE is the negative of MET ...
			ue->p4 -= rawmet->p4;

			// ... sum over all jets ...
			for (unsigned int j = 0; j < product.m_listValidJets[algoname_raw].size(); j++)
			{
				int n = product.m_listValidJets[algoname_raw].at(j);
				ue->p4 -= event.m_pfPointerJets.at(algoname_raw)->at(n)->p4;
			}
			// ... and Z.
			ue->p4 -= product.GetRefZ().p4;

		}
		return true;
	}

	static std::string Name()
	{
		return "unclustered_energy_producer";
	}



private:
	std::vector<std::string> m_basealgorithms;
};

}
