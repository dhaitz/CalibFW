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

#define SAFE_DELETE( first ) {  if ( first != NULL ) { delete first; } }

class evtData
{
public:
    TParticle *Z,*mu_minus,*mu_plus;

    TParticle *jets[3];

    Double_t xsection;

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

        return ev;
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

// uses evtData to output a formated line containing all important
// values
class EventFormater
{
public:
    inline void Header( ostream & os)
    {
        os << std::setw(10) << "cmsRun"  << std::setw(12) << "cmsEventNum" << std::setw(7) << "lumi"  << std::setw(10) << "Z.Pt()"
        << std::setw(10) << "Z.Mass()" << std::setw(10) << "Z.Phi()" << std::setw(10) << "jet.Pt()" << std::setw(10) << "jet.Eta()" << std::setw(10) << "jet.Phi()";
        //return "cmsRun\tcmsEventNum\tlumi\tZ.Pt()\tZ.Mass()\tZ.Phi()\tjet.Pt()\tjet.Eta()\tjet.Phi()\t";
    }

    inline void Format( ostream & os, evtData * pEv )
    {
        os << std::setprecision(3) << std::fixed ;
        os << std::setw(10) << pEv->cmsRun  << std::setw(12) << pEv->cmsEventNum << std::setw(7) << pEv->luminosityBlock  << std::setw(10) << pEv->Z->Pt()
        << std::setw(10) << pEv->Z->GetCalcMass() << std::setw(10) << pEv->Z->Phi() << std::setw(10) << pEv->jets[0]->Pt()
        << std::setw(10) << pEv->jets[0]->Eta() << std::setw(10) <<pEv->jets[0]->Phi();
    }
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
	m_l2Corr = 1.0f;
    }
    
    ~EventResult()
    {
	SAFE_DELETE( m_pData ); 
    }

    bool m_bUseL2;
    double m_l2Corr;

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
	fVal = fVal * this->m_l2Corr;
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
