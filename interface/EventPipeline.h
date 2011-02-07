#ifndef __EVENTPIPELINE_H__
#define __EVENTPIPELINE_H__

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>

#include <boost/property_tree/ptree.hpp>

#include <memory>

#include <sstream>
#include <typeinfo>

#include "GlobalInclude.h"

#include "Json_wrapper.h"

#include "RootIncludes.h"
#include "PtBinWeighter.h"
#include "EventData.h"
#include "CompleteJetCorrector.h"

/*
 This macro implements a Setting Propery including the property tree get\put methods
 */

#define IMPL_SETTING(TYPE, SNAME) \
private: \
TYPE m_##SNAME; 															\
public: \
std::string Key##SNAME () { return "##SNAME"; } 							\
std::string FullKey##SNAME () { return GetSettingsRoot() + "." + #SNAME; } 							\
VarCache<TYPE> Cache##SNAME; \
TYPE Get##SNAME ( ) { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); } 	\
	TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME ());	\
	Cache##SNAME.SetCache( val ); \
	return val;}		\
void Set##SNAME ( TYPE val) { GetPropTree()->put( FullKey##SNAME (), val);	\
								Cache##SNAME.SetCache( val );}	\


#define IMPL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) \
private: \
TYPE m_##SNAME; 															\
public: \
std::string Key##SNAME () { return "##SNAME"; } 							\
std::string FullKey##SNAME () { return GetSettingsRoot() + "." + #SNAME; } 							\
VarCache<TYPE> Cache##SNAME; \
TYPE Get##SNAME ( ) { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); } 	\
	TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME (), DEFAULT_VAL );	\
	Cache##SNAME.SetCache( val ); \
	return val;}		\
void Set##SNAME ( TYPE val) { GetPropTree()->put( FullKey##SNAME (), val);	\
								Cache##SNAME.SetCache( val );}	\

#define RETURN_CACHED(CACHE_MEMBER,VALUEPATH) \
{ if (! CACHE_MEMBER.IsCached() ) \
		CACHE_MEMBER.SetCache( VALUEPATH ); \
return CACHE_MEMBER.GetValue(); }\



namespace CalibFW
{

template<class TData>
class VarCache
{
public:
	VarCache() :
		m_isCached(false)
	{

	}

	void SetCache(TData t)
	{
		m_val = t;
		m_isCached = true;
	}

	TData GetValue()
	{
		if (!m_isCached)
			CALIB_LOG_FATAL("not Cached variable used")

		return m_val;
	}

	bool IsCached()
	{
		return m_isCached;
	}

	bool m_isCached;
	TData m_val;
};

class PropertyTreeSupport
{
public:
	static stringvector GetAsStringList(boost::property_tree::ptree * propTree,
			std::string path)
	{
		stringvector fvec;
		try
		{
			BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
					propTree->get_child(path))
{			fvec.push_back(v.second.data());
		}
	}
	catch ( boost::property_tree::ptree_bad_path & e)
	{
		// no problem, node optional
	}
	return fvec;
}
};

class PipelineSettings
{

public:
PipelineSettings()
{
	SetSettingsRoot("default");
}

enum WriteEventsEnum
{
	NoEvents, OnlyInCutEvents, AllEvents
};

IMPL_PROPERTY(boost::property_tree::ptree * , PropTree)
IMPL_PROPERTY(std::string, SettingsRoot)
IMPL_PROPERTY(unsigned long, OverallNumberOfProcessedEvents )

IMPL_SETTING( bool, EnableCaloMatching)


// Filter Settings
IMPL_SETTING(double, FilterPtBinLow)
IMPL_SETTING(double, FilterPtBinHigh)
IMPL_SETTING( int, FilterRecoVertLow)
IMPL_SETTING( int, FilterRecoVertHigh)

IMPL_SETTING(double, FilterSecondJetRatioLow)
IMPL_SETTING(double, FilterSecondJetRatioHigh)
IMPL_SETTING(unsigned long, FilterInCutIgnored)

IMPL_SETTING(std::string, AlgoName)
IMPL_SETTING(std::string, RootFileFolder)

IMPL_SETTING(std::string, SecondLevelFolderTemplate)


// only level 1 runs directly on data
IMPL_SETTING(int, Level)

IMPL_SETTING(std::string, WriteEvents)

WriteEventsEnum GetWriteEventsEnum()
{
	if (this->GetWriteEvents() == "all")
	return AllEvents;
	if (this->GetWriteEvents() == "incut")
	return OnlyInCutEvents;

	return NoEvents;
}

// Cut settings
IMPL_SETTING(double, CutMuonPt)
IMPL_SETTING(double, CutZMassWindow)
IMPL_SETTING(double, CutMuEta)
IMPL_SETTING(double, CutZPt)
IMPL_SETTING(double, CutMuonEta)
IMPL_SETTING(double, CutLeadingJetEta)
IMPL_SETTING(double, CutSecondLeadingToZPt)
IMPL_SETTING(double, CutSecondLeadingToZPtJet2Threshold)
IMPL_SETTING(double, CutSecondLeadingToZPtDeltaR)
IMPL_SETTING(double, CutBack2Back)
IMPL_SETTING(double, CutJetPt)

IMPL_PROPERTY(TFile *, RootOutFile)

InputTypeEnum GetInputType()
{
	std::string sInp = GetPropTree()->get< std::string >( "InputType");

	InputTypeEnum inp;

	if (sInp == "mc")
	inp = McInput;
	else if (sInp == "data")
	inp = DataInput;
	else
	CALIB_LOG_FATAL("Input type " + sInp + " not supported.")

	return inp;
}

VarCache< stringvector > m_filter;

stringvector GetFilter()
{
	RETURN_CACHED( m_filter, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".Filter") )
}

