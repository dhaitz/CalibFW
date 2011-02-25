#ifndef ___EVENTDATA_H
#define ___EVENTDATA_H

#include <set>

// this include will overload comparison headers automatically
//#include <utility>
//using namespace std::rel_ops;

#include <iomanip>

#include <vector>
#include <sstream>

#include <boost/ptr_container/ptr_vector.hpp>

#include "RootIncludes.h"
#include "GlobalInclude.h"
#include "PtBinWeighter.h"

#define SAFE_DELETE( first ) {  if ( first != NULL ) { delete first; } }

enum CorrectionLevelEnum
{
	Uncorrected, L2Corrected, L3Corrected
};
enum InputTypeEnum
{
	McInput, DataInput
};
/*
 * - Charged Energy Fraction distribution: ChargedHadronEnergy
- Neutral Energy Fraction distribution: NeutralHadronEnergy
- Photon Energy Fraction distributin: PhotonEnergy
 *
 */


typedef struct {float ChargedHadronEnergy,
                ChargedHadronMultiplicity,
                NeutralHadronEnergy,
                NeutralHadronMultiplicity,
                ChargedEmEnergy,
                NeutralEmEnergy,

                NeutralMultiplicity,
                ChargedMultiplicity,

                ElectronEnergy,
                ElectronMultiplicity,
                MuonEnergy,
                MuonMultiplicity,
                PhotonEnergy,
                PhotonMultiplicity,
              Constituents;} PFProperties;

class evtData: boost::noncopyable
{
public:
	TParticle *Z, *matched_Z,*mu_minus, *mu_plus;

	TParticle *jets[3];
	TParticle *matched_calo_jets[3];

	TParticle *met, *tcmet;

	PFProperties * pfProperties[3];

	TClonesArray * HLTriggers_accept;

	TClonesArray * recoVertices;
	TClonesArray * recoVerticesInfo;
	TClonesArray * recoVerticesError;

	TVector3 * beamSpot;

	Double_t xsection;
	Double_t weight;

	Long_t cmsEventNum;
	Long_t cmsRun;
	Int_t luminosityBlock;
    Int_t partonFlavour;

	evtData()
	{
		Z = matched_Z = mu_minus = mu_plus = jets[0] = jets[1] = jets[2] = met = tcmet
				= NULL;

		pfProperties[0] = pfProperties[1] =pfProperties[2] = NULL;

		matched_calo_jets[0] =matched_calo_jets[1] = matched_calo_jets[2] = NULL;
		HLTriggers_accept = recoVertices = recoVerticesInfo = recoVerticesError
				= NULL;
		beamSpot = NULL;
		weight = 1.0f;
	}

	~evtData()
	{
// 		SAFE_DELETE ( Z )
//      SAFE_DELETE ( matched_Z )
// 		SAFE_DELETE ( mu_minus )
// 		SAFE_DELETE ( mu_plus )
// 		SAFE_DELETE ( jets[0] )
// 		SAFE_DELETE ( jets[1] )
// 		SAFE_DELETE ( jets[2] )
// 
// 		SAFE_DELETE ( matched_calo_jets[0] )
// 		SAFE_DELETE ( matched_calo_jets[1] )
// 		SAFE_DELETE ( matched_calo_jets[2] )
// 
// 		SAFE_DELETE ( pfProperties[0] )
// 		SAFE_DELETE ( pfProperties[1] )
// 		SAFE_DELETE ( pfProperties[2] )
// 
// 
// 		SAFE_DELETE ( met )
// 		SAFE_DELETE ( tcmet )
// 		SAFE_DELETE ( recoVertices )
// 		SAFE_DELETE ( recoVerticesInfo )
// 		SAFE_DELETE ( recoVerticesError )
// 		SAFE_DELETE ( HLTriggers_accept )
// 		SAFE_DELETE ( beamSpot )
	}

	// true if the event is within cuts
	//bool inCut;
	evtData * Clone()
	{
		evtData * ev = new evtData();
		ev->Z = new TParticle(*this->Z);
        ev->matched_Z = new TParticle(*this->matched_Z);
		ev->mu_minus = new TParticle(*this->mu_minus);
		ev->mu_plus = new TParticle(*this->mu_plus);
		ev->met = new TParticle(*this->met);
		ev->tcmet = new TParticle(*this->tcmet);

		for (int i = 0; i < 3; ++i)
		{
			ev->jets[i] = new TParticle(*this->jets[i]);
		}

		for (int i = 0; i < 3; ++i)
		{
			if (this->matched_calo_jets[i] != NULL )
				ev->matched_calo_jets[i] = new TParticle(*this->matched_calo_jets[i]);
		}

		for (int i = 0; i < 3; ++i)
		{
			if (this->pfProperties[i] != NULL )
				ev->pfProperties[i] = new PFProperties(*this->pfProperties[i]);
		}

		this->HLTriggers_accept != NULL ? ev->HLTriggers_accept
				= new TClonesArray(*this->HLTriggers_accept)
				: ev->HLTriggers_accept = NULL;

		this->recoVertices != NULL ? ev->recoVertices = new TClonesArray(
				*this->recoVertices) : this->recoVertices = NULL;
		this->recoVerticesInfo != NULL ? ev->recoVerticesInfo
				= new TClonesArray(*this->recoVerticesInfo)
				: this->recoVerticesInfo = NULL;
		this->recoVerticesError != NULL ? ev->recoVerticesError
				= new TClonesArray(*this->recoVerticesError)
				: this->recoVerticesError = NULL;

		this->beamSpot != NULL ? ev->beamSpot = new TVector3(*this->beamSpot)
				: this->beamSpot = NULL;

		ev->cmsEventNum = this->cmsEventNum;
		ev->cmsRun = this->cmsRun;
		ev->luminosityBlock = this->luminosityBlock;

		ev->xsection = this->xsection;
		ev->weight = this->weight;

        ev->partonFlavour = this->partonFlavour;

		return ev;
	}
};

