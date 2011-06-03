#pragma once

#include "EventPipeline.h"
#include "PipelineSettings.h"

#include "EventData.h"
#include "CutHandler.h"

//#include "ZJetFilter.h"

namespace CalibFW
{

class ZJetPipelineSettings;
typedef EventPipeline<EventResult, ZJetPipelineSettings> ZJetPipeline;

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

	// if a cut is enabled, the bit corresponding to the cut id it set in this integer
IMPL_PROPERTY(unsigned long, CutEnabledBitmask)

IMPL_PROPERTY(TFile *, RootOutFile)

	InputTypeEnum GetInputType()
	{
		std::string sInp = GetPropTree()->get<std::string> ("InputType");

		InputTypeEnum inp = McInput;

		if (sInp == "mc")
			inp = McInput;
		else if (sInp == "data")
			inp = DataInput;
		else
		CALIB_LOG_FATAL("Input type " + sInp + " not supported.")

		return inp;
	}

	VarCache<stringvector> m_filter;

	stringvector GetFilter()
	{
		RETURN_CACHED( m_filter, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".Filter") )
	}

	VarCache<stringvector> m_L2Corr;

	stringvector GetL2Corr()
	{
		RETURN_CACHED( m_L2Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L2Corr") )
	}

	VarCache<stringvector> m_L3Corr;

	stringvector GetL3Corr()
	{
		RETURN_CACHED( m_L3Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L3Corr") )
	}

	VarCache<stringvector> m_additionalConsumer;

	stringvector GetAdditionalConsumer()
	{
		RETURN_CACHED( m_additionalConsumer, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".AdditionalConsumer") )
	}

	static std::vector<PtBin> GetAsPtBins(stringvector & sv)
	{
		std::vector<PtBin> bins;

		int i = 0;
		for (stringvector::iterator it = (sv.begin() + 1); it < sv.end(); it++)
		{
			int ilow = atoi(sv[i].c_str());
			int ihigh = atoi(sv[i + 1].c_str());

			bins.push_back(PtBin(ilow, ihigh));
			i++;
		}

		return bins;
	}

	VarCache<doublevector> m_recovertWeight;

	doublevector GetRecovertWeight()
	{
		RETURN_CACHED( m_recovertWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetSettingsRoot() + ".RecovertWeight") )
	}

	VarCache<stringvector> m_jetRespBins;

	stringvector GetCustomBins()
	{
		RETURN_CACHED( m_jetRespBins, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".CustomBins") )
	}
	// TODO: maybe cache this for better performance

	VarCache<stringvector> m_cuts;

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

typedef FilterBase<EventResult, ZJetPipelineSettings> ZJetFilterBase;

class RecoVertFilter: public ZJetFilterBase
{
public:
	RecoVertFilter() :
		ZJetFilterBase()
	{

	}

	virtual bool DoesEventPass(EventResult & event)
	{
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (event.GetRecoVerticesCount()
				>= GetPipelineSettings()->GetFilterRecoVertLow()
				&& event.GetRecoVerticesCount()
						<= GetPipelineSettings()->GetFilterRecoVertHigh());
	}

	virtual std::string GetFilterId()
	{
		return "recovert";
	}
};

class JetEtaFilter: public ZJetFilterBase
{
public:
	JetEtaFilter() :
		ZJetFilterBase()
	{

	}

	virtual bool DoesEventPass(EventResult & event)
	{
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (TMath::Abs(event.m_pData->jets[0]->Eta())
				>= GetPipelineSettings()->GetFilterJetEtaLow() && TMath::Abs(
				event.m_pData->jets[0]->Eta())
				<= GetPipelineSettings()->GetFilterJetEtaHigh());
	}

	virtual std::string GetFilterId()
	{
		return "jeteta";
	}
};

class SecondJetRatioFilter: public ZJetFilterBase
{
public:
	SecondJetRatioFilter() :
		ZJetFilterBase()
	{
	}

	virtual bool DoesEventPass(EventResult & event)
	{
		bool bPass = true;
		double fBinVal = event.GetCorrectedJetPt(1) / event.m_pData->Z->Pt();

		if (!(fBinVal >= GetPipelineSettings()->GetFilterSecondJetRatioLow()))
			bPass = false;

		if (!(fBinVal < GetPipelineSettings()->GetFilterSecondJetRatioHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "secondjetratio";
	}
};

class PtWindowFilter: public ZJetFilterBase
{
public:
	PtWindowFilter() :
		ZJetFilterBase(), m_binWith(ZPtBinning)
	{

	}

	virtual bool DoesEventPass(EventResult & event)
	{
		bool bPass = true;
		double fBinVal;
		if (m_binWith == ZPtBinning)
			fBinVal = event.m_pData->Z->Pt();
		else
			fBinVal = event.GetCorrectedJetPt(0);

		if (!(fBinVal >= GetPipelineSettings()->GetFilterPtBinLow()))
			bPass = false;

		if (!(fBinVal < GetPipelineSettings()->GetFilterPtBinHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "ptbin";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		std::stringstream s;

		if (bVerbose)
		{
			s << "Binned with ";
			if (m_binWith == ZPtBinning)
				s << " ZPt ";
			else
				s << " Jet1Pt ";

			s << " from " << GetPipelineSettings()->GetFilterPtBinHigh()
					<< " to " << GetPipelineSettings()->GetFilterPtBinLow();
		}
		else
		{
			s << "Pt" << std::setprecision(0)
					<< GetPipelineSettings()->GetFilterPtBinLow() << "to"
					<< std::setprecision(0)
					<< GetPipelineSettings()->GetFilterPtBinHigh();
		}
		return s.str();
	}

	enum BinWithEnum
	{
		ZPtBinning, Jet1PtBinning
	};
	BinWithEnum m_binWith;
};

class InCutFilter: public ZJetFilterBase
{
public:

	virtual bool DoesEventPass(EventResult & event)
	{
		unsigned long ignoredCut =
				GetPipelineSettings()->GetFilterInCutIgnored();
		// no section here is allowed to set to true again, just to false ! avoids coding errors
		return event.IsInCutWhenIgnoringCut(ignoredCut);
	}

	virtual std::string GetFilterId()
	{
		return "incut";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "InCut";
	}

};

class ZJetPipelineInitializer: public PipelineInitilizerBase<EventResult,
		ZJetPipelineSettings>
{
public:
	void InitPipeline(ZJetPipeline * pLine, ZJetPipelineSettings * pset);
};

}
