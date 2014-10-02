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
						ZJetProduct const& product, ZJetPipelineSettings const& settings) const
	{
		return product.m_listValidMuons.size();
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		return product.m_listValidMuons.at(n);
	};

};

}
