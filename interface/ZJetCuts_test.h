#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "EventPipeline_test.h"
#include "EventPipelineRunner.h"

#include <boost/test/included/unit_test.hpp>

#include "ZJetCuts.h"
#include "ZJetTestSupport.h"


namespace CalibFW
{


BOOST_AUTO_TEST_CASE( test_cut_jet_eta )
{
	LeadingJetEtaCut ecut;

	TestZJetEventData evtData;
	evtData.m_jets[0].p4.SetEta( 1.0f);

	ZJetMetaData metData;

	ZJetPipelineSettings pSettings;
	pSettings.CacheCutLeadingJetEta.SetCache( 1.13 );
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");

	evtData.m_jets[0].p4.SetEta( 0.3f);
	ecut.PopulateMetaData( evtData, metData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( LeadingJetEtaCut::CudId ) );

	evtData.m_jets[0].p4.SetEta( 1.3f);
    ecut.PopulateMetaData( evtData, metData, pSettings );
    BOOST_CHECK(! metData.IsCutPassed( LeadingJetEtaCut::CudId ) );

	evtData.m_jets[0].p4.SetEta( -0.3f);
    ecut.PopulateMetaData( evtData, metData, pSettings );
	BOOST_CHECK( metData.IsCutPassed( LeadingJetEtaCut::CudId ) );

	evtData.m_jets[0].p4.SetEta( -1.3f);
	ecut.PopulateMetaData( evtData, metData, pSettings );
	BOOST_CHECK(! metData.IsCutPassed( LeadingJetEtaCut::CudId ) );

}



}


