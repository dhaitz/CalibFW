#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "NtupleConsumer.h"
#include "NtupleObjectConsumerBase.h"

/*
    This consumer creates an ntuple with an entry for each muon in the event.
*/


namespace Artus
{

class NtupleValidMuonConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "muon";
	}

	virtual std::vector<std::string> GetStringvector() const
	{
		return {"pt", "eta", "phi", "mass"};
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetMetaData const& metaData, ZJetPipelineSettings const& settings) const
	{
		return metaData.m_listValidMuons.size();
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		return metaData.m_listValidMuons.at(n);
	};

};

}
