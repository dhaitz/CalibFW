
#pragma once

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Pipeline/EventPipeline.h"
#include "ZJetPipelineSettings.h"
#include "ZJetEventData.h"


#include "KappaTools/RootTools/HLTTools.h"


namespace CalibFW
{

class ZJetMetaData: public CalibFW::EventMetaDataBase
{
public:
	ZJetMetaData();

	~ZJetMetaData(){
		ClearContent();
	}

	void ClearContent();

	virtual std::string GetContent() const;

	// cutPassed is true, if the event was not dropped by the cut
	void SetCutResult(long cutId, bool cutPassed)
	{
		// ensure the bit is removed if it was set before
		this->SetCutBitmask(((!cutPassed) * cutId) | (GetCutBitmask()
				& (~cutId)));
	}

	KDataLV * GetValidPrimaryJet( ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData) const
	{
		return GetValidJet( psettings, evtData, 0);
	}

	KDataLV * GetValidJet( ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index) const
	{
		return GetValidJet( psettings,
				evtData,
				index,
				psettings.GetJetAlgorithm()) ;
	}

	KEventMetadata * GetKappaMetaData( ZJetEventData const& evtData, ZJetPipelineSettings const& psettings )
			const;

	bool IsMetaJetAlgo ( std::string const& algoName ) const
	{
		return ( m_validPFJets.find( algoName ) != m_validPFJets.end() );
	}

	KDataLV * GetValidJet( ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index,
			std::string algoName) const;

	void AddValidJet( KDataPFJet const& jet, std::string algoName)
	{
		m_validPFJets[algoName].push_back( jet );
	}


	std::vector< KDataPFJet > & GetPFValidJetCollection( std::string const& algoName )
	{
        return m_validPFJets.at( algoName );
	}


	unsigned int GetValidJetCount(
			ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData) const
	{
		return GetValidJetCount( psettings, evtData, psettings.GetJetAlgorithm());
	}

	unsigned int GetValidJetCount(
			ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData, std::string algoName) const;

	unsigned int GetInvalidJetCount(ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData) const
	{
		return this->m_listInvalidJets[ psettings.GetJetAlgorithm() ].size();
	}

	bool HasValidZ() const
	{
		return this->GetValidZ();
	}

	bool HasValidJet(ZJetPipelineSettings const& psettings , ZJetEventData const& evtData) const
	{
		return GetValidJetCount(psettings, evtData ) > 0;
	}

	bool IsAllCutsPassed() const
	{
		return (this->GetCutBitmask() == 0);
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

    IMPL_PROPERTY_READONLY(long, CutBitmask)

    IMPL_PROPERTY(bool, ValidZ)

    IMPL_PROPERTY(KDataLV, Z)

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

	void InitMetaJetCollection( std::string algoname)
	{
		m_validPFJets[ algoname ] = std::vector< KDataPFJet >();
	}

	void SortJetCollections();

	KDataMuons m_listValidMuons;
	KDataMuons m_listInvalidMuons;


	typedef  std::map< std::string , std::vector<unsigned int> >  JetMapping ;
	typedef  JetMapping::iterator  JetMappingIterator ;

	mutable JetMapping m_listValidJets;
	mutable JetMapping m_listInvalidJets;

	// create a complete copy of the jet collections ??
	typedef std::map < std::string, std::vector<KDataPFJet> > MetaPFJetContainer;
	mutable MetaPFJetContainer m_validPFJets;

    // Jet Matching Result
	typedef std::map< std::string , std::vector<int> > MatchingResults;
	MatchingResults m_matchingResults;

	HLTTools * m_hltInfo;
};

}
