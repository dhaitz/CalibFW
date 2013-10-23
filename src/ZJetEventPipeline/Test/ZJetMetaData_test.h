#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/test/included/unit_test.hpp>

#include "ZJetTestSupport.h"


namespace CalibFW
{


BOOST_AUTO_TEST_CASE( test_meta_data_meta_jet )
{
	ZJetPipelineSettings set;
	TestZJetEventData evt;
	ZJetMetaData mData;

	KDataPFTaggedJet pjet1;
	KDataPFTaggedJet pjet2;

	pjet1.area = 23.0f;
	pjet2.area = 42.0f;

	std::string algoName1 = "ak5pf";
	std::string algoNoMeta = "aknometa";

	mData.m_listValidJets[algoNoMeta].push_back(0);
	mData.m_listValidJets[algoNoMeta].push_back(1);

	set.CacheJetAlgorithm.SetCache(algoName1 );
	mData.InitMetaJetCollection(algoName1);

	BOOST_CHECK_EQUAL( mData.GetValidJetCount(set, evt, algoName1 ), 0  );
	BOOST_CHECK_EQUAL( mData.GetValidJetCount(set, evt, algoNoMeta ), 2 );

	mData.AddValidJet( pjet1, algoName1);

	BOOST_CHECK_EQUAL( mData.GetValidJetCount(set, evt, algoName1 ), 1  );
	BOOST_CHECK_EQUAL( mData.GetValidJetCount(set, evt, algoNoMeta ), 2 );

	mData.AddValidJet( pjet2, algoName1);
	BOOST_CHECK_EQUAL( mData.GetValidJetCount(set, evt, algoName1 ), 2  );
	BOOST_CHECK_EQUAL( mData.GetValidJetCount(set, evt, algoNoMeta ), 2 );

	KDataPFJet * pJet = static_cast<KDataPFJet * > ( mData.GetValidJet(set, evt, 0, algoName1 )  );
	BOOST_CHECK_EQUAL( pJet->area, 23.0f );

	pJet = static_cast<KDataPFJet * > ( mData.GetValidJet(set, evt, 1, algoName1 )  );
	BOOST_CHECK_EQUAL( pJet->area, 42.0f );
}

BOOST_AUTO_TEST_CASE( test_meta_data_sort_jets )
{
	ZJetPipelineSettings set;
	TestZJetEventData evt;
	ZJetMetaData mData;

	KDataPFTaggedJet pjet1;
	KDataPFTaggedJet pjet2;
	KDataPFTaggedJet pjet3;

	pjet1.p4.SetPt(23.0f );
	pjet2.p4.SetPt(2.0f );
	pjet3.p4.SetPt(33.0f );

	std::string algoName1 = "ak5pf";

	mData.AddValidJet( pjet1, algoName1 );
	mData.AddValidJet( pjet2, algoName1 );
	mData.AddValidJet( pjet3, algoName1 );

	BOOST_CHECK_EQUAL( mData.GetValidJetCount( set, evt, algoName1), 3 );

	BOOST_CHECK_EQUAL( mData.GetValidJet(  set,  evt, 0, algoName1)->p4.Pt(), 23.0f);
	BOOST_CHECK_EQUAL( mData.GetValidJet(  set,  evt, 1, algoName1)->p4.Pt(), 2.0f);
	BOOST_CHECK_EQUAL( mData.GetValidJet(  set,  evt, 2, algoName1)->p4.Pt(), 33.0f);

	mData.SortJetCollections();

	BOOST_CHECK_EQUAL( mData.GetValidJet(  set,  evt, 0, algoName1)->p4.Pt(), 33.0f);
	BOOST_CHECK_EQUAL( mData.GetValidJet(  set,  evt, 1, algoName1)->p4.Pt(), 23.0f);
	BOOST_CHECK_EQUAL( mData.GetValidJet(  set,  evt, 2, algoName1)->p4.Pt(), 2.0f);
}


}


