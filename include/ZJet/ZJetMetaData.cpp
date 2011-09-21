#include <sstream>

#include "ZJetMetaData.h"

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
	assert( GetValidJetCount(psettings, algoName) > index );

	if (IsMetaJetAlgo(algoName))
	{
		return &(m_validPFJets.at(algoName).at(index));
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
		ZJetPipelineSettings const& psettings, std::string algoName) const
{
	if (IsMetaJetAlgo(algoName))
	{
		return m_validPFJets.at(algoName).size();
	}
	else
	{
		return this->m_listValidJets.at(algoName).size();
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

}
