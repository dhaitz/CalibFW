#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "EventPipeline_test.h"
#include "EventPipelineRunner.h"

#include <boost/test/included/unit_test.hpp>

#include "ZJetCuts.h"

namespace CalibFW
{

class TestZJetEventData : public ZJetEventData
{
public:

	// create some default jets
	TestZJetEventData()
	{
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );

		m_muons = new KDataMuons();
		m_muons->push_back(KDataMuon() );
		m_muons->push_back(KDataMuon() );
	}

	virtual KDataLV * GetPrimaryJet ( ZJetPipelineSettings const& psettings ) const
		{
			return GetJet( psettings, 0);
		}

	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index ) const
			{
				assert ( index >= m_jets.size());

				return &m_jets[index];

			}

	mutable KDataLVs m_jets;
};



}

