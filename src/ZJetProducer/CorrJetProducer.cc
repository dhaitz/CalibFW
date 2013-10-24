#include "ZJetProducer/CorrJetProducer.h"

#include <boost/algorithm/string/predicate.hpp>

namespace CalibFW
{

CorrJetProducer::CorrJetProducer(std::string corBase, std::string l1cor, std::vector<std::string> baseAlgos) :
	ZJetGlobalMetaDataProducerBase(), m_corectionFileBase(corBase),
	m_l1correction(l1cor), m_basealgorithms(baseAlgos)
{
	CALIB_LOG_FILE("Loading JEC from " << m_corectionFileBase);
}


void CorrJetProducer::PopulateMetaData(ZJetEventData const& data,
		ZJetMetaData& metaData, ZJetPipelineSettings const& m_pipelineSettings) const
{
	// nothing to do here
}


void CorrJetProducer::InitCorrection(std::string algoName, std::string algoCorrectionAlias,
		ZJetEventData const& event, double rcorr, std::string prefix) const
{
	if (m_corrService.find(algoName) != m_corrService.end())
		// already loaded
		return;

	CALIB_LOG_FILE("Jet corrections enabled for " << algoName << " jets");
	std::cout << "    Levels: ";
	std::vector<std::string> corLevel;
	if (prefix == "")
		prefix = m_corectionFileBase;

	corLevel.push_back(m_l1correction);
	m_corrService.insert(algoCorrectionAlias, new JecCorrSet());
	m_corrService[algoCorrectionAlias].m_l1.reset(new JECService(
			event.m_vertexSummary, event.m_jetArea, prefix, corLevel, algoName,
			event.m_eventmetadata, 0, 0, 0)
	);

	// only apply one correction step in a round!
	corLevel.clear();
	corLevel.push_back("L2Relative");
	m_corrService[algoCorrectionAlias].m_l2.reset(new JECService(
			event.m_vertexSummary, event.m_jetArea, prefix, corLevel, algoName,
			event.m_eventmetadata, 0, 0, 0)
	);

	// only apply one correction step in a round!
	corLevel.clear();
	corLevel.push_back("L3Absolute");
	m_corrService[algoCorrectionAlias].m_l3.reset(new JECService(
			event.m_vertexSummary, event.m_jetArea, prefix, corLevel, algoName,
			event.m_eventmetadata, 0, 0, 0)
	);

	// only used for data
	corLevel.clear();
	corLevel.push_back("L2L3Residual");
	m_corrService[algoCorrectionAlias].m_l2l3res.reset(new JECService(
			event.m_vertexSummary, event.m_jetArea, prefix, corLevel, algoName,
			event.m_eventmetadata, 0, 0, rcorr)
	);
	CALIB_LOG_FILE("");
}


void CorrJetProducer::CreateCorrections(
	std::string algoName,
	std::string algoPostfix,
	ZJetEventData const& event,
	ZJetMetaData& metaData,
	ZJetPipelineSettings const& settings,
	std::string algoCorrectionAlias) const
{
	std::string algoName_raw = algoName + algoPostfix;
	std::string algoName_l1 = algoName_raw + "L1";
	std::string algoName_l2 = algoName_raw + "L1L2";
	std::string algoName_l3 = algoName_raw + "L1L2L3";
	std::string algoName_l3res = algoName_raw + "L1L2L3Res";

	CorrectJetCollection(algoName_raw, algoName_l1,
						 this->m_corrService.at(algoCorrectionAlias).m_l1,
						 event, metaData, settings);
	CorrectJetCollection(algoName_l1, algoName_l2,
						 this->m_corrService.at(algoCorrectionAlias).m_l2,
						 event, metaData, settings);
	CorrectJetCollection(algoName_l2, algoName_l3,
						 this->m_corrService.at(algoCorrectionAlias).m_l3,
						 event, metaData, settings);

	// residual for data
	if (settings.Global()->GetInputType() == DataInput)
	{
		CorrectJetCollection(algoName_l3, algoName_l3res,
							 this->m_corrService.at(algoCorrectionAlias).m_l2l3res,
							 event, metaData, settings);
	}
}


bool CorrJetProducer::PopulateGlobalMetaData(ZJetEventData const& event,
		ZJetMetaData& metaData, ZJetPipelineSettings const& settings) const
{
	for (int i = 0; i < m_basealgorithms.size(); i++)
	{
		double runcorr = settings.Global()->GetHcalCorrection();
		if (m_basealgorithms[i].find("chs") == std::string::npos)
		{
			InitCorrection(m_basealgorithms[i], m_basealgorithms[i], event, runcorr);
			CreateCorrections(m_basealgorithms[i], "Jets", event, metaData, settings, m_basealgorithms[i]);
		}
		else
		{
			InitCorrection(m_basealgorithms[i], m_basealgorithms[i], event, runcorr);
			CreateCorrections(m_basealgorithms[i].substr(0, 5), "JetsCHS", event, metaData, settings, m_basealgorithms[i]);
		}
	}
	return true;
}


void CorrJetProducer::CorrectJetCollection(
	std::string algoName,
	std::string newAlgoName,
	boost::scoped_ptr<JECService> const& corrService,
	ZJetEventData const& event,
	ZJetMetaData& metaData,
	ZJetPipelineSettings const& settings) const
{
	unsigned int jetcount = metaData.GetValidJetCount(settings, event, algoName);

	// copy the jet collection
	for (unsigned int i = 0; i < jetcount; ++i)
	{
		KDataPFTaggedJet* jet = static_cast<KDataPFTaggedJet*>(
				metaData.GetValidJet(settings, event, i, algoName));

		// create a copy
		KDataPFTaggedJet jet_corr = *jet;
		metaData.AddValidJet(jet_corr, newAlgoName);
	}

	// correct the copied jet collection
	corrService->correct(&metaData.m_validPFJets[newAlgoName]);
}

}


