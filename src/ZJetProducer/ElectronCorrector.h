#pragma once

/*
This producer corrects electron momenta
*/

namespace Artus
{

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

class ElectronCorrector: public ZJetGlobalProductProducerBase
{
public:

	ElectronCorrector() : ZJetGlobalProductProducerBase() {}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const
	{
		for (auto it = product.m_listValidElectrons.begin();
			 it != product.m_listValidElectrons.end(); it++)
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
