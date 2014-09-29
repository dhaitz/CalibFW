#pragma once


#include "ZJetEventPipeline/Pipeline.h"

namespace Artus
{

// PDG: http://pdg.lbl.gov/2011/tables/rpp2011-sum-gauge-higgs-bosons.pdf
const double g_kZmass = 91.1876;

class ZJetCutBase : public LocalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
{
public:
	virtual std::string GetCutShortName() const = 0;
	virtual unsigned long GetId() const = 0;
};

//order of cuts
/*
1. muon eta
2. muon pt
3. Z pt
4. Z mass window
5. leading jet pt
6. leading jet eta
7. second jet to Z cut
8. back-to-back
*/

class MuonEtaCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		KDataLVs muons;
		if (m_pipelineSettings.GetGenCuts())
			muons = metaData.GetAsLVs(metaData.m_genMuons);
		else
			muons = metaData.GetAsLVs(metaData.m_listValidMuons);

		bool allPassed = true;
		for (KDataLVs::const_iterator it = muons.begin(); it != muons.end(); it++)
		{
			allPassed = allPassed
				&& std::abs(it->p4.Eta()) < m_pipelineSettings.GetCutMuonEta();
		}
		localMetaData.SetCutResult(this->GetId(), allPassed);
	}

	unsigned long GetId() const { return MuonEtaCut::CutId; }
	std::string GetCutName() { return "1) muon eta cut"; }
	std::string GetCutShortName() const { return "muon_eta"; }
	static const long CutId = 1 << 0;
};

class MuonPtCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		KDataLVs muons;
		if (m_pipelineSettings.GetGenCuts())
			muons = metaData.GetAsLVs(metaData.m_genMuons);
		else
			muons = metaData.GetAsLVs(metaData.m_listValidMuons);

		for (KDataLVs::const_iterator it = muons.begin(); it != muons.end(); it++)
		{
			if (it->p4.Pt() < m_pipelineSettings.GetCutMuonPt())
			{
				localMetaData.SetCutResult(this->GetId(), false);
				return;
			}
		}

		localMetaData.SetCutResult(this->GetId(), true);
	}

	unsigned long GetId() const {return MuonPtCut::CutId;}
	std::string GetCutName() {return "2) muon pt cut";}
	std::string GetCutShortName() const {return "muon_pt";}
	static const long CutId = 1 << 1;
};

class ZPtCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidKindZ(m_pipelineSettings.GetGenCuts()))
			return;
		localMetaData.SetCutResult(this->GetId(),
				metaData.GetRefKindZ(m_pipelineSettings.GetGenCuts()).p4.Pt()
				> m_pipelineSettings.GetCutZPt());
	}

	unsigned long GetId() const { return ZPtCut::CutId; }
	std::string GetCutName() { return "3) z pt"; }
	std::string GetCutShortName() const { return "zpt"; }
	static const long CutId = 1 << 2;
};

class ZMassWindowCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidKindZ(m_pipelineSettings.GetGenCuts()))
			return;
		localMetaData.SetCutResult(this->GetId(),
			std::abs(metaData.GetRefKindZ(m_pipelineSettings.GetGenCuts()).p4.mass() - g_kZmass)
			< m_pipelineSettings.GetCutZMassWindow()
		);
	}

	unsigned long GetId() const { return ZMassWindowCut::CutId; }
	std::string GetCutName() { return "4) z mass window"; }
	std::string GetCutShortName() const { return "zmass_window"; }
	static const long CutId = 1 << 3;
};

class LeadingJetPtCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidJet(m_pipelineSettings, data))
			return;

		KDataLV* jet = metaData.GetValidPrimaryJet(m_pipelineSettings, data);
		localMetaData.SetCutResult(this->GetId(),
			jet->p4.Pt() > m_pipelineSettings.GetCutLeadingJetPt()
		);
	}

	unsigned long GetId() const { return LeadingJetPtCut::CutId; }
	std::string GetCutName() { return "5) Leading Jet Pt Cut"; }
	std::string GetCutShortName() const { return "leadingjet_pt"; }
	static const long CutId = 1 << 4;
};


class LeadingJetEtaCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidJet(m_pipelineSettings,data))
			return;

		KDataLV* jet = metaData.GetValidPrimaryJet(m_pipelineSettings, data);

		localMetaData.SetCutResult(this->GetId(),
			std::abs(jet->p4.Eta()) < m_pipelineSettings.GetCutLeadingJetEta()
		);

	}

	unsigned long GetId() const { return LeadingJetEtaCut::CutId; }
	std::string GetCutName() { return "6) leading jet eta cut"; }
	std::string GetCutShortName() const { return "leadingjet_eta"; }
	static const long CutId = 1 << 5;
};


class SecondLeadingToZPtCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidKindZ(m_pipelineSettings.GetGenCuts()))
			return;
		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
		{
			localMetaData.SetCutResult(this->GetId(), true);
			return;
		}

		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1);

		localMetaData.SetCutResult(this->GetId(),
			(metaData.GetBalance(jet2, metaData.GetPtKindZ(m_pipelineSettings.GetGenCuts()))
				< m_pipelineSettings.GetCutSecondLeadingToZPt()) || (jet2->p4.Pt() < m_pipelineSettings.GetCutLeadingJetPt()));
	}

	unsigned long GetId() const { return SecondLeadingToZPtCut::CutId; }
	std::string GetCutName() { return "7) 2nd leading jet to Z pt"; }
	std::string GetCutShortName() const { return "secondleading_to_zpt"; }
	static const long CutId = 1 << 6;
};

class BackToBackCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidJet(m_pipelineSettings, data))
			return;
		KDataLV * jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, data);

		if (!metaData.HasValidKindZ(m_pipelineSettings.GetGenCuts()))
			return;
		double zphi = metaData.GetRefKindZ(m_pipelineSettings.GetGenCuts()).p4.Phi();

		localMetaData.SetCutResult(this->GetId(),
				(std::abs(std::abs(jet1->p4.Phi() - zphi) - ROOT::Math::Pi()))
					< m_pipelineSettings.GetCutBack2Back()
		);
	}

	unsigned long GetId() const { return BackToBackCut::CutId; }
	std::string GetCutName() { return "8) back to back/jet to z"; }
	std::string GetCutShortName() const { return "back_to_back"; }
	static const long CutId = 1 << 7;
};



class SecondLeadingToZPtRegionCut: public ZJetCutBase
// covers a region of the cut 0.1 > x > 0.2 etc...
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidKindZ(m_pipelineSettings.GetGenCuts()))
			return;

		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
		{
			// is ok, there seems to be no 2nd Jet in the event
			localMetaData.SetCutResult(this->GetId(), true);
			return;
		}

		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1);

		localMetaData.SetCutResult(this->GetId(),
			(metaData.GetBalance(jet2, metaData.GetPtKindZ(m_pipelineSettings.GetGenCuts()))
				>= m_pipelineSettings.GetCutSecondLeadingToZPtRegionLow()) &&
			(metaData.GetBalance(jet2, metaData.GetPtKindZ(m_pipelineSettings.GetGenCuts()))
				< m_pipelineSettings.GetCutSecondLeadingToZPtRegionHigh())
		);
	}

	unsigned long GetId() const { return SecondLeadingToZPtRegionCut::CutId; }
	std::string GetCutName() { return "9) 2nd leading jet to Z pt (region)"; }
	std::string GetCutShortName() const { return "secondleading_to_zpt_region"; }
	static const long CutId = 1 << 10;
};



class DeltaEtaCut: public ZJetCutBase
{
public:
	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidJet(m_pipelineSettings, data))
			return;
		if (!metaData.HasValidKindZ(m_pipelineSettings.GetGenCuts()))
			return;
		double jeta = metaData.GetValidPrimaryJet(m_pipelineSettings, data)->p4.Eta();
		double zeta = metaData.GetRefKindZ(m_pipelineSettings.GetGenCuts()).p4.Eta();

		localMetaData.SetCutResult(this->GetId(),
			std::abs(jeta - zeta) > m_pipelineSettings.GetCutDeltaEtaLow() &&
			std::abs(jeta - zeta) < m_pipelineSettings.GetCutDeltaEtaHigh()
		);
	}

	unsigned long GetId() const
	{
		return DeltaEtaCut::CutId;
	}
	std::string GetCutName()
	{
		return "8) delta eta";
	}
	std::string GetCutShortName() const
	{
		return "deltaeta";
	}
	static const long CutId = 1 << 5;
};


