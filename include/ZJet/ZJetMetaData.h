
#pragma once

#include "../EventPipeline.h"
#include "ZJetPipelineSettings.h"
#include "ZJetEventData.h"

namespace CalibFW
{


class ZJetMetaData: public CalibFW::EventMetaDataBase
{
public:
	ZJetMetaData()
	{
		SetCutBitmask(0);
		SetWeight(1.0f);
		SetValidZ(false);
	}

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
		assert( GetValidJetCount(psettings) > index );

		KDataLV * j = evtData.GetJet( psettings, m_listValidJets [ psettings.GetJetAlgorithm() ].at(index) );
		assert( j != NULL);

		return j;
	}

	unsigned int GetValidJetCount(ZJetPipelineSettings const& psettings ) const
	{
		return this->m_listValidJets[ psettings.GetJetAlgorithm() ].size();
	}

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
		return (this->m_listValidJets[ psettings.GetJetAlgorithm() ].size() > 0);
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

	double GetMPF(KDataLV * met) const
	{
		double scalPtEt = 	GetRefZ().p4.Px() 	* met->p4.Px() +
							GetRefZ().p4.Py() 	* met->p4.Py();

		double scalPtSq = 	GetRefZ().p4.Px() 	* 	GetRefZ().p4.Px() +
							GetRefZ().p4.Py()	*	GetRefZ().p4.Py();

		return 1.0f + (scalPtEt /scalPtSq);
	}

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
};


}
