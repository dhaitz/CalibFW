#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/test/included/unit_test.hpp>



#include "Pipeline/Test/EventPipeline_test.h"
#include "Pipeline//EventPipelineRunner.h"


#include "../ZJetCuts.h"
#include "../../Test/ZJetTestSupport.h"


namespace CalibFW
{


BOOST_AUTO_TEST_CASE( test_cut_jet_eta )
{
	LeadingJetEtaCut ecut;

	TestZJetEventData evtData;
	evtData.m_jets[0].p4.SetEta( 1.0f);

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutLeadingJetEta.SetCache( 1.13 );
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");

	ZJetMetaData metData;
    
	metData.m_listValidJets[pSettings.GetJetAlgorithm()].push_back( 0);
	metData.m_listValidJets[pSettings.GetJetAlgorithm()].push_back( 1);

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	evtData.m_jets[0].p4.SetEta( 0.3f);
	ecut.PopulateLocal( evtData, metData, localData, pSettings );
/*
	BOOST_CHECK( metData.IsCutPassed( LeadingJetEtaCut::CutId ) );

	evtData.m_jets[0].p4.SetEta( 1.3f);
    ecut.PopulateMetaData( evtData, metData, localData, pSettings );
    BOOST_CHECK(! metData.IsCutPassed( LeadingJetEtaCut::CutId ) );

	evtData.m_jets[0].p4.SetEta( -0.3f);
    ecut.PopulateMetaData( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( LeadingJetEtaCut::CutId ) );

	evtData.m_jets[0].p4.SetEta( -1.3f);
	ecut.PopulateMetaData( evtData, metData, localData, pSettings );
	BOOST_CHECK(! metData.IsCutPassed( LeadingJetEtaCut::CutId ) );*/
}

BOOST_AUTO_TEST_CASE( test_cut_muon_eta )
{
	MuonEtaCut ecut;

	TestZJetEventData evtData;
	ZJetMetaData metData;

	KDataMuon km1,km2;
	km1.p4.SetEta(1.23f);
	metData.m_listValidMuons.push_back(km1);
	km2.p4.SetEta(4.23f);
	metData.m_listValidMuons.push_back(km2);

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutMuonEta.SetCache( 1.3f );

	ecut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK(! metData.IsCutPassed( MuonEtaCut::CutId ) );


	metData.m_listValidMuons.clear();
	km1.p4.SetEta(1.23f);
	metData.m_listValidMuons.push_back(km1);
	km2.p4.SetEta(-1.23f);
	metData.m_listValidMuons.push_back(km2);

	localData.SetCutResult( MuonEtaCut::CutId, false );
	ecut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( MuonEtaCut::CutId ) );

}

BOOST_AUTO_TEST_CASE( test_cut_muon_pt )
{
	MuonPtCut ecut;

	TestZJetEventData evtData;
	ZJetMetaData metData;

	KDataMuon km1,km2;
	km1.p4.SetPt(10.0f);
	metData.m_listValidMuons.push_back(km1);
	km2.p4.SetPt(30.0f);
	metData.m_listValidMuons.push_back(km2);

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutMuonPt.SetCache( 15.0f );

	ecut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK(! metData.IsCutPassed( MuonPtCut::CutId ) );

	metData.m_listValidMuons[0].p4.SetPt( 50.0 );

	localData.SetCutResult( MuonPtCut::CutId, false );
	ecut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( MuonPtCut::CutId ) );

}



BOOST_AUTO_TEST_CASE( test_cut_back2back )
{
	BackToBackCut cut;

	TestZJetEventData evtData;

	// phi goes from -Pi to + Pi
	evtData.m_jets[0].p4.SetPhi( 3.0f);

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutBack2Back.SetCache(0.34);
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");


	ZJetMetaData metData;
	metData.m_listValidJets[pSettings.GetJetAlgorithm()].push_back( 0);
	metData.m_listValidJets[pSettings.GetJetAlgorithm()].push_back( 1);

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	KDataLV v = metData.GetZ();
	metData.SetValidZ(true);
	v.p4.SetPhi(0.0f);
	metData.SetZ( v );

	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( BackToBackCut::CutId ) );

	v.p4.SetPhi(1.5f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK(! metData.IsCutPassed( BackToBackCut::CutId ) );

	v.p4.SetPhi(-.3f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( BackToBackCut::CutId ) );

	evtData.m_jets[0].p4.SetPhi( 1.7f);
	v.p4.SetPhi(-1.7f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( BackToBackCut::CutId ) );

	evtData.m_jets[0].p4.SetPhi( 3.1f);
	v.p4.SetPhi(3.1f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK(! metData.IsCutPassed( BackToBackCut::CutId ) );
}


BOOST_AUTO_TEST_CASE( test_cut_zmass )
{
	ZMassWindowCut cut;

	TestZJetEventData evtData;
	ZJetMetaData metData;

	KDataLV v = metData.GetZ();
	metData.SetValidZ(true);
	v.p4.SetM(90.0f);
	metData.SetZ( v );

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutZMassWindow.SetCache(20.0f);
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( ZMassWindowCut::CutId ) );

	v.p4.SetM(69.0f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( !metData.IsCutPassed( ZMassWindowCut::CutId ) );

	v.p4.SetM(113.0f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( !metData.IsCutPassed( ZMassWindowCut::CutId ) );
}

BOOST_AUTO_TEST_CASE( test_cut_zpt )
{
	ZPtCut cut;

	TestZJetEventData evtData;
	ZJetMetaData metData;

	KDataLV v = metData.GetZ();
	metData.SetValidZ(true);
	v.p4.SetPt(20.0f);
	metData.SetZ( v );

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutZPt.SetCache(15.0f);
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( ZPtCut::CutId ) );

	v.p4.SetPt(10.0f);
	metData.SetZ( v );
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( !metData.IsCutPassed( ZPtCut::CutId ) );

}

BOOST_AUTO_TEST_CASE( test_cut_second_jet )
{
	SecondLeadingToZPtCut cut;

	TestZJetEventData evtData;

	evtData.m_jets[0].p4.SetPt( 100.f);
	evtData.m_jets[1].p4.SetPt( 10.f);

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutSecondLeadingToZPt.SetCache(0.2);
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");

	ZJetMetaData metData;
	metData.m_listValidJets[pSettings.GetJetAlgorithm()].push_back( 0);
	metData.m_listValidJets[pSettings.GetJetAlgorithm()].push_back( 1);

	KDataLV v = metData.GetZ();
	metData.SetValidZ(true);
	v.p4.SetPt(100.0f);
	metData.SetZ( v );

    typename ZJetMetaData::LocalMetaDataType localData;
    metData.SetLocalMetaData ( & localData );

	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( SecondLeadingToZPtCut::CutId ) );

	evtData.m_jets[1].p4.SetPt( 24.f);
	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( ! metData.IsCutPassed( SecondLeadingToZPtCut::CutId ) );

	// no second jet will also let the event pass
	while ( metData.m_listValidJets[pSettings.GetJetAlgorithm()].size() > 1)
	{
		metData.m_listValidJets[pSettings.GetJetAlgorithm()].pop_back();
	}

	cut.PopulateLocal( evtData, metData, localData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( SecondLeadingToZPtCut::CutId ) );

}



}


