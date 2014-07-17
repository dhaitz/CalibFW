#pragma once

/*
This producer corrects electron momenta
*/

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;

class ElectronCorrector: public ZJetGlobalMetaDataProducerBase
{
public:

	ElectronCorrector() : ZJetGlobalMetaDataProducerBase() {}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		for (auto it = metaData.m_listValidElectrons.begin();
			 it != metaData.m_listValidElectrons.end(); it++)
		{
			//correct
		}
		return true;
	}

	static std::string Name()
	{
		return "electron_corrector";
	}

};


}
