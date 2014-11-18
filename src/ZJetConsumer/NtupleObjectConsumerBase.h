#pragma once

#include <string>
#include <memory>
#include <boost/algorithm/string/join.hpp>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "NtupleConsumer.h"

/**
    \brief Base class for NtupleObjectConsumer.

    In contrast to the 'normal' Ntuple Consumer, the NtupleObjectConsumer creates
    an entry for each object (of a certain type) in the event.
    This file contains the base class, where actual implementations (for muons,
    jets, ...) can be derived from.
*/


namespace Artus
{

class NtupleObjectConsumerBase : public NtupleConsumerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
{
public:
	virtual std::string GetName() const
	{
		return GetObjectName() + "ntuple";
	}

	void Init(PipelineTypeForThis* pset)
	{
		EventConsumerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>::Init(pset);
		m_ntuple = new TNtuple((m_name + "NTuple").c_str(), (m_name + "NTuple").c_str(), GetQuantitiesString().c_str());

	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
									  ZJetProduct const& product)
	{
		EventConsumerBase< ZJetEventData, ZJetProduct, ZJetPipelineSettings>::ProcessFilteredEvent(event, product);

		std::vector<float> array;
		std::vector<std::string> strvec = GetStringvector();

		int n = getsize(event, product, this->GetPipelineSettings());
		//iterate over each object in the event
		for (int i = 0; i < n; i++)
		{
			//iterate over string vector and fill the array for each quantitiy
			for (std::vector<std::string>::iterator it = strvec.begin(); it != strvec.end(); ++it)
				array.push_back(returnvalue(i, *it, event, product, this->GetPipelineSettings()));
			// add the array to the ntuple
			m_ntuple->Fill(&array.front());
			array.clear();
		}
	}

	virtual void Finish()
	{
		m_ntuple->Write((GetObjectName() + "_" + this->GetPipelineSettings().GetName()).c_str());
	}

private:
	std::string GetQuantitiesString() const
	{
		return boost::algorithm::join(GetStringvector(), ":");
	}

protected:
	TNtuple* m_ntuple;
	std::string m_name;

	// TODO Make this configurable
	virtual std::vector<std::string> GetStringvector() const
	{
		return {};
	}

	virtual std::string GetObjectName() const
	{
		return "";
	}

	virtual float returnvalue(int n, std::string string, ZJetEventData const& event,
							  ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		// basic kinematics
		if (string == "pt")
			return GetSingleObject(n, event, product, s).p4.Pt();
		else if (string == "phi")
			return GetSingleObject(n, event, product, s).p4.Phi();
		else if (string == "eta")
			return GetSingleObject(n, event, product, s).p4.Eta();
		else if (string == "mass")
			return GetSingleObject(n, event, product, s).p4.mass();
		else if (string == "weight")
			return product.GetWeight();
		else
			LOG_FATAL(GetObjectName() << "NtupleConsumer: Quantity (" << string << ") not available!");

	};

	virtual int getsize(ZJetEventData const& event,
						ZJetProduct const& product, ZJetPipelineSettings const& settings) const
	{
		return 0;
	};

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		return * new KDataLV;
	};

};

}
