#pragma once

#include <memory>

#include "../ZJetMetaData.h"
#include "ZJetMetaDataProducer.h"

#define USE_JEC
#include "../../Misc/JECTools.h"

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
class CorrJetProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	static std::string Name() { return "corr_jet_producer"; }


	CorrJetProducer( std::string corBase );

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const;

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const;

	void InitCorrection( std::string algoName,
			     std::string algoCorrectionAlias,
			ZJetEventData const& event,
			std::string prefix = "" ) const;

	void CorrectJetCollection( std::string algoName, std::string newAlgoName,
					boost::scoped_ptr< JECService > const& corrService,
					ZJetEventData const& event,
					ZJetMetaData & metaData,
					ZJetPipelineSettings const& settings) const;

	void CreateCorrections( std::string algoName,
            std::string algoPostfix,
			ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& settings,
			std::string algoAlias  ) const;

private:
	mutable boost::ptr_map< std::string, JecCorrSet> m_corrService;
	std::string m_corectionFileBase;
};

}
