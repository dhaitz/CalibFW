#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "Draw/Profile.h"

#include <boost/scoped_ptr.hpp>

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

#include "Draw/ProfileConsumerBase.h"

#include "../Sources/Sources.h"

namespace CalibFW
{

template < class TEvent, class TMetaData, class TSettings >
class GenericTwoDConsumerBase : public EventConsumerBase< TEvent, TMetaData, TSettings>
{
public:
	typedef SourceBase<TEvent, TMetaData, TSettings> SourceTypeForThis;
	typedef EventPipeline<TEvent, TMetaData, TSettings> PipelineTypeForThis;
};


class GenericTwoDConsumer : public GenericTwoDConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings >
{
public:
	GenericTwoDConsumer( boost::property_tree::ptree * ptree , std::string configPath ): GenericTwoDConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings > ()
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
		else if ( ref == "npv")
			this->SetYSource ( new SourceRecoVert());
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

		else if ( ref == "ptbalance" )
			this->SetYSource ( new SourcePtBalance() );
		else if ( ref == "mpf" )
			this->SetYSource ( new SourceMPFresponse() );

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
		else if ( ref == "etaabsdiff" )
		{
			std::string Name1 = ptree->get<std::string>( configPath + ".YName1");
			std::string Name2 = ptree->get<std::string>( configPath + ".YName2");
			this->SetYSource ( new SourceEtaAbsDiff( Name1, Name2) );
		}
		else if ( ref == "phiabsdiff" )
		{
			std::string Name1 = ptree->get<std::string>( configPath + ".YName1");
			std::string Name2 = ptree->get<std::string>( configPath + ".YName2");
			this->SetYSource ( new SourcePhiAbsDiff( Name1, Name2) );
		}

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
		
		
		ref = ptree->get<std::string>( configPath + ".ZSource");
		
		if ( ref == "npv")
			this->SetZSource ( new SourceRecoVert());
        else if ( ref == "jet1pt" )
			this->SetZSource ( new SourceJetPt() );
		else if ( ref == "jet1eta" )
			this->SetZSource ( new SourceJetEta() );
		else if ( ref == "jet1abseta" )
			this->SetZSource ( new SourceJetAbsEta() );
		else if ( ref == "jet1phi" )
			this->SetZSource ( new SourceJetPhi() );

		else if ( ref == "jet2pt" )
			this->SetZSource ( new SourceJet2Pt() );
		else if ( ref == "jet2eta" )
			this->SetZSource ( new SourceJet2Eta() );
		else if ( ref == "jet2abseta" )
			this->SetZSource ( new SourceJet2AbsEta() );
		else if ( ref == "jet2phi" )
			this->SetZSource ( new SourceJet2Phi() );

		else if ( ref == "zpt" )
			this->SetZSource ( new SourceZPt() );
		else if ( ref == "zeta" )
			this->SetZSource ( new SourceZEta() );
		else if ( ref == "zabseta" )
			this->SetZSource ( new SourceZAbsEta() );
		else if ( ref == "zphi" )
			this->SetZSource ( new SourceZPhi() );

		else if ( ref == "ptbalance" )
			this->SetZSource ( new SourcePtBalance() );
		else if ( ref == "mpf" )
			this->SetZSource ( new SourceMPFresponse() );
		else if ( ref == "alpha" )
			this->SetZSource ( new SourceAlpha() );

		else if ( ref == "METpt" )
			this->SetZSource ( new SourceMETPt() );
		else if ( ref == "METeta" )
			this->SetZSource ( new SourceMETEta() );
		else if ( ref == "METphi" )
			this->SetZSource ( new SourceMETPhi() );
		else if ( ref == "sumEt" )
			this->SetZSource ( new SourceSumEt() );
		else if ( ref == "METfraction" )
			this->SetZSource ( new SourceMETFraction() );
		else if ( ref == "jetsvalid" )
			this->SetZSource ( new SourceValidJets() );
        else if ( ref == "cutvalue" )
		{
			long l = ptree->get<long>( configPath + ".ZCutId");
			this->SetZSource ( new SourceCutValue( l) );
		}
		else if ( ref == "etaabsdiff" )
		{
			std::string Name1 = ptree->get<std::string>( configPath + ".ZName1");
			std::string Name2 = ptree->get<std::string>( configPath + ".ZName2");
			this->SetZSource ( new SourceEtaAbsDiff( Name1, Name2) );
		}
		else if ( ref == "phiabsdiff" )
		{
			std::string Name1 = ptree->get<std::string>( configPath + ".ZName1");
			std::string Name2 = ptree->get<std::string>( configPath + ".ZName2");
			this->SetZSource ( new SourcePhiAbsDiff( Name1, Name2) );
		}
		else if ( ref == "jet1neutralemfraction" )
			this->SetZSource ( new SourceJet1NeutralEMFraction() );
		else if ( ref == "jet1chargedemfraction" )
			this->SetZSource ( new SourceJet1ChargedEMFraction() );
		else if ( ref == "jet1neutralhadfraction" )
			this->SetZSource ( new SourceJet1NeutralHadFraction() );
		else if ( ref == "jet1chargedhadfraction" )
			this->SetZSource ( new SourceJet1ChargedHadFraction() );
		else if ( ref == "jet1HFhadfraction" )
			this->SetZSource ( new SourceJet1HFHadFraction() );
		else if ( ref == "jet1HFemfraction" )
			this->SetZSource ( new SourceJet1HFEMFraction() );
		else if ( ref == "jet1photonfraction" )
			this->SetZSource ( new SourceJet1photonFraction() );
		else if ( ref == "jet1electronfraction" )
			this->SetZSource ( new SourceJet1electronFraction() );
		else
        {
			CALIB_LOG_FATAL( "Source " << ref << " not supported" )
		}
		
	}

