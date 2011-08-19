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
	TestZJetEventData() : returnNullJet( false )
	{
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
		m_jets.push_back( KDataLV() );
	}

	virtual KDataLV * GetPrimaryJet ( ZJetPipelineSettings const& psettings ) const
		{
			return GetJet( psettings, 0);
		}

	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index ) const
			{
				if ( returnNullJet )
					return NULL;

				if ( index >= m_jets.size())
					return NULL;

				return &m_jets[index];

			}

	mutable KDataLVs m_jets;
	bool returnNullJet;
};



}


