#pragma once

#include <memory>

#include "ZJetEventPipeline/Product.h"

#define USE_JEC
#include "RootTools/JECTools.h"

namespace Artus
{

struct JecCorrSet
{
	boost::scoped_ptr<JECService> m_l1;
	boost::scoped_ptr<JECService> m_rc;
	boost::scoped_ptr<JECService> m_l2;
	boost::scoped_ptr<JECService> m_l3;
	boost::scoped_ptr<JECService> m_l5g;
	boost::scoped_ptr<JECService> m_l5q;
	boost::scoped_ptr<JECService> m_l5b;
	boost::scoped_ptr<JECService> m_l5c;
	boost::scoped_ptr<JECService> m_l2l3res;
};

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

// takes the jets contained in an event and applies the necessary corrections
class JetCorrector: public ZJetGlobalProductProducerBase
{
public:

	static std::string Name()
	{
		return "jet_corrector";
	}


	JetCorrector(std::string corBase, std::string l1cor, stringvector baseAlgos, bool rc, bool flavour);

	virtual void PopulateProduct(ZJetEventData const& data,
								 ZJetProduct& product,
								 ZJetPipelineSettings const& m_pipelineSettings) const;

	virtual bool PopulateGlobalProduct(ZJetEventData const& event,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& m_pipelineSettings) const;

	void InitCorrection(std::string algoName,
						std::string algoCorrectionAlias,
						ZJetEventData const& event, double hcal,
						std::string prefix = "") const;

	void CorrectJetCollection(std::string algoName, std::string newAlgoName,
							  boost::scoped_ptr< JECService > const& corrService,
							  ZJetEventData const& event,
							  ZJetProduct& product,
							  ZJetPipelineSettings const& settings) const;

	void CreateCorrections(std::string algoName,
						   std::string algoPostfix,
						   ZJetEventData const& event,
						   ZJetProduct& product,
						   ZJetPipelineSettings const& settings,
						   std::string algoAlias) const;

private:
	mutable boost::ptr_map<std::string, JecCorrSet> m_corrService;
	std::string m_corectionFileBase;
	std::string m_l1correction;
	std::vector<std::string> m_basealgorithms;
	bool m_rc;
	bool m_flavour;

};

}
