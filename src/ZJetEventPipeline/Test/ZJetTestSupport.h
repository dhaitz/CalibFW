#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Pipeline/Test/EventPipeline_test.h"
#include "Pipeline/EventPipelineRunner.h"

#include <boost/test/included/unit_test.hpp>

#include "../Producer/ZJetCuts.h"

namespace Artus
{

class TestZJetEventData : public ZJetEventData
{
public:

	// create some default jets
	TestZJetEventData()
	{
		m_jets.push_back(KDataLV());
		m_jets.push_back(KDataLV());
		m_jets.push_back(KDataLV());
		m_jets.push_back(KDataLV());
		m_jets.push_back(KDataLV());
		m_jets.push_back(KDataLV());

		m_muons = new KDataMuons();
		m_muons->push_back(KDataMuon());
		m_muons->push_back(KDataMuon());
	}

	virtual KDataLV* GetPrimaryJet(ZJetPipelineSettings const& psettings) const
	{
		return GetJet(psettings, 0);
	}

	virtual KDataLV* GetJet(ZJetPipelineSettings const& psettings,
							unsigned int index, std::string algoName) const
	{
		assert(index < m_jets.size());

		return &m_jets[index];

	}

	virtual KDataLV* GetJet(ZJetPipelineSettings const& psettings,
							unsigned int index) const
	{
		return GetJet(psettings, index, "no algo used for this test code");
	}

	mutable KDataLVs m_jets;
};



}


