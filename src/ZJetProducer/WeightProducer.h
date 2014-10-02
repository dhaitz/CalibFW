#pragma once

// from ROOT
#include <Math/VectorUtil.h>
#include "RootTools/RootIncludes.h"
#include "ZJetProducer/TriggerWeights.h"


namespace Artus
{
/// Event reweighting depending on pile-up and/or 2nd jet \f$pT\f$.
/** Modify the event weight depending on the number of true interactions per
    event to fit the estimated distribution in data.

    This can only be used on MC.
*/

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

class WeightProducer: public ZJetGlobalProductProducerBase
{
private:
	std::vector<double> m_pileupweights;
	double m_bins;
	std::vector<double> m_triggerweights;
	double m_triggerbinwidth;

public:
	WeightProducer(std::string weightfile)
	{
		// load pile-up weights
		const std::string s = "pileup";
		LOG("Loading pile-up weights (" << s << "):\n  " << weightfile);
		TFile file(weightfile.c_str(), "READONLY");
		TH1D* pileuphisto = (TH1D*) file.Get("pileup");

		m_pileupweights.clear();
		for (int i = 1; i <= pileuphisto->GetNbinsX(); ++i)
			m_pileupweights.emplace_back(pileuphisto->GetBinContent(i));
		m_bins = 1.0 / pileuphisto->GetBinWidth(1);
		delete pileuphisto;
		file.Close();

		double w[] = {1.0, 1.0133, 1.00312, 0.960151, 0.976782, 0.982018, 0.951591, 0.968691, 0.958286, 0.964922, 0.963885, 0.945895, 0.952558, 0.9771, 0.977798, 0.98852, 1.05932, 1.03958, 1.04062, 1.03216, 1.03077, 1.00564, 1.02296, 1.0295, 1.02713, 1.01387, 1.01669, 1.02451, 1.04553, 1.03359, 1.05437, 1.04735, 0.984159, 1.00111, 0.99083, 0.952875, 0.930611, 0.953789, 0.95413, 0.929454, 0.981264, 1.00026, 1.01724, 1.01122, 0.987915, 1.02896, 1.01948, 1.0};
		for (int i = 0; i < 48; i++)
			m_triggerweights.emplace_back(w[i]);
		m_triggerbinwidth = 0.1;
	}

	virtual void PopulateProduct(ZJetEventData const& data,
								 ZJetProduct& product,
								 ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing todo here
	}

	static std::string Name()
	{
		return "weight_producer";
	}


	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& m_pipelineSettings) const
	{
		double weight = 1.0;

		// pu reweighting
		if (m_pipelineSettings.Global()->GetEnablePuReweighting())
		{
			assert(data.m_geneventproduct != NULL);
			double npu = data.m_geneventproduct->numPUInteractionsTruth;
			if (npu < m_pileupweights.size())
				weight *= m_pileupweights.at(int(npu * m_bins));
			else
				weight *= 0;
		}

		// 2nd jet pt reweighting
		if (m_pipelineSettings.Global()->GetEnable2ndJetReweighting() &&
			product.GetValidJetCount(m_pipelineSettings, data, "AK5PFJetsCHSL1L2L3") < 2)
		{
			KDataLV* jet2 = product.GetValidJet(m_pipelineSettings, data, 1, "AK5PFJetsCHSL1L2L3");
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
			long long ntotal = m_pipelineSettings.Global()->GetNEvents();

			//evaluate in this order: config, external, internal
			if (m_pipelineSettings.Global()->GetXSection() > 0)
				xsec = m_pipelineSettings.Global()->GetXSection();
			else if (data.m_genlumiproduct->xSectionExt > 0)
				xsec = data.m_genlumiproduct->xSectionExt;
			else if (data.m_genlumiproduct->xSectionInt > 0)
				xsec *= data.m_genlumiproduct->xSectionInt;
			else
				LOG_FATAL("Lumi reweighting enabled but no cross section given");

			if (data.m_genlumiproduct->filterEff > 0)
				xsec *= data.m_genlumiproduct->filterEff;
			weight *= xsec / ntotal * 1000.0;  // normalize to 1/fb

			if (unlikely(xsec <= 0))
				LOG("cfg=" << m_pipelineSettings.Global()->GetXSection() <<
					", ext=" << data.m_genlumiproduct->xSectionExt <<
					", int=" << data.m_genlumiproduct->xSectionInt <<
					", eff=" << data.m_genlumiproduct->filterEff <<
					", nevents=" << ntotal <<
					", xsec=" << xsec << ", weight=" << weight);
		}

		// muon trigger reweighting
		//
		if (m_pipelineSettings.Global()->GetEnableTriggerReweighting())
		{
			double eta1 = product.m_listValidMuons[0].p4.Eta();
			double eta2 = product.m_listValidMuons[1].p4.Eta();
			double pt1 = product.m_listValidMuons[0].p4.Pt();
			double pt2 = product.m_listValidMuons[1].p4.Pt();

			// ϵ(8,A) · ϵ(17,B) + ϵ(17,A) · ϵ(8,B) - ϵ(17,A) · ϵ(17,B)
			double eff = efficiencyMu17(eta1) * efficiencyMu8(eta2)
						 + efficiencyMu17(eta2) * efficiencyMu8(eta1)
						 - efficiencyMu17(eta1) * efficiencyMu17(eta2);
			eff *= turnonMu17(pt1) * turnonMu17(pt2);
			if (eta1 < -2.4) eta1 = -2.4;
			if (eta1 > +2.4) eta1 = +2.4;
			if (eta2 < -2.4) eta2 = -2.4;
			if (eta2 > +2.4) eta2 = +2.4;
			eff *= m_triggerweights.at(int((eta1 + 2.4) / m_triggerbinwidth));
			eff *= m_triggerweights.at(int((eta2 + 2.4) / m_triggerbinwidth));
			weight *= eff;
			product.SetEfficiency(eff);
		}

		product.SetWeight(product.GetWeight() * weight);
		return true;
	}
};


}
