#pragma once

#include <memory>

#include "ZJetEventPipeline/MetaData.h"
#include "ZJetProducer/MetadataProducer.h"

#define USE_JEC
#include "RootTools/JECTools.h"

namespace CalibFW
{

struct JecCorrSet
{
	boost::scoped_ptr<JECService> m_l1;
	boost::scoped_ptr<JECService> m_l2;
	boost::scoped_ptr<JECService> m_l3;
	boost::scoped_ptr<JECService> m_l2l3res;
};

// takes the jets contained in an event and applies the necessary corrections
class CorrJetProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	static std::string Name()
	{
		return "corr_jet_producer";
	}


	CorrJetProducer(std::string corBase, std::string l1cor, stringvector baseAlgos);

	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const;

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& m_pipelineSettings) const;

	void InitCorrection(std::string algoName,
						std::string algoCorrectionAlias,
						ZJetEventData const& event, double hcal,
						std::string prefix = "") const;

	void CorrectJetCollection(std::string algoName, std::string newAlgoName,
							  boost::scoped_ptr< JECService > const& corrService,
							  ZJetEventData const& event,
							  ZJetMetaData& metaData,
							  ZJetPipelineSettings const& settings) const;

	void CreateCorrections(std::string algoName,
						   std::string algoPostfix,
						   ZJetEventData const& event,
						   ZJetMetaData& metaData,
						   ZJetPipelineSettings const& settings,
						   std::string algoAlias) const;

private:
	mutable boost::ptr_map< std::string, JecCorrSet> m_corrService;
	std::string m_corectionFileBase;
	std::string m_l1correction;
	std::vector<std::string> m_basealgorithms;

};

}
