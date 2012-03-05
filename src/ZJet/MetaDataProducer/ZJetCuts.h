#pragma once


#include "../ZJetPipeline.h"

namespace CalibFW
{

// PDG: http://pdg.lbl.gov/2011/tables/rpp2011-sum-gauge-higgs-bosons.pdf
const double g_kZmass = 91.1876;

class ZJetCutBase : public MetaDataProducerBase<ZJetEventData , ZJetMetaData,  ZJetPipelineSettings >
{
public:
	virtual std::string GetCutShortName() const = 0;
	virtual unsigned long GetId() const = 0;
};
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;
/*
class JsonCut: public ZJetCutBase
{
public:
	JsonCut(Json_wrapper * jsonFile)
	{
		m_jsonFile = jsonFile;
	}

	bool IsInCut(ZJetEventData * pEv, ZJetPipelineSettings * pset)
	{
		if ((m_jsonFile == NULL) || (!m_jsonFile->isValid()))
		{
			CALIB_LOG_FATAL("No valid JSON file loaded.")
		}

		return m_jsonFile->has(pEv->m_pData->cmsRun,
				pEv->m_pData->luminosityBlock);
		// todo
		return true;
	}

	unsigned long GetId()
	{
		return JsonCut::CutId;
	}
	std::string GetCutName()
	{
		return "1) invalidated by json file";
	}
	std::string GetCutShortName()
	{
		return "json";
	}
	static const long CutId = 1 << 0;

	Json_wrapper * m_jsonFile;
};*/

class MuonPtCut: public ZJetCutBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		for ( KDataMuons::const_iterator it = metaData.GetValidMuons().begin();
				it != metaData.GetValidMuons().end(); it ++)
		{
			if( it->p4.Pt() < m_pipelineSettings.GetCutMuonPt())
			{
				metaData.SetCutResult( this->GetId(), false );
				return;
			}
		}

		metaData.SetCutResult( this->GetId(), true );
	}

	unsigned long GetId() const
	{
		return MuonPtCut::CutId;
	}
	std::string GetCutName()
	{
		return "2) muon pt cut";
	}
	std::string GetCutShortName() const
	{
		return "muon_pt";
	}
	static const long CutId = 1 << 1;
};

class MuonEtaCut: public ZJetCutBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		bool oneFailed = false;

		for ( KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				it != metaData.m_listValidMuons.end(); it ++)
		{
			oneFailed = !(TMath::Abs(it->p4.Eta())
							< m_pipelineSettings.GetCutMuonEta()) || oneFailed;
		}

		metaData.SetCutResult( this->GetId(), !oneFailed );
	}

	unsigned long GetId() const
	{
		return MuonEtaCut::CutId;
	}
	std::string GetCutName()
	{
		return "3) muon eta cut";
	}
	std::string GetCutShortName() const
	{
		return "muon_eta";
	}
	static const long CutId = 1 << 2;
};

class LeadingJetEtaCut: public ZJetCutBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if( !metaData.HasValidJet( m_pipelineSettings,event ) )
		{
			// no decision possible for this event
			return;
		}

		KDataLV * jet = metaData.GetValidPrimaryJet( m_pipelineSettings, event );

		metaData.SetCutResult ( this->GetId(),
				(TMath::Abs(jet->p4.Eta())
						< m_pipelineSettings.GetCutLeadingJetEta() ));

	}

	unsigned long GetId() const
	{
		return LeadingJetEtaCut::CutId;
	}
	std::string GetCutName()
	{
		return "4) leading jet eta cut";
	}
	std::string GetCutShortName() const
	{
		return "leadingjet_eta";
	}
	static const long CutId = 1 << 3;
};

class SecondLeadingToZPtCut: public ZJetCutBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if ( !metaData.HasValidZ() )
		{
			// no decision possible for this event
			return;
		}

		if (metaData.GetValidJetCount( m_pipelineSettings, event) < 2)
		{
			// is ok, there seems to be no 2nd Jet in the event
			metaData.SetCutResult ( this->GetId(), true );
			return;
		}

		KDataLV * Jet2 = metaData.GetValidJet( m_pipelineSettings, event, 1 );

		metaData.SetCutResult ( this->GetId(),
				metaData.GetBalance(Jet2)
				< m_pipelineSettings.GetCutSecondLeadingToZPt());
	}

	unsigned long GetId() const
	{
		return SecondLeadingToZPtCut::CutId;
	}
	std::string GetCutName()
	{
		return "5) 2nd leading jet to Z pt";
	}
	std::string GetCutShortName() const
	{
		return "secondleading_to_zpt";
	}
	static const long CutId = 1 << 4;
};


