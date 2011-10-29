#include "CorrJetProducer.h"

namespace CalibFW
{

CorrJetProducer::CorrJetProducer( std::string corBase) :
		 //"data/jec_data/MC_42_V13_";
	ZJetMetaDataProducerBase(),
			m_corectionFileBase ( corBase )
{
	std::vector<std::string> corLevel;

	CALIB_LOG( "Loading JEC from " + m_corectionFileBase)
	InitCorrection( "AK5PF" );
	InitCorrection( "AK7PF" );
}

void CorrJetProducer::PopulateMetaData(ZJetEventData const& data,
		ZJetMetaData & metaData, ZJetPipelineSettings const& m_pipelineSettings) const
{
	// nothing to do here
}


void CorrJetProducer::InitCorrection( std::string algoName )
{
	std::vector<std::string> corLevel;
	std::string prefix = m_corectionFileBase + algoName + "_";


	corLevel.push_back("L1FastJet");
	//corLevel.push_back("L1Offset");


	m_corrService[ algoName ].m_l1 = std::shared_ptr<JECService>(new JECService(
			prefix, corLevel, -1.0) // -1.0 takes the area of the jet from FastJet calculation
			);

	// only apply one correction step in a round !
	corLevel.clear();
	corLevel.push_back("L2Relative");

	m_corrService[ algoName ].m_l2 = std::shared_ptr<JECService>(new JECService(
			prefix, corLevel,-1.0) // -1.0 takes the area of the jet from FastJet calculation
			);

	// only apply one correction step in a round !
	corLevel.clear();
	corLevel.push_back("L3Absolute");

	m_corrService[ algoName ].m_l3 = std::shared_ptr<JECService>(new JECService(
			prefix, corLevel, -1.0) // -1.0 takes the area of the jet from FastJet calculation
		);

	// only used for data
	corLevel.clear();
	corLevel.push_back("L2L3Residual");

	m_corrService[ algoName ].m_l2l3res = std::shared_ptr<JECService>(new JECService(
			prefix, corLevel,-1.0) // -1.0 takes the area of the jet from FastJet calculation
			);
}

void CorrJetProducer::CreateCorrections( std::string algoName,
		ZJetEventData const& event,
		ZJetMetaData & metaData,
		ZJetPipelineSettings const& settings) const
{

	std::string algoName_raw =algoName + "Jets";
	std::string algoName_l1 = algoName_raw + "L1";
	std::string algoName_l2 = algoName_raw + "L1L2";
	std::string algoName_l3 = algoName_raw + "L1L2L3";
	std::string algoName_l3res = algoName_raw + "L1L2L3Res";


	CorrectJetCollection( algoName_raw, algoName_l1,
				this->m_corrService.at( algoName ).m_l1,
				event,
				metaData,
				settings );

	CorrectJetCollection( algoName_l1, algoName_l2,
				this->m_corrService.at( algoName ).m_l2,
				event,
				metaData,
				settings );
	CorrectJetCollection( algoName_l2, algoName_l3,
				this->m_corrService.at( algoName ).m_l3,
				event,
				metaData,
				settings );

	// residual for data
	if (settings.Global()->GetInputType() == DataInput )
	{
		//CALIB_LOG_FATAL("Implement this ")
		// do l2l3res here
		CorrectJetCollection( algoName_l3, algoName_l3res,
					this->m_corrService.at( algoName ).m_l2l3res,
					event,
					metaData,
					settings );
	}
}

void CorrJetProducer::PopulateGlobalMetaData(ZJetEventData const& event,
		ZJetMetaData & metaData, ZJetPipelineSettings const& settings) const
{
	CreateCorrections( "AK5PF", event, metaData, settings );
	CreateCorrections( "AK7PF", event, metaData, settings );

}

void CorrJetProducer::CorrectJetCollection( std::string algoName, std::string newAlgoName,
				std::shared_ptr< JECService > corrService,
				ZJetEventData const& event,
				ZJetMetaData & metaData,
				ZJetPipelineSettings const& settings) const
{
	unsigned int jetcount = metaData.GetValidJetCount(settings,
			algoName);

	for (unsigned int i = 0; i < jetcount; ++i)
	{
		KDataPFJet * jet = static_cast<KDataPFJet*> (metaData.GetValidJet(
				settings, event, i, algoName ));

		// create a copy
		KDataPFJet jet_corr = *jet;
		metaData.AddValidJet(jet_corr, newAlgoName);
	}

	corrService->correct(
			&metaData.m_validPFJets[newAlgoName],
			event.m_vertexSummary, event.m_jetArea);
}



}


