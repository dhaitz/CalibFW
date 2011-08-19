#pragma once

#include "EventData.h"
#include "CutHandler.h"
#include "ZJetPipeline.h"

namespace CalibFW
{

const double g_kZmass = 91.19;

typedef MetaDataProducerBase<ZJetEventData , ZJetMetaData,  ZJetPipelineSettings > ZJetCutBase;
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

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
/*
		return m_jsonFile->has(pEv->m_pData->cmsRun,
				pEv->m_pData->luminosityBlock);*/
		// todo
		return true;
	}

	unsigned long GetId()
	{
		return JsonCut::CudId;
	}
	std::string GetCutName()
	{
		return "1) invalidated by json file";
	}
	std::string GetCutShortName()
	{
		return "json";
	}
	static const long CudId = 1;

	Json_wrapper * m_jsonFile;
};

class MuonPtCut: public ZJetCutBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		// todo
	}

	bool IsInCut(ZJetEventData * pEv, ZJetPipelineSettings * pset)
	{/*
		return ((pEv->m_pData->mu_plus->Pt() > pset->GetCutMuonPt())
				&& (pEv->m_pData->mu_minus->Pt() > pset->GetCutMuonPt()));*/
		// todo
		return true;
	}

	unsigned long GetId()
	{
		return MuonPtCut::CudId;
	}
	std::string GetCutName()
	{
		return "2) muon pt cut";
	}
	std::string GetCutShortName()
	{
		return "muon_pt";
	}
	static const long CudId = 2;
};

class MuonEtaCut: public ZJetCutBase
{
public:

	bool IsInCut(ZJetEventData * pEv, ZJetPipelineSettings * pset)
	{/*
		return ((TMath::Abs(pEv->m_pData->mu_plus->Eta())
				< pset->GetCutMuonEta()) && (TMath::Abs(
				pEv->m_pData->mu_minus->Eta()) < pset->GetCutMuonEta()));
		*/
		// todo
		return true;	}

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		//todo
	}

	unsigned long GetId()
	{
		return MuonEtaCut::CudId;
	}
	std::string GetCutName()
	{
		return "3) muon eta cut";
	}
	std::string GetCutShortName()
	{
		return "muon_eta";
	}
	static const long CudId = 4;
};

class LeadingJetEtaCut: public ZJetCutBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		KDataLV * jet = data.GetPrimaryJet( m_pipelineSettings );

		if ( jet != NULL)
		{
			metaData.SetCutResult ( this->GetId(),
					(TMath::Abs(jet->p4.Eta())
							< m_pipelineSettings.GetCutLeadingJetEta() ));
		}
	}

	unsigned long GetId()
	{
		return LeadingJetEtaCut::CudId;
	}
	std::string GetCutName()
	{
		return "4) leading jet eta cut";
	}
	std::string GetCutShortName()
	{
		return "leadingjet_eta";
	}
	static const long CudId = 8;
};

class SecondLeadingToZPtCut: public ZJetCutBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		KDataLV * Jet2 = data.GetJet( m_pipelineSettings, 1 );

		if ( ! metaData.HasValidZ())
		{
			// no decision possible for this event
			return;
		}

		if ( Jet2 == NULL )
		{
			// is ok, there seems to be no 2nd Jet in the event
			metaData.SetCutResult ( this->GetId(), true );
			return;
		}

		metaData.SetCutResult ( this->GetId(),
				 Jet2->p4.Pt() / metaData.GetRefZ().p4.Pt()
  				 < m_pipelineSettings.GetCutSecondLeadingToZPt());
	}

	unsigned long GetId()
	{
		return SecondLeadingToZPtCut::CudId;
	}
	std::string GetCutName()
	{
		return "5) 2nd leading jet to Z pt";
	}
	std::string GetCutShortName()
	{
		return "secondleading_to_zpt";
	}
	static const long CudId = 16;
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
 return SecondLeadingToZPtCutDir::CudId;
 }
 std::string GetCutShortName()
 {
 return "secondleading_to_zpt_dir";
 }
 static const long CudId = 2048;
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
 return SecondLeadingToZPtGeomCut::CudId;
 }
 std::string GetCutName()
 {
 return "2nd leading jet to Z pt Geom";
 }
 std::string GetCutShortName()
 {
 return "secondleading_to_zpt_geom";
 }
 static const long CudId = 1024;

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
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		KDataLV * Jet1 = data.GetPrimaryJet( m_pipelineSettings);

		if ( (!metaData.HasValidZ()) && (Jet1 == NULL))
		{
			//No valid objects found to apply this cut
			return;
		}

		metaData.SetCutResult ( this->GetId(),
				(TMath::Abs(TMath::Abs(Jet1->p4.Phi() - metaData.GetRefZ().p4.Phi()) - TMath::Pi() ) )
				< m_pipelineSettings.GetCutBack2Back()
				);
	}


	unsigned long GetId()
	{
		return BackToBackCut::CudId;
	}
	std::string GetCutName()
	{
		return "6) back to back/jet to z";
	}
	std::string GetCutShortName()
	{
		return "back_to_back";
	}
	static const long CudId = 32;
};

class ZMassWindowCut: public ZJetCutBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
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

	unsigned long GetId()
	{
		return ZMassWindowCut::CudId;
	}
	std::string GetCutName()
	{
		return "7) z mass window";
	}
	std::string GetCutShortName()
	{
		return "zmass_window";
	}
	static const long CudId = 64;
};

class ZPtCut: public ZJetCutBase
{
public:
	ZPtCut()
	{

	}

	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		if ( !metaData.HasValidZ())
		{
			//No valid objects found to apply this cut
			return;
		}

		metaData.SetCutResult ( this->GetId(),
				metaData.GetRefZ().p4.Pt() > m_pipelineSettings.GetCutZPt() );
	}

	unsigned long GetId()
	{
		return ZPtCut::CudId;
	}
	std::string GetCutName()
	{
		return "7.5) z pt";
	}
	std::string GetCutShortName()
	{
		return "zpt";
	}
	static const long CudId = 128;
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

	unsigned long GetId()
	{
		return HltCut::CudId;
	}
	std::string GetCutName()
	{
		return "Hlt Cut";
	}
	std::string GetCutShortName()
	{
		return "hlt";
	}
	static const long CudId = 512;
};

}
