#include <sstream>
#include <algorithm>

#include "ZJetMetaData.h"
#include "Pipeline/JetTools.h"

namespace CalibFW
{

ZJetMetaData::ZJetMetaData()
{
	SetCutBitmask(0);
	SetWeight(1.0f);
	SetValidZ(false);
}

void ZJetMetaData::ClearContent()
{
	m_validPFJets.clear();
}

KEventMetadata *  ZJetMetaData::GetKappaMetaData( ZJetEventData const& evtData,
		ZJetPipelineSettings const& psettings ) const
		{
			if ( psettings.Global()->GetInputType() == McInput)
				return evtData.m_geneventmetadata;
			else
				return evtData.m_eventmetadata;
		}

std::string ZJetMetaData::GetContent()
{
	std::stringstream s;

	s << "PF ValidJets collection:" << std::endl;
	for (JetMappingIterator it = m_listValidJets.begin(); it
			!= m_listValidJets.end(); ++it)
	{
		s << it->first << " count " << it->second.size() << std::endl;

	}


	s << "PF MetaJets collection:" << std::endl;
	for (MetaPFJetContainer::iterator it = m_validPFJets.begin(); it
			!= m_validPFJets.end(); ++it)
	{
		s << it->first << " count " << it->second.size() << std::endl;

	}

	return s.str();
}

KDataLV * ZJetMetaData::GetValidJet(ZJetPipelineSettings const& psettings,
		ZJetEventData const& evtData, unsigned int index, std::string algoName) const
{
	assert( GetValidJetCount(psettings, evtData, algoName) > index );

	if (IsMetaJetAlgo(algoName))
	{
		return &(m_validPFJets.at(algoName).at(index));
	}
	else if ( JetType::IsGen( algoName ) )
	{
		// all gen jets are valid ...
		KDataLV * j = evtData.GetJet(psettings, index, algoName);
		assert( j != NULL);

		return j;
	}
	else
	{
		KDataLV * j = evtData.GetJet(psettings, m_listValidJets[algoName].at(
				index), algoName);
		assert( j != NULL);

		return j;
	}
}

unsigned int ZJetMetaData::GetValidJetCount(
		ZJetPipelineSettings const& psettings,
		ZJetEventData const& evtData, std::string algoName) const
{
	if (IsMetaJetAlgo(algoName))
	{
		return SafeMap<std::string, std::vector<KDataPFJet> >::Get( algoName,  m_validPFJets).size();
	}
	else if ( JetType::IsGen( algoName ) )
	{
		return evtData.GetJetCount ( psettings, algoName );
	}
	else
	{
		return SafeMap<std::string, std::vector<unsigned int> >::Get( algoName,  m_listValidJets).size();
	}
}

double ZJetMetaData::GetMPF(KDataLV * met) const
{
	double scalPtEt = GetRefZ().p4.Px() * met->p4.Px() + GetRefZ().p4.Py()
			* met->p4.Py();

	double scalPtSq = GetRefZ().p4.Px() * GetRefZ().p4.Px() + GetRefZ().p4.Py()
			* GetRefZ().p4.Py();

	return 1.0f + (scalPtEt / scalPtSq);
}

bool cmpPFJetPt (KDataPFJet i,KDataPFJet j)
{ return (i.p4.Pt()<j.p4.Pt());
}


void ZJetMetaData::SortJetCollections()
{

	for ( MetaPFJetContainer::iterator it = m_validPFJets.begin();
			it != m_validPFJets.end();
			++ it)
	{
		std::vector<KDataPFJet> & jet_vect = it->second;

		std::sort( jet_vect.begin(), jet_vect.end(), cmpPFJetPt);
		std::reverse( jet_vect.begin(), jet_vect.end() );
	}
}

}
