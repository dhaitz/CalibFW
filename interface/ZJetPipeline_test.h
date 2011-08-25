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

	mData.m_listValidJets.push_back( 0);
	mData.m_listValidJets.push_back( 1);
	ZJetPipelineSettings set;

	PtWindowFilter ptfilter( PtWindowFilter::Jet1PtBinning );

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

	// default is PtWindowFilter::ZPtBinning
	PtWindowFilter zfilter;

	set.CacheFilterPtBinHigh.SetCache(100.0f);
	set.CacheFilterPtBinLow.SetCache(10.0f);

	mData.SetValidZ(true);
	KDataLV v =mData.GetZ();
	v.p4.SetPt(123.0f);
	mData.SetZ( v );
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass( evt, mData, set ), false );

	v.p4.SetPt(23.0f);
	mData.SetZ( v );
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass( evt, mData, set ), true );

	v.p4.SetPt(3.2f);
	mData.SetZ( v );
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass( evt, mData, set ), false );

	mData.SetValidZ(false);
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass( evt, mData, set ), false );

}


BOOST_AUTO_TEST_CASE( test_filter_incut )
{
	TestZJetEventData evt;
	ZJetMetaData mData;
	ZJetPipelineSettings set;

	InCutFilter filter;

	set.CacheFilterInCutIgnored.SetCache(0);


	BOOST_CHECK_EQUAL(filter.DoesEventPass( evt, mData, set ), true );

	mData.SetCutResult(8, false);
	BOOST_CHECK_EQUAL(filter.DoesEventPass( evt, mData, set ), false );

	mData.SetCutResult(8, true);
	BOOST_CHECK_EQUAL(filter.DoesEventPass( evt, mData, set ), true );

}


BOOST_AUTO_TEST_CASE( test_filter_valid_z )
{
	TestZJetEventData evt;
	ZJetMetaData mData;
	ZJetPipelineSettings set;

	ValidZFilter filter;

	set.CacheFilterInCutIgnored.SetCache(0);


	mData.SetValidZ(true);
	BOOST_CHECK_EQUAL(filter.DoesEventPass( evt, mData, set ), true );

	mData.SetValidZ(false);
	BOOST_CHECK_EQUAL(filter.DoesEventPass( evt, mData, set ), false );

}

}


