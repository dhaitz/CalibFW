#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "EventData.h"

#define BOOST_TEST_MODULE EventPipeline
#include <boost/test/included/unit_test.hpp>

namespace CalibFW
{

BOOST_AUTO_TEST_CASE( test_zjet_metadata )
{
	ZJetMetaData mdata;

	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 0);
	BOOST_CHECK( mdata.IsAllCutsPassed());

	mdata.SetCutResult(8, false);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 8);
	BOOST_CHECK( ! mdata.IsCutPassed(8));
	BOOST_CHECK( ! mdata.IsAllCutsPassed());

	mdata.SetCutResult(16, false);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 24);
	BOOST_CHECK( !mdata.IsCutPassed(16));
	BOOST_CHECK( !mdata.IsAllCutsPassed());
	BOOST_CHECK( mdata.IsCutPassed(32));

	mdata.SetCutResult(16, true);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 8);
	BOOST_CHECK( mdata.IsCutPassed(16));
	BOOST_CHECK(! mdata.IsAllCutsPassed());

	mdata.SetCutResult(8, true);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 0);
	BOOST_CHECK( mdata.IsCutPassed(8));
	BOOST_CHECK( mdata.IsAllCutsPassed());




}



}


