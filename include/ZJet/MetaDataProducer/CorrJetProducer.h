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

	CorrJetProducer();

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const;

	virtual void PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const;

	void CorrectJetCollection( std::string algoName, std::string newAlgoName,
					std::shared_ptr< JECService > corrService,
					ZJetEventData const& event,
					ZJetMetaData & metaData,
					ZJetPipelineSettings const& settings) const;

private:
	std::unique_ptr<JECService> m_jecService;
	std::shared_ptr<JECService> m_l1;
	std::shared_ptr<JECService> m_l2;
	std::shared_ptr<JECService> m_l3;
};

}
