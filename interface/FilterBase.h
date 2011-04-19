#pragma once



#include "EventPipeline.h"

namespace CalibFW {

class FilterResult {
public:
	FilterResult(bool bHasPassed) :
		m_bHasPassed(bHasPassed) {
	}

	bool HasPassed() {
		return m_bHasPassed;
	}
	;

	bool m_bHasPassed;
};

template <class TData, class TSettings>
class EventPipeline;

template<class TData, class TSettings>
class FilterBase {
public:
	virtual ~FilterBase() {}
	virtual void Init(EventPipeline<TData, TSettings> * pset) {
		m_pipeline = pset;
	}
	virtual void Finish() {
	}

	virtual std::string GetFilterId() = 0;

	virtual bool DoesEventPass(TData & event) = 0;

	virtual std::string ToString(bool bVerbose = false) {
		return "FilterBase";
	}

	TSettings * GetPipelineSettings()
		{
			return m_pipeline->GetSettings();
		}

	EventPipeline<TData, TSettings> * m_pipeline;
};

}

