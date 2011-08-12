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
#include "ZJetMetaDataProducer.h"

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


BOOST_AUTO_TEST_CASE( test_producer_z )
{
	TestZJetEventData evtData;

	ZProducer z;
	ZJetMetaData mdata;
	ZJetPipelineSettings set;

	evtData.m_muons = new std::vector<KDataMuon>;

	z.PopulateMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidMuons() );

	evtData.m_muons = new std::vector<KDataMuon>;
	evtData.m_muons->push_back( KDataMuon() );
	z.PopulateMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidMuons() );


	KDataMuon m1;
	m1.p4.SetPt(45.0);
	m1.p4.SetPhi(0.0f);
	m1.p4.SetEta(0.0f);
	m1.charge = +1;

	KDataMuon m2;
	m2.p4.SetPt(45.0);
	m2.p4.SetPhi(0.1f);
	m2.p4.SetEta(0.1f);
	m2.charge = -1;

	evtData.m_muons = new std::vector<KDataMuon>;
	evtData.m_muons->push_back( m1 );
	evtData.m_muons->push_back( m2 );
	z.PopulateMetaData( evtData, mdata, set );
	BOOST_CHECK( mdata.HasValidMuons() );
	BOOST_CHECK( mdata.GetZ().p4.Pt() > 88.0f );
	BOOST_CHECK( mdata.GetZ().p4.Phi() < .1f );
	BOOST_CHECK( mdata.GetZ().p4.Eta() < .1f );

}

}