// covers a region of the cut 0.1 > x > 0.2 etc...
class SecondLeadingToZPtRegionCut: public ZJetCutBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if ( !metaData.HasValidZ() )
		{
			// no decision possible for this event
			return;
		}

		if (metaData.GetValidJetCount( m_pipelineSettings, event) < 2)
		{
			// is ok, there seems to be no 2nd Jet in the event
			metaData.SetCutResult ( this->GetId(), true );
			return;
		}

		KDataLV * Jet2 = metaData.GetValidJet( m_pipelineSettings, event, 1 );

		metaData.SetCutResult(this->GetId(),
			(metaData.GetBalance(Jet2) >= m_pipelineSettings.GetCutSecondLeadingToZPtRegionLow() ) &&
			(metaData.GetBalance(Jet2) < m_pipelineSettings.GetCutSecondLeadingToZPtRegionHigh() )
		);
	}

	unsigned long GetId() const
	{
		return SecondLeadingToZPtRegionCut::CutId;
	}
	std::string GetCutName()
	{
		return "9) 2nd leading jet to Z pt (region)";
	}
	std::string GetCutShortName() const
	{
		return "secondleading_to_zpt_region";
	}
	static const long CutId = 1 << 10;
};

/* 31.5.2011, Thomas: commented for now, as it is not used any more
 class SecondLeadingToZPtCutDir: public EventCutBase<EventResult *>
 {
 public:
 SecondLeadingToZPtCutDir()
 {
 }

 bool IsInCut(EventResult * pEv)
 {

 if ( pEv->GetCorrectedJetPt(1) < m_f2ndJetThreshold )
 return true;

 if ( DeltaHelper::GetDeltaR(pEv->m_pData->jets[0], pEv->m_pData->jets[1]) > m_fDeltaR )
 return true;

 return (pEv->GetCorrectedJetPt(1) / pEv->m_pData->Z->Pt()
 < m_f2ndJetRatio);
 }
 void Configure( ZJetPipelineSettings * pset)
 {
 m_f2ndJetRatio = pset->GetCutSecondLeadingToZPt();
 m_f2ndJetThreshold = pset->GetCutSecondLeadingToZPtJet2Threshold();
 m_fDeltaR = pset->GetCutSecondLeadingToZPtDeltaR();
 }

 unsigned long GetId()
 {
 return SecondLeadingToZPtCutDir::CutId;
 }
 std::string GetCutShortName()
 {
 return "secondleading_to_zpt_dir";
 }
 static const long CutId = 2048;
 double m_f2ndJetRatio;
 double m_fDeltaR;
 double m_f2ndJetThreshold;
 };

 class SecondLeadingToZPtGeomCut: public EventCutBase<EventResult *>
 {
 public:
 SecondLeadingToZPtGeomCut(double f2ndJetRatio) :
 m_f2ndJetRatio(f2ndJetRatio)
 {
 }

 bool IsInCut(EventResult * pEv)
 {

 if ( pEv->GetCorrectedJetPt(1) < m_f2ndJetThreshold )
 return true;

 return (pEv->GetCorrectedJetPt(1) / pEv->m_pData->Z->Pt()
 < m_f2ndJetRatio);
 }
 void Configure( ZJetPipelineSettings * pset)
 {
 m_f2ndJetRatio = pset->GetCutSecondLeadingToZPt();
 m_f2ndJetThreshold = pset->GetCutSecondLeadingToZPtJet2Threshold();
 }

 unsigned long GetId()
 {
 return SecondLeadingToZPtGeomCut::CutId;
 }
 std::string GetCutName()
 {
 return "2nd leading jet to Z pt Geom";
 }
 std::string GetCutShortName()
 {
 return "secondleading_to_zpt_geom";
 }
 static const long CutId = 1024;

 double m_f2ndJetRatio;
 double m_f2ndJetThreshold;

 // the region in phi-eta space around the Jet1 that should be considered with this cut
 double m_fJetRcone;
 // the region in phi-eta space around the Z that should be considered with this cut
 double m_fZRcone;

 };
 */

