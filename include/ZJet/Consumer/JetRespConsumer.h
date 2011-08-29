




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

#include "../../RootIncludes.h"

#include "../../DrawModifierBase.h"
#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"


namespace CalibFW
{

class JetRespConsumer: public ZJetConsumer
{
public:

	JetRespConsumer( boost::property_tree::ptree * ptree , std::string configPath )
	{
		/*
	 "Name" : "response_balance",
	 "SourceFolder" : [ "Pt30to60_incut", "Pt60to100_incut" ],
	 "SourceResponse" : "balresp_AK5PFJets",
	 "SourceBinning" : "z_pt_AK5PFJets"
		 */
		m_sourceFolder = PropertyTreeSupport::GetAsStringList( ptree, configPath + ".SourceFolder" );

		if (m_sourceFolder.size() == 0)
		{
			CALIB_LOG_FATAL("Empty SourceFolder list given")
		}

		m_sourceResponse = ptree->get<std::string>( configPath + ".SourceResponse");
		m_sourceBinning =  ptree->get<std::string>( configPath + ".SourceBinning");
		m_productName =  ptree->get<std::string>( configPath + ".ProductName");
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetConsumer::Init( pset );

		m_resp = new GraphErrors( m_productName, "");
		m_resp->Init();
	}

	virtual void Process()
	{
		ZJetConsumer::Process();
		//m_run->Fill( event.m_eventmetadata->nRun, metaData.GetWeight());

		BOOST_FOREACH(std::string folderName, m_sourceFolder )
		{
			TH1D * histResp =
			RootFileHelper::SafeGet<TH1D*>(this->GetPipelineSettings().GetRootOutFile(),
											folderName + "/" + m_sourceResponse );
			TH1D * histBin =
			RootFileHelper::SafeGet<TH1D*>(this->GetPipelineSettings().GetRootOutFile(),
											folderName + "/" + m_sourceBinning );
			//std::cout << histBin->GetMean() << " __ " << histResp->GetMean() << std::endl;
			m_resp->AddPoint(	histBin->GetMean(), 	histResp->GetMean(),
								histBin->GetMeanError(), histResp->GetMeanError());
		}
	}

	virtual void Finish()
	{
		m_resp->Store(this->GetPipelineSettings().GetRootOutFile());
	}

	static std::string GetName()
	{
		return "response_balance";
	}

private:
	GraphErrors * m_resp;
	stringvector m_sourceFolder;
	std::string m_sourceResponse;
	std::string m_sourceBinning;
	std::string m_productName;
};


}