class HltCut: public ZJetCutBase
{
public:

	bool IsInCut(ZJetEventData * pEv, ZJetPipelineSettings * pset)
	{/*
		const int nHLTriggers = pEv->m_pData->HLTriggers_accept->GetEntries();

		if (nHLTriggers == 0)
		{
			LOG_FATAL("No HLT Trigger in Event!");
		}

		TObjString *theHLTbit = NULL;

		for (int i = 0; i < nHLTriggers; ++i)
		{
			theHLTbit = (TObjString*) pEv->m_pData->HLTriggers_accept->At(i);
			TString curName = theHLTbit->GetString();

			if (pset->GetCutHLT() == "SingleMu")
			{
				if (hltSingleMu == curName)
					return true;
			}
			else
			{
				if (hltDoubleMu1 == curName || hltDoubleMu2 == curName)
					return true;
			}
		}
		return false;
		*/
		// todo
		return true;
	}

	unsigned long GetId() const { return HltCut::CutId; }
	std::string GetCutName() { return "Hlt Cut"; }
	std::string GetCutShortName() const {return "hlt"; }
	static const long CutId = 1 << 9;
};


//VBF Cuts
/*	order name              bit
	1.) Jet pT cut          11
	2.) 2nd jet pT cut      12
	4.) Jet eta cut          3
	3.) 2nd jet eta cut     13
	5.) Rapidity gap cuts   14
	6.) Jet Mass cut        15
	7.) No b tags           16
	8.) central jet veto    17
	9.)                     18
*/


class SecondJetPtCut: public ZJetCutBase
// pT_jet2 > 30 GeV
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
		{
			// is not ok, there seems to be no 2nd jet in the event
			return;
		}
		KDataLV* jet = metaData.GetValidJet(m_pipelineSettings, data, 1);
		localMetaData.SetCutResult(this->GetId(),
			jet->p4.Pt() > m_pipelineSettings.GetCutSecondJetPt()
		);
	}

	unsigned long GetId() const { return SecondJetPtCut::CutId; }
	std::string GetCutName() { return "2) Second Jet Pt Cut"; }
	std::string GetCutShortName() const { return "secondjet_pt"; }
	static const long CutId = 1 << 12;
};


class SecondJetEtaCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
		{
			// is not ok, there seems to be no 2nd jet in the event
			return;
		}
		KDataLV* jet = metaData.GetValidJet(m_pipelineSettings, data, 1);
		localMetaData.SetCutResult(this->GetId(),
			std::abs(jet->p4.Eta()) < m_pipelineSettings.GetCutSecondJetEta()
		);
	}

	unsigned long GetId() const { return SecondJetEtaCut::CutId; }
	std::string GetCutName() { return "3) second jet eta cut"; }
	std::string GetCutShortName() const { return "secondjet_eta"; }
	static const long CutId = 1 << 13;
};


class RapidityGapCut: public ZJetCutBase
// | eta_jet1 - eta_jet2 | > 4.2 && eta_jet1 * eta_jet2 < 0
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
			// is not ok, there seems to be no 2nd Jet in the event
			return;

		KDataLV * jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, data);
		KDataLV * jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1);

		localMetaData.SetCutResult(this->GetId(),
			std::abs(jet1->p4.Eta() - jet2->p4.Eta())
				> m_pipelineSettings.GetCutRapidityGap()
			&& jet1->p4.Eta() * jet2->p4.Eta() < 0.0
		);
	}

	unsigned long GetId() const { return RapidityGapCut::CutId; }
	std::string GetCutName() { return "5) Rapidity Gap Cut"; }
	std::string GetCutShortName() const { return "rapidity_gap"; }
	static const long CutId = 1 << 14;
};