class BackToBackCut: public ZJetCutBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (! metaData.HasValidJet(m_pipelineSettings, event) || !metaData.HasValidZ() )
			//No valid objects found to apply this cut
			return ;

		KDataLV * Jet1 = metaData.GetValidPrimaryJet( m_pipelineSettings, event);

		metaData.SetCutResult ( this->GetId(),
				(TMath::Abs(TMath::Abs(Jet1->p4.Phi() - metaData.GetRefZ().p4.Phi()) - TMath::Pi() ) )
				< m_pipelineSettings.GetCutBack2Back()
				);
	}


	unsigned long GetId() const
	{
		return BackToBackCut::CutId;
	}
	std::string GetCutName()
	{
		return "6) back to back/jet to z";
	}
	std::string GetCutShortName() const
	{
		return "back_to_back";
	}
	static const long CutId = 1 << 5;
};

class ZMassWindowCut: public ZJetCutBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if ( !metaData.HasValidZ())
		{
			//No valid objects found to apply this cut
			return;
		}

		metaData.SetCutResult ( this->GetId(),
				TMath::Abs(metaData.GetRefZ().p4.mass() - g_kZmass)
				< m_pipelineSettings.GetCutZMassWindow() );
	}

	unsigned long GetId() const
	{
		return ZMassWindowCut::CutId;
	}
	std::string GetCutName()
	{
		return "7) z mass window";
	}
	std::string GetCutShortName() const
	{
		return "zmass_window";
	}
	static const long CutId = 1 << 6;
};

class ZPtCut: public ZJetCutBase
{
public:
	ZPtCut()
	{

	}

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if ( !metaData.HasValidZ())
		{
			//No valid objects found to apply this cut
			return;
		}

		metaData.SetCutResult ( this->GetId(),
				metaData.GetRefZ().p4.Pt() > m_pipelineSettings.GetCutZPt() );
	}

	unsigned long GetId() const
	{
		return ZPtCut::CutId;
	}
	std::string GetCutName()
	{
		return "7.5) z pt";
	}
	std::string GetCutShortName() const
	{
		return "zpt";
	}
	static const long CutId = 1 << 7;
};


class HltCut: public ZJetCutBase
{
public:

	bool IsInCut(ZJetEventData * pEv, ZJetPipelineSettings * pset)
	{/*
		// SingleMu trigger: use always the lowest-pt unprescaled Mu trigger
		TString hltSingleMu = "HLT_Mu9";
		if (pEv->m_pData->cmsRun >= 147146) // 2010B (up to about 149711, json up to 149442)
			hltSingleMu = "HLT_Mu15_v1";
		if (pEv->m_pData->cmsRun >= 160000) // 2011A (ongoing, json starting with 160404)
			hltSingleMu = "HLT_Mu15_v2";

		// DoubleMu trigger: use unprescaled DoubleMu7 trigger
		TString hltDoubleMu1 = "HLT_DoubleMu7_v1";
		TString hltDoubleMu2 = "HLT_DoubleMu7_v2";

		const int nHLTriggers = pEv->m_pData->HLTriggers_accept->GetEntries();

		if (nHLTriggers == 0)
		{
			CALIB_LOG_FATAL( "No HLT Trigger in Event!");
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

	unsigned long GetId() const
	{
		return HltCut::CutId;
	}
	std::string GetCutName()
	{
		return "Hlt Cut";
	}
	std::string GetCutShortName() const
	{
		return "hlt";
	}
	static const long CutId = 1 << 9;
};

//VBF Cuts
/*    order name                bit
	1.) Jet pT cut		11
	2.) 2nd jet pT cut	12
	4.) Jet eta cut		 3
	3.) 2nd jet eta cut	13
	5.) Rapidity gap cuts	14
	6.) Jet Mass cut	15
	7.) No b tags		16
	8.) central jet veto	17
	9.)			18
*/

class LeadingJetPtCut: public ZJetCutBase
// pT_jet1 > 50 GeV
{
public:
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
		ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidJet(m_pipelineSettings, event))
		{
			// is not ok, there seems to be no jet in the event
			return;
		}

		KDataLV* jet = metaData.GetValidPrimaryJet(m_pipelineSettings, event);
		metaData.SetCutResult(this->GetId(),
			jet->p4.Pt() > m_pipelineSettings.GetCutLeadingJetPt()
		);
	}

