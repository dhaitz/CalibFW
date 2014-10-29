#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"

#include "LambdaMaps/LambdaMapGeneral.h"
#include "LambdaMaps/LambdaMapMCGenerator.h"
#include "LambdaMaps/LambdaMapMuons.h"
#include "LambdaMaps/LambdaMapZ.h"
#include "LambdaMaps/LambdaMapElectrons.h"
#include "LambdaMaps/LambdaMapJets.h"
#include "LambdaMaps/LambdaMapMET.h"
#include "LambdaMaps/LambdaMapTagger.h"



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

	void Init(PipelineTypeForThis* pset)
	{
		// set up NTuple and fetch config parameters
		EventConsumerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>::Init(pset);
		std::string quantities = this->GetPipelineSettings().GetQuantitiesString();
		m_ntuple = new TNtuple("NTuple", "NTuple", quantities.c_str());
		stringvector quantitiesvector = this->GetPipelineSettings().GetQuantities();

		InitializeLambdaMap();

		// build lambda function vector
		for (const auto & quantity : quantitiesvector)
		{
			if (m_lambdaMap.find(quantity) == m_lambdaMap.end())
				LOG_FATAL("The quantity '" << quantity << "' could not be found in the LambdaMap!")
				else
					m_lambdaVector.emplace_back(m_lambdaMap[quantity]);
		}
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
									  ZJetProduct const& product)
	{
		EventConsumerBase< ZJetEventData, ZJetProduct, ZJetPipelineSettings>::ProcessFilteredEvent(event, product);

		std::vector<float> array;

		//iterate over lambda function vector and fill the float array for each function
		for (const auto & function : m_lambdaVector)
			array.emplace_back(function(event, product, this->GetPipelineSettings()));

		// add the array to the ntuple
		m_ntuple->Fill(&array[0]);
	}

	virtual void Finish()
	{
		m_ntuple->Write(this->GetPipelineSettings().GetName().c_str());
	}


private:

	TNtuple* m_ntuple;
	ZJetLambdaMap m_lambdaMap;
	std::vector<ZJetLambdaFunction> m_lambdaVector;

	void InitializeLambdaMap()
	{
		std::vector<ZJetLambdaMap> mapvector;

		mapvector.emplace_back(LambdaMapMCGenerator().GetMap());
		mapvector.emplace_back(LambdaMapMuons().GetMap());
		mapvector.emplace_back(LambdaMapZ().GetMap());
		mapvector.emplace_back(LambdaMapGeneral().GetMap());
		mapvector.emplace_back(LambdaMapElectrons().GetMap());
		mapvector.emplace_back(LambdaMapJets().GetMap());
		mapvector.emplace_back(LambdaMapMET().GetMap());
		mapvector.emplace_back(LambdaMapTagger().GetMap());

		// build the map from all the small maps
		for (const auto & map : mapvector)
			m_lambdaMap.insert(map.begin(), map.end());

	}

};


}
