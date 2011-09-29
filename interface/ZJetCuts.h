#pragma once

#include "EventData.h"
#include "CutHandler.h"
#include "ZJetPipeline.h"

namespace CalibFW
{

const double g_kZmass = 91.19;

typedef EventCutBase<EventResult *, ZJetPipelineSettings *> ZJetCutBase;
typedef CutHandler<EventResult *, ZJetPipelineSettings *> ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

class JsonCut: public ZJetCutBase
{
public:
	JsonCut(Json_wrapper * jsonFile)
	{
		m_jsonFile = jsonFile;
	}

	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		if ((m_jsonFile == NULL) || (!m_jsonFile->isValid()))
		{
			CALIB_LOG_FATAL("No valid JSON file loaded.")
		}

		return m_jsonFile->has(pEv->m_pData->cmsRun,
				pEv->m_pData->luminosityBlock);
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

	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return ((pEv->m_pData->mu_plus->Pt() > pset->GetCutMuonPt())
				&& (pEv->m_pData->mu_minus->Pt() > pset->GetCutMuonPt()));
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
	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return ((TMath::Abs(pEv->m_pData->mu_plus->Eta())
				< pset->GetCutMuonEta()) && (TMath::Abs(
				pEv->m_pData->mu_minus->Eta()) < pset->GetCutMuonEta()));
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
	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return (TMath::Abs(pEv->m_pData->jets[0]->Eta())
				< pset->GetCutLeadingJetEta());
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

	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{

		if (pEv->GetCorrectedJetPt(1)
				< pset->GetCutSecondLeadingToZPtJet2Threshold())
			return true;

		return (pEv->GetCorrectedJetPt(1) / pEv->m_pData->Z->Pt()
				< pset->GetCutSecondLeadingToZPt());
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
	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return (TMath::Abs(TMath::Abs(pEv->m_pData->jets[0]->Phi()
				- pEv->m_pData->Z->Phi()) - TMath::Pi())
				< pset->GetCutBack2Back());
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
	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return (TMath::Abs(pEv->m_pData->Z->GetCalcMass() - g_kZmass)
				< pset->GetCutZMassWindow());
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

	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return (pEv->m_pData->Z->Pt() > pset->GetCutZPt());
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

class JetPtCut: public ZJetCutBase
{
public:
	JetPtCut()
	{
	}

	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		return (pEv->GetCorrectedJetPt(0) > pset->GetCutJetPt());
	}

	unsigned long GetId()
	{
		return JetPtCut::CudId;
	}
	std::string GetCutName()
	{
		return "10) jet pt cut";
	}
	std::string GetCutShortName()
	{
		return "jet_pt";
	}

	static const long CudId = 256;
};

class HltCut: public ZJetCutBase
{
public:

	bool IsInCut(EventResult * pEv, ZJetPipelineSettings * pset)
	{
		std::string triggerName = "HLT";

		// DoubleMu triggers (2011)
		if (pEv->m_pData->cmsRun < 165000)
			triggerName = "HLT_DoubleMu7_v*";
		else
			triggerName = "HLT_Mu13_Mu8_v*";

		// SingleMu triggers (used for 2010A+B)
		if (pset->GetCutHLT() == "SingleMu") {
			if (pEv->m_pData->cmsRun < 147146)
				triggerName = "HLT_Mu9";
			else
				triggerName = "HLT_Mu15_v*";
		}

		// Use wildcards at the end of string
		size_t n = triggerName.find('*');
		triggerName = triggerName.substr(0,n);

		const int nHLTriggers = pEv->m_pData->HLTriggers_accept->GetEntries();
		if (nHLTriggers == 0)
			CALIB_LOG_FATAL( "No HLT Trigger in Event!");
		TObjString *theHLTbit = NULL;

		for (int i = 0; i < nHLTriggers; ++i)
		{
			theHLTbit = (TObjString*) pEv->m_pData->HLTriggers_accept->At(i);
			std::string curName = theHLTbit->GetString().Data();

			if (n == std::string::npos) {
				// No wildcards used (exact match)
				if (curName == triggerName) return true;
			} else {
				// Use wildcards
				if (curName.find(triggerName) == 0) return true;
			}
		}
		return false;
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

extern std::vector<ZJetCutBase *> g_ZJetCuts;

}