	unsigned long GetId() const { return LeadingJetPtCut::CutId; }
	std::string GetCutName() { return "1) Leading Jet Pt Cut"; }
	std::string GetCutShortName() const { return "leadingjet_pt"; }
	static const long CutId = 1 << 11;
};

class SecondJetPtCut: public ZJetCutBase
// pT_jet2 > 30 GeV
{
public:
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
		ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 2)
		{
			// is not ok, there seems to be no 2nd jet in the event
			return;
		}
		KDataLV* jet = metaData.GetValidJet(m_pipelineSettings, event, 1);
		metaData.SetCutResult(this->GetId(),
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
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
		ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 2)
		{
			// is not ok, there seems to be no 2nd jet in the event
			return;
		}
		KDataLV* jet = metaData.GetValidJet(m_pipelineSettings, event, 1);
		metaData.SetCutResult(this->GetId(),
			TMath::Abs(jet->p4.Eta()) < m_pipelineSettings.GetCutSecondJetEta()
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
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
		ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (!metaData.HasValidJet(m_pipelineSettings, event))
		{
			// no decision possible for this event
			// redundant?
			return;
		}
		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 2)
		{
			// is not ok, there seems to be no 2nd Jet in the event
			//metaData.SetCutResult ( this->GetId(), true );
			return;
		}

		KDataLV * jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, event);
		KDataLV * jet2 = metaData.GetValidJet(m_pipelineSettings, event, 1);

		metaData.SetCutResult(this->GetId(), (
			(TMath::Abs(jet1->p4.Eta() - jet2->p4.Eta())
				> m_pipelineSettings.GetCutRapidityGap())
			&& (jet1->p4.Eta() * jet2->p4.Eta() < 0.0)
		));
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
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{

		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 2)
		{
			// is not ok, there seems to be no 2nd Jet in the event
			return;
		}

		KDataLV* jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, event);
		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, event, 1);

		metaData.SetCutResult(this->GetId(),
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
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 2)
		{
			// is not ok, there seems to be no 2nd Jet in the event
			return;
		}

		KDataLV* jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, event);
		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, event, 1);

		metaData.SetCutResult(this->GetId(),
			!false && !false
			//!jet1->btag && !jet2->btag
		);
	}

	unsigned long GetId() const { return BtagCut::CutId; }
	std::string GetCutName() { return "7) No b tag"; }
	std::string GetCutShortName() const { return "no_btag"; }
	static const long CutId = 1 << 16;
};

/* not in header!
double zeppenfeld(double y1, double y2, double y3, bool reduced=false)
{
	double
	if (reduced)
		return y
	return y;
}
*/

class CentralJetVeto: public ZJetCutBase
// no 3rd jet in between
{
public:
	virtual void PopulateMetaData(ZJetEventData const& event, ZJetMetaData& metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 2)
		{
			// this is no vbf event, 2nd jet is missing
			return;
		}

		if (metaData.GetValidJetCount(m_pipelineSettings, event) < 3)
		{
			// is ok, there seems to be no 3rd Jet in the event
			metaData.SetCutResult(this->GetId(), true);
			return;
		}


		KDataLV* jet1 = metaData.GetValidPrimaryJet(m_pipelineSettings, event);
		KDataLV* jet2 = metaData.GetValidJet(m_pipelineSettings, event, 1);
		KDataLV* jet3 = metaData.GetValidJet(m_pipelineSettings, event, 2);

		if (jet3->p4.Pt() <= m_pipelineSettings.GetCutThirdJetPt())
		{
			// is ok, the 3rd jet is very soft
			metaData.SetCutResult(this->GetId(), true);
			return;
		}

		metaData.SetCutResult(this->GetId(),
			//TMath::Abs(zeppenfeld(eta1, eta2, eta3, true)) > 0.5
			TMath::Abs((2.0 * jet3->p4.Eta() - jet1->p4.Eta() - jet2->p4.Eta()) / (jet1->p4.Eta() - jet2->p4.Eta())) < 1.0
		);
	}

	unsigned long GetId() const { return CentralJetVeto::CutId; }
	std::string GetCutName() { return "8) Central Jet Veto"; }
	std::string GetCutShortName() const { return "jetveto"; }
	static const long CutId = 1 << 17;
};

}
