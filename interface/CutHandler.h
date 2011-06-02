#pragma once

#include <string>
#include <algorithm>
#include <vector>

#include "GlobalInclude.h"
#include "RootIncludes.h"

#include "EventPipeline.h"
#include "Json_wrapper.h"



namespace CalibFW
{


template<class TEvent, class TCutConfig>
class EventCutBase
{
public:

		// returns true, if an Event passes the cuts, this Method returns True. The Event passes the cut.

	virtual ~EventCutBase() {}
	virtual bool IsInCut(TEvent evt, TCutConfig cutConf) = 0;
	virtual unsigned long GetId() = 0;
	virtual std::string GetCutName() { return "No Cut Name given";}
	virtual std::string GetCutShortName() = 0;

};


template < class TEvent, class TCutConfig >
class CutHandler
{

public:

//	typedef std::vector<EventCutBase<TEvent, TCutConfig> *> CutVector;

	typedef EventCutBase<TEvent, TCutConfig> 	CutClass;
	typedef std::vector< CutClass * > 			CutVector;
	typedef typename CutVector::iterator		CutVectorIterator;

	~CutHandler()
	{
		for (CutVectorIterator it = m_cuts.begin();
				!(it == m_cuts.end()); it++)
		{
			delete (*it);
		}
	}

	void AddCut(CutClass * pCut)
	{
		pCut->m_bCutEnabled = true;
		m_cuts.push_back(pCut);
	}
/*
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
				//(*it)->Configure(pset);
			}
		}
	}*/

	CutClass * GetById(unsigned long id)
	{
		/*for (CutVector::iterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ((*it)->GetId() == id)
			{
				return (*it);
			}
		}
*/
		// :( not found, yoda sad ...
		return NULL;
	}

	// returns true if all cuts were passed
	bool ApplyCuts(TEvent evt, TCutConfig cfg)
	{
		evt->m_cutBitmask = 0;

		for (CutVectorIterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ( IsCutEnabled( (*it, cfg) ))
				if (!(*it)->IsInCut(evt, cfg))
					evt->m_cutBitmask = evt->m_cutBitmask | (*it)->GetId();
		}

		// true if all cuts were passed
		return (evt->m_cutBitmask == 0);
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

	static bool IsCutEnabled( CutClass * pCut, TCutConfig cfg  )
	{
		return (cfg->GetCutEnabledBitmask() & pCut->GetId() ) > 0;
	}

	// is in Json file and passend the HLT selection, so is a valid event
	// from a technical standpoint, but not necessarily in physics cuts
	static bool IsValidEvent( TEvent * pEvRes)
	{
//		return pEvRes->
		// for MC Events, this should always return true
//		return true;	// I don't understand why I should include this check here. - Now I understand it.
//						// This logically leads to 100% accepted in json and hlt cut.
/*		return (! IsCutInBitmask( JsonCut::CudId, pEvRes->m_cutBitmask ) &&
				! IsCutInBitmask( HltCut::CudId, pEvRes->m_cutBitmask ));*/

		return false;
	}

private:
	CutVector m_cuts;
};

}

