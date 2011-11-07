#pragma once

#include <memory>

#include "../ZJetMetaData.h"
#include "ZJetMetaDataProducer.h"

#define USE_JEC
#include <KappaTools/RootTools/JECTools.h>

namespace CalibFW
{

struct JecCorrSet
{
	boost::scoped_ptr<JECService> m_l1;
	boost::scoped_ptr<JECService> m_l2;
	boost::scoped_ptr<JECService> m_l3;

	// for data
	boost::scoped_ptr<JECService> m_l2l3res;
};

// takes the jets contained in an event and applies the necessary corrections
class CorrJetProducer: public ZJetMetaDataProducerBase
{
public:

	CorrJetProducer( std::string corBase );

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const;

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const;

	void InitCorrection( std::string algoName,
			ZJetEventData const& event ) const;

	void CorrectJetCollection( std::string algoName, std::string newAlgoName,
					boost::scoped_ptr< JECService > const& corrService,
					ZJetEventData const& event,
					ZJetMetaData & metaData,
					ZJetPipelineSettings const& settings) const;

	void CreateCorrections( std::string algoName,
			ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& settings ) const;

private:
	mutable boost::ptr_map< std::string, JecCorrSet> m_corrService;
	std::string m_corectionFileBase;
};

}
