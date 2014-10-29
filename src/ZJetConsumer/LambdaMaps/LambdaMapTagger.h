#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapTagger : public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"qglikelihood", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("QGlikelihood", event.m_taggerproduct);
				}
			},
			{
				"qgmlp", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("QGmlp", event.m_taggerproduct);
				}
			},

			{
				"trackcountinghigheffbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("trackCountingHighEffBTag", event.m_taggerproduct);
				}
			},
			{
				"trackcountinghighpurbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("trackCountingHighPurBTag", event.m_taggerproduct);
				}
			},
			{
				"jetprobabilitybjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("jetProbabilityBTag", event.m_taggerproduct);
				}
			},
			{
				"jetbprobabilitybjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("jetBProbabilityBTag", event.m_taggerproduct);
				}
			},
			{
				"softelectronbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softElectronBTag", event.m_taggerproduct);
				}
			},
			{
				"softmuonbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softMuonBTag", event.m_taggerproduct);
				}
			},
			{
				"softmuonbyip3dbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softMuonByIP3dBTag", event.m_taggerproduct);
				}
			},
			{
				"softmuonbyptbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softMuonByPtBTag", event.m_taggerproduct);
				}
			},
			{
				"simplesecondaryvertexbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("simpleSecondaryVertexBTag", event.m_taggerproduct);
				}
			},
			{
				"combinedsecondaryvertexbjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerproduct);
				}
			},
			{
				"combinedsecondaryvertexmvabjettag", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("CombinedSecondaryVertexMVABJetTags", event.m_taggerproduct);
				}
			},
			{
				"jet1puJetIDFullTight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullTight", event.m_taggerproduct);
				}
			},
			{
				"jet1puJetIDFullMedium", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullMedium", event.m_taggerproduct);
				}
			},
			{
				"jet1puJetIDFullLoose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullLoose", event.m_taggerproduct);
				}
			},
			{
				"jet1puJetIDCutbasedTight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedTight", event.m_taggerproduct);
				}
			},
			{
				"jet1puJetIDCutbasedMedium", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedMedium", event.m_taggerproduct);
				}
			},
			{
				"jet1puJetIDCutbasedLoose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedLoose", event.m_taggerproduct);
				}
			},

			{
				"jet2puJetIDFullTight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0.;
					if (product.GetValidJetCount(s, event) > 1)
						value = static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullTight", event.m_taggerproduct);
					return value;
				}
			},
			{
				"jet2puJetIDFullMedium", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0.;
					if (product.GetValidJetCount(s, event) > 1)
						value = static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullMedium", event.m_taggerproduct);
					return value;
				}
			},
			{
				"jet2puJetIDFullLoose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0.;
					if (product.GetValidJetCount(s, event) > 1)
						value = static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullLoose", event.m_taggerproduct);
					return value;
				}
			},
			{
				"jet2puJetIDCutbasedTight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0.;
					if (product.GetValidJetCount(s, event) > 1)

						value = static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedTight", event.m_taggerproduct);
					return value;;
				}
			},
			{
				"jet2puJetIDCutbasedMedium", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0.;
					if (product.GetValidJetCount(s, event) > 1)

						value = static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedMedium", event.m_taggerproduct);
					return value;
				}
			},
			{
				"jet2puJetIDCutbasedLoose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0.;
					if (product.GetValidJetCount(s, event) > 1)

						value = static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedLoose", event.m_taggerproduct);
					return value;
				}
			},

		};
		return lambdaMap;
	};

};
}
