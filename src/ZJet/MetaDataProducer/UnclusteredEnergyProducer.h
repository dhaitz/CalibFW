#pragma once

namespace CalibFW
{

//apply typeI-corrections to MET
class UnclusteredEnergyProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	UnclusteredEnergyProducer(stringvector baseAlgos) : ZJetGlobalMetaDataProducerBase(), m_basealgorithms(baseAlgos)
 {}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalsettings) const
	{
	for (int i = 0; i < m_basealgorithms.size(); i++)
	{

		std::string algoname_raw;
		KDataPFMET * rawmet;
        KDataLV * ue;

		//check if CHS or no CHS
		if (std::string::npos == m_basealgorithms[i].find("chs"))
		{
			algoname_raw = m_basealgorithms[i] + "Jets";
            rawmet = event.m_pfMet;
			ue = &metaData.UE;
		}
		else
		{
			algoname_raw = m_basealgorithms[i].substr(0, 5) + "JetsCHS";
            rawmet = event.m_pfMetChs;
			ue = &metaData.UEchs;
		}

        // UE is the negative of MET ...
        ue->p4 -= rawmet->p4;

        // ... sum over all jets ...
		for (int j=0; j < metaData.m_listValidJets[algoname_raw].size(); j++)
		{
            int n = metaData.m_listValidJets[algoname_raw].at(j);
            ue->p4 -= event.m_pfPointerJets.at(algoname_raw)->at(n)->p4;
        }
        // ... and Z.
        ue->p4 -= metaData.GetRefZ().p4;

	}
		return true;
	}

	static std::string Name() { return "unclustered_energy_producer"; }



private:
	std::vector<std::string> m_basealgorithms;
};

}