class RootNamer
{
public:
	static unsigned long m_lTempNameAppend;

	static std::string GetTempHistoName()
	{
		std::stringstream tempname;
		m_lTempNameAppend++;
		tempname << "root_temp_histo_" << m_lTempNameAppend;
		return tempname.str();
	}

    static TString GetFolderName( PtBin * pBin )
    {
        if ( pBin == NULL )
            return "NoBinning_incut/";
        else
            return pBin->id() + "_incut/";

    }

	static TString GetHistoName(TString algoName, TString quantName,
			InputTypeEnum inpType, int corr = 0, PtBin * pBin = NULL,
			bool isNoCut = false, TString algoNameAppend = "Zplusjet")
	{
		TString sinput = "";
		TString scorr = "";
		TString binning = "";

		stringvector tagList;

		tagList.push_back(quantName.Data());
		tagList.push_back(algoName.Data());
		tagList.push_back(algoNameAppend.Data());

		if (inpType == McInput)
			tagList.push_back("mc");
		if (inpType == DataInput)
			tagList.push_back("data");

		if (corr == 2)
			tagList.push_back("l2corr");
		if (corr == 3)
			tagList.push_back("l3corr");

		if (pBin != NULL)
			tagList.push_back(pBin->id().Data());

		if (isNoCut)
			tagList.push_back("nocut");

		std::stringstream fullString;

		for (stringvector::iterator it = tagList.begin(); it != tagList.end(); it++)
		{
			fullString << (*it);
			if (it != (tagList.end() - 1))
				fullString << "_";
		}
		return fullString.str();
	}
};

unsigned long RootNamer::m_lTempNameAppend = 0;

class EventId
{
public:
	EventId(Long_t run, Int_t lumi, Long_t evNum)
	{
		this->m_cmsEventNum = evNum;
		this->m_luminosityBlock = lumi;
		this->m_cmsRun = run;
	}

	EventId(evtData * pData)
	{
		this->m_cmsEventNum = pData->cmsEventNum;
		this->m_cmsRun = pData->cmsRun;
		this->m_luminosityBlock = pData->luminosityBlock;
	}

	inline bool operator==(const EventId &b) const
	{
		return (b.m_cmsEventNum == this->m_cmsEventNum) && (b.m_cmsRun
				== this->m_cmsRun);
	}

	inline bool operator<(const EventId &b) const
	{
		if (this->m_cmsRun < b.m_cmsRun)
			return true;

		if (this->m_cmsRun == b.m_cmsRun)
			return (this->m_cmsEventNum < b.m_cmsEventNum);
		else
			return false;
	}

	TString ToString()
	{
		TString sevt;
		sevt += "Run: ";
		sevt += this->m_cmsRun;
		sevt += " Lumi: ";
		sevt += this->m_luminosityBlock;
		sevt += " Evt#: ";
		sevt += this->m_cmsEventNum;

		return sevt;
	}

	Long_t m_cmsEventNum;
	Long_t m_cmsRun;

	// just for completeness, not part of the eventst unique id
	Int_t m_luminosityBlock;
};

class EventResult
{
public:
	EventResult()
	{
		m_weight = 1.0f;
		m_pData = NULL;
		m_bUseL2 = false;
		m_bUseL3 = false;
		m_bEventReweighting = false;

		m_l2CorrPtJets[0] = 1.0f;
		m_l2CorrPtJets[1] = 1.0f;
		m_l2CorrPtJets[2] = 1.0f;

		m_l3CorrPtJets[0] = 1.0f;
		m_l3CorrPtJets[1] = 1.0f;
		m_l3CorrPtJets[2] = 1.0f;

		m_cutBitmask = 0;
	}

	~EventResult()
	{
		// don't delete here, since this a pointer which is managed by root when calling
		// TChain::GetEntry
		//SAFE_DELETE( m_pData );
	}

	bool m_bUseL2;
	Double_t m_l2CorrPtJets[3];

	bool m_bUseL3;
	Double_t m_l3CorrPtJets[3];

	unsigned long m_cutBitmask;


