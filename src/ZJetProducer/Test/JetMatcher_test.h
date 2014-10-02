#pragma once

#include <vector>
#include <boost/test/included/unit_test.hpp>

#include "Pipeline/Test/EventPipeline_test.h"
#include "Pipeline/EventPipelineRunner.h"

#include "../ZJetCuts.h"

#include "../../Test/ZJetTestSupport.h"
#include "ZJet/Producer/JetMatcher.h"

namespace Artus
{

BOOST_AUTO_TEST_CASE(test_jetmatcher)
{
	/*
	ZJetProduct mdata;
	TestZJetEventData data;
	ZJetPipelineSettings set;

	const std::string algo1 = "PFJETS";
	const std::string algo2 = "GENJETS";
	const std::string match_prod = "GENMATCH";

	// PF JETS
	KDataPFJet j1_pf;
	j1_pf.p4.SetEta(2.0);
	j1_pf.p4.SetPhi(1.0);
	KDataPFJet j2_pf;
	j2_pf.p4.SetEta(0.0);
	j2_pf.p4.SetPhi(-1.0);
	// PU JET, cant be matched
	KDataPFJet j3_pf;
	j3_pf.p4.SetEta(0.0);
	j3_pf.p4.SetPhi(2.0);

	mdata.AddValidJet( j1_pf, algo1 );
	mdata.AddValidJet( j2_pf, algo1 );
	mdata.AddValidJet( j3_pf, algo1 );

	// Gen JETS
	KDataPFJet j1_gen;
	j1_gen.p4.SetEta(2.0);
	j1_gen.p4.SetPhi(1.0);
	KDataPFJet j2_gen;
	j2_gen.p4.SetEta(0.0);
	j2_gen.p4.SetPhi(-1.0);

	mdata.AddValidJet( j2_gen, algo2 );
	mdata.AddValidJet( j1_gen, algo2 );

	JetMatcher jm( algo1, algo2, match_prod );

	jm.PopulateGlobalProduct( data, mdata, set );

	MatchingResult & mres = mdata.GetMatchingResults( match_prod );
	*/
	/*	BOOST_CHECK_EQUAL( mres.GetEntryCount(),3 );
		BOOST_CHECK_EQUAL( mres.GetMatchingJet(0),1 );
		BOOST_CHECK_EQUAL( mres.GetMatchingJet(1),0 );
		BOOST_CHECK_EQUAL( mres.GetMatchingJet(1),MatchingPair::NoMatchFound );

		// jet 3 is not there an was never matched ...
		BOOST_CHECK_EQUAL( mres.GetMatchingJet(3),MatchingPair::NotMatched );*/
}


}


