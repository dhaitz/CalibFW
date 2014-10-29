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
				"uept", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetUE(event, s)->p4.Pt();
				}
			},
			{
				"uephi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetUE(event, s)->p4.Phi();
				}
			},
			{
				"ueeta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetUE(event, s)->p4.Eta();
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
			{
				"otherjetspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetValidJetCount(s, event) < 2)
						return 0.;

					return (-(product.GetRefZ().p4
					+ product.GetValidPrimaryJet(s, event)->p4
					+ product.GetMet(event, s)->p4
					+ product.GetValidJet(s, event, 1)->p4
					+ product.GetUE(event, s)->p4
							 )).Pt();
				}
			},
			{
				"otherjetsphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetValidJetCount(s, event) < 2)
						return 0.;

					return (-(product.GetRefZ().p4
					+ product.GetValidPrimaryJet(s, event)->p4
					+ product.GetMet(event, s)->p4
					+ product.GetValidJet(s, event, 1)->p4
					+ product.GetUE(event, s)->p4
							 )).Phi();
				}
			},
			{
				"otherjetseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetValidJetCount(s, event) < 2)
						return 0.;

					return (-(product.GetRefZ().p4
					+ product.GetValidPrimaryJet(s, event)->p4
					+ product.GetMet(event, s)->p4
					+ product.GetValidJet(s, event, 1)->p4
					+ product.GetUE(event, s)->p4
							 )).Eta();
				}
			},
		};
		return lambdaMap;
	};

};
}
