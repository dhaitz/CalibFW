#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "EventPipeline_test.h"
#include "EventPipelineRunner.h"

#include <boost/test/included/unit_test.hpp>

#include "ZJetCuts.h"

#include "ZJetTestSupport.h"
#include "EventData.h"

namespace CalibFW
{


BOOST_AUTO_TEST_CASE( test_filter_ptwin )
{

	TestZJetEventData evt;
	ZJetMetaData mData;
	ZJetPipelineSettings set;

	PtWindowFilter ptfilter;

	set.CacheFilterPtBinHigh.SetCache(100.0f);
	set.CacheFilterPtBinLow.SetCache(10.0f);

	evt.m_jets[0].p4.SetPt(123.0f);
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass( evt, mData, set ), false );

	evt.m_jets[0].p4.SetPt(23.0f);
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass( evt, mData, set ), true );

	evt.m_jets[0].p4.SetPt(3.2f);
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass( evt, mData, set ), false );

	evt.returnNullJet = true;
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass( evt, mData, set ), false );
}


BOOST_AUTO_TEST_CASE( test_filter_incut )
{
	TestZJetEventData evt;
	ZJetMetaData mData;
	ZJetPipelineSettings set;

	InCutFilter ptfilter;

	set.CacheFilterInCutIgnored.SetCache(0);

	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass( evt, mData, set ), true );
}


}


