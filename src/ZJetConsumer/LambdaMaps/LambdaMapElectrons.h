#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapElectrons: public BaseLambdaMap
{
public:
	static ZJetLambdaMap GetMap()
	{
		ZJetLambdaMap lambdaMap =
		{
			{
				"nelectrons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.GetValidElectrons().size();
				}
			},
			{
				"emass", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadinge.p4.mass();
				}
			},
			{
				"ept", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadinge.p4.Pt();
				}
			},
			{
				"eeta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadinge.p4.Eta();
				}
			},
			{
				"eminusmass", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.p4.mass();
				}
			},
			{
				"eminuspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.p4.Pt();
				}
			},
			{
				"eminuseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.p4.Eta();
				}
			},
			{
				"eminusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.p4.Phi();
				}
			},
			{
				"eplusmass", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.p4.mass();
				}
			},
			{
				"epluspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.p4.Pt();
				}
			},
			{
				"epluseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.p4.Eta();
				}
			},
			{
				"eplusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.p4.Phi();
				}


			},
			{
				"eminusiso", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.trackIso03;
				}
			},
			{
				"eplusiso", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.trackIso03;
				}
			},
			{
				"eminusecaliso03", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.ecalIso03;
				}
			},
			{
				"eplusecaliso03", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.ecalIso03;
				}
			},
			{
				"eminusecaliso04", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.ecalIso04;
				}
			},
			{
				"eplusecaliso04", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.ecalIso04;
				}
			},
			{
				"eminusid", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.idMvaNonTrigV0;
				}
			},
			{
				"eplusid", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.idMvaNonTrigV0;
				}
			},
			{
				"eminustrigid", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.idMvaTrigV0;
				}
			},
			{
				"eplustrigid", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.idMvaTrigV0;
				}
			},
			{
				"genepluspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genInternalElectrons)
						if (it.charge() > 0) value = it.p4.Pt();
					return value;
				}
			},
			{
				"genepluseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genInternalElectrons)
						if (it.charge() > 0) value = it.p4.Eta();
					return value;
				}
			},
			{
				"geneplusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genInternalElectrons)
						if (it.charge() > 0) value = it.p4.Phi();
					return value;
				}
			},
			{
				"geneminuspt", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genInternalElectrons)
						if (it.charge() < 0) value = it.p4.Pt();
					return value;
				}
			},
			{
				"geneminuseta", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genInternalElectrons)
						if (it.charge() < 0) value = it.p4.Eta();
					return value;
				}
			},
			{
				"geneminusphi", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					float value = -999;
					for (const auto & it : product.m_genInternalElectrons)
						if (it.charge() < 0) value = it.p4.Phi();
					return value;
				}
			},
			{
				"ngenelectrons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genElectrons.size();
				}
			},
			{
				"ngeninternalelectrons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genInternalElectrons.size();
				}
			},
			{
				"ngenintermediateelectrons", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.m_genIntermediateElectrons.size();
				}
			},
			{
				"sf", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.scalefactor;
				}
			},
			{
				"sfplus", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.sfplus;
				}
			},
			{
				"sfminus", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.sfminus;
				}
			},
			{
				"eidveto", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.electronidveto;
				}
			},
			{
				"eplusidloose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.cutbasedIDloose;
				}
			},
			{
				"eplusidmedium", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.cutbasedIDmedium;
				}
			},
			{
				"eplusidtight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.cutbasedIDtight;
				}
			},
			{
				"eplusidveto", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeplus.cutbasedIDveto;
				}
			},
			{
				"eminusidloose", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.cutbasedIDloose;
				}
			},
			{
				"eminusidmedium", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.cutbasedIDmedium;
				}
			},
			{
				"eminusidtight", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.cutbasedIDtight;
				}
			},
			{
				"eminusidveto", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{
					return product.leadingeminus.cutbasedIDveto;
				}
			},
			{
				"deltaReplusgeneplus", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{

					for (const auto & it : product.m_genInternalElectrons)
					{
						if (it.charge() > 0)
							return ROOT::Math::VectorUtil::DeltaR(product.leadingeplus.p4, it.p4);
					}
					return 999;
				}
			},
			{
				"deltaReminusgeneminus", [](ZJetEventData const & event, ZJetProduct const & product, ZJetPipelineSettings const & s) -> float
				{

					for (const auto & it : product.m_genInternalElectrons)
					{
						if (it.charge() < 0)
							return ROOT::Math::VectorUtil::DeltaR(product.leadingeminus.p4, it.p4);
					}
					return 999;
				}
			}
		};
		return lambdaMap;
	};

};
}
