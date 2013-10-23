#include "EventPipeline/JetTools.h"

#include "ZJetEventPipeline/Pipeline.h"
#include "ZJetProducer/Cuts.h"


using namespace CalibFW;

void ZJetPipeline::GetSupportedCuts(ZJetPipeline::MetaDataProducerVector& cuts)
{
	cuts.clear();
	cuts.push_back(new MuonEtaCut());
	cuts.push_back(new MuonPtCut());

	cuts.push_back(new ZPtCut());
	cuts.push_back(new ZMassWindowCut());

	cuts.push_back(new LeadingJetPtCut());
	cuts.push_back(new LeadingJetEtaCut());

	cuts.push_back(new SecondLeadingToZPtCut());
	cuts.push_back(new BackToBackCut());

	cuts.push_back(new SecondJetPtCut());
	cuts.push_back(new SecondJetEtaCut());
	cuts.push_back(new RapidityGapCut());
	cuts.push_back(new InvariantMassCut());
}


