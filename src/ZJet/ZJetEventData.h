#pragma once

#include <sstream>
#include "DataFormats/interface/Kappa.h"

#include <KappaTools/RootTools/FileInterface.h>
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
	KDataPFJets * PF_jets;// = fi.Get<KDataPFJets>(names[0]);

	// contains
	KDataPFJets * m_primaryJetCollection;
	KDataMuons * m_muons;
	KDataPFMET * m_pfMet;
	KDataPFMET * m_pfMetL1;
	KDataPFMET * m_pfMetL2L3;
	KDataPFMET * m_pfMetL1L2L3;
	KDataPFMET * m_pfMetL2L3Res;
	KDataPFMET * m_pfMetChs;
	KDataPFMET * m_pfMetChsL1;
	KDataPFMET * m_pfMetChsL2L3;
	KDataPFMET * m_pfMetChsL1L2L3;
	KDataPFMET * m_pfMetChsL2L3Res;
	//KDataPFMET * m_pfMetChsL2L3phi;
	//KDataPFMET * m_pfMetChsL2L3Resphi;


	// only use directly if there is no other way
	//FileInterface * m_fi;

	typedef std::map<std::string, KDataPFJets *> PfMap;
	typedef PfMap::const_iterator PfMapIterator;

	typedef std::map<std::string, KDataLVs *> GenJetMap;
	typedef GenJetMap::const_iterator GenJetMapIterator;

	typedef std::map<std::string, KDataJets *> JetMap;
	typedef JetMap::const_iterator JetMapIterator;

	//typedef std::map<std::string, K * > GenMap;
	PfMap m_pfJets;
	JetMap m_jets;
	GenJetMap m_genJets;

	virtual KDataLVs * GetGenJetCollection (std::string const& name) const
	{
		return SafeMap<std::string, KDataLVs *>::Get(name, m_genJets);
	}

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
		if (JetType::IsPF(algoName))
		{
			return SafeMap<std::string, KDataPFJets *>::Get(algoName, m_pfJets)->size();
		}
		if (JetType::IsGen(algoName))
		{
			return SafeMap<std::string, KDataLVs *>::Get(algoName, m_genJets)->size();
		}
		else
		{
			return SafeMap<std::string, KDataJets *>::Get(algoName, m_jets)->size();
		}
	}

	// return MET, depending on which correction level we are looking at right now
	virtual KDataPFMET * GetMet(ZJetPipelineSettings const& psettings) const
	{
		std::string corr = psettings.GetJetAlgorithm().substr(9); //remove the "AKxPFJets"

		//to do: implement AK7 MET and phi-corrected MET while ensuring backwards-compatibility

		if (corr == "" || corr == "L1")
			return m_pfMet;
		else if (corr == "L1L2" || corr == "L1L2L3") // || (corr == "L1L2L3Res" && psettings.IsMC())
			return m_pfMetL2L3;
		else if (corr == "L1L2L3Res") // && psettings.IsData())
			return m_pfMetL2L3Res;

		else if (corr == "CHS" || corr == "CHSL1")
			return m_pfMetChs;
		else if (corr == "CHSL1L2" || corr == "CHSL1L2L3")
			return m_pfMetChsL2L3;
		else if (corr == "CHSL1L2L3Res")
			return m_pfMetChsL2L3Res;

		CALIB_LOG_FATAL("The correction level \"" << corr << "\" for MET is unknown.");
		return NULL;
	}


	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index) const
	{
		return GetJet(psettings, index, psettings.GetJetAlgorithm());
	}

	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index, std::string algoName) const
	{
		if (JetType::IsPF(algoName))
		{
			KDataPFJets * pfJets =
				SafeMap<std::string, KDataPFJets *>::Get(algoName, m_pfJets);

			if (pfJets->size() <= index)
				return NULL;

			return &pfJets->at(index);
		}
		else if (JetType::IsGen(algoName))
		{
			KDataLVs * genJets =
				SafeMap<std::string, KDataLVs *>::Get(algoName, m_genJets);

			if (genJets->size() <= index)
				return NULL;

			return &genJets->at(index);
		}
		else
		{
			CALIB_LOG_FATAL("not implmented")
			KDataJets * jets = m_jets.at(algoName);

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

			KDataPFJets * pfJets = it->second;
			for (unsigned int i = 0; i < pfJets->size(); ++ i)
			{
				s << "Jet " << i << ":" << std::endl << pfJets->at(i) << std::endl; 
			}
		}

		s << "Gen Jets collection:" << std::endl;
		for (GenJetMapIterator it = m_genJets.begin(); it != m_genJets.end(); ++it)
		{
			s << it->first << " count " << it->second->size() <<  std::endl;
			
			KDataLVs * genJets = it->second;
			for (unsigned int i = 0; i < genJets->size(); ++ i)
			{
				s << "Jet " << i << ":" << std::endl << genJets->at(i) << std::endl; 
			}
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

};
}
