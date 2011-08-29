#pragma once


#include "ZJetFilterBase.h"

namespace CalibFW
{

class InCutFilter: public ZJetFilterBase
{
public:

	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		//unsigned long ignoredCut = settings.GetFilterInCutIgnored();
		// no section here is allowed to set to true again, just to false ! avoids coding errors
		//return event.IsInCutWhenIgnoringCut(ignoredCut);
		// todo
		return metaData.IsAllCutsPassed();
	}

	virtual std::string GetFilterId()
	{
		return InCutFilter::Id();
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "InCut";
	}

	static std::string Id()
	{
		return "incut";
	}
};

class ValidJetFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		// std::cout << "val z " << metaData.HasValidZ() << std::endl;
		return metaData.HasValidJet();
	}

	virtual std::string GetFilterId()
	{
		return "valid_jet";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Valid Jet Filter";
	}

};


class ZJetPipelineInitializer: public PipelineInitilizerBase<ZJetEventData,
		ZJetMetaData, ZJetPipelineSettings>
{
public:
	virtual void
			InitPipeline(EventPipeline<ZJetEventData, ZJetMetaData,
					ZJetPipelineSettings> * pLine,
					ZJetPipelineSettings const& pset) const;
};

}
