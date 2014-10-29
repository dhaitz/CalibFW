#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapMuons: public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"mupluspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == 1) return muon.p4.Pt();
					}
					return 0.;
				}
			},
			{
				"mupluseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == 1) return muon.p4.Eta();
					}
					return 0.;
				}
			},
			{
				"muplusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == 1) return muon.p4.Phi();
					}
					return 0.;
				}
			},
			{
				"muminuspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == -1) return muon.p4.Pt();
					}
					return 0.;
				}
			},
			{
				"muminuseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == -1) return muon.p4.Eta();
					}
					return 0.;
				}
			},
			{
				"muminusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == -1) return muon.p4.Phi();
					}
					return 0.;
				}
			},
			{
				"muplusiso", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == +1) return muon.trackIso03;
					}
					return 0.;
				}
			},
			{
				"muminusiso", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					for (auto muon : product.m_listValidMuons)
					{
						if (muon.charge == -1) return muon.trackIso03;
					}
					return 0.;
				}
			},
			{
				"mu1pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataMuon muon;
					for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
					it != product.m_listValidMuons.end(); it ++)
					{
						if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
							muon = *it;
					}
					return muon.p4.Pt();
				}
			},
			{
				"mu1phi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataMuon muon;
					for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
					it != product.m_listValidMuons.end(); it ++)
					{
						if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
							muon = *it;
					}
					return muon.p4.Phi();
				}
			},
			{
				"mu1eta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataMuon muon;
					for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
					it != product.m_listValidMuons.end(); it ++)
					{
						if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
							muon = *it;
					}
					return muon.p4.Eta();
				}
			},
			{
				"mu2pt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataMuon muon;
					for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
					it != product.m_listValidMuons.end(); it ++)
					{
						if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
							muon = *it;
					}
					return muon.p4.Pt();
				}
			},
			{
				"mu2phi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataMuon muon;
					for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
					it != product.m_listValidMuons.end(); it ++)
					{
						if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
							muon = *it;
					}
					return muon.p4.Phi();
				}
			},
			{
				"mu2eta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					KDataMuon muon;
					for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
					it != product.m_listValidMuons.end(); it ++)
					{
						if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
							muon = *it;
					}
					return muon.p4.Eta();
				}
			},
			{
				"genmupluspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genMuons)
						if (it.charge() > 0) value = it.p4.Pt();
					return value;
				}
			},
			{
				"genmupluseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genMuons)
						if (it.charge() > 0) value = it.p4.Eta();
					return value;
				}
			},
			{
				"genmuplusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genMuons)
						if (it.charge() > 0) value = it.p4.Phi();
					return value;
				}
			},
			{
				"genmuminuspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genMuons)
						if (it.charge() < 0) value = it.p4.Pt();
					return value;
				}
			},
			{
				"genmuminuseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genMuons)
						if (it.charge() < 0) value = it.p4.Eta();
					return value;
				}
			},
			{
				"genmuminusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genMuons)
						if (it.charge() < 0) value = it.p4.Phi();
					return value;
				}
			},
			{
				"nmuons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_listValidMuons.size();
				}
			},
			{
				"ngenmuons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genMuons.size();
				}
			},
			{
				"ninternalmuons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genInternalMuons.size();
				}
			},
			{
				"nintermediatemuons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genIntermediateMuons.size();
				}
			},
			{
				"ptdiff13", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0;
					if (product.m_genMuons.size() < 2 || product.m_genInternalMuons.size() < 2)
						value = -999.;
					else
						value = product.m_genMuons[0].p4.Pt()
						+ product.m_genMuons[1].p4.Pt()
						- product.m_genInternalMuons[0].p4.Pt()
						- product.m_genInternalMuons[1].p4.Pt();
					return value;
				}
			},
			{
				"ptdiff12", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0;
					if (product.m_genMuons.size() < 2 || product.m_genIntermediateMuons.size() < 2)
						value = -999.;
					else					value = product.m_genMuons[0].p4.Pt()
						+ product.m_genMuons[1].p4.Pt()
						- product.m_genIntermediateMuons[0].p4.Pt()
						- product.m_genIntermediateMuons[1].p4.Pt();
					return value;
				}
			},
			{
				"ptdiff23", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = 0;
					if (product.m_genIntermediateMuons.size() < 2 || product.m_genInternalMuons.size() < 2)
						value = -999.;
					else
						value = product.m_genMuons[0].p4.Pt()
						+ product.m_genMuons[1].p4.Pt()
						- product.m_genIntermediateMuons[0].p4.Pt()
						- product.m_genIntermediateMuons[1].p4.Pt();
					return value;
				}
			},
		};
		return lambdaMap;
	};

};
}
