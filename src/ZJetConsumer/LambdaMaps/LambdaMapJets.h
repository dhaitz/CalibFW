#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapJets: public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"jet1pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetValidPrimaryJet(s, event)->p4.Pt();
				}
			},
			{
				"jet1eta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetValidPrimaryJet(s, event)->p4.Eta();
				}
			},
			{
				"jet1y", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetValidPrimaryJet(s, event)->p4.Rapidity();
				}
			},
			{
				"jet1phi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetValidPrimaryJet(s, event)->p4.Phi();
				}
				// leading jet composition
			},
			{
				"jet1photonfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->photonFraction;
				}
			},
			{
				"jet1chargedemfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->chargedEMFraction;
				}
			},
			{
				"jet1chargedhadfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->chargedHadFraction;
				}
			},
			{
				"jet1neutralhadfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->neutralHadFraction;
				}
			},
			{
				"jet1muonfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->muonFraction;
				}
			},
			{
				"jet1HFhadfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->HFHadFraction;
				}
			},
			{
				"jet1HFemfraction", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->HFEMFraction;
				}
			},			{
				"jet1unc", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingjetuncertainty[s.GetJetAlgorithm()];
				}
			},
			{
				"jet2pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetValidJetCount(s, event) > 1)
						return product.GetValidJet(s, event, 1)->p4.Pt();

					return 0.;
				}
			},
			{
				"jet2phi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetValidJetCount(s, event) > 1)
						return product.GetValidJet(s, event, 1)->p4.Phi();

					return 0.;
				}
			},
			{
				"jet2eta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetValidJetCount(s, event) > 1)
						return product.GetValidJet(s, event, 1)->p4.Eta();

					return 0.;
				}
			},
			{
				"njets", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetValidJetCount(s, event);
				}
			},
			{
				"njets30", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float // needed for Zee studies
				{
					float count = 0.;
					for (unsigned int i = 0; i < product.GetValidJetCount(s, event); i++)
					{
						if (product.GetValidJet(s, event, i)->p4.Pt() > 30)
							count += 1.;
					}
					return count;
				}
			},
			{
				"njets30barrel", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float // needed for Zee studies
				{
					float count = 0.;
					for (unsigned int i = 0; i < product.GetValidJetCount(s, event); i++)
					{
						if (product.GetValidJet(s, event, i)->p4.Pt() > 30 && std::abs(product.GetValidJet(s, event, i)->p4.Eta()) < 2.4)
							count += 1.;
					}
					return count;
				}
			},
			{
				"njetsinv", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_listInvalidJets["AK5PFJetsCHS"].size();
				}
			},
			{
				"genjet1pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

					if (product.GetValidJetCount(s, event, genName) == 0)
						return false;

					return product.GetValidJet(s, event, 0, genName)->p4.Pt();
				}
			},
			{
				"genjet1ptneutrinos", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

					if (product.GetValidJetCount(s, event, genName) == 0)
						return false;

					KDataLV v = * product.GetValidJet(s, event, 0, genName);
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							v.p4 += it.p4;
					return v.p4.Pt();
				}
			},
			{
				"genjet1eta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

					if (product.GetValidJetCount(s, event, genName) == 0)
						return false;

					return product.GetValidJet(s, event, 0, genName)->p4.Eta();
				}
			},
			{
				"genjet1phi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

					if (product.GetValidJetCount(s, event, genName) == 0)
						return false;

					return product.GetValidJet(s, event, 0, genName)->p4.Phi();
				}
			},
			{
				"matchedgenjet1pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMatchedGenJet(event, s, 0)->p4.Pt();
				}
			},
			{
				"matchedgenjet2pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMatchedGenJet(event, s, 1)->p4.Pt();
				}
			},
			{
				"genjet2pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

					if (product.GetValidJetCount(s, event, genName) < 2)
						return false;

					return product.GetValidJet(s, event, 1, genName)->p4.Pt();
				}
			},
		};
		return lambdaMap;
	};

};
}
