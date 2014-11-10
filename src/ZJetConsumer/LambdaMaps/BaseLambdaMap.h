#pragma once

#include <functional>
#include "ZJetEventPipeline/Product.h"


namespace Artus
{

typedef std::function<float(ZJetEventData const&, ZJetProduct const&, ZJetPipelineSettings const&)> ZJetLambdaFunction;
typedef std::map<std::string, ZJetLambdaFunction> ZJetLambdaMap;

class BaseLambdaMap {};
}
