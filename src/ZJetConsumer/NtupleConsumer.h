#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "ZJetEventPipeline/Product.h"
#include "LambdaMaps/BaseLambdaMap.h"


namespace Artus
{

template <class TEvent, class TProduct, class TSettings>
class NtupleConsumerBase : public EventConsumerBase<TEvent, TProduct, TSettings>
{
public:
	typedef EventPipeline<TEvent, TProduct, TSettings> PipelineTypeForThis;
};

/*
In this class, the creation of the final NTuple is managed.

A large map (string, lambdafunction) is created from various smaller maps in
the InitializeLambdaMap member function.

Then, according to the 'quantities' configuration, the corresponding lambda functions
for each quantity is taken from the map and pushed back into a vector.

During the event processing, each item of the vector, i.e. each lambda function,
is executed, giving the values for the final NTuple.

*/
class NtupleConsumer : public NtupleConsumerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
{
public:
	static std::string GetName()
	{
		return "ntuple";
	}

	void Init(PipelineTypeForThis* pset);

	void ProcessFilteredEvent(ZJetEventData const& event,
							  ZJetProduct const& product);

	void Finish();


private:

	TNtuple* m_ntuple;
	ZJetLambdaMap m_lambdaMap;
	std::vector<ZJetLambdaFunction> m_lambdaVector;

	void InitializeLambdaMap();

};


}
