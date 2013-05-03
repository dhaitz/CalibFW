#pragma once

#include <string>
#include <iostream>
#include <list>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>

#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/ptree.hpp>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "Draw/DrawBase.h"
#include "Pipeline/JetTools.h"
#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

namespace CalibFW
{

/** Calculates the Response distribution with a Histogram

   "Name" : "response_balance",
   "ProductName" : "binresp",
   "ResponseType": "bal"|"mpf"|"two"|"z",
   "JetNumber" : 1 to n
*/
class BinResponseConsumer: public ZJetMetaConsumer
{
public:
	BinResponseConsumer(boost::property_tree::ptree* ptree, std::string configPath):
			ZJetMetaConsumer(), m_useGenJet(false), m_useGenJetAsReference(false)
	{
		m_jetnum = ptree->get<unsigned int>(configPath + ".JetNumber", 1) - 1;
		m_name = ptree->get<std::string>(configPath + ".ProductName");
		const std::string sType = ptree->get<std::string>(configPath + ".ResponseType");

		if (sType == "bal")
			m_respType = Balance;
		else if (sType == "mpf")
			m_respType = Mpf;
		else if (sType == "mpfraw")
			m_respType = MpfRaw;
		else if (sType == "two")
			m_respType = TwoBalance;
		else if (sType == "zep")
			m_respType = Zeppenfeld;
		else if (sType == "z")
			m_respType = Z;
		else if (sType == "recogen")
			m_respType = RecoGen;
		else if (sType == "muresp")
			m_respType = MuonResponse;
		else if (sType == "genbal")
			m_respType = GenBalance;
		else if (sType == "genmpf")
			m_respType = GenMpf;
		else if (sType == "gentwo")
			m_respType = GenTwoBalance;
		else if (sType == "genzep")
			m_respType = GenZeppenfeld;
		else if (sType == "parton")
			m_respType = Parton;
		else if (sType == "balparton")
			m_respType = BalancedParton;
		else if (sType == "genbal_toparton")
			m_respType = GenBalanceToParton;
		else if (sType == "genbal_tobalparton")
			m_respType = GenBalanceToBalancedParton;
		else if (sType == "quality")
			m_respType = BalanceQuality;
		else if (sType == "flavorbal")
			m_respType = FlavorBal;
		else if (sType == "flavormpf")
			m_respType = FlavorMpf;
		else
			CALIB_LOG_FATAL("Unknown Response type " << sType << "!");
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_histo = new Hist1D(m_name, GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetResponseModifier());
		AddPlot(m_histo);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
		assert(metaData.HasValidZ());
		ZJetPipelineSettings const& set = this->GetPipelineSettings();
		std::string genName(JetType::GetGenName(set.GetJetAlgorithm()));

		if (m_respType == Balance)
		{
			if (m_jetnum >= metaData.GetValidJetCount(set, event)
				|| !metaData.HasValidZ())
				return;
			KDataLV* jet = metaData.GetValidJet(set, event, m_jetnum);
			m_histo->Fill(metaData.GetBalance(jet), metaData.GetWeight());
		}

		else if (m_respType == RecoGen)
		{
			if (m_jetnum >= metaData.GetValidJetCount(set, event, genName)
					|| m_jetnum >= metaData.GetValidJetCount(set, event))
				return;

			// function GetMatchedJet(genname)
			if (metaData.m_matchingResults.find(genName) == metaData.m_matchingResults.end())
				return;
			std::vector<int> const& matchList = metaData.m_matchingResults.at(genName);

			if (unlikely(m_jetnum >= matchList.size()))
			{
				CALIB_LOG("\nJetnum: " << m_jetnum << ">= listsize: " << matchList.size()
					<< ", Matchlist: " << matchList)
				CALIB_LOG("nGenJets: " << metaData.GetValidJetCount(set, event, genName)
					<< ", nPFJets: " << metaData.GetValidJetCount(set, event))
				for (int i = 0; i < metaData.GetValidJetCount(set, event, genName); i++)
					CALIB_LOG("Gen" << i << ": " << *metaData.GetValidJet(set, event, i, genName))

				for (int i = 0; i < metaData.GetValidJetCount(set, event); i++)
					CALIB_LOG("PF" << i << ": " << *((KDataPFJet*) metaData.GetValidJet(set, event, i)))

				CALIB_LOG("Reco to gen matching: Not enough jets (looking for jet_"
					<< m_jetnum << " but only " << matchList.size() << " jet(s) are matched)!")
				return;
			}
			int iMatchedGen = matchList.at(m_jetnum);
			if (iMatchedGen <= -1)
			{
				if (unlikely(m_jetnum < 0)) // that happens very often for 2nd jet and leading vbf jets!
					CALIB_LOG("Reco to gen matching: No gen jet matches jet_" << m_jetnum)
				return;
			}
			if (iMatchedGen >= metaData.GetValidJetCount(set, event, genName))
			{
				CALIB_LOG_FATAL("Reco to gen matching: No reference gen jet found! "
					<< iMatchedGen <<" >= " << metaData.GetValidJetCount(set, event, genName))
				return;
			}
			KDataLV* genjet = metaData.GetValidJet(set, event, iMatchedGen, genName);
			KDataLV* jet = metaData.GetValidJet(set, event, m_jetnum);
			assert(genjet !=NULL);
			m_histo->Fill(metaData.GetBalance(jet, genjet), metaData.GetWeight());
		}

		else if (m_respType == GenBalance)
		{
			if (m_jetnum >= metaData.GetValidJetCount(set, event, genName)
					|| !metaData.HasValidGenZ())
			{
				CALIB_LOG("GenBalance: No gen jet_" << m_jetnum << " found!")
				return;
			}
			KDataLV* genjet = metaData.GetValidJet(set, event, m_jetnum, genName);
			assert(genjet != NULL);
			m_histo->Fill(metaData.GetGenBalance(genjet), metaData.GetWeight());
		}

		else if (m_respType == Mpf)
		{
			if (!metaData.HasValidZ())
				return;
			m_histo->Fill(metaData.GetMPF(metaData.GetMet(event, set)), metaData.GetWeight());
		}

		else if (m_respType == MpfRaw)
		{
			if (!metaData.HasValidZ())
				return;
			m_histo->Fill(metaData.GetMPF(event.m_pfMet), metaData.GetWeight());
		}

		else if (m_respType == TwoBalance)
		{
			if (metaData.GetValidJetCount(set, event) < 2)
				return;
			KDataLV* jet0 = metaData.GetValidJet(set, event, 0);
			KDataLV* jet1 = metaData.GetValidJet(set, event, 1);
			m_histo->Fill(metaData.GetTwoBalance(jet0, jet1), metaData.GetWeight());
		}

		else if (m_respType == GenTwoBalance)
		{
			if (metaData.GetValidJetCount(set, event, genName) < 2)
				return;
			KDataLV* jet0 = metaData.GetValidJet(set, event, 0, genName);
			KDataLV* jet1 = metaData.GetValidJet(set, event, 1, genName);
			m_histo->Fill(metaData.GetGenTwoBalance(jet0, jet1), metaData.GetWeight());
		}

		else if (m_respType == Zeppenfeld)
		{
			if (metaData.GetValidJetCount(set, event) < 3)
				return;
			KDataLV* jet0 = metaData.GetValidJet(set, event, 0);
			KDataLV* jet1 = metaData.GetValidJet(set, event, 1);
			KDataLV* jet2 = metaData.GetValidJet(set, event, 2);
			m_histo->Fill(metaData.GetZeppenfeld(jet0, jet1, jet2), metaData.GetWeight());
		}

		else if (m_respType == GenZeppenfeld)
		{
			if (metaData.GetValidJetCount(set, event, genName) < 3)
				return;
			KDataLV* jet0 = metaData.GetValidJet(set, event, 0, genName);
			KDataLV* jet1 = metaData.GetValidJet(set, event, 1, genName);
			KDataLV* jet2 = metaData.GetValidJet(set, event, 2, genName);
			m_histo->Fill(metaData.GetZeppenfeld(jet0, jet1, jet2), metaData.GetWeight());
		}

		else if (m_respType == GenMpf)
		{
			if (!metaData.HasValidGenZ())
				return;
			m_histo->Fill(metaData.GetGenMPF(metaData.GetPtGenMet()), metaData.GetWeight());
		}

		else if (m_respType == Z)
		{
			if (!metaData.HasValidZ() || !metaData.HasValidGenZ())
				return;
			m_histo->Fill(metaData.GetGenBalance(metaData.GetPtZ()), metaData.GetWeight());
		}

		else if (m_respType == Parton)
		{
			if (!metaData.HasValidZ() || !metaData.HasValidParton())
				return;
			m_histo->Fill(metaData.GetGenBalance(metaData.GetPtLeadingParton()), metaData.GetWeight());
		}

		else if (m_respType == BalancedParton)
		{
			if (!metaData.HasValidZ() || !metaData.HasValidParton())
				return;
			m_histo->Fill(metaData.GetGenBalance(metaData.GetPtBalancedParton()), metaData.GetWeight());
		}

		else if (m_respType == GenBalanceToParton)
		{
			if (m_jetnum >= metaData.GetValidJetCount(set, event, genName)
					|| !metaData.HasValidParton()
					|| metaData.GetBalanceQuality() > 1.0)
				return;
			KDataLV* genjet = metaData.GetValidJet(set, event, m_jetnum, genName);
			m_histo->Fill(metaData.GetGenBalance(genjet, metaData.GetPtLeadingParton()), metaData.GetWeight());
		}

		else if (m_respType == GenBalanceToBalancedParton)
		{
			if (m_jetnum >= metaData.GetValidJetCount(set, event, genName)
					|| !metaData.HasBalancedParton())
				return;
			KDataLV* genjet = metaData.GetValidJet(set, event, m_jetnum, genName);
			m_histo->Fill(metaData.GetGenBalance(genjet, metaData.GetPtBalancedParton()), metaData.GetWeight());
		}

		else if (m_respType == BalanceQuality)
		{
			m_histo->Fill(metaData.GetBalanceQuality(), metaData.GetWeight());
		}

		else if (m_respType == MuonResponse)
		{
			if (metaData.m_genMuons.size() != 2 || metaData.GetValidMuons().size() != 2)
				return;
			m_histo->Fill(metaData.GetMuonResponse(), metaData.GetWeight());
		}

		else if (m_respType == FlavorBal)
		{
			if (metaData.GetValidJetCount(set, event) == 0
				|| !metaData.HasValidZ()
				|| !metaData.HasBalancedParton()
			)
				return;

			// m_jetnum is misused as flavor
			int flavor = m_jetnum + 1;
			int pdg = std::abs(metaData.GetBalancedParton().pdgId());
			bool good = (flavor == 123456) &&
				(pdg == 1 || pdg == 2 || pdg == 3 || pdg == 4 || pdg == 5 || pdg == 6);
			good = good || (flavor == 123) && (pdg == 1 || pdg == 2 || pdg == 3);
			good = good || (flavor == pdg);

			if (!good)
				return;

			KDataLV* jet = metaData.GetValidJet(set, event, 0);
			m_histo->Fill(metaData.GetBalance(jet), metaData.GetWeight());
		}

		else if (m_respType == FlavorMpf)
		{
			if (!metaData.HasValidZ() || !metaData.HasBalancedParton())
				return;

			// m_jetnum is misused as flavor
			int flavor = m_jetnum + 1;
			int pdg = std::abs(metaData.GetBalancedParton().pdgId());
			bool good = (flavor == 123456) &&
				(pdg == 1 || pdg == 2 || pdg == 3 || pdg == 4 || pdg == 5 || pdg == 6);
			good = good || (flavor == 123) && (pdg == 1 || pdg == 2 || pdg == 3);
			good = good || (flavor == pdg);

			if (!good)
				return;

			m_histo->Fill(metaData.GetMPF(metaData.GetMet(event, set)), metaData.GetWeight());
		}

		else
			CALIB_LOG_FATAL("Response type " << m_respType << " not implemented!")
	}

	static std::string GetName()
	{
		return "bin_response";
	}

	// if true, the GenJet.Pt is used to calculate the balance instead of the RECO Jet
	bool m_useGenJet;
	// if true, the GenJet.Pt will be used as reference object instead of the Z.Pt()
	bool m_useGenJetAsReference;
	unsigned int m_jetnum;
	std::string m_name;

private:
	Hist1D* m_histo;
	enum rType {
		// reco level
		Mpf, MpfRaw, Balance, TwoBalance, Zeppenfeld,
		// reco to gen
		Z, RecoGen, MuonResponse,
		// gen level
		GenMpf, GenBalance, GenTwoBalance, GenZeppenfeld, Parton, BalancedParton,
		GenBalanceToParton, GenBalanceToBalancedParton,
		BalanceQuality,
		FlavorBal, FlavorMpf
	} m_respType;
};

}

