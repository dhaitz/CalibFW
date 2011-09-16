#pragma once

#include <memory>

#include "../ZJetMetaData.h"
#include "ZJetMetaDataProducer.h"

#define USE_JEC
#include <KappaTools/RootTools/JECTools.h>

namespace CalibFW
{

// takes the jets contained in an event and applies the necessary corrections
class CorrJetProducer: public ZJetMetaDataProducerBase
{
public:

	CorrJetProducer() : ZJetMetaDataProducerBase()
	{
		std::vector < std::string> corLevel;

		corLevel.push_back("L1FastJet");

		m_jecService = std::unique_ptr<JECService>(
				new JECService("MC_42_V13_AK5PF",
						corLevel, 23.0) // verify
						);
	}

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual void PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		std::string algoName =  "AK5PF";
		unsigned int jetcount = metaData.GetValidJetCount(m_pipelineSettings, algoName );

		for ( unsigned int i = 0;
				i < jetcount; ++i)
		{
			KDataPFJet * jet = static_cast<KDataPFJet*>( metaData.GetValidJet( m_pipelineSettings, event, i) );

			// create a copy
			KDataPFJet jet_corr = *jet;

			//m_jecSerice->correct

			metaData.AddValidJet( jet_corr, algoName );
		}
	}
private:
	std::unique_ptr<JECService> m_jecService;
};

}
