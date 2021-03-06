#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Pipeline/Test/EventPipeline_test.h"
#include "Pipeline/EventPipelineRunner.h"

#include <boost/test/included/unit_test.hpp>

#include "../Producer/ZJetCuts.h"

#include "ZJetTestSupport.h"

#include "../Filter/PtWindowFilter.h"
#include "../Filter/ValidZFilter.h"
#include "../Filter/InCutFilter.h"


namespace Artus
{


BOOST_AUTO_TEST_CASE(test_filter_ptwin)
{

	TestZJetEventData evt;
	ZJetProduct mData;

	ZJetPipelineSettings set;
	set.CacheCutZPt.SetCache(15.0f);
	set.CacheJetAlgorithm.SetCache("AK5PFJets");

	mData.m_listValidJets[set.GetJetAlgorithm()].push_back(0);
	mData.m_listValidJets[set.GetJetAlgorithm()].push_back(1);

	PtWindowFilter ptfilter(PtWindowFilter::Jet1PtBinning);

	set.CacheFilterPtBinHigh.SetCache(100.0f);
	set.CacheFilterPtBinLow.SetCache(10.0f);

	evt.m_jets[0].p4.SetPt(123.0f);
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass(evt, mData, set), false);

	evt.m_jets[0].p4.SetPt(23.0f);
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass(evt, mData, set), true);

	evt.m_jets[0].p4.SetPt(3.2f);
	BOOST_CHECK_EQUAL(ptfilter.DoesEventPass(evt, mData, set), false);


	// default is PtWindowFilter::ZPtBinning
	PtWindowFilter zfilter;

	set.CacheFilterPtBinHigh.SetCache(100.0f);
	set.CacheFilterPtBinLow.SetCache(10.0f);

	mData.SetValidZ(true);
	BOOST_CHECK(mData.HasValidZ());
	KDataLV v = mData.GetZ();
	v.p4.SetPt(123.0f);
	mData.SetZ(v);
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass(evt, mData, set), false);

	v.p4.SetPt(23.0f);
	mData.SetZ(v);
	zfilter.DoesEventPass(evt, mData, set);
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass(evt, mData, set), true);

	v.p4.SetPt(3.2f);
	mData.SetZ(v);
	BOOST_CHECK_EQUAL(zfilter.DoesEventPass(evt, mData, set), false);
}


BOOST_AUTO_TEST_CASE(test_filter_incut)
{
	TestZJetEventData evt;
	ZJetProduct mData;
	typename ZJetProduct::LocalproductType localData;
	ZJetPipelineSettings set;

	InCutFilter filter;

	set.CacheFilterInCutIgnored.SetCache(0);
	mData.SetLocalproduct(&localData);

	BOOST_CHECK_EQUAL(filter.DoesEventPass(evt, mData, set), true);

	localData.SetCutResult(8, false);
	BOOST_CHECK_EQUAL(filter.DoesEventPass(evt, mData, set), false);

	localData.SetCutResult(8, true);
	BOOST_CHECK_EQUAL(filter.DoesEventPass(evt, mData, set), true);

}


BOOST_AUTO_TEST_CASE(test_filter_valid_z)
{
	TestZJetEventData evt;
	ZJetProduct mData;
	ZJetPipelineSettings set;

	ValidZFilter filter;

	set.CacheFilterInCutIgnored.SetCache(0);

	mData.SetValidZ(true);
	BOOST_CHECK_EQUAL(filter.DoesEventPass(evt, mData, set), true);

	mData.SetValidZ(false);
	BOOST_CHECK_EQUAL(filter.DoesEventPass(evt, mData, set), false);

}

}


