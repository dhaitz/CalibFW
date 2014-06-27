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

class NtupleGenParticleConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "genparticle";
	}

	virtual std::vector<std::string> GetStringvector() const
	{
		return {"pt", "eta", "phi", "mass", "id", "status"};
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetMetaData const& metaData, ZJetPipelineSettings const& settings) const
	{
		return event.m_particles->size();
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		return event.m_particles->at(n);
	};

	virtual float returnvalue(int n, std::string string, ZJetEventData const& event,
							  ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		if (string == "id")
			return event.m_particles->at(n).pdgId();
		else if (string == "status")
			return event.m_particles->at(n).status();
		else
			return NtupleObjectConsumerBase::returnvalue(n, string, event, metaData, s);
	};

};

}
