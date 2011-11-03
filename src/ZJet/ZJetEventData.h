#pragma once

#include <sstream>
#include "DataFormats/interface/Kappa.h"

#include <KappaTools/RootTools/FileInterface.h>

#include "ZJetPipelineSettings.h"
#include "Misc/SafeMap.h"

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

	// only use directly if there is no other way
	FileInterface * m_fi;

	typedef std::map<std::string, KDataPFJets *> PfMap;
	typedef PfMap::const_iterator PfMapIterator;

	typedef std::map<std::string, KLV *> GenJetMap;
	typedef PfMap::const_iterator GenJetMapIterator;

	typedef std::map<std::string, KDataJets *> JetMap;
	typedef PfMap::const_iterator JetMapIterator;

	//typedef std::map<std::string, K * > GenMap;
	PfMap m_pfJets;
	JetMap m_jets;
	GenJetMap m_genJets;

	// May return null, if no primary jet is available
	virtual KDataLV * GetPrimaryJet(ZJetPipelineSettings const& psettings) const
	{
		return GetJet(psettings, 0);
	}

	virtual unsigned int GetJetCount(ZJetPipelineSettings const& psettings) const
	{
		return GetJetCount(psettings, psettings.GetJetAlgorithm());
	}

	// May return null, if the jet is not available
	virtual unsigned int GetJetCount(ZJetPipelineSettings const& psettings, std::string algoName) const
	{
		if (psettings.IsPF(algoName))
		{
			return m_pfJets.at(algoName)->size();
		}
		else
		{
			return m_jets.at(algoName)->size();
		}
	}

	// return Calo or PF , depending on which jets we are looking at right now
	virtual KDataMET * GetMet(ZJetPipelineSettings const& psettings) const
	{
		return m_pfMet;
	}


	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index) const
	{
		return GetJet( psettings, index, psettings.GetJetAlgorithm());
	}

	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index, std::string algoName) const
	{
		if (psettings.IsPF(algoName))
		{
			KDataPFJets * pfJets =
			SafeMap<std::string, KDataPFJets *>::Get( algoName, m_pfJets );

			//KDataPFJets * pfJets = m_pfJets.at(algoName);

			if (pfJets->size() <= index)
				return NULL;

			return &pfJets->at(index);
		}
		else
		{
			assert ("we dont use this");
			KDataJets * jets = m_jets.at(algoName);

			if (jets->size() >= index)
				return NULL;

			return &jets->at(index);
		}
	}

	virtual std::string GetContent() const
	{

		std::stringstream s;

		s << "PF Jets collection:" << std::endl;
		for (PfMapIterator it = m_pfJets.begin(); it != m_pfJets.end(); ++it)
		{
			s << it->first << " count " << it->second->size() <<  std::endl;
		}

		return s.str();
	}

	KDataMuons * Muons;
	KEventMetadata * m_eventmetadata;

	KLumiMetadata * m_lumimetadata;

	KGenLumiMetadata * GetGenLumiMetadata() const
	{
		return (KGenLumiMetadata *) m_lumimetadata ;
	}

	KDataLumiMetadata * GetDataLumiMetadata() const
	{
		return (KDataLumiMetadata *) m_lumimetadata;
	}


	KGenEventMetadata * m_geneventmetadata;

	KVertexSummary * m_vertexSummary;
	KJetArea * m_jetArea;

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
