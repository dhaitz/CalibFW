#pragma once

#include "ZJetProducerBase.h"

namespace Artus
{

/** Select only valid jets.

    The jets are valid if the pass these criteria
    1. muon isolation
    1. jetID
*/
class ValidJetProducer: public ZJetGlobalProductProducerBase
{

public:

	ValidJetProducer(bool Tagged, bool VetoPu, bool MuonIso) :
		ZJetGlobalProductProducerBase(), tagged(Tagged), vetopu(VetoPu), muonIso(MuonIso)
	{
		Init();
	}
	ValidJetProducer(bool Tagged, bool VetoPu) :
		ZJetGlobalProductProducerBase(), tagged(Tagged), vetopu(VetoPu),  muonIso(true)
	{
		Init();
	}


	virtual void PopulateProduct(ZJetEventData const& data,
								 ZJetProduct& product,
								 ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual bool PopulateGlobalProduct(ZJetEventData const& event,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const;

	static std::string Name()
	{
		return "valid_jet_producer";
	}
private:
	bool tagged;
	bool vetopu;
	bool muonIso;

protected:
	virtual void Init()
	{
		if (vetopu)
			LOG_FILE(blue << Name() << ":\n   Mark pile-up jets as invalid." << reset);
	}
};

/** Select only valid jets (FOR ELECTRON ANALYSIS).

    Same as ValidJetProducer but without muon isolation.
*/
class ValidJetEEProducer: public ValidJetProducer
{
public:
	ValidJetEEProducer(bool Tagged, bool VetoPu) : ValidJetProducer(Tagged, VetoPu, false) {}

	static std::string Name()
	{
		return "valid_jet_ee_producer";
	}
};
}
