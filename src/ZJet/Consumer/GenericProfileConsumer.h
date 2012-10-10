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
		else if ( ref == "selectedhltprescale" )
		{
			this->SetYSource ( new SourceSelectedHltPrescale( ) );
		}

		else if ( ref == "eventcount" )
		{
			this->SetYSource ( new SourceEventcount() );
		}
		else if ( ref == "jetptratio" )
		{
			std::string jet1Name = ptree->get<std::string>( configPath + ".Jet1Name");
			std::string jet2Name = ptree->get<std::string>( configPath + ".Jet2Name");
			unsigned int jet1Num = ptree->get<unsigned int>( configPath + ".Jet1Num");
			unsigned int jet2Num = ptree->get<unsigned int>( configPath + ".Jet2Num");

			this->SetYSource ( new SourceJetPtRatio( jet1Name, jet2Name, jet1Num, jet2Num) );
		}
		else if ( ref == "jetptabsdiff" )
		{
			std::string jet1Name = ptree->get<std::string>( configPath + ".Jet1Name");
			std::string jet2Name = ptree->get<std::string>( configPath + ".Jet2Name");
			unsigned int jet1Num = ptree->get<unsigned int>( configPath + ".Jet1Num");
			unsigned int jet2Num = ptree->get<unsigned int>( configPath + ".Jet2Num");

			this->SetYSource ( new SourceJetPtAbsDiff( jet1Name, jet2Name, jet1Num, jet2Num) );
		}
		else if ( ref == "jet1pt" )
			this->SetYSource ( new SourceJetPt() );
		else if ( ref == "jet1eta" )
			this->SetYSource ( new SourceJetEta() );
		else if ( ref == "jet1abseta" )
			this->SetYSource ( new SourceJetAbsEta() );
		else if ( ref == "jet1phi" )
			this->SetYSource ( new SourceJetPhi() );

		else if ( ref == "jet2pt" )
			this->SetYSource ( new SourceJet2Pt() );
		else if ( ref == "jet2eta" )
			this->SetYSource ( new SourceJet2Eta() );
		else if ( ref == "jet2abseta" )
			this->SetYSource ( new SourceJet2AbsEta() );
		else if ( ref == "jet2phi" )
			this->SetYSource ( new SourceJet2Phi() );

		else if ( ref == "zpt" )
			this->SetYSource ( new SourceZPt() );
		else if ( ref == "zeta" )
			this->SetYSource ( new SourceZEta() );
		else if ( ref == "zabseta" )
			this->SetYSource ( new SourceZAbsEta() );
		else if ( ref == "zphi" )
			this->SetYSource ( new SourceZPhi() );
		else if ( ref == "zmass" )
			this->SetYSource ( new SourceZMass() );

		else if ( ref == "ptbalance" )
			this->SetYSource ( new SourcePtBalance() );
		else if ( ref == "mpf" )
			this->SetYSource ( new SourceMPFresponse() );
		else if ( ref == "alpha" )
			this->SetYSource ( new SourceAlpha() );

		else if ( ref == "METpt" )
			this->SetYSource ( new SourceMETPt() );
		else if ( ref == "METeta" )
			this->SetYSource ( new SourceMETEta() );
		else if ( ref == "METphi" )
			this->SetYSource ( new SourceMETPhi() );

		else if ( ref == "sumEt" )
			this->SetYSource ( new SourceSumEt() );
		else if ( ref == "METfraction" )
			this->SetYSource ( new SourceMETFraction() );

		else if ( ref == "jetsvalid" )
			this->SetYSource ( new SourceValidJets() );

		else if ( ref == "jet1neutralemfraction" )
			this->SetYSource ( new SourceJet1NeutralEMFraction() );
		else if ( ref == "jet1chargedemfraction" )
			this->SetYSource ( new SourceJet1ChargedEMFraction() );
		else if ( ref == "jet1neutralhadfraction" )
			this->SetYSource ( new SourceJet1NeutralHadFraction() );
		else if ( ref == "jet1chargedhadfraction" )
			this->SetYSource ( new SourceJet1ChargedHadFraction() );
		else if ( ref == "jet1HFhadfraction" )
			this->SetYSource ( new SourceJet1HFHadFraction() );
		else if ( ref == "jet1HFemfraction" )
			this->SetYSource ( new SourceJet1HFEMFraction() );
		else if ( ref == "jet1photonfraction" )
			this->SetYSource ( new SourceJet1photonFraction() );
		else if ( ref == "jet1electronfraction" )
			this->SetYSource ( new SourceJet1electronFraction() );

		else
		{
			CALIB_LOG_FATAL( "Source " << ref << " not supported" )
		}



		ref = ptree->get<std::string>( configPath + ".XSource");

		if ( ref == "jet1pt" )
			this->SetXSource ( new SourceJetPt() );
		else if ( ref == "npv")
			this->SetXSource ( new SourceRecoVert());
		else if ( ref == "jet1eta")
			this->SetXSource ( new SourceJetEta());
		else if ( ref == "jet1phi")
			this->SetXSource ( new SourceJetPhi());
		else if ( ref == "jet2pt" )
			this->SetXSource ( new SourceJet2Pt() );
		else if ( ref == "jet2eta" )
			this->SetXSource ( new SourceJet2Eta() );
		else if ( ref == "jet2phi" )
			this->SetXSource ( new SourceJet2Phi() );
		else if ( ref == "jetptratio" )
		{
			std::string jet1Name = ptree->get<std::string>( configPath + ".Jet1Name");
			std::string jet2Name = ptree->get<std::string>( configPath + ".Jet2Name");
			unsigned int jet1Num = ptree->get<unsigned int>( configPath + ".Jet1Num");
			unsigned int jet2Num = ptree->get<unsigned int>( configPath + ".Jet2Num");

			this->SetXSource ( new SourceJetPtRatio( jet1Name, jet2Name, jet1Num, jet2Num) );
		}
		else if ( ref == "jetptabsdiff" )
		{
			std::string jet1Name = ptree->get<std::string>( configPath + ".Jet1Name");
			std::string jet2Name = ptree->get<std::string>( configPath + ".Jet2Name");
			unsigned int jet1Num = ptree->get<unsigned int>( configPath + ".Jet1Num");
			unsigned int jet2Num = ptree->get<unsigned int>( configPath + ".Jet2Num");

			this->SetXSource ( new SourceJetPtAbsDiff( jet1Name, jet2Name, jet1Num, jet2Num) );
		}
		else if ( ref == "etaabsdiff" )
		{
			std::string Name1 = ptree->get<std::string>( configPath + ".XName1");
			std::string Name2 = ptree->get<std::string>( configPath + ".XName2");
			this->SetXSource ( new SourceEtaAbsDiff( Name1, Name2) );
		}
		else if ( ref == "phiabsdiff" )
		{
			std::string Name1 = ptree->get<std::string>( configPath + ".XName1");
			std::string Name2 = ptree->get<std::string>( configPath + ".XName2");
			this->SetXSource ( new SourcePhiAbsDiff( Name1, Name2) );
		}

		else if ( ref == "zpt")
			this->SetXSource ( new SourceZPt());
		else if ( ref == "zphi")
			this->SetXSource ( new SourceZPhi());
		else if ( ref == "zeta")
			this->SetXSource ( new SourceZEta());

		else if ( ref == "METpt" )
			this->SetXSource ( new SourceMETPt() );
		else if ( ref == "METeta" )
			this->SetXSource ( new SourceMETEta() );
		else if ( ref == "METphi" )
			this->SetXSource ( new SourceMETPhi() );
		else if ( ref == "sumEt" )
			this->SetXSource ( new SourceSumEt() );

		else if ( ref == "jetsvalid" )
			this->SetXSource ( new SourceValidJets() );
		else if ( ref == "alpha" )
			this->SetXSource ( new SourceAlpha() );

		else if ( ref == "deltarsecondjettoz" )
			this->SetXSource ( new SourceDeltaRSecondJetToZ() );
		else if ( ref == "deltarsecondjettoleadingjet" )
			this->SetXSource ( new SourceDeltaRSecondJetToLeadingJet() );

		else if ( ref == "run")
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