VarCache< stringvector > m_L2Corr;

stringvector GetL2Corr()
{
	RETURN_CACHED( m_L2Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L2Corr") )
}

VarCache< stringvector > m_L3Corr;

stringvector GetL3Corr()
{
	RETURN_CACHED( m_L3Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L3Corr") )
}


VarCache< stringvector > m_additionalConsumer;

stringvector GetAdditionalConsumer()
{
	RETURN_CACHED( m_additionalConsumer, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".AdditionalConsumer") )
}

static std::vector<PtBin> GetAsPtBins( stringvector & sv )
{
	std::vector< PtBin > bins;

	int i = 0;
	for ( stringvector::iterator it = (sv.begin() + 1);
			it < sv.end();
			it ++)
	{
		int ilow = atoi ( sv[i].c_str() );
		int ihigh = atoi ( sv[i+1].c_str() );

		bins.push_back( PtBin( ilow, ihigh));
		i++;
	}

	return bins;
}

VarCache< stringvector > m_jetRespBins;

stringvector GetCustomBins()
{
	RETURN_CACHED( m_jetRespBins, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".CustomBins") )
}
// TODO: maybe cache this for better performance

VarCache< stringvector > m_cuts;

stringvector GetCuts()
{
	RETURN_CACHED( m_cuts,PropertyTreeSupport::GetAsStringList( GetPropTree(),GetSettingsRoot() + ".Cuts" ) )
}
/*
 void AddFilter( std::string sFilterId)
 {
 m_filter.push_back( sFilterId);
 }

 stringvector m_filter;*/

};

class FilterResult
{
public:
FilterResult(bool bHasPassed) :
m_bHasPassed(bHasPassed)
{
}

bool HasPassed()
{
	return m_bHasPassed;
}
;

bool m_bHasPassed;
};

template<class TData>
class FilterBase
{
public:
virtual void Init(PipelineSettings * pset)
{
	m_pipelineSettings = pset;
}
virtual void Finish()
{
}

virtual std::string GetFilterId() = 0;

virtual bool DoesEventPass(TData & event) = 0;

virtual std::string ToString(bool bVerbose = false)
{
	return "FilterBase";
}

PipelineSettings * m_pipelineSettings;
};


class RecoVertFilter: public FilterBase<EventResult>
{
public:
	RecoVertFilter( ) : FilterBase<EventResult>()
	{


	}

	virtual bool DoesEventPass(EventResult & event)
	{
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (  event.GetRecoVerticesCount()  >= m_pipelineSettings->GetFilterRecoVertLow()
				&& event.GetRecoVerticesCount() <= m_pipelineSettings->GetFilterRecoVertHigh() );
	}

	virtual std::string GetFilterId()
	{
		return "recovert";
	}
};


class SecondJetRatioFilter: public FilterBase<EventResult>
{
public:
	SecondJetRatioFilter( ) : FilterBase<EventResult>()
	{
	}


	virtual bool DoesEventPass(EventResult & event)
	{
	bool bPass = true;
	double fBinVal = event.GetCorrectedJetPt(1) / event.m_pData->Z->Pt();


	if (!(fBinVal >= m_pipelineSettings->GetFilterSecondJetRatioLow()))
		bPass = false;

	if (!(fBinVal < m_pipelineSettings->GetFilterSecondJetRatioHigh()))
		bPass = false;

	return bPass;
}

virtual std::string GetFilterId()
{
	return "secondjetratio";
}
};


class PtWindowFilter: public FilterBase<EventResult>
{
public:
	PtWindowFilter( ) : FilterBase<EventResult>(), m_binWith( ZPtBinning)
	{


	}


virtual bool DoesEventPass(EventResult & event)
{
	bool bPass = true;
	double fBinVal;
	if (m_binWith == ZPtBinning)
	fBinVal = event.m_pData->Z->Pt();
	else
	fBinVal = event.GetCorrectedJetPt(0);

	if (!(fBinVal >= m_pipelineSettings->GetFilterPtBinLow()))
	bPass = false;

	if (!(fBinVal < m_pipelineSettings->GetFilterPtBinHigh()))
	bPass = false;

	return bPass;
}

virtual std::string GetFilterId()
{
	return "ptbin";
}

virtual std::string ToString(bool bVerbose = false)
{
	std::stringstream s;

	if (bVerbose)
	{
		s << "Binned with ";
		if (m_binWith == ZPtBinning)
		s << " ZPt ";
		else
		s << " Jet1Pt ";

		s << " from " << m_pipelineSettings->GetFilterPtBinHigh() << " to "
		<< m_pipelineSettings->GetFilterPtBinLow();
	}
	else
	{
		s << "Pt" << std::setprecision(0)
		<< m_pipelineSettings->GetFilterPtBinLow() << "to"
		<< std::setprecision(0)
		<< m_pipelineSettings->GetFilterPtBinHigh();
	}
	return s.str();
}

enum BinWithEnum
{
	ZPtBinning, Jet1PtBinning
};
BinWithEnum m_binWith;
};

