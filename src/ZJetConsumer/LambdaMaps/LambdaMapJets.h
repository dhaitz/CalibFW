#pragma once

#include "BaseLambdaMap.h"

namespace Artus
{
class LambdaMapJets: public BaseLambdaMap
{
public:
	static const ZJetLambdaMap GetMap();
};
}
