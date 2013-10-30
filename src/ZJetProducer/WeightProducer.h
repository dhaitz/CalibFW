#pragma once

// from ROOT
#include <Math/VectorUtil.h>
#include "ZJetProducer/MetadataProducer.h"

namespace Artus
{
/// Event reweighting depending on pile-up and/or 2nd jet \f$pT\f$.
/** Modify the event weight depending on the number of true interactions per
    event to fit the estimated distribution in data.

    This can only be used on MC.
*/
class WeightProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing todo here
	}

	static std::string Name() { return "weight_producer"; }


	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& m_pipelineSettings) const
	{
		double weight = 1.0;

		// pu reweighting
		if (m_pipelineSettings.Global()->GetEnablePuReweighting())
		{
			assert(data.m_geneventmetadata != NULL);
			weight *= m_pipelineSettings.Global()->GetPuReweighting().at(
						int(data.m_geneventmetadata->numPUInteractionsTruth + 0.5));
		}

		// 2nd jet pt reweighting
		if (m_pipelineSettings.Global()->GetEnable2ndJetReweighting() &&
			metaData.GetValidJetCount(m_pipelineSettings, data, "AK5PFJetsCHSL1L2L3") < 2)
		{
			KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1, "AK5PFJetsCHSL1L2L3");
			// apply a new weight for 2 GeV pt bins
			if (m_pipelineSettings.Global()->Get2ndJetReweighting().size() > unsigned(jet2->p4.Pt() / 2.0))
				weight *= m_pipelineSettings.Global()->Get2ndJetReweighting().at(
							unsigned(jet2->p4.Pt() / 2.0));
			else
				weight = 0.0;
		}

		// sample reweighting (based on file name)
		if (m_pipelineSettings.Global()->GetEnableSampleReweighting())
		{
			size_t i = data.m_pthatbin; // sample index
			if (m_pipelineSettings.Global()->GetSampleReweighting().size() > i)
				weight *= m_pipelineSettings.Global()->GetSampleReweighting().at(i);
			else
				LOG_FATAL("No sample weight specified for sample " << i);
		}

		// lumi reweighting
		// not needed for every event, should be done once per lumi section
		if (m_pipelineSettings.Global()->GetEnableLumiReweighting())
		{
			double xsec = 1.0;  // in pb
			long long ntotal = 1.0e6;

			//evaluate in this order: config, external, internal
			if (m_pipelineSettings.Global()->GetXSection() > 0)
				xsec = m_pipelineSettings.Global()->GetXSection();
			else if (data.m_genlumimetadata->xSectionExt > 0)
				xsec = data.m_genlumimetadata->xSectionExt;
			else if (data.m_genlumimetadata->xSectionInt > 0)
				xsec *= data.m_genlumimetadata->xSectionInt;
			else
				LOG_FATAL("Lumi reweighting enabled but no cross section given");

			if (data.m_genlumimetadata->filterEff > 0)
				xsec *= data.m_genlumimetadata->filterEff;
			weight *= xsec / ntotal * 1000.0;  // normalize to 1/fb

			if (unlikely(xsec <= 0))
				LOG("cfg=" << m_pipelineSettings.Global()->GetXSection() <<
					", ext=" << data.m_genlumimetadata->xSectionExt <<
					", int=" << data.m_genlumimetadata->xSectionInt <<
					", eff=" << data.m_genlumimetadata->filterEff <<
					", xsec=" << xsec <<", weight=" << weight);
		}

		metaData.SetWeight(metaData.GetWeight() * weight);
		return true;
	}
};


}
