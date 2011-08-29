
#pragma once


#include "DataFormats/interface/Kappa.h"

#include "ZJetPipelineSettings.h"

namespace CalibFW
{

class ZJetEventData
{

public:
	KDataPFJets * PF_jets;// = fi.Get<KDataPFJets>(names[0]);

	// contains
	KDataPFJets * m_primaryJetCollection;
	KDataMuons * m_muons;
	KDataPFMET * m_pfMet;

	typedef std::map<std::string, KDataPFJets *> PfMap;
	typedef std::map<std::string, KDataJets *> JetMap;
	//typedef std::map<std::string, K * > GenMap;
	PfMap m_pfJets;
	JetMap m_jets;

	// May return null, if no primary jet is available
	virtual KDataLV * GetPrimaryJet(ZJetPipelineSettings const& psettings) const
	{
		return GetJet(psettings, 0);
	}

	// May return null, if the jet is not available
	virtual unsigned int GetJetCount(ZJetPipelineSettings const& psettings) const
	{
		if (psettings.IsPF())
		{
			return m_pfJets.at(psettings.GetJetAlgorithm())->size();
		}
		else
		{
			return m_jets.at(psettings.GetJetAlgorithm())->size();
		}
	}

	// return Calo or PF , depending on which jets we are looking at right now
	virtual KDataMET * GetMet( ZJetPipelineSettings const& psettings ) const
		{
			return m_pfMet;
		}

	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index) const
	{
		if (psettings.IsPF())
		{
			KDataPFJets * pfJets = m_pfJets.at(psettings.GetJetAlgorithm());

			if (pfJets->size() <= index)
				return NULL;

			return &pfJets->at(index);
		}
		else
		{
			assert ("we dont use this");
			KDataJets * jets = m_jets.at(psettings.GetJetAlgorithm());

			if (jets->size() >= index)
				return NULL;

			return &jets->at(index);
		}
	}

	KDataMuons * Muons;
	KEventMetadata * m_eventmetadata;
	KGenEventMetadata * m_geneventmetadata;

	KVertexSummary * m_primaryVertex;

	// convinience functions
	//double GetPrimaryJetPt() const { return PF_Jets->at(0).Pt(); }
	//KDataPFJet const& GetPrimaryJet() const { return PF_Jets->at(0); }

	/*
	 template <class TJetType>
	 std::vector<TJetType> * GetJets(std::string jetAlgo) const
	 {
	 return m_fi->Get< std::vector<TJetType> >( jetAlgo );
	 }*/
	/*
	 KDataLV GetPrimaryJet( std::string sJetAlgo)
	 {
	 return m_fi->Get<KDataLVs>(sJetAlgo)->at(0);
	 }
	 */
private:

};
}
