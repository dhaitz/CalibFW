#pragma once

#include <boost/algorithm/string.hpp>

#include "RootTools/RootIncludes.h"
#include "Misc/PtBin.h"
#include "Pipeline/PipelineSettings.h"

namespace CalibFW
{

/** Global settings for all pipelines

This class contains settings which are (by design) the same for all
pipelines.
*/
class ZJetGlobalSettings
{
public:
	doublevector const& GetPuReweighting() const
	{
		return m_recovertWeight;
	}

	doublevector const& Get2ndJetReweighting() const
	{
		return m_2ndJetWeight;
	}

	IMPL_PROPERTY(bool, EnablePuReweighting)
	IMPL_PROPERTY(bool, Enable2ndJetReweighting)
	IMPL_PROPERTY(bool, MuonID2011)
	IMPL_PROPERTY(long long, SkipEvents)
	IMPL_PROPERTY(long long, EventCount)
	IMPL_PROPERTY(std::string, JsonFile)
	IMPL_PROPERTY(InputTypeEnum, InputType)

	doublevector m_metphi;

	doublevector m_recovertWeight;
	doublevector m_2ndJetWeight;
};


class ZJetPipelineSettings
{
public:

	ZJetPipelineSettings()
	{
		m_globalSettings = NULL;
		SetSettingsRoot("default");
		//SetEnableReweighting(false);
	}

	enum WriteEventsEnum
	{
		NoEvents, OnlyInCutEvents, AllEvents
	};

	std::string ToString() const
	{
		return this->GetRootFileFolder();
	}

	ZJetGlobalSettings const* Global() const
	{
		assert(m_globalSettings != NULL);
		return m_globalSettings;
	}
	ZJetGlobalSettings const* m_globalSettings;

	IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)
	IMPL_PROPERTY(std::string, SettingsRoot)
	IMPL_PROPERTY(unsigned long, OverallNumberOfProcessedEvents)

	IMPL_SETTING(bool, EnableCaloMatching)
	IMPL_SETTING(bool, EnableReweighting)

	// Filter Settings
	IMPL_SETTING(double, FilterPtBinLow)
	IMPL_SETTING(double, FilterPtBinHigh)
	IMPL_SETTING(double, FilterJetPtLow)
	IMPL_SETTING(double, FilterJetPtHigh)
	IMPL_SETTING(unsigned int, FilterNpvLow)
	IMPL_SETTING(unsigned int, FilterNpvHigh)
	IMPL_SETTING(double, FilterJetEtaLow)
	IMPL_SETTING(double, FilterJetEtaHigh)

	IMPL_SETTING(unsigned int, FilterRunRangeLow)
	IMPL_SETTING(unsigned int, FilterRunRangeHigh)

	IMPL_SETTING(unsigned int, FilterRunRangeLumiLow)
	IMPL_SETTING(unsigned int, FilterRunRangeLumiHigh)
	IMPL_SETTING(unsigned int, FilterRunRangeEventLow)
	IMPL_SETTING(unsigned int, FilterRunRangeEventHigh)

	IMPL_SETTING(double, FilterResponseLow)
	IMPL_SETTING(double, FilterResponseHigh)
	IMPL_SETTING(double, FilterDeltaEtaLow)
	IMPL_SETTING(double, FilterDeltaEtaHigh)
	IMPL_SETTING(double, FilterSecondJetRatioLow)
	IMPL_SETTING(double, FilterSecondJetRatioHigh)
	IMPL_SETTING(unsigned long, FilterInCutIgnored)

	IMPL_SETTING(bool, Filter2ndJetPtCutSet)
	IMPL_SETTING(bool, FilterDeltaPhiCutSet)

	IMPL_SETTING(std::string, JetAlgorithm)

	std::string GetCorrespondingGenJetAlgorithm()
	{
		std::string s = GetJetAlgorithm();
		boost::algorithm::replace_first(s, "PFJets", "GenJets");
		boost::algorithm::replace_first(s, "CaloJets", "GenJets");

		// remove any correction, we dont need that on gen jets. JEAH !
		boost::algorithm::replace_first(s, "L1", "");
		boost::algorithm::replace_first(s, "L2", "");
		boost::algorithm::replace_first(s, "L3", "");

		return s;
	}

