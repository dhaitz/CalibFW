#include "ZJetProducer/ValidJetProducer.h"


namespace Artus
{

bool ValidJetProducer::PopulateGlobalProduct(ZJetEventData const& event,
		ZJetProduct& product,
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
		product.m_listValidJets.insert(sAlgoName, new std::vector<unsigned int> ());
		product.m_listInvalidJets.insert(sAlgoName, new std::vector<unsigned int> ());

		int i = 0;

		for (std::vector<KDataPFJet*>::iterator itjet = italgo->second->begin();
			 itjet != italgo->second->end(); ++itjet)
		{
			bool good_jet = true;

			// 5 GeV minimum pT
			good_jet = good_jet && ((*itjet)->p4.Pt() > 12);

			//isolation DeltaR > 0.5

			float dr1, dr2;
			dr1 = 99999.0f;
			dr2 = 99999.0f;

			if (product.HasValidZ())
			{
				if (muonIso)
				{
					//for (auto muon: product.GetValidMuons())
					for (const auto muon : *event.m_muons)
					{
						good_jet = good_jet && (ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
												muon.p4) > 0.5);
					}
					//if (product.GetValidMuons().size() > 0)
					//dr1 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
					//									 product.GetValidMuons().at(0).p4);
					//if (product.GetValidMuons().size() > 1)
					//dr2 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
					//									 product.GetValidMuons().at(1).p4);
				}
				else
				{
					dr1 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
														 product.GetValidElectrons().at(0).p4);
					dr2 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
														 product.GetValidElectrons().at(1).p4);
				}

			}
			//good_jet = good_jet && (dr1 > 0.5) && (dr2 > 0.5);
			/*
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
							bool puID = static_cast<KDataPFTaggedJet*>(*itjet)->getpuJetID("puJetIDFullMedium", event.m_taggerproduct);
							good_jet = good_jet && puID;
						}
			*/
			if (good_jet)
				product.m_listValidJets[italgo->first].emplace_back(i);
			else
				product.m_listInvalidJets[italgo->first].emplace_back(i);
			i++;
		}
	}

	return true;
}
}
			if (good_jet)
				product.m_listValidJets[italgo->first].emplace_back(i);
			else
				product.m_listInvalidJets[italgo->first].emplace_back(i);
			i++;
		}
	}

	return true;
}
}
