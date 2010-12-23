#ifndef __CUTHANDLER_H
#define __CUTHANDLER_H

#include <string>
#include <algorithm>


#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "GlobalInclude.h"
#include "EventPipeline.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "Json_wrapper.h"

using namespace CalibFW;

const double g_kZmass = 91.19;

const double g_kCutZPt = 15.0; // Z.Pt() > 15 !
const double g_kCutMuEta = 2.3;
const double g_kCutLeadingJetEta = 1.3;

template<class TEvent>
class EventCutBase
{
public:
	virtual bool IsInCut(TEvent evt) = 0;
	virtual unsigned long GetId() = 0;
	virtual std::string GetCutName() = 0;
	virtual std::string GetCutShortName() = 0;
	virtual void Configure( PipelineSettings * pset) =0;

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

	void Configure( PipelineSettings * pset)
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

	void Configure( PipelineSettings * pset)
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

	void Configure( PipelineSettings * pset)
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

	void Configure( PipelineSettings * pset)
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
		return (pEv->GetCorrectedJetPt(1) / pEv->m_pData->Z->Pt()
				< m_f2ndJetRatio);
	}
	void Configure( PipelineSettings * pset)
	{
		m_f2ndJetRatio = pset->GetCutSecondLeadingToZPt();
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
  double m_f2nd_Backness;
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
	void Configure( PipelineSettings * pset)
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
	void Configure( PipelineSettings * pset)
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

	void Configure( PipelineSettings * pset)
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

	void Configure( PipelineSettings * pset)
	{
		m_fMinJetPt  = pset->GetCutJetPt();
	}


	bool IsInCut(EventResult * pEv)
	{
		return (pEv->GetCorrectedJetPt(0) > m_fMinJetPt);
	}

	unsigned long GetId()
	{
		return MuonPtCut::CudId;
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
		TString hltName = "HLT_Mu9";

		/* 1 trigger approach */
		if (pEv->m_pData->cmsRun >= 147146)
			hltName = "HLT_Mu15_v1";

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

	void Configure( PipelineSettings * pset)
	{
		// No Config here. Json is global for now
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

	typedef boost::ptr_vector<EventCutBase<EventResult *> > CutVector;

	void AddCut(EventCutBase<EventResult *> * pCut)
	{
		pCut->m_bCutEnabled = true;
		m_cuts.push_back(pCut);
	}

	void ConfigureCuts(PipelineSettings * pset)
	{
		// "activated" cuts
		stringvector svec = pset->GetCuts();

		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ( find(svec.begin(), svec.end(), it->GetCutShortName()) == svec.end())
			{
				it->m_bCutEnabled = false;
			}
			else
			{
				it->m_bCutEnabled = true;
				it->Configure(pset);
			}
		}
	}

	EventCutBase<EventResult *> * GetById(unsigned long id)
	{
		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if (it->GetId() == id)
			{
				return &*it;
			}
		}

		// :( not found, yoda sad ...
		return NULL;
	}

	// returns true if all cuts were passed
	bool ApplyCuts(EventResult * evt)
	{
		evt->m_cutBitmask = 0;
		evt->m_sCutResult = " -- not set any more --";
		evt->m_sCutUsed = "";

		for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if (it->m_bCutEnabled)
			{
				if (!it->IsInCut(evt))
				{
					evt->m_cutBitmask = evt->m_cutBitmask | it->GetId();

					// legacy stuff
					if (evt->m_sCutUsed == "")
					{
						evt->m_sCutUsed = it->GetCutName();
					}

					if (it->GetId() == (unsigned long)JsonCut::CudId)
						evt->m_cutResult = NotInJson;
					else
						evt->m_cutResult = NotInCutParameters;
				}
			}
		}

		if (evt->m_cutBitmask == 0)
		{
			// all cuts are good
			evt->m_sCutUsed = "8) within cut";
			evt->m_cutResult = InCut;
			return true;
		}

		return false;
	}

	CutVector m_cuts;

	CutVector & GetCuts()
	{
		return m_cuts;
	}

	bool IsCutInBitmask( unsigned long cutId, unsigned long bitmask)
	{
		return ( cutId & bitmask ) > 0;
	}

	void SetEnableCut(unsigned long CutId, bool val)
	{
		GetById(CutId)->m_bCutEnabled = val;
	}

};

// global cut handler used by all parts of the program
CutHandler g_cutHandler;

#endif 
