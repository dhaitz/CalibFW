#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/test/included/unit_test.hpp>


#include "DataFormats/interface/Kappa.h"

#include "Pipeline/Test/EventPipeline_test.h"
#include "Pipeline/EventPipelineRunner.h"

#include "../ZJetCuts.h"

#include "../../Test/ZJetTestSupport.h"
#include "../ZJetMetaDataProducer.h"

namespace Artus
{



BOOST_AUTO_TEST_CASE( test_zjet_metadata )
{
	ZJetMetaData mdata;
    typename ZJetMetaData::LocalMetaDataType localData;
    mdata.SetLocalMetaData( & localData );

	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 0);
	BOOST_CHECK( mdata.IsAllCutsPassed());

	localData.SetCutResult(8, false);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 8);
	BOOST_CHECK( ! mdata.IsCutPassed(8));
	BOOST_CHECK( ! mdata.IsAllCutsPassed());

	localData.SetCutResult(16, false);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 24);
	BOOST_CHECK( !mdata.IsCutPassed(16));
	BOOST_CHECK( !mdata.IsAllCutsPassed());
	BOOST_CHECK( mdata.IsCutPassed(32));

	localData.SetCutResult(16, true);
	BOOST_CHECK_EQUAL( mdata.GetCutBitmask(), 8);
	BOOST_CHECK( mdata.IsCutPassed(16));
	BOOST_CHECK(! mdata.IsAllCutsPassed());

	localData.SetCutResult(8, true);
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

	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	mdata.m_listValidMuons.push_back( KDataMuon() );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	KDataMuon m1;
	m1.p4.SetPt(45.0);
	m1.p4.SetPhi(0.0f);
	m1.p4.SetEta(0.0f);
	m1.charge = +1;

	KDataMuon m2;
	m2.p4.SetPt(50.0);
	m2.p4.SetPhi(3.1f);
	m2.p4.SetEta(0.1f);
	m2.charge = -1;

	mdata.m_listValidMuons.clear();
	mdata.m_listValidMuons.push_back( m1 );
	mdata.m_listValidMuons.push_back( m2 );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( mdata.HasValidZ() );

    BOOST_CHECK( ( mdata.GetZ().p4.Pt() > 5.0f ) && ( mdata.GetZ().p4.Pt() < 7.0f ));
/*
	BOOST_CHECK( mdata.GetZ().p4.Phi() < .1f );
	BOOST_CHECK( mdata.GetZ().p4.Eta() < .1f );
*/
}


BOOST_AUTO_TEST_CASE( test_producer_z_invalid)
{
	TestZJetEventData evtData;

	ZProducer z;
	ZJetMetaData mdata;
	ZJetPipelineSettings set;

	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	mdata.m_listValidMuons.push_back( KDataMuon() );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	KDataMuon m1;
	m1.p4.SetPt(45.0);
	m1.p4.SetPhi(0.0f);
	m1.p4.SetEta(0.0f);
	m1.charge = +1;


	KDataMuon m2;
	m2.p4.SetPt(50.0);
	m2.p4.SetPhi(0.0f);
	m2.p4.SetEta(0.1f);
	m2.charge = +1;

	mdata.m_listValidMuons.clear();
	mdata.m_listValidMuons.push_back( m1 );
	mdata.m_listValidMuons.push_back( m2 );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( ! mdata.HasValidZ() );
}



BOOST_AUTO_TEST_CASE( test_producer_z_invalid2)
{
	TestZJetEventData evtData;

	ZProducer z;
	ZJetMetaData mdata;
	ZJetPipelineSettings set;

	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	mdata.m_listValidMuons.push_back( KDataMuon() );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	KDataMuon m1;
	m1.p4.SetPt(45.0);
	m1.p4.SetPhi(0.0f);
	m1.p4.SetEta(0.0f);
	m1.charge = +1;


	KDataMuon m2;
	m2.p4.SetPt(435.0);
	m2.p4.SetPhi(0.0f);
	m2.p4.SetEta(0.1f);
	m2.charge = -1;
;

	mdata.m_listValidMuons.clear();
	mdata.m_listValidMuons.push_back( m1 );
	mdata.m_listValidMuons.push_back( m2 );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( ! mdata.HasValidZ() );
}


BOOST_AUTO_TEST_CASE( test_producer_z_three_muon )
{
	TestZJetEventData evtData;

	ZProducer z;
	ZJetMetaData mdata;
	ZJetPipelineSettings set;

	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	mdata.m_listValidMuons.push_back( KDataMuon() );
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( !mdata.HasValidZ() );

	KDataMuon m1;
	m1.p4.SetPt(45.0);
	m1.p4.SetPhi(0.0f);
	m1.p4.SetEta(0.0f);
	m1.charge = +1;

    // this wont make a good Z ...
	KDataMuon m1_wrong;
	m1_wrong.p4.SetPt(300.0);
	m1_wrong.p4.SetPhi(0.0f);
    m1_wrong.p4.SetEta(0.1f);
    m1_wrong.charge = +1;


	KDataMuon m2;
	m2.p4.SetPt(50.0);
	m2.p4.SetPhi(3.1f);
	m2.p4.SetEta(0.1f);
	m2.charge = -1;

	mdata.m_listValidMuons.clear();
	mdata.m_listValidMuons.push_back( m1 );
	mdata.m_listValidMuons.push_back( m2 );
	mdata.m_listValidMuons.push_back(m1_wrong);
	z.PopulateGlobalMetaData( evtData, mdata, set );
	BOOST_CHECK( mdata.HasValidZ() );
    BOOST_CHECK( ( mdata.GetZ().p4.Pt() > 5.0f ) && ( mdata.GetZ().p4.Pt() < 7.0f ));
/*	BOOST_CHECK( mdata.GetZ().p4.Phi() < .1f );
	BOOST_CHECK( mdata.GetZ().p4.Eta() < .1f );
*/
}

}


