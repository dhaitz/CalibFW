#include <sstream>
#include <algorithm>
#include <cmath>

#include "ZJetMetaData.h"
#include "Pipeline/JetTools.h"

namespace CalibFW
{

ZJetMetaData::ZJetMetaData()
{
	SetWeight(1.0f);
	SetValidZ(false);
}

void ZJetMetaData::ClearContent()
{
	m_validPFJets.clear();
}

KEventMetadata* ZJetMetaData::GetKappaMetaData(ZJetEventData const& evtData,
		ZJetPipelineSettings const& psettings) const
{
	if (psettings.Global()->GetInputType() == McInput)
		return evtData.m_geneventmetadata;
	else
		return evtData.m_eventmetadata;
}

std::string ZJetMetaData::GetContent() const
{
	std::stringstream s;
	s << "PF ValidJets collection:" << std::endl;
	for (JetMappingIterator it = m_listValidJets.begin();
		it != m_listValidJets.end(); ++it)
	{
		s << it->first << " count " << it->second->size() << std::endl;
	}

	s << "PF MetaJets collection:" << std::endl;
	for (MetaPFJetContainer::iterator it = m_validPFJets.begin();
		it != m_validPFJets.end(); ++it)
	{
		s << it->first << " count " << it->second->size() << std::endl;

	}

	return s.str();
}

KDataLV* ZJetMetaData::GetValidJet(ZJetPipelineSettings const& psettings,
		ZJetEventData const& evtData, unsigned int index, std::string algoName) const
{
	backtrace_assert(GetValidJetCount(psettings, evtData, algoName) > index);

	if (IsMetaJetAlgo(algoName))
	{
		return &(m_validPFJets.at(algoName).at(index));
	}
	else if (JetType::IsGen(algoName))
	{
		// all gen jets are valid ...
		KDataLV* j = evtData.GetJet(psettings, index, algoName);
		assert(j != NULL);
		return j;
	}
	else
	{
		KDataLV* j = evtData.GetJet(psettings, m_listValidJets[algoName].at(
				index), algoName);
		assert(j != NULL);
		return j;
	}
}

KDataLV* ZJetMetaData::GetInvalidJet(ZJetPipelineSettings const& psettings,
		ZJetEventData const& evtData, unsigned int index, std::string algoName) const
{
	backtrace_assert( GetInvalidJetCount(psettings, evtData, algoName) > index );

	if (IsMetaJetAlgo(algoName))
	{
		CALIB_LOG_FATAL("Invalid jets are not provided for corrected algorithm " + algoName)
		return NULL;
	}
	else if (JetType::IsGen(algoName))
	{
		CALIB_LOG_FATAL("Invalid jets are not provided for GenJets algorithm " + algoName)
		return NULL;
	}
	else
	{
		KDataLV* j = evtData.GetJet(psettings,
				m_listInvalidJets[algoName].at(index), algoName);
		assert(j != NULL);

		return j;
	}
}

unsigned int ZJetMetaData::GetValidJetCount(ZJetPipelineSettings const& psettings,
		ZJetEventData const& evtData, std::string algoName) const
{
	if (IsMetaJetAlgo(algoName))
	{
		return SafeMap<std::string, std::vector<KDataPFJet> >::GetPtrMap(algoName, m_validPFJets).size();
	}
	else if (JetType::IsGen(algoName))
	{
		return evtData.GetJetCount(psettings, algoName);
	}
	else
	{
		return SafeMap<std::string, std::vector<unsigned int> >::GetPtrMap(algoName, m_listValidJets).size();
	}
}

double ZJetMetaData::GetMPF(const KDataLV* met) const
{
	double scalPtEt = GetRefZ().p4.Px() * met->p4.Px()
			+ GetRefZ().p4.Py() * met->p4.Py();

	double scalPtSq = GetRefZ().p4.Px() * GetRefZ().p4.Px()
			+ GetRefZ().p4.Py() * GetRefZ().p4.Py();

	return 1.0f + scalPtEt / scalPtSq;
}

double ZJetMetaData::GetGenMPF(const KDataLV* met) const
{
	double scalPtEt = GetRefGenZ().p4.Px() * met->p4.Px()
			+ GetRefGenZ().p4.Py() * met->p4.Py();

	double scalPtSq = GetRefGenZ().p4.Px() * GetRefGenZ().p4.Px()
			+ GetRefGenZ().p4.Py() * GetRefGenZ().p4.Py();

	return 1.0f + scalPtEt / scalPtSq;
}

double ZJetMetaData::GetTwoBalance(const KDataLV* jet1, const KDataLV* jet2) const
{
	return (jet1->p4 + jet2->p4).Pt() / GetRefZ().p4.Pt();
}

double ZJetMetaData::GetGenTwoBalance(const KDataLV* jet1, const KDataLV* jet2) const
{
	return (jet1->p4 + jet2->p4).Pt() / GetRefGenZ().p4.Pt();
}

bool cmpPFJetPt (const KDataPFJet i, const KDataPFJet j)
{
	return (i.p4.Pt() < j.p4.Pt());
}

double ZJetMetaData::GetZeppenfeld(const KDataLV* jet1, const KDataLV* jet2, const KDataLV* jet3) const
{
	double eta1 = jet1->p4.Eta();
	double eta2 = jet2->p4.Eta();
	double eta3 = jet3->p4.Eta();
	return std::abs((2.0 * eta3 - eta1 - eta2) / (eta1 - eta2));
}

void ZJetMetaData::SortJetCollections()
{
	for (MetaPFJetContainer::iterator it = m_validPFJets.begin();
			it != m_validPFJets.end(); ++ it)
	{
		std::vector<KDataPFJet>* jet_vect = it->second;
		std::sort( jet_vect->begin(), jet_vect->end(), cmpPFJetPt);
		std::reverse( jet_vect->begin(), jet_vect->end() );
	}
/*
	for testing purpses, to ensure the jets are ordered
	for (MetaPFJetContainer::iterator it = m_validPFJets.begin();
			it != m_validPFJets.end(); ++ it)
	{
		std::vector<KDataPFJet> & jet_vect = it->second;
		double largest = 50000000.0;
		for(unsigned int i = 0; i < jet_vect.size(); ++ i)
		{
			if (jet_vect[i].p4.Pt() > largest)
				CALIB_LOG_FATAL("Jets not sorted correctly")
			largest = jet_vect[i].p4.Pt();
		}
	}
*/
}

}
