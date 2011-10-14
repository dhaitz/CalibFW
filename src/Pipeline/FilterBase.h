#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <sstream>

namespace CalibFW
{

// forward define to be able to use the event pipeline here
template<class TData, class TMetaData, class TSettings>
class EventPipeline;

class FilterResult
{
public:

	typedef std::map<std::string, bool> FilterDecisions;
	typedef std::map<std::string, bool>::const_iterator FilterDecisionsIterator_const;

	FilterResult():
			m_cacheHasPassed(false), m_IsCachedHasPassed( false)
	{
	}

	// Note: only call this, when all FilterDecisions have been added, as this result is cached
	bool HasPassed() const
	{
		if ( m_IsCachedHasPassed)
			return m_cacheHasPassed;

		m_cacheHasPassed = true;

		for ( std::map<std::string, bool>::const_iterator it = GetFilterDecisions().begin();
			it != GetFilterDecisions().end();
			it ++ )
		{
			if ( it->second == false)
			{
				m_cacheHasPassed = false;
			}
		}

		m_IsCachedHasPassed = true;
		return m_cacheHasPassed;
	}

	bool HasPassedIfExcludingFilter(std::string const& excludedFilter ) const
	{
		for ( std::map<std::string, bool>::const_iterator it = GetFilterDecisions().begin();
			it != GetFilterDecisions().end();
			it ++ )
		{
			if ( it->second == false)
			{
				if( it->first != excludedFilter )
					return false;
			}
		}

		return true;
	}

	bool GetFilterDecision( std::string filterName) const
	{
		return GetFilterDecisions().at(filterName);
	}

	FilterDecisions const&  GetFilterDecisions() const
	{
		return m_filterDecision;
	}

	void  SetFilterDecisions( std::string filterName, bool passed)
	{
		m_filterDecision[filterName] = passed;
	}


	std::string ToString() const
	{
		std::stringstream s;

		s << "== Filter Decision == " << std::endl;
		//s << "Overall: " << this->m_bHasPassed << std::endl;
		for ( FilterDecisions::const_iterator it = m_filterDecision.begin();
				it != m_filterDecision.end();
				it ++)
		{
			s << it->first << " : " << it->second << std::endl;
		}

		return s.str();
	}

private:

	// optimize this, without strings
	FilterDecisions m_filterDecision;

	mutable bool m_cacheHasPassed;
	mutable bool m_IsCachedHasPassed;
};

template<class TData, class TMetaData, class TSettings>
class FilterBase: public boost::noncopyable
{
public:
	virtual ~FilterBase()
	{
	}

	// todo: do we event need this init this ?
	virtual void Init(EventPipeline<TData, TMetaData, TSettings> * pset)
	{
		//m_pipeline = pset;
	}

	virtual void Finish()
	{
	}

	virtual std::string GetFilterId() = 0;

	virtual bool DoesEventPass( TData const& event, TMetaData const& metaData, TSettings const& settings) = 0;

	virtual std::string ToString(bool bVerbose = false)
	{
		return GetFilterId();
	}
/*
	TSettings * GetPipelineSettings()
	{
		return m_pipeline->GetSettings();
	}
*/
	//EventPipeline<TData, TMetaData, TSettings> * m_pipeline;
};

}

