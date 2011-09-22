
#pragma once

#include <boost/ptr_container/ptr_map.hpp>

#include "../EventPipeline.h"
#include "ZJetPipelineSettings.h"
#include "ZJetEventData.h"




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

	virtual std::string GetContent();

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


	bool IsMetaJetAlgo ( std::string const& algoName ) const
	{
		return ( m_validPFJets.find( algoName ) != m_validPFJets.end() );
	}

	KDataLV * GetValidJet( ZJetPipelineSettings const& psettings,
			ZJetEventData const& evtData,
			unsigned int index,
			std::string algoName) const;

	void AddValidJet( KDataPFJet const& jet, std::string algoName)
	{/*
		if (m_validPFJets.find(algoName ) == m_validPFJets.end())
		{
			m_validPFJets[algoName] = new std::vector<KDataPFJet>();
		}*/

		m_validPFJets[algoName].push_back( jet );
	}

	unsigned int GetValidJetCount(ZJetPipelineSettings const& psettings ) const
	{
		return GetValidJetCount( psettings, psettings.GetJetAlgorithm());
	}

	unsigned int GetValidJetCount(ZJetPipelineSettings const& psettings,
			std::string algoName) const;

	unsigned int GetInvalidJetCount(ZJetPipelineSettings const& psettings) const
	{
		return this->m_listInvalidJets[ psettings.GetJetAlgorithm() ].size();
	}

	bool HasValidZ() const
	{
		return this->GetValidZ();
	}

	bool HasValidJet(ZJetPipelineSettings const& psettings) const
	{
		return GetValidJetCount(psettings) > 0;
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

	double GetMPF(KDataLV * met) const;

IMPL_PROPERTY_READONLY(long, CutBitmask)

IMPL_PROPERTY(bool, ValidZ)

IMPL_PROPERTY(KDataLV, Z)

IMPL_PROPERTY(double, Weight)

	KDataMuons const& GetValidMuons() const
	{
		return m_listValidMuons;
	}

	KDataMuons const& GetInvalidMuons() const
	{
		return m_listInvalidMuons;
	}
/*
	std::list<unsigned int> const& GetValidJets() const
	{
		return m_listValidJets;
	}
*/
	KDataMuons m_listValidMuons;
	KDataMuons m_listInvalidMuons;


	typedef  std::map< std::string , std::vector<unsigned int> >  JetMapping ;
	typedef typename JetMapping::iterator  JetMappingIterator ;

	mutable JetMapping m_listValidJets;
	mutable JetMapping m_listInvalidJets;



	// create a complete copy of the jet collections ??
	typedef typename std::map < std::string, std::vector<KDataPFJet>> MetaPFJetContainer;
	mutable MetaPFJetContainer m_validPFJets;


	std::vector< KDataLV *> m_primaryJets;
};


}