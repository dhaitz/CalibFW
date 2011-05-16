#pragma once

#include "EventPipeline.h"
#include "PipelineSettings.h"

#include "EventData.h"


// todo: move zJet specific pline settings here

namespace CalibFW
{

class ZJetPipelineSettings
{

public:
	ZJetPipelineSettings()
	{
		SetSettingsRoot("default");
//		SetEnableReweighting(false);
	}

	enum WriteEventsEnum
	{
		NoEvents, OnlyInCutEvents, AllEvents
	};

	IMPL_PROPERTY(boost::property_tree::ptree * , PropTree)
	IMPL_PROPERTY(std::string, SettingsRoot)
	IMPL_PROPERTY(unsigned long, OverallNumberOfProcessedEvents )

	IMPL_SETTING( bool, EnableCaloMatching)
	IMPL_SETTING( bool, EnableReweighting)

	// Filter Settings
	IMPL_SETTING(double, FilterPtBinLow)
	IMPL_SETTING(double, FilterPtBinHigh)
	IMPL_SETTING( int, FilterRecoVertLow)
	IMPL_SETTING( int, FilterRecoVertHigh)
	IMPL_SETTING(double, FilterJetEtaLow)
	IMPL_SETTING(double, FilterJetEtaHigh)

	IMPL_SETTING(double, FilterSecondJetRatioLow)
	IMPL_SETTING(double, FilterSecondJetRatioHigh)
	IMPL_SETTING(unsigned long, FilterInCutIgnored)

	IMPL_SETTING(std::string, AlgoName)
	IMPL_SETTING(std::string, RootFileFolder)
	IMPL_SETTING(std::string, SecondLevelFolderTemplate)

	// only level 1 runs directly on data
	IMPL_SETTING(int, Level)

	IMPL_SETTING(std::string, WriteEvents)

	WriteEventsEnum GetWriteEventsEnum()
	{
		if (this->GetWriteEvents() == "all")
			return AllEvents;
		if (this->GetWriteEvents() == "incut")
			return OnlyInCutEvents;

		return NoEvents;
	}

	// Cut settings
	IMPL_SETTING(std::string, CutHLT)
	IMPL_SETTING(double, CutMuonPt)
	IMPL_SETTING(double, CutZMassWindow)
	IMPL_SETTING(double, CutMuEta)
	IMPL_SETTING(double, CutZPt)
	IMPL_SETTING(double, CutMuonEta)
	IMPL_SETTING(double, CutLeadingJetEta)
	IMPL_SETTING(double, CutSecondLeadingToZPt)
	IMPL_SETTING(double, CutSecondLeadingToZPtJet2Threshold)
	IMPL_SETTING(double, CutSecondLeadingToZPtDeltaR)
	IMPL_SETTING(double, CutBack2Back)
	IMPL_SETTING(double, CutJetPt)

	IMPL_PROPERTY(TFile *, RootOutFile)

	InputTypeEnum GetInputType()
	{
		std::string sInp = GetPropTree()->get< std::string >( "InputType");

		InputTypeEnum inp;

		if (sInp == "mc")
			inp = McInput;
		else if (sInp == "data")
			inp = DataInput;
		else
			CALIB_LOG_FATAL("Input type " + sInp + " not supported.")

			return inp;
	}

	VarCache< stringvector > m_filter;

	stringvector GetFilter()
	{
		RETURN_CACHED( m_filter, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".Filter") )
	}

	VarCache< stringvector > m_L2Corr;

	stringvector GetL2Corr()
	{
		RETURN_CACHED( m_L2Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L2Corr") )
	}

	VarCache< stringvector > m_L3Corr;

	stringvector GetL3Corr()
	{
		RETURN_CACHED( m_L3Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L3Corr") )
	}

	VarCache< stringvector > m_additionalConsumer;

	stringvector GetAdditionalConsumer()
	{
		RETURN_CACHED( m_additionalConsumer, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".AdditionalConsumer") )
	}

	static std::vector<PtBin> GetAsPtBins( stringvector & sv )
		{
		std::vector< PtBin > bins;

		int i = 0;
		for ( stringvector::iterator it = (sv.begin() + 1);
				it < sv.end();
				it ++)
		{
			int ilow = atoi ( sv[i].c_str() );
			int ihigh = atoi ( sv[i+1].c_str() );

			bins.push_back( PtBin( ilow, ihigh));
			i++;
		}

		return bins;
		}

	VarCache< doublevector > m_recovertWeight;

	doublevector GetRecovertWeight()
	{
		RETURN_CACHED( m_recovertWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetSettingsRoot() + ".RecovertWeight") )
	}

	VarCache< stringvector > m_jetRespBins;

	stringvector GetCustomBins()
	{
		RETURN_CACHED( m_jetRespBins, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".CustomBins") )
	}
	// TODO: maybe cache this for better performance

	VarCache< stringvector > m_cuts;

	stringvector GetCuts()
	{
		RETURN_CACHED( m_cuts,PropertyTreeSupport::GetAsStringList( GetPropTree(),GetSettingsRoot() + ".Cuts" ) )
	}
	/*
 void AddFilter( std::string sFilterId)
 {
 m_filter.push_back( sFilterId);
 }

 stringvector m_filter;*/

};

typedef EventPipeline<EventResult, ZJetPipelineSettings> ZJetPipeline;
	//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;
	
}
