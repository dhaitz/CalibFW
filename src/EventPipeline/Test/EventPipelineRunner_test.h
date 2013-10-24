#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "EventPipeline_test.h"
#include "../EventPipelineRunner.h"
#include "KappaTools/RootTools/HLTTools.h"

#include <boost/test/included/unit_test.hpp>

namespace Artus
{

class TestPipeline : public EventPipeline<TestData, TestMetaData,TestSettings>
{
public:

	TestPipeline() : iRunEvent( 0 ), iFinish(0), bCheckMetaProducer( false)
	{

	}

	virtual void RunEvent(TestData const& evt, TestMetaData const& meta)
	{
		iRunEvent++;
		if ( bCheckMetaProducer)
		{
			BOOST_CHECK( meta.iGlobalMetaData == 1 );
		}
	}

	virtual void FinishPipeline()
	{
		iFinish ++;
	}


	void CheckCalls( int RunEvent )
	{
		BOOST_CHECK_EQUAL( iFinish, 1 );
		BOOST_CHECK_EQUAL( iRunEvent, RunEvent );
	}

	int iRunEvent;
	int iFinish;
	bool bCheckMetaProducer;
};


class TestEventProvider : public EventProvider< TestData >
{
	virtual TestData const& GetCurrentEvent() const
			{
		return m_data;
			}
	virtual bool GotoEvent( long long lEventNumber,HLTTools * hltInfo )
	{
		return true;
	}
	virtual long long GetOverallEventCount() const
			{
		return 10;
			}

	TestData m_data;
};

BOOST_AUTO_TEST_CASE( test_event_prunner_global_metadata )
{/*
	TestPipeline * tline1 = new TestPipeline;
	TestPipeline * tline2 = new TestPipeline;
	TestPipeline * tline3 = new TestPipeline;
	TestPipeline * tline4 = new TestPipeline;
	TestPipeline * tline5 = new TestPipeline;


	tline1->bCheckMetaProducer = true;
	tline2->bCheckMetaProducer = true;
	tline3->bCheckMetaProducer = true;
	tline4->bCheckMetaProducer = true;
	tline5->bCheckMetaProducer = true;

	TestSettings tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, TestPipelineInitilizer() );
	tline2->InitPipeline( tset, TestPipelineInitilizer() );
	tline3->InitPipeline( tset, TestPipelineInitilizer() );
	tline4->InitPipeline( tset, TestPipelineInitilizer() );
	tline5->InitPipeline( tset, TestPipelineInitilizer() );

	EventPipelineRunner<TestPipeline, TestMetaDataProducerBase> prunner;

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );

	prunner.AddGlobalMetaProducer( new TestMetaDataProducer() );

	std::vector<TestPipeline *> vPipes;
	vPipes.push_back( tline3 );
	vPipes.push_back( tline4 );
	vPipes.push_back( tline5 );
	prunner.AddPipelines( vPipes );

	TestEventProvider evtProvider;
	prunner.RunPipelines<TestData, TestMetaData, TestSettings>( evtProvider, tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);*/

}


BOOST_AUTO_TEST_CASE( test_event_prunner )
{/*
	TestPipeline * tline1 = new TestPipeline;
	TestPipeline * tline2 = new TestPipeline;
	TestPipeline * tline3 = new TestPipeline;
	TestPipeline * tline4 = new TestPipeline;
	TestPipeline * tline5 = new TestPipeline;


	TestSettings tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, TestPipelineInitilizer() );
	tline2->InitPipeline( tset, TestPipelineInitilizer() );
	tline3->InitPipeline( tset, TestPipelineInitilizer() );
	tline4->InitPipeline( tset, TestPipelineInitilizer() );
	tline5->InitPipeline( tset, TestPipelineInitilizer() );

	EventPipelineRunner<TestPipeline, TestMetaDataProducerBase> prunner;

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );

	std::vector<TestPipeline *> vPipes;
	vPipes.push_back( tline3 );
	vPipes.push_back( tline4 );
	vPipes.push_back( tline5 );
	prunner.AddPipelines( vPipes );

	TestEventProvider evtProvider;
	prunner.RunPipelines<TestData, TestMetaData, TestSettings>( evtProvider, tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);
*/
}



}


