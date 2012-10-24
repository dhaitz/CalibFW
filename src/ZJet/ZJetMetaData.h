#pragma once

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Pipeline/EventPipeline.h"
#include "ZJetPipelineSettings.h"
#include "ZJetEventData.h"


#include "KappaTools/RootTools/HLTTools.h"


namespace CalibFW
{


class ZJetLocalMetaData : public boost::noncopyable
{
public:
	ZJetLocalMetaData(): m_CutBitmask(0) {}

	// cutPassed is true, if the event was not dropped by the cut
	void SetCutResult(long cutId, bool cutPassed)
	{
		// ensure the bit is removed if it was set before
		this->SetCutBitmask((!cutPassed * cutId) | (GetCutBitmask() & ~cutId));
	}

	void SetCutBitmask(long val)
	{
		m_CutBitmask = val;
	}

	long GetCutBitmask() const
	{
		return m_CutBitmask;
	}

private:
	long m_CutBitmask;

};


class ZJetMetaData : public boost::noncopyable
{
public:
	ZJetMetaData();

	~ZJetMetaData(){
		ClearContent();
	}

	typedef ZJetLocalMetaData LocalMetaDataType;

	void ClearContent();

	void SetLocalMetaData(LocalMetaDataType * pipelineMetaData)
	{
		assert(pipelineMetaData != NULL);
		m_pipelineMetaData = pipelineMetaData;
	}

	// holds pipeline specific metadata
	LocalMetaDataType * GetLocalMetaData() const
	{
		assert(m_pipelineMetaData != NULL);
		return m_pipelineMetaData;
	}

	std::string GetContent() const;


	KEventMetadata * GetKappaMetaData(ZJetEventData const& evtData,
			ZJetPipelineSettings const& psettings) const;

	bool IsMetaJetAlgo(std::string const& algoName) const
	{
		return (m_validPFJets.find(algoName) != m_validPFJets.end());
	}


	void AddValidJet(KDataPFJet const& jet, std::string algoName)
	{
		m_validPFJets[algoName].push_back(jet);
	}


	std::vector<KDataPFJet> & GetPFValidJetCollection(std::string const& algoName)
	{
		return m_validPFJets.at(algoName);
	}

	// ## ACCESS TO VALID JETS
	unsigned int GetValidJetCount(
			ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData, std::string algoName) const;

	unsigned int GetValidJetCount(
			ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData) const
	{
		return GetValidJetCount(psettings, evtData, psettings.GetJetAlgorithm());
	}

	KDataLV * GetValidJet(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index,
			std::string algoName) const;

	KDataLV * GetValidPrimaryJet(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData) const
	{
		return GetValidJet(psettings, evtData, 0);
	}

	KDataLV * GetValidJet(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index) const
	{
		return GetValidJet(psettings,
				evtData,
				index,
				psettings.GetJetAlgorithm());
	}

	// ## ACCESS TO INVALID JETS

	KDataLV * GetInvalidJet(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index,
			std::string algoName) const;

	KDataLV * GetInvalidJet(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index) const
	{
		return GetInvalidJet(psettings,
				evtData,
				index,
				psettings.GetJetAlgorithm());
	}

	unsigned int GetInvalidJetCount(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData, std::string algoName) const
	{
		return this->m_listInvalidJets[algoName].size();
	}

	unsigned int GetInvalidJetCount(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData) const
	{
		return GetInvalidJetCount(psettings, evtData, psettings.GetJetAlgorithm());
	}



	bool HasValidZ() const
	{
		return this->GetValidZ();
	}

	bool HasValidJet(ZJetPipelineSettings const& psettings, ZJetEventData const& evtData) const
	{
		return GetValidJetCount(psettings, evtData) > 0;
	}

	bool IsAllCutsPassed() const
	{
		return (this->GetCutBitmask() == 0);
	}

	long GetCutBitmask() const
	{
		return GetLocalMetaData()->GetCutBitmask();
	}


	bool IsCutPassed(long cutId) const
	{
		return (this->GetCutBitmask() & cutId) == 0;
	}

	double GetBalance(KDataLV * jet) const
	{
		return jet->p4.Pt() / this->GetRefZ().p4.Pt();
	}

	double GetBalanceBetweenJets(KDataLV * jet1, KDataLV * jet_ref) const
	{
		return jet1->p4.Pt() / jet_ref->p4.Pt();
	}

	double GetMPF(KDataLV * met) const;

	double GetTwoJetBalance(KDataLV * jet1, KDataLV * jet2) const;

	double GetZeppenfeld(KDataLV * jet1, KDataLV * jet2, KDataLV * jet3) const;

	double GetJetGenZBalance(KDataLV * genjet) const
	{
		return genjet->p4.Pt() / this->GetRefGenZ().p4.Pt();
	}

	double GetJetPartonBalance(KDataLV * genjet) const
	{
		return genjet->p4.Pt() / this->GetRefParton().p4.Pt();
	}

	double GetZBalance() const
	{
		return this->GetRefZ().p4.Pt() / this->GetRefGenZ().p4.Pt();
	}

	double GetPartonBalance() const
	{
		return this->GetRefParton().p4.Pt() / this->GetRefGenZ().p4.Pt();
	}

	IMPL_PROPERTY(bool, ValidZ)
	IMPL_PROPERTY(bool, ValidGenZ)
	IMPL_PROPERTY(bool, ValidParton)

	IMPL_PROPERTY(KDataLV, Z)
	IMPL_PROPERTY(KParton, GenZ)
	IMPL_PROPERTY(KParton, Parton)
	IMPL_PROPERTY(KParton, LeadingParton)
	IMPL_PROPERTY(double, BalanceQuality)

	IMPL_PROPERTY(double, Weight)

	IMPL_PROPERTY(std::string, SelectedHlt)

	KDataMuons const& GetValidMuons() const
	{
		return m_listValidMuons;
	}

	KDataMuons const& GetInvalidMuons() const
	{
		return m_listInvalidMuons;
	}

	void InitMetaJetCollection(std::string algoname)
	{
		m_validPFJets[algoname] = std::vector<KDataPFJet>();
	}

	void SortJetCollections();

	KDataMuons m_listValidMuons;
	KDataMuons m_listInvalidMuons;

	KPartons m_genPartons;
	KPartons m_genMuons;
	KPartons m_genZs;

	typedef boost::ptr_map<std::string, std::vector<unsigned int> > JetMapping;
	typedef JetMapping::iterator JetMappingIterator;

	mutable JetMapping m_listValidJets;
	mutable JetMapping m_listInvalidJets;

	// create a complete copy of the jet collections ??
	typedef boost::ptr_map<std::string, std::vector<KDataPFJet> > MetaPFJetContainer;
	mutable MetaPFJetContainer m_validPFJets;

	// Jet Matching Result
	typedef boost::ptr_map<std::string, std::vector<int> > MatchingResults;
	MatchingResults m_matchingResults;

	HLTTools * m_hltInfo;

	// holds pipeline specific metadata of the current pipeline
	LocalMetaDataType * m_pipelineMetaData;
};

}
