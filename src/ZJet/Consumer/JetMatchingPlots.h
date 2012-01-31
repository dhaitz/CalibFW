#pragma once

#include <string>
#include <iostream>

#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp>
#include <list>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <boost/property_tree/ptree.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"

#include "Draw/DrawBase.h"
#include "Pipeline/JetTools.h"

#include "../ZJetPipeline.h"
#include "ZJetDrawConsumer.h"

namespace CalibFW
{

class JetMatchingConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_histJetMatch = new Hist1D("jet1_jet_matching_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetJetMatchingModifier());


		AddPlot(m_histJetMatch);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		
        std::string genName ( JetType::GetGenName ( this->GetPipelineSettings().GetJetAlgorithm() ) );
        std::vector < int > const& matchList = metaData.m_matchingResults.at( genName );
        
        if ( matchList.size() == 0 )
        {
            // something is wrong with the mapping
            m_histJetMatch->Fill( -2, metaData.GetWeight());
        }
        else
        {
            m_histJetMatch->Fill( matchList[0], metaData.GetWeight());
        }
	}

	Hist1D * m_histJetMatch;
	std::string m_sObjectName;
};


}

