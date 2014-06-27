#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "NtupleConsumer.h"
#include "NtupleObjectConsumerBase.h"

/*
    This consumer creates an ntuple with an entry for each electron in the event.
*/


namespace Artus
{

class NtupleValidElectronConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "electron";
	}

	virtual std::vector<std::string> GetStringvector() const
	{
		return {"pt", "eta", "phi", "mass"};
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetMetaData const& metaData, ZJetPipelineSettings const& settings) const
	{
		return metaData.m_listValidElectrons.size();
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		return metaData.m_listValidElectrons.at(n);
	};

};

}
