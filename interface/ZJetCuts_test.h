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


	ZJetPipelineSettings pSettings;
	pSettings.CacheCutLeadingJetEta.SetCache( 1.13 );
	pSettings.CacheJetAlgorithm.SetCache("AK5PFJets");

	evtData.m_jets[0].p4.SetEta( 0.3f);
	BOOST_CHECK_EQUAL(ecut.IsInCut( evtData, pSettings ), true);

	evtData.m_jets[0].p4.SetEta( 1.3f);
	BOOST_CHECK_EQUAL(ecut.IsInCut( evtData, pSettings ), false);

	evtData.m_jets[0].p4.SetEta( -0.3f);
	BOOST_CHECK_EQUAL(ecut.IsInCut( evtData, pSettings ), true);

	evtData.m_jets[0].p4.SetEta( -1.3f);
	BOOST_CHECK_EQUAL(ecut.IsInCut( evtData, pSettings ), false);

}



}


