#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "Draw/Profile.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"
#include "../ZJetPipelineSettings.h"

#include "Draw/ProfileConsumerBase.h"

#include "../Sources/Sources.h"

namespace CalibFW
{

class GenericProfileConsumer : public ProfileConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings >
{
public:
	GenericProfileConsumer( boost::property_tree::ptree * ptree , std::string configPath)
	 : ProfileConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings > ()
	{
		// make this more generic
		this->SetPlotName( ptree->get<std::string>( configPath + ".ProductName") );
		this->SetRunUnfiltered( ptree->get<bool>( configPath + ".RunUnfiltered", false) );

		std::string ref = ptree->get<std::string>( configPath + ".YSource");

		if ( ref == "cutvalue" )
		{
			long l = ptree->get<long>( configPath + ".CutId");

			this->SetYSource ( new SourceCutValue( l) );
		}

		else if ( ref == "hltprescale" )
		{
			this->SetYSource ( new SourceHltPrescale( ptree->get<std::string>( configPath + ".YSourceConfig") ) );
		}

		else if ( ref == "eventcount" )
		{
			this->SetYSource ( new SourceEventcount() );
		}
		else if ( ref == "jetptratio" )
		{

			std::string jet1Name = ptree->get<std::string>( configPath + ".Jet1Ratio");
			std::string jet2Name = ptree->get<std::string>( configPath + ".Jet2Ratio");

			this->SetYSource ( new SourceJetPtRatio( jet1Name, jet2Name, 0,0) );
		}
		else
		{
			CALIB_LOG_FATAL( "Source " << ref << " not supported" )
		}

		ref = ptree->get<std::string>( configPath + ".XSource");

		if ( ref == "jetpt" )
			this->SetXSource ( new SourceJetPt() );
		else if ( ref == "reco")
			this->SetXSource ( new SourceRecoVert());
		else if ( ref == "jeteta")
			this->SetXSource ( new SourceJetEta());
		else if ( ref == "zpt")
			this->SetXSource ( new SourceZPt());
		else if ( ref == "runnumber")
			this->SetXSource ( new SourceRunNumber());
		else if ( ref == "intlumi")
			this->SetXSource ( new SourceIntegratedLumi() );
		else
		{
			CALIB_LOG_FATAL( "Source " << ref << " not supported" )
		}
	}

	static std::string GetName()
	{
		return "generic_profile_consumer";
	}

	std::string m_name;
};

}