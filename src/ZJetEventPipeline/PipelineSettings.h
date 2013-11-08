#pragma once

#include <boost/algorithm/string.hpp>
#include "EventPipeline/PipelineSettings.h"
#include "RootTools/RootIncludes.h"

namespace Artus
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

	doublevector const& GetSampleReweighting() const
	{
		return m_sampleWeight;
	}

	IMPL_PROPERTY(bool, EnablePuReweighting)
	IMPL_PROPERTY(bool, Enable2ndJetReweighting)
	IMPL_PROPERTY(bool, EnableSampleReweighting)
	IMPL_PROPERTY(bool, EnableLumiReweighting)
	IMPL_PROPERTY(bool, EnableMetPhiCorrection)
	IMPL_PROPERTY(bool, MuonID2011)
	IMPL_PROPERTY(double, HcalCorrection)
	IMPL_PROPERTY(double, XSection)
	IMPL_PROPERTY(long long, NEvents)
	IMPL_PROPERTY(long long, SkipEvents)
	IMPL_PROPERTY(long long, EventCount)
	IMPL_PROPERTY(std::string, JsonFile)
	IMPL_PROPERTY(std::string, PileupWeights)
	IMPL_PROPERTY(InputTypeEnum, InputType)

	doublevector m_metphi;

	doublevector m_recovertWeight;
	doublevector m_2ndJetWeight;
	doublevector m_sampleWeight;
};


class ZJetPipelineSettings
{
public:

	ZJetPipelineSettings()
	{
		m_globalSettings = NULL;
		SetName("default");
	}

	ZJetGlobalSettings const* Global() const
	{
		assert(m_globalSettings != NULL);
		return m_globalSettings;
	}
	ZJetGlobalSettings const* m_globalSettings;

	IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)
	IMPL_PROPERTY(std::string, Name)
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
	IMPL_SETTING(unsigned int, FilterFlavour)

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

	IMPL_SETTING(std::string, QuantitiesString)


	// only level 1 runs directly on data
	IMPL_SETTING(unsigned int, Level)

	IMPL_SETTING(std::string, WriteEvents)


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
	IMPL_PROPERTY(TFile*, RootOutFile)

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
			LOG_FATAL("Input type " + sInp + " not supported.");

		return inp;
	}

	TH1D m_puweights;

	VarCache<stringvector> m_filter;
	stringvector GetFilter() const
	{
		RETURN_CACHED(m_filter, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".Filter"))
	}

	VarCache<doublevector> m_2ndJetWeight;
	doublevector Get2ndJetWeight()
	{
		RETURN_CACHED(m_2ndJetWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), "Pipelines." + GetName() + ".2ndJetWeight"))
	}

	VarCache<doublevector> m_recovertWeight;
	doublevector GetRecovertWeight()
	{
		RETURN_CACHED(m_recovertWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), "Pipelines." + GetName() + ".RecovertWeight"))
	}

	VarCache<stringvector> m_jetRespBins;
	stringvector GetCustomBins()
	{
		RETURN_CACHED(m_jetRespBins, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".CustomBins"))
	}

	VarCache<stringvector> m_cuts;
	stringvector GetCuts() const
	{
		RETURN_CACHED(m_cuts, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".Cuts"))
	}

	VarCache<stringvector> m_producers;
	stringvector GetMetaDataProducers() const
	{
		RETURN_CACHED(m_producers, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".MetaDataProducers"))
	}

	VarCache<stringvector> m_consumer;
	stringvector GetConsumer() const
	{
		RETURN_CACHED(m_consumer, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".Consumer"))
	}

	VarCache<stringvector> m_quantities;
	stringvector GetQuantities() const
	{
		RETURN_CACHED(m_quantities, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".QuantitiesVector"))
	}

};

}
