#pragma once

namespace Artus
{


/** Select only valid jets.

    The jets are valid if the pass these criteria
    1. muon isolation
    1. jetID
*/
class ValidJetProducer: public ZJetGlobalMetaDataProducerBase
{

public:

	ValidJetProducer(bool Tagged, bool VetoPu, bool MuonIso) :
		ZJetGlobalMetaDataProducerBase(), tagged(Tagged), vetopu(VetoPu), muonIso(MuonIso)
	{
		Init();
	}
	ValidJetProducer(bool Tagged, bool VetoPu) :
		ZJetGlobalMetaDataProducerBase(), tagged(Tagged), vetopu(VetoPu),  muonIso(true)
	{
		Init();
	}


	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{

		// for tagged PF jets: fill the m_pfPointerJets with a collection of
		// casted KDataPFJet pointers pointing to the KDataPFTaggedJet
		if (tagged)
		{
			for (ZJetEventData::PfTaggedMapIterator italgo = event.m_pfTaggedJets.begin();
				 italgo != event.m_pfTaggedJets.end(); ++italgo)
			{
				std::string sAlgoNameTagged = italgo->first;
				std::string sAlgoName = sAlgoNameTagged;
				sAlgoName.replace(sAlgoName.find("TaggedJets"), 10, "Jets");

				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;


				KDataPFTaggedJets* tjets = event.m_pfTaggedJets.at(sAlgoNameTagged);
				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;
				for (KDataPFTaggedJets::iterator it = tjets->begin(); it != tjets->end(); ++it)
					event.m_pfPointerJets[sAlgoName]->emplace_back(dynamic_cast<KDataPFJet*>(&(*it)));
			}
		}
		// for regular (non-tagged) PF jets: fill the m_pfPointerJets with simple pointers
		else
		{
			for (ZJetEventData::PfMapIterator italgo = event.m_pfJets.begin();
				 italgo != event.m_pfJets.end(); ++italgo)
			{
				std::string sAlgoName = italgo->first;
				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;

				KDataPFJets* tjets = event.m_pfJets.at(sAlgoName);
				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;
				for (KDataPFJets::iterator it = tjets->begin(); it != tjets->end(); ++it)
					event.m_pfPointerJets[sAlgoName]->emplace_back(&(*it));
			}
		}

		// validate PF Jets
		for (ZJetEventData::PfPointerMapIterator italgo = event.m_pfPointerJets.begin(); italgo
			 != event.m_pfPointerJets.end(); ++italgo)
		{
			// init collections for this algorithm
			std::string sAlgoName = italgo->first;
			metaData.m_listValidJets.insert(sAlgoName, new std::vector<unsigned int> ());
			metaData.m_listInvalidJets.insert(sAlgoName, new std::vector<unsigned int> ());

			int i = 0;

			for (std::vector<KDataPFJet*>::iterator itjet = italgo->second->begin();
				 itjet != italgo->second->end(); ++itjet)
			{
				bool good_jet = true;

				// 5 GeV minimum pT
				good_jet = good_jet && ((*itjet)->p4.Pt() > 5);

				//isolation DeltaR > 0.5

				float dr1, dr2;
				dr1 = 99999.0f;
				dr2 = 99999.0f;

				if (metaData.HasValidZ())
				{

					if (muonIso)
					{
						dr1 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
															 metaData.GetValidMuons().at(0).p4);
						dr2 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
															 metaData.GetValidMuons().at(1).p4);
					}
					else
					{
						dr1 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
															 metaData.GetValidElectrons().at(0).p4);
						dr2 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
															 metaData.GetValidElectrons().at(1).p4);
					}

				}
				good_jet = good_jet && (dr1 > 0.5) && (dr2 > 0.5);

				// JetID
				// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
				// PFJets, all eta
				good_jet = good_jet
						   && (*itjet)->neutralHadFraction + (*itjet)->HFHadFraction < 0.99
						   && (*itjet)->neutralEMFraction < 0.99
						   && (*itjet)->nConst > 1;
				// PFJets, |eta| < 2.4 (tracker)
				if (std::abs((*itjet)->p4.eta()) < 2.4)
				{
					good_jet = good_jet
							   && (*itjet)->chargedHadFraction > 0.0
							   && (*itjet)->nCharged > 0
							   && (*itjet)->chargedEMFraction < 0.99;
				}

				if (vetopu)
				{
					bool puID = static_cast<KDataPFTaggedJet*>(*itjet)->getpuJetID("puJetIDFullMedium", event.m_taggermetadata);
					good_jet = good_jet && puID;
				}

				if (good_jet)
					metaData.m_listValidJets[italgo->first].emplace_back(i);
				else
					metaData.m_listInvalidJets[italgo->first].emplace_back(i);
				i++;
			}
		}

		return true;
	}

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