	// if a jet has .Pt() == 0.0f , there is no 2nd/3rd jet in this event. dont add this
	// to distributions
	bool IsJetValid(int jetNum)
	{
		return (this->GetCorrectedJetPt(jetNum) > 0.0f);
	}

	double GetCorrectedJetPt(int jetIndex)
	{
		double fVal = this->m_pData->jets[jetIndex]->Pt();
		if (m_bUseL2)
		{
			fVal = fVal * this->m_l2CorrPtJets[jetIndex];
		}
		if (m_bUseL3)
		{
			fVal = fVal * this->m_l3CorrPtJets[jetIndex];
		}
		return fVal;
	}

	double GetCorrectedJetResponse()
	{
		return this->GetCorrectedJetPt(0) / this->m_pData->Z->Pt();
	}

	int GetRecoVerticesCount()
	{
		int val = 0;
		if (this->m_pData->recoVertices != NULL)
			val = this->m_pData->recoVertices->GetEntries();

		return val;
	}

	bool IsInCut()
	{
		return (this->m_cutBitmask == 0);
	}

	bool IsInCutWhenIgnoringCut(unsigned long ignoredCut)
	{
		// ~ is bitwise negation
		return (((~ignoredCut ) & this->m_cutBitmask) == 0);
	}


	evtData * m_pData;


	double GetWeight( )
	{
		if ( ! m_bEventReweighting)
		{
			return m_weight;
		}
		else
		{
			double add=1.0f;

			switch ( this->GetRecoVerticesCount() )
			{
			case 1:
				add =  1.4361101137043686;// orig  1.4961101137043686;
				break;
			case 2:
				add = 0.9526167265264238;// orig1.026167265264238;
				break;
			case 3:
				add = 0.80542188805346695;
				break;
			case 4:
				add = 0.65220042700827331;
				break;
			case 5:
				add = 0.66773504273504269;
				break;
			case 6:
				add = 0.72688153284777646;
				break;
			case 7:
				add = 1.0978997178397725;
				break;
			case 8:
				add = 1.0f;
				break;
			}

			return (m_weight * add);
		}
	}


	void SetWeight( double v)
	{
		m_weight = v;
	}

	bool m_bEventReweighting;
private:
	// only useful for mc
	double m_weight;

};

// uses evtData to output a formated line containing all important
// values
class EventFormater
{
public:
	inline void Header(ostream & os)
	{
		os << std::setw(10) << "cmsRun" << std::setw(12) << "cmsEventNum"
				<< std::setw(7) << "lumi" << std::setw(10) << "Z.Pt()"
				<< std::setw(10) << "Z.Mass()" << std::setw(10) << "Z.Phi()"
				<< std::setw(10) << "jet.Pt()" << std::setw(10) << "uncorr"
				<< std::setw(10) << "jet.Eta()" << std::setw(10) << "jet.Phi()";
		//return "cmsRun\tcmsEventNum\tlumi\tZ.Pt()\tZ.Mass()\tZ.Phi()\tjet.Pt()\tjet.Eta()\tjet.Phi()\t";
	}

	inline void Format(ostream & os, evtData * pEv)
	{
		os << std::setprecision(3) << std::fixed;
		os << std::setw(10) << pEv->cmsRun << std::setw(12) << pEv->cmsEventNum
				<< std::setw(7) << pEv->luminosityBlock << std::setw(10)
				<< pEv->Z->Pt() << std::setw(10) << pEv->Z->GetCalcMass()
				<< std::setw(10) << pEv->Z->Phi() << std::setw(10)
				<< pEv->jets[0]->Pt() << std::setw(10) << "nop"
				<< std::setw(10) << pEv->jets[0]->Eta() << std::setw(10)
				<< pEv->jets[0]->Phi();
	}

	inline void FormatEventResultCorrected(ostream & os, EventResult * pEv)
	{
		os << std::setprecision(3) << std::fixed;
		os << std::setw(10) << pEv->m_pData->cmsRun << std::setw(12)
				<< pEv->m_pData->cmsEventNum << std::setw(7)
				<< pEv->m_pData->luminosityBlock << std::setw(10)
				<< pEv->m_pData->Z->Pt() << std::setw(10)
				<< pEv->m_pData->Z->GetCalcMass() << std::setw(10)
				<< pEv->m_pData->Z->Phi() << std::setw(10)
				<< pEv->GetCorrectedJetPt(0) << std::setw(10)
				<< pEv->m_pData->jets[0]->Pt() << std::setw(10)
				<< pEv->m_pData->jets[0]->Eta() << std::setw(10)
				<< pEv->m_pData->jets[0]->Phi();
	}
};

struct CompareEventResult: std::binary_function<EventResult, EventResult, bool>
{
	CompareEventResult()
	{
	}

	bool operator()(const EventResult& v1, const EventResult& v2) const
	{
		return EventId(v1.m_pData) < EventId(v2.m_pData);
	}
};

typedef std::set<EventId> EventSet;
typedef boost::ptr_vector<EventResult> EventVector;
typedef boost::ptr_vector<evtData> EventDataVector;

#endif