class InCutFilter: public FilterBase<EventResult>
{
public:

virtual bool DoesEventPass(EventResult & event)
{
	unsigned long ignoredCut = m_pipelineSettings->GetFilterInCutIgnored();
	// no section here is allowed to set to true again, just to false ! avoids coding errors
	return event.IsInCutWhenIgnoringCut(ignoredCut);
}

virtual std::string GetFilterId()
{
	return "incut";
}

virtual std::string ToString(bool bVerbose = false)
{
	return "InCut";
}

};

class EventPipeline;

template<class TData>
class EventConsumerBase
{
public:
virtual void Init(EventPipeline * pset)
{
	m_pipeline = pset;
}
virtual void Finish() = 0;

// this method is only called for events which have passed the filter imposed on the
// pipeline
virtual void ProcessFilteredEvent(TData & event)
{
}

// this method is called for all events
virtual void ProcessEvent(TData & event, FilterResult & result)
{
}

// this method is called for seconddary pipelines
virtual void Process()
{
}

virtual std::string GetId()
{
	return "default";
}


PipelineSettings * GetPipelineSettings()
{
	return this->m_pipeline->GetSettings();
}

EventPipeline * m_pipeline;
};

class EventPipeline
{
public:

typedef boost::ptr_vector<EventConsumerBase<EventResult> > ConsumerVector;
typedef boost::ptr_vector<FilterBase<EventResult> > FilterVector;


void InitPipeline(PipelineSettings * pset)
{
	m_pipelineSettings = pset;

	// load filter from the Settings and add them
	stringvector fvec = this->GetSettings()->GetFilter();
	BOOST_FOREACH( std::string sid, fvec )
	{ // make this more beatiful :)
		if ( sid == PtWindowFilter().GetFilterId())
			m_filter.push_back( new PtWindowFilter);
		else if ( sid == InCutFilter().GetFilterId())
			m_filter.push_back( new InCutFilter);
		else if ( sid == RecoVertFilter().GetFilterId())
			m_filter.push_back( new RecoVertFilter);
		else if ( sid == SecondJetRatioFilter().GetFilterId())
			m_filter.push_back( new SecondJetRatioFilter);
		else
			CALIB_LOG_FATAL( "Filter " << sid << " not found." )
	}

	for (FilterVector::iterator itfilter = m_filter.begin();
			!(itfilter== m_filter.end()); itfilter++)
	{
		itfilter->Init(pset);
	}

	for (ConsumerVector::iterator itcons = m_consumer.begin(); !(itcons
					== m_consumer.end()); itcons++)
	{
		itcons->Init(this);
	}

}

void FinishPipeline()
{
	for (ConsumerVector::iterator itcons = m_consumer.begin(); !(itcons
					== m_consumer.end()); itcons++)
	itcons->Finish();
	for (FilterVector::iterator itfilter = m_filter.begin(); !(itfilter
					== m_filter.end()); itfilter++)
	itfilter->Finish();
}

void Run()
{
	for (ConsumerVector::iterator itcons = m_consumer.begin(); !(itcons
					== m_consumer.end()); itcons++)
	{
		itcons->Process();
	}
}

void RunEvent(EventResult & evt)
{

	bool bPassed = true;
	for (FilterVector::iterator itfilter = m_filter.begin(); !(itfilter
					== m_filter.end()); itfilter++)
	{
		if (!itfilter->DoesEventPass(evt))
		{
			bPassed = false;
			break;
		}
	}

	FilterResult fres(bPassed);

	for (ConsumerVector::iterator itcons = m_consumer.begin(); !(itcons
					== m_consumer.end()); itcons++)
	{
		if (bPassed)
			itcons->ProcessFilteredEvent(evt);

		// ensure the event is valid, ( if coming from data )
		//if ( CutHandler::IsValidEvent( evt))
		itcons->ProcessEvent(evt, fres);
	}
}

FilterBase<EventResult> * FindFilter(std::string sFilterId)
{
	for (FilterVector::iterator it = m_filter.begin(); !(it
					== m_filter.end()); it++)
	{
		if (it->GetFilterId() == sFilterId)
		return &(*it);
	}

	return NULL;
}

PipelineSettings * GetSettings()
{
	return m_pipelineSettings;
}

const FilterVector& GetFilters()
{
	return m_filter;
}

ConsumerVector m_consumer;
FilterVector m_filter;
CompleteJetCorrector m_corr;

PipelineSettings * m_pipelineSettings;
};

}

#endif
