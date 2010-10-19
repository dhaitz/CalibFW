#ifndef ___EVENTDATA_H
#define ___EVENTDATA_H

#include <set>

// this include will overload comparison headers automatically
//#include <utility>
//using namespace std::rel_ops;

#include <iomanip>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>


#include "RootIncludes.h"

#include "PtBinWeighter.h"

#define SAFE_DELETE( first ) {  if ( first != NULL ) { delete first; } }

enum CorrectionLevelEnum { Uncorrected, L2Corrected, L3Corrected }; 
enum InputTypeEnum { McInput, DataInput };


class evtData : boost::noncopyable
{
public:
    TParticle *Z,*mu_minus,*mu_plus;

    TParticle *jets[3];

    Double_t xsection;
    Double_t weight;

    Long_t cmsEventNum;
    Long_t cmsRun;
    Int_t luminosityBlock;

      evtData()
      {
	Z =  mu_minus = mu_plus = jets[0] = jets[1] =jets[2] = NULL;
      }

      ~evtData()
      {
	SAFE_DELETE ( Z )
	SAFE_DELETE ( mu_minus )
	SAFE_DELETE ( mu_plus )
	SAFE_DELETE ( jets[0] )
	SAFE_DELETE ( jets[1] )
	SAFE_DELETE ( jets[2] )
      }

    // true if the event is within cuts
    //bool inCut;
    evtData * Clone()
    {
        evtData * ev = new evtData();
        ev->Z = new TParticle( *this->Z);
        ev->mu_minus = new TParticle( *this->mu_minus );
        ev->mu_plus= new TParticle( *this->mu_plus );

        for (int i = 0; i < 3; ++i)
        {
            ev->jets[i]= new TParticle( *this->jets[i]);
        }

        ev->cmsEventNum = this->cmsEventNum;
        ev->cmsRun = this->cmsRun;
        ev->luminosityBlock = this->luminosityBlock;

        ev->xsection = this->xsection;
	ev->weight = this->weight;
	
        return ev;
    }
};

class RootNamer
{
  public:
    
    static TString GetHistoName( TString algoName, 
			  TString quantName, 
			  InputTypeEnum inpType,
			  int corr = 0,
			  PtBin * pBin = NULL,
			  TString algoNameAppend = "Jets_Zplusjet")
      {
	TString sinput;
	TString scorr;
	TString binning;

	
	if (inpType == McInput )
	  sinput = "_mc_";
	if ( inpType == DataInput )
	  sinput = "_data_";
	
	if ( corr == 2 )
	  scorr = "l2corr_";
	if ( corr == 3 )
	  scorr = "l3corr_";

	
	if ( pBin != NULL )
	  binning = pBin->id();
	
	return quantName + "_" + algoName + algoNameAppend + sinput  + scorr + binning + "_hist";
      }
};

class EventId
{
public:
    EventId( Long_t run, Int_t lumi,  Long_t evNum  )
    {
        this->m_cmsEventNum = evNum;
        this->m_luminosityBlock = lumi;
        this->m_cmsRun = run;
    }

    EventId( evtData * pData )
    {
        this->m_cmsEventNum = pData->cmsEventNum;
        this->m_cmsRun = pData->cmsRun;
        this->m_luminosityBlock = pData->luminosityBlock;
    }

    inline bool operator== (const EventId &b) const
    {
        return ( b.m_cmsEventNum == this->m_cmsEventNum) &&
               ( b.m_cmsRun == this->m_cmsRun);
    }

