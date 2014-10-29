#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapGeneral : public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"npv", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.m_vertexSummary->nVertices;
				}
			},
			{
				"npu", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.m_geneventproduct->numPUInteractions0;
				}
			},
			{
				"nputruth", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (s.IsMC())
						return event.m_geneventproduct->numPUInteractionsTruth;
					return product.GetNpuTruth();
				},
			},
			{
				"rho", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.m_jetArea->median;
				}
			},
			{
				"run", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.m_eventproduct->nRun;
				}
			},
			{
				"weight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetWeight();
				}
			},
			{
				"eff", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetEfficiency();
				}
			},
			{
				"eventnr", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.m_eventproduct->nEvent;
				}
			},
			{
				"lumisec", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return event.m_eventproduct->nLumi;
				}
			},
			{
				"hlt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					if (product.GetSelectedHlt().empty()) // no HLT found
						return 0.;

					return event.m_eventproduct->hltFired(product.GetSelectedHlt(), event.m_lumiproduct);
				}

			},

		};
		return lambdaMap;
	};

};
}
