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
	virtual ~EventCutBase() {}
	// returns true, if an Event passes the cuts
	virtual bool IsInCut(TEvent evt, TCutConfig cutConf) = 0;

	virtual unsigned long GetId() = 0;
	virtual std::string GetCutName() { return "No Cut Name given";}
	virtual std::string GetCutShortName() = 0;

	virtual ~EventCutBase(){}
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
		m_cuts.push_back(pCut);
	}

	CutClass * GetById(unsigned long id)
	{
		for (CutVectorIterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
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
	bool ApplyCuts(TEvent evt, TCutConfig cfg)
	{
		evt->m_cutBitmask = 0;

		for (CutVectorIterator it = m_cuts.begin(); !(it == m_cuts.end()); it++)
		{
			if ( IsCutEnabled( *it, cfg ))
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

	static bool IsCutEnabled( CutClass * pCut, TCutConfig cfg  )
	{
		return (cfg->GetCutEnabledBitmask() & pCut->GetId() ) > 0;
	}

private:
	CutVector m_cuts;
};

}

