#pragma once

// from ROOT
#include <Math/VectorUtil.h>

#include "ZJetMetaDataProducer.h"

namespace CalibFW
{


/// Event reweighting depending on pile-up and/or 2nd jet \f$pT\f$.
/** Modify the event weight depending on the number of true interactions per
    event to fit the estimated distribution in data.

    This can only be used on MC.
*/
class PuReweightingProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing todo here
	}

	static std::string Name() { return "pu_reweighting_producer"; }


	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!m_pipelineSettings.Global()->GetEnablePuReweighting())
			return true;

		assert(data.m_geneventmetadata != NULL);
		double fact = m_pipelineSettings.Global()->GetPuReweighting().at(
			int(data.m_geneventmetadata->numPUInteractionsTruth + 0.5) );
		metaData.SetWeight(metaData.GetWeight() * fact);

		// additional 2nd jet pt reweighting
		if (!m_pipelineSettings.Global()->GetEnable2ndJetReweighting())
			return true;

		if (metaData.GetValidJetCount(m_pipelineSettings, data, "AK5PFJetsCHSL1L2L3") < 2)
			return true;

		KDataLV * jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1, "AK5PFJetsCHSL1L2L3");
		// apply a new weight for 2 GeV pt bins
		if (m_pipelineSettings.Global()->Get2ndJetReweighting().size() > int(jet2->p4.Pt() / 2.0))
			fact = m_pipelineSettings.Global()->Get2ndJetReweighting().at(
				int(jet2->p4.Pt() / 2.0) );
		else
			fact = 0.0f;
		metaData.SetWeight(metaData.GetWeight() * fact);

		return true;
	}
};


}