class InvariantMassCut: public ZJetCutBase
// m_jetjet > 600 GeV
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{

		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
		{
			// is not ok, there seems to be no 2nd Jet in the event
			return;
		}

		KDataLV* jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, data);
		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1);

		localMetaData.SetCutResult(this->GetId(),
			(jet1->p4 + jet2->p4).M() > m_pipelineSettings.GetCutInvariantMass()
		);
	}

	unsigned long GetId() const { return InvariantMassCut::CutId; }
	std::string GetCutName() { return "6) Invariant Jet Mass Cut"; }
	std::string GetCutShortName() const { return "jet_mass"; }
	static const long CutId = 1 << 15;
};


class BtagCut: public ZJetCutBase
// no b tags
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
		{
			// is not ok, there seems to be no 2nd Jet in the event
			return;
		}

		//KDataLV* jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, data);
		//KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1);

		localMetaData.SetCutResult(this->GetId(),
			!false && !false
			//!jet1->btag && !jet2->btag
		);
	}

	unsigned long GetId() const { return BtagCut::CutId; }
	std::string GetCutName() { return "7) No b tag"; }
	std::string GetCutShortName() const { return "no_btag"; }
	static const long CutId = 1 << 16;
};


class CentralJetVeto: public ZJetCutBase
// no 3rd jet in between
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
			ZJetMetaData::LocalMetaDataType & localMetaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 2)
			// this is no vbf event, 2nd jet is missing
			return;

		if (metaData.GetValidJetCount(m_pipelineSettings, data) < 3)
		{
			// is ok, there seems to be no 3rd Jet in the event
			localMetaData.SetCutResult(this->GetId(), true);
			return;
		}

		KDataLV* jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, data);
		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, data, 1);
		KDataLV* jet3 = metaData.GetValidJet(m_pipelineSettings, data, 2);

		if (jet3->p4.Pt() <= m_pipelineSettings.GetCutThirdJetPt())
		{
			// is ok, the 3rd jet is very soft
			localMetaData.SetCutResult(this->GetId(), true);
			return;
		}

		localMetaData.SetCutResult(this->GetId(),
			std::abs(metaData.GetZeppenfeld(jet1, jet2, jet3)) < 1.0
		);
	}

	unsigned long GetId() const { return CentralJetVeto::CutId; }
	std::string GetCutName() { return "8) Central Jet Veto"; }
	std::string GetCutShortName() const { return "jetveto"; }
	static const long CutId = 1 << 17;
};


class ElectronEtaCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
							   ZJetMetaData::LocalMetaDataType& localMetaData,
							   ZJetPipelineSettings const& m_pipelineSettings) const
	{
		bool allPassed = (
							 (std::abs(metaData.leadingeminus.p4.Eta()) < m_pipelineSettings.GetCutElectronEta())
							 && (std::abs(metaData.leadingeplus.p4.Eta()) < m_pipelineSettings.GetCutElectronEta())
						 );

		localMetaData.SetCutResult(this->GetId(), allPassed);
	}

	unsigned long GetId() const
	{
		return ElectronEtaCut::CutId;
	}
	std::string GetCutName()
	{
		return "9) electron eta cut";
	}
	std::string GetCutShortName() const
	{
		return "electron_eta";
	}
	static const long CutId = 1 << 18;
};

class ElectronPtCut: public ZJetCutBase
{
public:

	virtual void PopulateLocal(ZJetEventData const& data, ZJetMetaData const& metaData,
							   ZJetMetaData::LocalMetaDataType& localMetaData,
							   ZJetPipelineSettings const& m_pipelineSettings) const
	{
		bool allPassed = (
							 (metaData.leadingeminus.p4.Pt() > m_pipelineSettings.GetCutElectronPt())
							 && (metaData.leadingeplus.p4.Pt() > m_pipelineSettings.GetCutElectronPt())
						 );

		localMetaData.SetCutResult(this->GetId(), allPassed);
	}

	unsigned long GetId() const
	{
		return ElectronPtCut::CutId;
	}
	std::string GetCutName()
	{
		return "10) electron pt cut";
	}
	std::string GetCutShortName() const
	{
		return "electron_pt";
	}
	static const long CutId = 1 << 19;
};


}
