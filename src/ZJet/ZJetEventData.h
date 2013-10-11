#pragma once

#include <sstream>
#include <Kappa/DataFormats/interface/Kappa.h>
#include <KappaTools/RootTools/FileInterface2.h>
#include <Kappa/DataFormats/interface/KDebug.h>


#include "ZJetPipelineSettings.h"
#include "Misc/SafeMap.h"

#include "Pipeline/JetTools.h"

namespace CalibFW
{

/*
 * Event Data for the ZJet analyses
 */
class ZJetEventData
{

public:
	KDataPFJets* PF_jets; // = fi.Get<KDataPFJets>(names[0]);

	// contains
	KDataPFJets* m_primaryJetCollection;
	KDataMuons* m_muons;
	KDataPFMET* m_pfMet;
	KDataPFMET* m_pfMetL2L3;
	KDataPFMET* m_pfMetL2L3Res;
	KDataPFMET* m_pfMetChs;
	KDataPFMET* m_pfMetChsL2L3;
	KDataPFMET* m_pfMetChsL2L3Res;
	KGenParticles* m_particles;

	//typedef std::map<std::string, KDataPFJets *> PfMap;
	typedef std::map<std::string, KDataPFTaggedJets*> PfTaggedMap;
	typedef PfTaggedMap::const_iterator PfTaggedMapIterator;

	typedef std::map<std::string, KDataPFJets*> PfMap;
	typedef PfMap::const_iterator PfMapIterator;

	typedef std::map<std::string, KDataJets*> CaloMap;
	typedef CaloMap::const_iterator CaloMapIterator;

	typedef std::map<std::string, KDataLVs*> GenJetMap;
	typedef GenJetMap::const_iterator GenJetMapIterator;

	typedef std::map<std::string, KDataJets*> JetMap;
	typedef JetMap::const_iterator JetMapIterator;

	PfMap m_pfJets;
	PfTaggedMap m_pfTaggedJets;
	CaloMap m_caloJets;
	JetMap m_jets;
	GenJetMap m_genJets;

	typedef std::map<std::string, std::vector<KDataPFJet*>*> PfPointerMap;
	typedef PfPointerMap::const_iterator PfPointerMapIterator;
	mutable PfPointerMap m_pfPointerJets;


	virtual KDataLVs* GetGenJetCollection(std::string const& name) const
	{
		return SafeMap<std::string, KDataLVs*>::Get(name, m_genJets);
	}

	// May return null, if no primary jet is available
	virtual KDataLV* GetPrimaryJet(ZJetPipelineSettings const& psettings) const
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
		if (JetType::IsPF(algoName))
		{
			return SafeMap<std::string, KDataPFJets*>::Get(algoName, m_pfJets)->size();
		}
		if (JetType::IsGen(algoName))
		{
			return SafeMap<std::string, KDataLVs*>::Get(algoName, m_genJets)->size();
		}
		else
		{
			return SafeMap<std::string, KDataJets*>::Get(algoName, m_jets)->size();
		}
	}

	// return MET, depending on which correction level we are looking at right now
	virtual KDataPFMET* GetMet(ZJetPipelineSettings const& psettings) const
	{
		std::string corr = psettings.GetJetAlgorithm();
		//to do: implement MET for all algorithms while ensuring backwards-compatibility
		if (corr.find("GenJets") != std::string::npos || corr.find("Jets") == std::string::npos)
			corr = "";
		else
			corr = corr.substr(std::max(corr.find("Jets") + 4, corr.find("CHS") + 3), std::string::npos);


		if (std::string::npos != corr.find("CHS"))
			return m_pfMetChs;
		else
			return m_pfMet;

		CALIB_LOG_FATAL("The correction level \"" << corr << "\" for MET is unknown.");
		return NULL;
	}

	virtual KDataLV* GetJet(ZJetPipelineSettings const& psettings,
							unsigned int index) const
	{
		return GetJet(psettings, index, psettings.GetJetAlgorithm());
	}

	virtual KDataLV* GetJet(ZJetPipelineSettings const& psettings,
							unsigned int index, std::string algoName) const
	{
		if (JetType::IsPF(algoName))
		{
			std::vector<KDataPFJet*>* pfJets =
				SafeMap<std::string, std::vector<KDataPFJet*> *>::Get(algoName, m_pfPointerJets);

			if (pfJets->size() <= index)
				return NULL;
			return pfJets->at(index);
		}
		else if (JetType::IsGen(algoName))
		{
			KDataLVs* genJets =
				SafeMap<std::string, KDataLVs*>::Get(algoName, m_genJets);

			if (genJets->size() <= index)
				return NULL;
			return &genJets->at(index);
		}
		else if (JetType::IsCalo(algoName))
		{
			KDataJets* caloJets =
				SafeMap<std::string, KDataJets*>::Get(algoName, m_caloJets);

			if (caloJets->size() <= index)
				return NULL;
			return &caloJets->at(index);
		}
		else
		{
			CALIB_LOG_FATAL("This jet type is not implemented." << algoName)
			KDataJets* jets = m_jets.at(algoName);

			if (jets->size() >= index)
				return NULL;
			return &jets->at(index);
		}
	}

	virtual std::string GetContent(ZJetPipelineSettings const& settings) const
	{
		std::stringstream s;
		s << "EventMetadada " << std::endl;
		s << "Run: " << m_eventmetadata->nRun;
		s << " LumiSec: " << m_eventmetadata->nLumi;
		s << " EventNum: " << m_eventmetadata->nEvent << std::endl;

		if (settings.IsData())
		{
			s << "KDataLumiMetadata: " << std::endl;
			s << "CurLumi: " << GetDataLumiMetadata()->getLumi() << std::endl;
		}

		s << "PF Jets collection:" << std::endl;
		for (PfMapIterator it = m_pfJets.begin(); it != m_pfJets.end(); ++it)
		{
			s << it->first << " count " << it->second->size() <<  std::endl;

			KDataPFJets* pfJets = it->second;
			for (unsigned int i = 0; i < pfJets->size(); ++ i)
			{
				s << "Jet " << i << ":" << std::endl << pfJets->at(i) << std::endl;
			}
		}

		s << "Gen Jets collection:" << std::endl;
		for (GenJetMapIterator it = m_genJets.begin(); it != m_genJets.end(); ++it)
		{
			s << it->first << " count " << it->second->size() <<  std::endl;
			KDataLVs* genJets = it->second;
			for (unsigned int i = 0; i < genJets->size(); ++ i)
			{
				s << "Jet " << i << ":" << std::endl << genJets->at(i) << std::endl;
			}
		}

		return s.str();
	}

	KDataMuons* Muons;
	KGenEventMetadata* m_geneventmetadata;
	KEventMetadata* m_eventmetadata;
	KLumiMetadata* m_lumimetadata;
	KGenLumiMetadata* m_genlumimetadata;
	KFilterMetadata* m_filtermetadata;
	KFilterSummary* m_filter;
	KVertexSummary* m_vertexSummary;
	KDataBeamSpot* m_beamSpot;
	KJetArea* m_jetArea;
	int m_pthatbin;

	KGenLumiMetadata* GetGenLumiMetadata() const
	{
		return (KGenLumiMetadata*) m_lumimetadata;
	}

	KDataLumiMetadata* GetDataLumiMetadata() const
	{
		return (KDataLumiMetadata*) m_lumimetadata;
	}

};
}
