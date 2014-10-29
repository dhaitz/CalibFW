#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapMCGenerator : public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{

			{
				"ngenphotons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genPhotons.size();
				}
			},
			{
				"closestphotondr", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					double dR = 999.;
					double d = dR;
					for (const auto & mu : product.m_genMuons)
						for (const auto & ph : product.m_genPhotons)
						{
							d = ROOT::Math::VectorUtil::DeltaR(mu.p4, ph.p4);
							if (d < dR && ph.p4.Pt() > 1)
								dR = d;
						}
					return dR;
				}
			},
			{
				"ngenphotonsclose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					const double dR = 0.3;
					int n = 0;
					for (const auto & mu : product.m_genMuons)
						for (const auto & ph : product.m_genPhotons)
							if (ROOT::Math::VectorUtil::DeltaR(mu.p4, ph.p4) < dR)
								n++;
					return n;
				}
			},
			{
				"ptgenphotonsclose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					const double dR = 0.3;
					double pt = 0;
					for (const auto & mu : product.m_genMuons)
						for (const auto & ph : product.m_genPhotons)
							if (ROOT::Math::VectorUtil::DeltaR(mu.p4, ph.p4) < dR)
								pt += ph.p4.Pt();
					return pt;
				}
			},
			{
				"ptgenphotonsfar", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					const double dR = 0.3;
					double pt = 0;
					for (const auto & mu : product.m_genMuons)
					{
						// look at a cone perpendicular to the muon
						KGenParticle nomu = mu;
						nomu.p4.SetPhi(nomu.p4.Phi() + ROOT::Math::Pi() / 2);
						for (const auto & ph : product.m_genPhotons)
							if (ROOT::Math::VectorUtil::DeltaR(nomu.p4, ph.p4) < dR)
								pt += ph.p4.Pt();
					}
					return pt;
				}
			},
			{
				"lhezpt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetLHEZ().p4.Pt();
				}
			},
			{
				"lhezeta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetLHEZ().p4.Eta();
				}
			},
			{
				"lhezy", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetLHEZ().p4.Rapidity();
				}
			},
			{
				"lhezphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetLHEZ().p4.Phi();
				}
			},
			{
				"lhezmass", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetLHEZ().p4.mass();
				}
			},
			{
				"nlhemuons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_nLHEMuons;
				}
			},
			{
				"nlheelectrons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_nLHEElectrons;
				}
			},
			{
				"nlhetaus", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_nLHETaus;
				}
			},
			{
				"genmpf", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetGenMPF(product.GetPtGenMet());
				}
			},
			{
				"algoflavour", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetAlgoFlavour(s);
				}
			},
			{
				"physflavour", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetPhysFlavour(s);
				}
			},
			{
				"algopt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetAlgoPt(s);
				}
			},
			{
				"physpt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetPhysPt(s);
				}
			},
			{
				"jet1ptneutrinos", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataLV v = * product.GetValidPrimaryJet(s, event);
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							v.p4 += it.p4;
					return v.p4.Pt();
				}
			},
			{
				"mpfneutrinos", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataPFMET met = * product.GetMet(event, s);
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							met.p4 -= it.p4;
					met.p4.SetEta(0);
					return product.GetMPF(&met);
				}
			},
			{
				"neutralpt3", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataLV v;
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrals3[genName].size() > 0)
						for (const auto & it : product.m_neutrals3[genName])
							v.p4 += it.p4;
					return v.p4.Pt();
				}
			},
			{
				"neutralpt5", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataLV v;
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrals5[genName].size() > 0)
						for (const auto & it : product.m_neutrals5[genName])
							v.p4 += it.p4;
					return v.p4.Pt();
				}
			},
			{
				"mpfalgo", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMPF(&product.m_MET[s.GetJetAlgorithm() + "L5Algo"]);
				}
			},
			{
				"mpfphys", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMPF(&product.m_MET[s.GetJetAlgorithm() + "L5Phys"]);
				}
			},
			{
				"mpfneutrinosalgo", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataPFMET met = product.m_MET[s.GetJetAlgorithm() + "L5Algo"];
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							met.p4 -= it.p4;
					met.p4.SetEta(0);
					return product.GetMPF(&met);
				}
			},
			{
				"mpfneutrinosphys", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataPFMET met = product.m_MET[s.GetJetAlgorithm() + "L5Phys"];
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							met.p4 -= it.p4;
					met.p4.SetEta(0);
					return product.GetMPF(&met);
				}
			},
			{
				"jet1ptneutrinosalgo", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{

					KDataLV v = * product.GetValidJet(s, event, 0, s.GetJetAlgorithm() + "L5Algo");
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							v.p4 += it.p4;
					return v.p4.Pt();
				}
			},
			{
				"jet1ptneutrinosphys", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataLV v = * product.GetValidJet(s, event, 0, s.GetJetAlgorithm() + "L5Phys");
					std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
					if (product.m_neutrinos[genName].size() > 0)
						for (const auto & it : product.m_neutrinos[genName])
							v.p4 += it.p4;
					return v.p4.Pt();

				}
			},
			{
				"algol5pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					int flavour = std::abs(product.GetAlgoFlavour(s));

					if (flavour == 21)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5g").at(0).p4.Pt();
					if (flavour == 4)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5c").at(0).p4.Pt();
					if (flavour == 5)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5b").at(0).p4.Pt();
					if (flavour == 5)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5b").at(0).p4.Pt();
					if (flavour > 0 && flavour < 4)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5q").at(0).p4.Pt();

					return product.GetValidPrimaryJet(s, event)->p4.Pt();
				}
			},
			{
				"physl5pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					int flavour = std::abs(product.GetPhysFlavour(s));

					if (flavour == 21)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5g").at(0).p4.Pt();
					if (flavour == 4)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5c").at(0).p4.Pt();
					if (flavour == 5)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5b").at(0).p4.Pt();
					if (flavour == 5)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5b").at(0).p4.Pt();
					if (flavour > 0 && flavour < 4)
						return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5q").at(0).p4.Pt();

					return product.GetValidPrimaryJet(s, event)->p4.Pt();
				}
			},
		};
		return lambdaMap;
	};

};
}