	static std::string GetName()
	{
		return "generic_profile2d_consumer";
	}


	virtual void Init(PipelineTypeForThis * pset)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::Init( pset );

		// init the profile plot
		Profile3d::ModifierList modlist;

		if ( m_xsource->HasDefaultBinCount())
			modlist.push_back( new ModProfile3dXBinCount( m_xsource->GetDefaultBinCount()) );
		if ( m_xsource->HasDefaultBins())
			modlist.push_back( new ModProfile3dXBinRange( m_xsource->GetDefaultLowBin(), m_xsource->GetDefaultHighBin() ));

		if ( m_ysource->HasDefaultBinCount())
			modlist.push_back( new ModProfile3dYBinCount( m_ysource->GetDefaultBinCount()) );
		if ( m_ysource->HasDefaultBins())
			modlist.push_back( new ModProfile3dYBinRange( m_ysource->GetDefaultLowBin(), m_ysource->GetDefaultHighBin() ));



		m_profile.reset(
				new Profile3d( m_plotName,
								this->GetPipelineSettings().GetRootFileFolder())
		);
		m_profile->AddModifiers( modlist );
		m_profile->Init();
	}

	virtual void ProcessEvent(ZJetEventData const& event, ZJetMetaData const& metaData,
			FilterResult & result)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::ProcessEvent( event, metaData, result);

		if ( !m_runUnfiltered)
			return;

		if ( ! result.HasPassedIfExcludingFilter( "incut" ) )
			return;


		double xval;
		double yval;
		double zval;

		bool has_xval;
		bool has_yval;
		bool has_zval;

		has_xval = m_xsource->GetValue( event, metaData, this->GetPipelineSettings(), xval );
		has_yval = m_ysource->GetValue( event, metaData, this->GetPipelineSettings(), yval );
		has_zval = m_zsource->GetValue( event, metaData, this->GetPipelineSettings(), zval );

        if (has_xval && has_yval && has_zval) {
			m_profile->AddPoint( xval, yval, zval );
        }
		m_ysource->EndOnEvent( event, metaData, this->GetPipelineSettings() );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::ProcessFilteredEvent( event, metaData);

		if ( m_runUnfiltered)
			return;



		m_ysource->StartOnEvent( event, metaData, this->GetPipelineSettings() );

		//while ( m_zsource->HasValue())
		//{

		double xval;
		double yval;
		double zval;

		bool has_xval;
		bool has_yval;
		bool has_zval;

		has_xval = m_xsource->GetValue( event, metaData, this->GetPipelineSettings(), xval );
		has_yval = m_ysource->GetValue( event, metaData, this->GetPipelineSettings(), yval );
		has_zval = m_zsource->GetValue( event, metaData, this->GetPipelineSettings(), zval );

        if (has_xval && has_yval && has_zval) {
			m_profile->AddPoint( xval, yval, zval );
        }

		m_ysource->EndOnEvent( event, metaData, this->GetPipelineSettings() );
        //}
	}

	// store all histograms
	virtual void Finish()
	{
		m_profile->Store( this->GetPipelineSettings().GetRootOutFile());
	}


	void SetXSource ( SourceTypeForThis * xsource )
	{
		m_xsource.reset( xsource );
	}

	void SetYSource ( SourceTypeForThis * ysource )
	{
		m_ysource.reset( ysource );
	}

	void SetZSource ( SourceTypeForThis * zsource )
	{
		m_zsource.reset( zsource );
	}

	void SetPlotName ( std::string plotName )
	{
		m_plotName = plotName;
	}

	void SetRunUnfiltered( bool val)
	{
		m_runUnfiltered = val;
	}

private:
	boost::scoped_ptr<SourceTypeForThis> m_xsource;
	boost::scoped_ptr<SourceTypeForThis> m_ysource;
	boost::scoped_ptr<SourceTypeForThis> m_zsource;

	boost::scoped_ptr<Profile3d> m_profile;

	std::string m_plotName;
	bool m_runUnfiltered;






	std::string m_name;
};

}
