#include "NtupleConsumer.h"

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
	void NtupleConsumer::Init(PipelineTypeForThis* pset)
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

	void NtupleConsumer::ProcessFilteredEvent(ZJetEventData const& event,
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

	void NtupleConsumer::Finish()
	{
		m_ntuple->Write(this->GetPipelineSettings().GetName().c_str());
	}

	void NtupleConsumer::InitializeLambdaMap()
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
}
