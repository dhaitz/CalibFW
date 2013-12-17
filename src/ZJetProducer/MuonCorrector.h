#pragma once

#include "ZJetEventPipeline/MetaData.h"
#include "ZJetProducer/MetadataProducer.h"

#include "MuScleFitCorrection/MuScleFitCorrector.h"

#include <Math/LorentzVector.h>

namespace Artus
{

class MuonCorrector: public ZJetGlobalMetaDataProducerBase
{
public:
	MuonCorrector(std::string parameterfile, std::string parameterfileRunD, bool smeared = true, bool deterministic = false);

	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const {};

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& m_pipelineSettings) const;

	static std::string Name()
	{
		return "muon_corrector";
	}



private:
	const long startRunD;
	bool m_smearing;
	bool m_deterministic;
	std::string m_parameterfile;
	std::string m_parameterfileRunD;
	MuScleFitCorrector m_corrector;
	MuScleFitCorrector m_correctorRunD;

};

}
