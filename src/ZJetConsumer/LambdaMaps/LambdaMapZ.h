#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapZ: public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"zpt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetRefZ().p4.Pt();
				}
			},
			{
				"zeta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetRefZ().p4.Eta();
				}
			},
			{
				"zphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetRefZ().p4.Phi();
				}
			},
			{
				"zy", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetRefZ().p4.Rapidity();
				}
			},
			{
				"zmass", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetRefZ().p4.mass();
				}
			},			{
				"genzpt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -1;
					if (product.m_genZs.size() >= 1)
						value =  product.m_genZs[0].p4.Pt();
					return value;
				}
			},
			{
				"genzy", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.m_genZs.size() < 1)
						return -1;
					return product.m_genZs[0].p4.Rapidity();
				}
			},
			{
				"genzmass", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.m_genZs.size() < 1)
						return -1;
					return product.m_genZs[0].p4.mass();
				}
			},
			{
				"nzs", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genZs.size();
				}
			},
			{
				"deltaRzgenz", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.m_genZs.size() < 1)
						return -1;

					return ROOT::Math::VectorUtil::DeltaR(product.m_genZs[0].p4,
					product.GetRefZ().p4);
				}
			},
		};
		return lambdaMap;
	};

};
}
