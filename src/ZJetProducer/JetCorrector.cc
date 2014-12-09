#include "ZJetProducer/JetCorrector.h"

#include <boost/algorithm/string/predicate.hpp>

namespace Artus
{

JetCorrector::JetCorrector(std::string corBase, std::string l1cor, std::vector<std::string> baseAlgos, bool rc, bool flavour) :
	ZJetGlobalProductProducerBase(), m_corectionFileBase(corBase),
	m_l1correction(l1cor), m_basealgorithms(baseAlgos), m_rc(rc), m_flavour(flavour)
{
	LOG_FILE("Loading JEC from " << m_corectionFileBase);
}


void JetCorrector::PopulateProduct(ZJetEventData const& data,
								   ZJetProduct& product, ZJetPipelineSettings const& m_pipelineSettings) const
{
	// nothing to do here
}


void JetCorrector::InitCorrection(std::string algoName, std::string algoCorrectionAlias,
								  ZJetEventData const& event, double rcorr, std::string prefix) const
{
	if (m_corrService.find(algoName) != m_corrService.end())
		// already loaded
		return;

	LOG_FILE("Jet corrections enabled for " << algoName << " jets");
	std::cout << "    Levels: ";
	std::vector<std::string> corLevel;
	if (prefix == "")
		prefix = m_corectionFileBase;

	corLevel.emplace_back(m_l1correction);
	m_corrService.insert(algoCorrectionAlias, new JecCorrSet());
	m_corrService[algoCorrectionAlias].m_l1.reset(new JECService(
				event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
				prefix, corLevel, algoName, 0, 0, 0)
												 );

	if (m_rc)
	{
		//additional RC correction (for type-I)
		corLevel.clear();
		corLevel.emplace_back("RC");
		m_corrService.insert(algoCorrectionAlias, new JecCorrSet());
		m_corrService[algoCorrectionAlias].m_rc.reset(new JECService(
					event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
					prefix, corLevel, algoName, 0, 0, 0)
													 );
	}

	// only apply one correction step in a round!
	corLevel.clear();
	corLevel.emplace_back("L2Relative");
	m_corrService[algoCorrectionAlias].m_l2.reset(new JECService(
				event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
				prefix, corLevel, algoName, 0, 0, 0)
												 );

	// only apply one correction step in a round!
	corLevel.clear();
	corLevel.emplace_back("L3Absolute");
	m_corrService[algoCorrectionAlias].m_l3.reset(new JECService(
				event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
				prefix, corLevel, algoName, 0, 0, 0)
												 );

	// flavour-dependent corrections
	if (m_flavour)
	{
		corLevel.clear();
		corLevel.emplace_back("L5Flavor_qJ");
		m_corrService[algoCorrectionAlias].m_l5q.reset(new JECService(
					event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
					prefix, corLevel, algoName, 0, 0, 0)
													  );
		corLevel.clear();
		corLevel.emplace_back("L5Flavor_sJ");
		m_corrService[algoCorrectionAlias].m_l5s.reset(new JECService(
					event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
					prefix, corLevel, algoName, 0, 0, 0)
													  );
		corLevel.clear();
		corLevel.emplace_back("L5Flavor_cJ");
		m_corrService[algoCorrectionAlias].m_l5c.reset(new JECService(
					event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
					prefix, corLevel, algoName, 0, 0, 0)
													  );
		corLevel.clear();
		corLevel.emplace_back("L5Flavor_bJ");
		m_corrService[algoCorrectionAlias].m_l5b.reset(new JECService(
					event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
					prefix, corLevel, algoName, 0, 0, 0)
													  );
		corLevel.clear();
		corLevel.emplace_back("L5Flavor_gJ");
		m_corrService[algoCorrectionAlias].m_l5g.reset(new JECService(
					event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
					prefix, corLevel, algoName, 0, 0, 0)
													  );
	}


	// only used for data
	corLevel.clear();
	corLevel.emplace_back("L2L3Residual");
	m_corrService[algoCorrectionAlias].m_l2l3res.reset(new JECService(
				event.m_vertexSummary, event.m_jetArea, event.m_eventproduct,
				prefix, corLevel, algoName, 0, 0, rcorr)
													  );
	LOG_FILE("");
}


void JetCorrector::CreateCorrections(
	std::string algoName,
	std::string algoPostfix,
	ZJetEventData const& event,
	ZJetProduct& product,
	ZJetPipelineSettings const& settings,
	std::string algoCorrectionAlias) const
{
	std::string algoName_raw = algoName + algoPostfix;
	std::string algoName_l1 = algoName_raw + "L1";
	std::string algoName_rc = algoName_raw + "RC";
	std::string algoName_l2 = algoName_raw + "L1L2";
	std::string algoName_l3 = algoName_raw + "L1L2L3";
	std::string algoName_l5q = algoName_raw + "L1L2L3L5q";
	std::string algoName_l5s = algoName_raw + "L1L2L3L5s";
	std::string algoName_l5g = algoName_raw + "L1L2L3L5g";
	std::string algoName_l5c = algoName_raw + "L1L2L3L5c";
	std::string algoName_l5b = algoName_raw + "L1L2L3L5b";
	std::string algoName_l3res = algoName_raw + "L1L2L3Res";

	CorrectJetCollection(algoName_raw, algoName_l1,
						 this->m_corrService.at(algoCorrectionAlias).m_l1,
						 event, product, settings);
	if (m_rc)
	{
		CorrectJetCollection(algoName_raw, algoName_rc,
							 this->m_corrService.at(algoCorrectionAlias).m_rc,
							 event, product, settings);
	}
	CorrectJetCollection(algoName_l1, algoName_l2,
						 this->m_corrService.at(algoCorrectionAlias).m_l2,
						 event, product, settings);
	CorrectJetCollection(algoName_l2, algoName_l3,
						 this->m_corrService.at(algoCorrectionAlias).m_l3,
						 event, product, settings);

	if (m_flavour)
	{
		CorrectJetCollection(algoName_l3, algoName_l5q,
							 this->m_corrService.at(algoCorrectionAlias).m_l5q,
							 event, product, settings);
		CorrectJetCollection(algoName_l3, algoName_l5s,
							 this->m_corrService.at(algoCorrectionAlias).m_l5s,
							 event, product, settings);
		CorrectJetCollection(algoName_l3, algoName_l5c,
							 this->m_corrService.at(algoCorrectionAlias).m_l5c,
							 event, product, settings);
		CorrectJetCollection(algoName_l3, algoName_l5b,
							 this->m_corrService.at(algoCorrectionAlias).m_l5b,
							 event, product, settings);
		CorrectJetCollection(algoName_l3, algoName_l5g,
							 this->m_corrService.at(algoCorrectionAlias).m_l5g,
							 event, product, settings);
	}

	// residual for data
	if (settings.Global()->GetInputType() == DataInput)
	{
		CorrectJetCollection(algoName_l3, algoName_l3res,
							 this->m_corrService.at(algoCorrectionAlias).m_l2l3res,
							 event, product, settings);
	}
}


bool JetCorrector::PopulateGlobalProduct(ZJetEventData const& event,
		ZJetProduct& product, ZJetPipelineSettings const& settings) const
{
	for (unsigned int i = 0; i < m_basealgorithms.size(); i++)
	{
		double runcorr = settings.Global()->GetHcalCorrection();
		if (m_basealgorithms[i].find("chs") == std::string::npos)
		{
			InitCorrection(m_basealgorithms[i], m_basealgorithms[i], event, runcorr);
			CreateCorrections(m_basealgorithms[i], "Jets", event, product, settings, m_basealgorithms[i]);
		}
		else
		{
			InitCorrection(m_basealgorithms[i], m_basealgorithms[i], event, runcorr);
			CreateCorrections(m_basealgorithms[i].substr(0, 5), "JetsCHS", event, product, settings, m_basealgorithms[i]);
		}
	}
	return true;
}


void JetCorrector::CorrectJetCollection(
	std::string algoName,
	std::string newAlgoName,
	boost::scoped_ptr<JECService> const& corrService,
	ZJetEventData const& event,
	ZJetProduct& product,
	ZJetPipelineSettings const& settings) const
{
	unsigned int jetcount = product.GetValidJetCount(settings, event, algoName);

	// copy the jet collection
	for (unsigned int i = 0; i < jetcount; ++i)
	{
		KDataPFTaggedJet* jet = static_cast<KDataPFTaggedJet*>(
									product.GetValidJet(settings, event, i, algoName));

		// create a copy
		KDataPFTaggedJet jet_corr = *jet;
		product.AddValidJet(jet_corr, newAlgoName);
	}

	// correct the copied jet collection
	corrService->correct(&product.m_validPFJets[newAlgoName]);
}

}


