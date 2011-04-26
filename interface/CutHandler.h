#ifndef __CUTHANDLER_H
#define __CUTHANDLER_H

#include <string>
#include <algorithm>


#include <boost/shared_ptr.hpp>

#include "GlobalInclude.h"
#include "EventPipeline.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "Json_wrapper.h"
#include "ZJetPipeline.h"

namespace CalibFW
{

const double g_kZmass = 91.19;

const double g_kCutZPt = 15.0; // Z.Pt() > 15 !
const double g_kCutMuEta = 2.3;
const double g_kCutLeadingJetEta = 1.3;

template<class TEvent>
class EventCutBase
{
public:
	/*
		returns true, if an Event passes the cuts, this Method returns True. The Event passes the cut.
	 */
	virtual ~EventCutBase() {}
	virtual bool IsInCut(TEvent evt) = 0;
	virtual unsigned long GetId() = 0;
	virtual std::string GetCutName() { return "No Cut Name given";}
	virtual std::string GetCutShortName() = 0;
	virtual void Configure( ZJetPipelineSettings * pset) =0;

	bool m_bCutEnabled;
};

class JsonCut: public EventCutBase<EventResult *>
{
public:
	JsonCut(Json_wrapper * jsonFile)
	{
		m_jsonFile = jsonFile;
	}

	bool IsInCut(EventResult * pEv)
	{
		if ( (m_jsonFile == NULL) || ( ! m_jsonFile->isValid()))
		{
			CALIB_LOG_FATAL("No valid JSON file loaded.")
		}

		return m_jsonFile->has(pEv->m_pData->cmsRun,
				pEv->m_pData->luminosityBlock);
	}

