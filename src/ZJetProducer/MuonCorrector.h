#pragma once

#include "ZJetEventPipeline/MetaData.h"
#include "MuScleFitCorrection/MuScleFitCorrector.h"
#include <Math/LorentzVector.h>

namespace Artus
{

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

class MuonCorrector: public ZJetGlobalProductProducerBase
{
public:
	MuonCorrector(std::string parameterfile, std::string parameterfileRunD, bool smeared = true, bool deterministic = false, bool radiation = false);

	virtual void PopulateProduct(ZJetEventData const& data,
								 ZJetProduct& product,
								 ZJetPipelineSettings const& m_pipelineSettings) const {};

	virtual bool PopulateGlobalProduct(ZJetEventData const& event,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& m_pipelineSettings) const;

	static std::string Name()
	{
		return "muon_corrector";
	}



private:
	const long startRunD;
	bool m_smearing;
	bool m_deterministic;
	bool m_radiationcorr;
	std::string m_parameterfile;
	std::string m_parameterfileRunD;
	MuScleFitCorrector m_corrector;
	MuScleFitCorrector m_correctorRunD;

};

}