	IMPL_SETTING(std::string, RootFileFolder)
	IMPL_SETTING(std::string, SecondLevelFolderTemplate)

	// only level 1 runs directly on data
	IMPL_SETTING(unsigned int, Level)

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
	IMPL_SETTING(bool, GenCuts)
	IMPL_SETTING(double, CutMuonPt)
	IMPL_SETTING(double, CutZMassWindow)
	IMPL_SETTING(double, CutMuEta)
	IMPL_SETTING(double, CutZPt)
	IMPL_SETTING(double, CutMuonEta)
	IMPL_SETTING(double, CutLeadingJetEta)
	IMPL_SETTING(double, CutSecondLeadingToZPt)
	IMPL_SETTING(double, CutSecondLeadingToZPtJet2Threshold)

	IMPL_SETTING(double, CutDeltaEtaLow)
	IMPL_SETTING(double, CutDeltaEtaHigh)
	IMPL_SETTING(double, CutSecondLeadingToZPtRegionLow)
	IMPL_SETTING(double, CutSecondLeadingToZPtRegionHigh)

	IMPL_SETTING(double, CutSecondLeadingToZPtDeltaR)
	IMPL_SETTING(double, CutBack2Back)
	IMPL_SETTING(double, CutJetPt)
	IMPL_SETTING(double, CutLeadingJetPt)
	IMPL_SETTING(double, CutSecondJetPt)
	IMPL_SETTING(double, CutSecondJetEta)
	IMPL_SETTING(double, CutRapidityGap)
	IMPL_SETTING(double, CutInvariantMass)
	IMPL_SETTING(double, CutThirdJetPt)

	// if a cut is enabled, the bit corresponding to the cut id it set in this integer
	IMPL_PROPERTY(unsigned long, CutEnabledBitmask)
	IMPL_PROPERTY(TFile *, RootOutFile)

	bool IsMC() const
	{
		return (GetInputType() == McInput);
	}

	bool IsData() const
	{
		return (GetInputType() == DataInput);
	}

	bool IsGen() const
	{
		return (GetInputType() == GenInput);
	}

	InputTypeEnum GetInputType() const
	{
		std::string sInp = GetPropTree()->get<std::string>("InputType");

		InputTypeEnum inp = McInput;

		if (sInp == "data")
			inp = DataInput;
		else if (sInp == "mc")
			inp = McInput;
		else if (sInp == "gen")
			inp = GenInput;
		else
			CALIB_LOG_FATAL("Input type " + sInp + " not supported.")

		return inp;
	}

	VarCache<stringvector> m_filter;

	stringvector GetFilter() const
	{
		RETURN_CACHED(m_filter, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".Filter"))
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

	VarCache<doublevector> m_2ndJetWeight;

	doublevector Get2ndJetWeight()
	{
		RETURN_CACHED(m_2ndJetWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetSettingsRoot() + ".2ndJetWeight"))
	}

	VarCache<doublevector> m_recovertWeight;

	doublevector GetRecovertWeight()
	{
		RETURN_CACHED(m_recovertWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetSettingsRoot() + ".RecovertWeight"))
	}

	VarCache<stringvector> m_jetRespBins;

	stringvector GetCustomBins()
	{
		RETURN_CACHED(m_jetRespBins, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".CustomBins"))
	}
	//TODO: maybe cache this for better performance

	VarCache<stringvector> m_cuts;

	stringvector GetCuts() const
	{
		RETURN_CACHED(m_cuts, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".Cuts"))
	}

	VarCache<stringvector> m_metaDataProducers;
	stringvector GetMetaDataProducers() const
	{
		RETURN_CACHED(m_metaDataProducers, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".MetaDataProducers"))
	}
/*
	void AddFilter(std::string sFilterId)
	{
		m_filter.push_back(sFilterId);
	}

	stringvector m_filter;
*/

};

}