	void Configure( ZJetPipelineSettings * pset)
	{
		// No Config here. Json is global for now
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

class MuonPtCut: public EventCutBase<EventResult *>
{
public:
	MuonPtCut(double fMinMuonPt) :
		m_fMinMuonPt(fMinMuonPt)
	{
	}

	bool IsInCut(EventResult * pEv)
	{
		return ((pEv->m_pData->mu_plus->Pt() > m_fMinMuonPt)
				&& (pEv->m_pData->mu_minus->Pt() > m_fMinMuonPt));
	}

	void Configure( ZJetPipelineSettings * pset)
	{
		m_fMinMuonPt = pset->GetCutMuonPt();
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
	double m_fMinMuonPt;
};

class MuonEtaCut: public EventCutBase<EventResult *>
{
public:
	bool IsInCut(EventResult * pEv)
	{
		return ((TMath::Abs(pEv->m_pData->mu_plus->Eta()) < m_fMuEta)
				&& (TMath::Abs(pEv->m_pData->mu_minus->Eta()) < m_fMuEta));
	}

	void Configure( ZJetPipelineSettings * pset)
	{
		m_fMuEta = pset->GetCutMuonEta();
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
	double m_fMuEta;
};

class LeadingJetEtaCut: public EventCutBase<EventResult *>
{
public:
	bool IsInCut(EventResult * pEv)
	{
		return (TMath::Abs(pEv->m_pData->jets[0]->Eta()) < m_fLeadingJetEta);
	}

	void Configure( ZJetPipelineSettings * pset)
	{
		m_fLeadingJetEta = pset->GetCutLeadingJetEta();
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
	double m_fLeadingJetEta;
};

class SecondLeadingToZPtCut: public EventCutBase<EventResult *>
{
public:
	SecondLeadingToZPtCut(double f2ndJetRatio) :
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
	double m_f2ndJetRatio;
	double m_f2ndJetThreshold;
};


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

class BackToBackCut: public EventCutBase<EventResult *>
{
public:
	BackToBackCut(double fBackness) :
		m_fBackness(fBackness)
	{

	}

	bool IsInCut(EventResult * pEv)
	{
		return (TMath::Abs(TMath::Abs(pEv->m_pData->jets[0]->Phi()
				- pEv->m_pData->Z->Phi()) - TMath::Pi()) < m_fBackness);
	}
	void Configure( ZJetPipelineSettings * pset)
	{
		m_fBackness = pset->GetCutBack2Back();
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
	double m_fBackness;
	static const long CudId = 32;
};

class ZMassWindowCut: public EventCutBase<EventResult *>
{
public:
	ZMassWindowCut(double fWindowSize) :
		m_fWindowSize(fWindowSize)
	{

	}
	void Configure( ZJetPipelineSettings * pset)
	{
		m_fWindowSize  = pset->GetCutZMassWindow();
	}

	bool IsInCut(EventResult * pEv)
	{
		return (TMath::Abs(pEv->m_pData->Z->GetCalcMass() - g_kZmass)
				< m_fWindowSize);
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
	double m_fWindowSize;
};

class ZPtCut: public EventCutBase<EventResult *>
{
public:
	ZPtCut(double fMinZPt) :
		m_fMinZPt(fMinZPt)
	{

	}

	void Configure( ZJetPipelineSettings * pset)
	{
		m_fMinZPt  = pset->GetCutZPt();
	}


	bool IsInCut(EventResult * pEv)
	{
		return (pEv->m_pData->Z->Pt() > m_fMinZPt);
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
	double m_fMinZPt;
};

class JetPtCut: public EventCutBase<EventResult *>
{
public:
	JetPtCut(double fMinJetPt) :
		m_fMinJetPt(fMinJetPt)
	{
	}

	void Configure( ZJetPipelineSettings * pset)
	{
		m_fMinJetPt  = pset->GetCutJetPt();
	}


	bool IsInCut(EventResult * pEv)
	{
		return (pEv->GetCorrectedJetPt(0) > m_fMinJetPt);
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
	double m_fMinJetPt;

	static const long CudId = 256;
};


class HltCut: public EventCutBase<EventResult *>
{
public:
	HltCut()
	{
	}

	bool IsInCut(EventResult * pEv)
	{
		TString hltName = "HLT_Mu9";		// use always the lowest-pt unprescaled trigger

		/* 1 trigger approach */
		if (pEv->m_pData->cmsRun >= 147146)	// 2010B up to about 149711 (json up to 149442)
			hltName = "HLT_Mu15_v1";
		if (pEv->m_pData->cmsRun >= 160000)	// 2011A (ongoing, json starting with 160404)
			hltName = "HLT_Mu15_v2";

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

			if (hltName == curName)
			{
				return true;
			}
		}

		return false;
	}

	void Configure( ZJetPipelineSettings * pset)
	{
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

class CutHandler
{

public:

	typedef std::vector<EventCutBase<EventResult *> *> CutVector;

	~CutHandler()
	{
		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			delete (*it);
		}
	}

	void AddCut(EventCutBase<EventResult *> * pCut)
	{
		pCut->m_bCutEnabled = true;
		m_cuts.push_back(pCut);
	}

	void ConfigureCuts(ZJetPipelineSettings * pset)
	{
		// "activated" cuts
		stringvector svec = pset->GetCuts();

		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ( find(svec.begin(), svec.end(), (*it)->GetCutShortName()) == svec.end())
			{
				(*it)->m_bCutEnabled = false;
			}
			else
			{
				(*it)->m_bCutEnabled = true;
				(*it)->Configure(pset);
			}
		}
	}

	EventCutBase<EventResult *> * GetById(unsigned long id)
	{
		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ((*it)->GetId() == id)
			{
				return (*it);
			}
		}

		// :( not found, yoda sad ...
		return NULL;
	}

	// returns true if all cuts were passed
	bool ApplyCuts(EventResult * evt)
	{
		evt->m_cutBitmask = 0;

		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ((*it)->m_bCutEnabled)
			{
				if (!(*it)->IsInCut(evt))
				{
					evt->m_cutBitmask = evt->m_cutBitmask | (*it)->GetId();
				}
			}
		}

		if (evt->m_cutBitmask == 0)
		{
			return true;
		}

		return false;
	}

	CutVector & GetCuts()
	{
		return m_cuts;
	}

	static bool IsCutInBitmask( unsigned long cutId, unsigned long bitmask)
	{
		return ( cutId & bitmask ) > 0;
	}

	void SetEnableCut(unsigned long CutId, bool val)
	{
		GetById(CutId)->m_bCutEnabled = val;
	}

	// is in Json file and passend the HLT selection, so is a valid event
	// from a technical standpoint, but not necessarily in physics cuts
	static bool IsValidEvent( EventResult * pEvRes)
	{
		// for MC Events, this should always return true
//		return true;	// I don't understand why I should include this check here. - Now I understand it.
//						// This logically leads to 100% accepted in json and hlt cut.
		return (! IsCutInBitmask( JsonCut::CudId, pEvRes->m_cutBitmask ) &&
				! IsCutInBitmask( HltCut::CudId, pEvRes->m_cutBitmask ));

	}

private:
	CutVector m_cuts;
};

}
#endif 
