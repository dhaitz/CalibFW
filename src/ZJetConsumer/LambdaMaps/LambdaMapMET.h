#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapMET: public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"METpt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMet(event, s)->p4.Pt();
				}
			},
			{
				"METphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMet(event, s)->p4.Phi();
				}
			},
			{
				"sumEt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMet(event, s)->sumEt;
				}
			},
			{
				"rawMETpt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.GetMet(s)->p4.Pt();
				}
			},
			{
				"rawMETphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.GetMet(s)->p4.Phi();
				}
			},
			{
				"mpf", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMPF(product.GetMet(event, s));
				}
			},
			{
				"rawmpf", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetMPF(event.GetMet(s));
				}
			},
		};
		return lambdaMap;
	};

};
}
