#include "Pipeline/JetTools.h"

#include "ZJet/ZJetPipelineInitializer.h"

#include "ZJet/MetaDataProducer/ZJetCuts.h"
#include "ZJet/MetaDataProducer/PuReweightingProducer.h"
#include "ZJet/MetaDataProducer/ZJetMetaDataProducer.h"
#include "ZJet/MetaDataProducer/HltSelector.h"

#include "ZJet/Consumer/ZJetDrawConsumer.h"
#include "ZJet/Consumer/CutStatistics.h"
#include "ZJet/Consumer/FilterStatistics.h"
#include "ZJet/Consumer/GenericProfileConsumer.h"
#include "ZJet/Consumer/GenericTwoDConsumer.h"
#include "ZJet/Consumer/JetRespConsumer.h"
#include "ZJet/Consumer/Dumper.h"
#include "ZJet/Consumer/BinResponseConsumer.h"
#include "ZJet/Consumer/MetaConsumerDataLV.h"
#include "ZJet/Consumer/JetMatchingPlots.h"

#include "ZJet/Filter/ValidZFilter.h"
#include "ZJet/Filter/InCutFilter.h"
#include "ZJet/Filter/PtWindowFilter.h"
#include "ZJet/Filter/JsonFilter.h"
#include "ZJet/Filter/HltFilter.h"
#include "ZJet/Filter/RunRangeFilter.h"
#include "ZJet/Filter/NpvFilter.h"
#include "ZJet/Filter/JetEtaFilter.h"
#include "ZJet/Filter/ResponseFilter.h"
#include "ZJet/Filter/DeltaEtaFilter.h"

using namespace CalibFW;

