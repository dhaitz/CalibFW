#pragma once

#include "CondFormats/JetMETObjects/interface/SimpleJetCorrectionUncertainty.h"


namespace Artus
{

class LeadingJetUncertaintyProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	LeadingJetUncertaintyProducer(std::string corBase,
								  std::vector<std::string> baseAlgos) :
		ZJetGlobalMetaDataProducerBase(), m_corectionFileBase(corBase),
		m_basealgorithms(baseAlgos)
	{
		// iterate over base algorithms and write the respective uncertainty
		// objects into a map
		for (auto it = m_basealgorithms.begin();
			 it != m_basealgorithms.end(); ++it)
		{
			//std::string filename = m_corectionFileBase + "_Uncertainty_" + *it + ".txt";
			m_uncertainties.insert(*it, new SimpleJetCorrectionUncertainty(m_corectionFileBase + "_Uncertainty_" + *it + ".txt"));
		}
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& settings) const
	{
		bool multiple_uncertainty_files = true;
		SimpleJetCorrectionUncertainty* uncertainty = new SimpleJetCorrectionUncertainty();

		//if theres only one base algorithm, things are simple:
		if (m_uncertainties.size() == 1)
		{
			uncertainty = m_uncertainties.begin()->second;
			multiple_uncertainty_files = false;
		}

		// iterate over jet collections and get the uncertainty for the leading jet
		for (auto italgo = metaData.m_validPFJets.begin();
			 italgo != metaData.m_validPFJets.end(); ++italgo)
		{
			if (italgo->second->size() < 1)
				return false;

			// if there are multiple uncertainty files in use, we have to select
			// the correct one for each algorithm
			if (multiple_uncertainty_files)
				uncertainty = & m_uncertainties.at(metaData.GetCorrespondingBaseJetAlgorithm(italgo->first));

			std::vector<float> vec(1, italgo->second->at(0).p4.Eta());
			metaData.leadingjetuncertainty[italgo->first] = uncertainty->uncertainty(vec, italgo->second->at(0).p4.Pt(), 0);
		}
		return true;
	}

	static std::string Name()
	{
		return "leading_jet_uncertainty_producer";
	}

private:
	std::string m_corectionFileBase;
	std::vector<std::string> m_basealgorithms;
	mutable boost::ptr_map< std::string, SimpleJetCorrectionUncertainty> m_uncertainties;

};

}