    inline bool operator< (const EventId &b) const
    {
        if ( this->m_cmsRun < b.m_cmsRun )
            return true;

        if ( this->m_cmsRun == b.m_cmsRun )
            return ( this->m_cmsEventNum < b.m_cmsEventNum );
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


enum CutResultEnum {CutStatusUnknown, InCut, NotInJson, NotInCutParameters };


class EventResult
{
public:
    EventResult()
    {
        m_weight = 1.0f;
	m_pData = NULL;
	m_bUseL2 = false;
	m_bUseL3 = false;
	
	m_l2CorrPtJets[0] = 1.0f;
	m_l2CorrPtJets[1] = 1.0f;
	m_l2CorrPtJets[2] = 1.0f;
	
	m_l3CorrPtJets[0] = 1.0f;
	m_l3CorrPtJets[1] = 1.0f;
	m_l3CorrPtJets[2] = 1.0f;
	
    }
    
    ~EventResult()
    {
	SAFE_DELETE( m_pData ); 
    }

    bool m_bUseL2;
    Double_t m_l2CorrPtJets[3];
    
    bool m_bUseL3;
    Double_t m_l3CorrPtJets[3];    
    
    CutResultEnum m_cutResult;
    TString m_sCutResult;
    std::string m_sCutUsed;
    
    // only useful for mc
    double m_weight;

    double GetCorrectedJetPt( int jetIndex )
    {
      double fVal = this->m_pData->jets[jetIndex]->Pt();
      if ( m_bUseL2 ) 
      {
	fVal = fVal * this->m_l2CorrPtJets[jetIndex];
      }
      if ( m_bUseL3 ) 
      {
	fVal = fVal * this->m_l3CorrPtJets[jetIndex];
      }      
      return fVal;
    }
    
    bool IsInCut()
    {
        return (this->m_cutResult == InCut);
    }

    // is in Json file but not necessarily in cuts
    bool IsValidEvent()
    {
        // todo: remove, just for testing
        return (( this->m_cutResult == InCut ) || (this->m_cutResult == NotInCutParameters));
    }

    evtData * m_pData;
};

// uses evtData to output a formated line containing all important
// values
class EventFormater
{
public:
    inline void Header( ostream & os)
    {
        os << std::setw(10) << "cmsRun"  << std::setw(12) << "cmsEventNum" << std::setw(7) << "lumi"  << std::setw(10) << "Z.Pt()"
        << std::setw(10) << "Z.Mass()" << std::setw(10) << "Z.Phi()" << std::setw(10) << "jet.Pt()" << std::setw(10) << "uncorr"<< std::setw(10) << "jet.Eta()" << std::setw(10) << "jet.Phi()";
        //return "cmsRun\tcmsEventNum\tlumi\tZ.Pt()\tZ.Mass()\tZ.Phi()\tjet.Pt()\tjet.Eta()\tjet.Phi()\t";
    }

    inline void Format( ostream & os, evtData * pEv )
    {
        os << std::setprecision(3) << std::fixed ;
        os << std::setw(10) << pEv->cmsRun  << std::setw(12) << pEv->cmsEventNum << std::setw(7) << pEv->luminosityBlock  << std::setw(10) << pEv->Z->Pt()
        << std::setw(10) << pEv->Z->GetCalcMass() << std::setw(10) << pEv->Z->Phi() << std::setw(10) << pEv->jets[0]->Pt() << std::setw(10) << "nop" 
        << std::setw(10) << pEv->jets[0]->Eta() << std::setw(10) <<pEv->jets[0]->Phi();
    }
    
    inline void FormatEventResultCorrected( ostream & os, EventResult * pEv )
    {
        os << std::setprecision(3) << std::fixed ;
        os << std::setw(10) << pEv->m_pData->cmsRun << std::setw(12) << pEv->m_pData->cmsEventNum << std::setw(7) << pEv->m_pData->luminosityBlock  << std::setw(10) << pEv->m_pData->Z->Pt()
        << std::setw(10) << pEv->m_pData->Z->GetCalcMass() << std::setw(10) << pEv->m_pData->Z->Phi() << std::setw(10) << pEv->GetCorrectedJetPt(0)  << std::setw(10) << pEv->m_pData->jets[0]->Pt()
        << std::setw(10) << pEv->m_pData->jets[0]->Eta() << std::setw(10) <<pEv->m_pData->jets[0]->Phi();
    }    
};


struct CompareEventResult : std::binary_function<EventResult,EventResult,bool> {
   CompareEventResult(){ }

   bool operator()(const EventResult& v1, const EventResult& v2) const 
   {
	return EventId(v1.m_pData)< EventId(v2.m_pData);
   }
}; 

typedef std::set<EventId> EventSet;
typedef boost::ptr_vector<EventResult> EventVector;

#endif