void ZJetPipelineInitializer::InitPipeline(EventPipeline<ZJetEventData, ZJetMetaData, ZJetPipelineSettings> * pLine,
		ZJetPipelineSettings const& pset) const
{

	if ( pset.GetLevel() == 1 )
	{
		// load filter from the Settings and add them
		stringvector fvec = pset.GetFilter();
		BOOST_FOREACH( std::string sid, fvec )
		{		// make this more beatiful :)

			if ( sid == PtWindowFilter().GetFilterId())
				pLine->AddFilter( new PtWindowFilter);
			else if ( sid == JsonFilter().GetFilterId())
				pLine->AddFilter( new JsonFilter( pset.Global()->GetJsonFile()));
			else if ( sid == InCutFilter().GetFilterId())
				pLine->AddFilter( new InCutFilter);
			else if ( sid == ValidZFilter().GetFilterId())
				pLine->AddFilter( new ValidZFilter);
			else if ( sid == ValidJetFilter().GetFilterId())
				pLine->AddFilter( new ValidJetFilter);
			else if ( sid == NpvFilter().GetFilterId())
				pLine->AddFilter( new NpvFilter() );
			else if ( sid == JetEtaFilter().GetFilterId())
				pLine->AddFilter(new JetEtaFilter());
			else if ( sid == HltFilter().GetFilterId())
				pLine->AddFilter( new HltFilter);
			else if ( sid == RunRangeFilter().GetFilterId())
				pLine->AddFilter( new RunRangeFilter);
			else if ( sid == ResponseFilter().GetFilterId())
				pLine->AddFilter( new ResponseFilter);
			else if ( sid == DeltaEtaFilter().GetFilterId())
				pLine->AddFilter( new DeltaEtaFilter);
			else
				CALIB_LOG_FATAL( "Filter " << sid << " not found." )
		}

		// Cuts
		fvec = pset.GetCuts();
		BOOST_FOREACH( std::string sid, fvec )
		{		// make this more beautiful :)
			if ( sid == LeadingJetEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer( new LeadingJetEtaCut() );

			else if ( sid == SecondLeadingToZPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new SecondLeadingToZPtCut() );

			else if ( sid == SecondLeadingToZPtRegionCut().GetCutShortName())
				pLine->AddMetaDataProducer( new SecondLeadingToZPtRegionCut() );

			else if ( sid == MuonPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new MuonPtCut() );

			else if ( sid == MuonEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer( new MuonEtaCut() );

			else if ( sid == ZMassWindowCut().GetCutShortName())
				pLine->AddMetaDataProducer( new ZMassWindowCut() );

			else if ( sid == BackToBackCut().GetCutShortName())
				pLine->AddMetaDataProducer( new BackToBackCut() );

			else if ( sid == ZPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new ZPtCut() );
			// VBF
			else if ( sid == LeadingJetPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new LeadingJetPtCut() );

			else if ( sid == SecondJetPtCut().GetCutShortName())
				pLine->AddMetaDataProducer( new SecondJetPtCut() );

			else if ( sid == SecondJetEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer( new SecondJetEtaCut() );

			else if ( sid == RapidityGapCut().GetCutShortName())
				pLine->AddMetaDataProducer( new RapidityGapCut() );

			else if ( sid == InvariantMassCut().GetCutShortName())
				pLine->AddMetaDataProducer( new InvariantMassCut() );

			else if ( sid == DeltaEtaCut().GetCutShortName())
				pLine->AddMetaDataProducer( new DeltaEtaCut() );


			else {
			}
		}

		// Other MetaDataProducers
		/*
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
		pset.GetPropTree()->get_child( pset.GetSettingsRoot() + ".Consumer") )
		{
			std::string sName = v.second.get<std::string>("Name");
			std::string consPath = pset.GetSettingsRoot() + ".MetaDataProducer." + v.first.data();

			if ( sName == HltSelector::GetName() )
				pLine->AddMetaDataProducer( new HltSelector(  pset.GetPropTree(), consPath  ) );
			else
				CALIB_LOG_FATAL( "MetaDataProducer " << sName << " not found." )
		}*/
	}

	// add this for debugging output ...
	//pLine->AddConsumer( new Dumper() );

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
			pset.GetPropTree()->get_child( pset.GetSettingsRoot() + ".Consumer") )
	{
		std::string sName = v.second.get<std::string>("Name");
		std::string consPath = pset.GetSettingsRoot() + ".Consumer." + v.first.data();

		if (sName == "quantities_all" )
		{
			pLine->AddConsumer( new DataZConsumer( pset.GetJetAlgorithm() ));

			pLine->AddConsumer( new DataMuonConsumer(+1, pset.GetJetAlgorithm()));
			pLine->AddConsumer( new DataMuonConsumer(-1, pset.GetJetAlgorithm()));

			pLine->AddConsumer( new ValidMuonsConsumer());
			pLine->AddConsumer( new ValidJetsConsumer());

			pLine->AddConsumer( new DataMETConsumer( pset.GetJetAlgorithm() ));

			pLine->AddConsumer( new DeltaConsumer( pset.GetJetAlgorithm() ));

			
			pLine->AddConsumer( new PrimaryVertexConsumer( ) );

			if ( JetType::IsPF( pset.GetJetAlgorithm() ))
			{
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 0));
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 1, pset.GetJetAlgorithm(), true));
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 2, pset.GetJetAlgorithm(), true));

				if ( JetType::IsRaw ( pset.GetJetAlgorithm() ))
				{
					// plot the invalid jet 0
					pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(),
							0, pset.GetJetAlgorithm(), false, false, "jets_invalid_" ));
				}
			}

			if ( pset.IsMC() && (( pset.GetJetAlgorithm() == "AK5PFJetsL1L2L3" ) || ( pset.GetJetAlgorithm() == "AK7PFJetsL1L2L3" ) ) )
			{
				std::string genName = JetType::GetGenName( pset.GetJetAlgorithm() );

				// add gen jets plots
				pLine->AddConsumer( new DataGenJetConsumer( genName, 0,
									    genName));
				pLine->AddConsumer( new DataGenJetConsumer( genName, 1,
									    genName ));
				pLine->AddConsumer( new DataGenJetConsumer( genName, 2,
									    genName));

				pLine->AddConsumer( new DeltaConsumer( genName ));

			}


			if (pset.IsMC())
			{	std::string genName = JetType::GetGenName( pset.GetJetAlgorithm() );
				//pLine->AddConsumer( new BasicTwoDConsumer( genName ));
				pLine->AddConsumer( new GenMetadataConsumer( ) );
				// rate the matching
				pLine->AddConsumer( new JetMatchingConsumer( ) );
			}
			else
			{
				pLine->AddConsumer( new MetadataConsumer( ) );
			}
		}

		else if (sName == "quantities_basic" )
		{
			pLine->AddConsumer( new DataZConsumer( pset.GetJetAlgorithm() ));
			pLine->AddConsumer( new PrimaryVertexConsumer( ) );

			if ( JetType::IsPF( pset.GetJetAlgorithm() ))
			{
				pLine->AddConsumer( new DataPFJetsConsumer( pset.GetJetAlgorithm(), 0));
			}
		}



		// optional 1st Level Producer
		else if (sName == BinResponseConsumer::GetName())
		{
			BinResponseConsumer * resp = new BinResponseConsumer( pset.GetPropTree(), consPath );
			pLine->AddConsumer( resp );
			if ( pset.IsMC() && (resp->m_jetnum == 0) )
			{
				// do gen gesponse to z.pt
				BinResponseConsumer * gen = new BinResponseConsumer(pset.GetPropTree(), consPath);
				gen->m_useGenJet = true;
				gen->m_name.insert(0,"Gen_");
				pLine->AddConsumer(gen);

				// do Reco To Gen response
				BinResponseConsumer * reco_to_gen = new BinResponseConsumer(pset.GetPropTree(), consPath);
				reco_to_gen->m_useGenJetAsReference = true;
				reco_to_gen->m_name.insert(0,"RecoToGen_");
				pLine->AddConsumer(reco_to_gen);

			}
		}
		
		else if (sName == BasicTwoDConsumer::GetName())		
			pLine->AddConsumer( new BasicTwoDConsumer( pset.GetJetAlgorithm() ));

		else if (sName == GenericProfileConsumer::GetName())
			pLine->AddConsumer( new GenericProfileConsumer( pset.GetPropTree(), consPath ) );

		else if (sName == GenericTwoDConsumer::GetName())
			pLine->AddConsumer( new GenericTwoDConsumer( pset.GetPropTree(), consPath ) );

		else if (sName == CutStatisticsConsumer::GetName())
			pLine->AddConsumer( new CutStatisticsConsumer());

		else if (sName == FilterStatisticsConsumer::GetName())
			pLine->AddConsumer( new FilterStatisticsConsumer());

		// 2nd Level Producer
		else if( sName == JetRespConsumer::GetName() )
			pLine->AddConsumer( new JetRespConsumer( pset.GetPropTree(), consPath ) );
		else
			CALIB_LOG_FATAL( "Consumer " << sName << " not found." )
	}

}
