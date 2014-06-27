#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "NtupleConsumer.h"
#include "NtupleObjectConsumerBase.h"

/*
    This consumer creates an ntuple with an entry for each jet in the event.
*/


namespace Artus
{

class NtupleValidJetConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "jet";
	}

	virtual std::vector<std::string> GetStringvector() const
	{
		return {"pt", "eta", "phi"};
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetMetaData const& metaData, ZJetPipelineSettings const& settings) const
	{
		return metaData.GetValidJetCount(settings, event);
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		return * metaData.GetValidJet(s, event, n);
	};

};



}
