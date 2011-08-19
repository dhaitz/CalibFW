#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "EventPipeline.h"

#define BOOST_TEST_MODULE EventPipeline
#include <boost/test/included/unit_test.hpp>

namespace CalibFW
{
class TestMetaData
{
public:
	TestMetaData() : iMetaData(0) {}

	int iMetaData;
};


class TestData
{
public:
	TestData() : iVal(0) {}

	int iVal;

};

class TestSettings
{
	IMPL_PROPERTY(unsigned int, Level)
};

class TestFilter: public FilterBase<TestData, TestMetaData,TestSettings>
{
public:

	virtual std::string GetFilterId()
	{
		return "testfilter";
	}

	virtual bool DoesEventPass( const TestData & event,  TestMetaData const& metaData, TestSettings const& settings)
	{
		return ( event.iVal < 2 );
	}
};

class TestFilter2: public FilterBase<TestData, TestMetaData,TestSettings>
{
public:

	virtual std::string GetFilterId()
	{
		return "testfilter2";
	}

	virtual bool DoesEventPass( const TestData & event,  TestMetaData const& metaData, TestSettings const& settings)
	{
		return false;
	}
};

class TestMetaDataProducer : public MetaDataProducerBase<TestData, TestMetaData,TestSettings>
{
public:

	virtual void PopulateMetaData(TestData const& data, TestMetaData & metaData,
			TestSettings const& m_pipelineSettings)
	{
		metaData.iMetaData = data.iVal + 1;
	}


};


class TestEventConsumer: public EventConsumerBase< TestData, TestMetaData,TestSettings>
{
public:
	TestEventConsumer():
		iFinish( 0 ), iInit( 0 ), iProcessFilteredEvent( 0), iProcessEvent(0)
		{}


	virtual void Init(EventPipeline<TestData, TestMetaData, TestSettings> * pset)
	{
		m_pipeline = pset;
		iInit++;
	}

	virtual void Finish()
	{
		iFinish ++ ;
	}

	virtual void ProcessFilteredEvent(TestData const& event,
			TestMetaData const& metaData)
	{
		iProcessFilteredEvent++;
	}

	// this method is called for all events
	virtual void ProcessEvent(TestData const& event,
			TestMetaData const& metaData,
			FilterResult & result)
	{
		// did metaData work ?
		BOOST_CHECK_EQUAL(event.iVal + 1, metaData.iMetaData);

		iProcessEvent++;
		fres = result;
	}

	void CheckCalls( int ProcessFilteredEvent, int ProcessEvent)
	{
		BOOST_CHECK_EQUAL( iInit, 1 );
		BOOST_CHECK_EQUAL( iFinish, 1 );

		BOOST_CHECK_EQUAL( iProcessFilteredEvent, ProcessFilteredEvent );
		BOOST_CHECK_EQUAL( iProcessFilteredEvent, ProcessFilteredEvent );
	}

	int iFinish;
	int iInit;
	int iProcessFilteredEvent;
	int iProcessEvent;
	FilterResult fres;
};

class TestPipelineInitilizer: public PipelineInitilizerBase< TestData, TestMetaData,TestSettings>
{
public:
	virtual void InitPipeline(
			EventPipeline<TestData, TestMetaData,TestSettings> * pLine,
			TestSettings const& pset) const {}

};

BOOST_AUTO_TEST_CASE( test_event_pipeline )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();
	TestEventConsumer * pCons2 = new TestEventConsumer();

	EventPipeline<TestData, TestMetaData, TestSettings> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddMetaDataProducer( new TestMetaDataProducer() );

	TestPipelineInitilizer init;

	pline.InitPipeline( TestSettings(), init  );

	TestData td;

	pline.RunEvent( td );
	pline.RunEvent( td );
	pline.RunEvent( td );

	pline.FinishPipeline();

	pCons1->CheckCalls(3, 3);
	pCons2->CheckCalls(3, 3);
}


BOOST_AUTO_TEST_CASE( test_add_one_filter2times )
{
	EventPipeline<TestData, TestMetaData, TestSettings> pline;

	pline.AddFilter( new TestFilter() );
	BOOST_CHECK_THROW( pline.AddFilter( new TestFilter() ),
			std::exception );

}


BOOST_AUTO_TEST_CASE( test_event_filter )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();
	TestEventConsumer * pCons2 = new TestEventConsumer();

	EventPipeline<TestData, TestMetaData, TestSettings> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddFilter( new TestFilter() );
	pline.AddMetaDataProducer( new TestMetaDataProducer() );

	TestPipelineInitilizer init;

	pline.InitPipeline( TestSettings(), init  );

	TestData td;

	pline.RunEvent( td );
	td.iVal++;
	pline.RunEvent( td );
	td.iVal++;
	pline.RunEvent( td );

	pline.FinishPipeline();

	pCons1->CheckCalls(2, 3);
	pCons2->CheckCalls(2, 3);
}


BOOST_AUTO_TEST_CASE( test_event_multiplefilter )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();
	TestEventConsumer * pCons2 = new TestEventConsumer();

	EventPipeline<TestData, TestMetaData, TestSettings> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddFilter( new TestFilter() );
	pline.AddFilter( new TestFilter2() );
	pline.AddMetaDataProducer( new TestMetaDataProducer() );

	TestPipelineInitilizer init;
	pline.InitPipeline( TestSettings(), init  );

	TestData td;

	pline.RunEvent( td );
	pline.RunEvent( td );
	pline.RunEvent( td );

	pline.FinishPipeline();

	BOOST_CHECK( pCons1->fres.GetFilterDecisions().at("testfilter") == true );
	BOOST_CHECK( pCons1->fres.GetFilterDecisions().at("testfilter2") == false );
/*
	for ( FilterResult::FilterDecisions::const_iterator it = pCons1->fres.GetFilterDecisions().begin();
			it != pCons1->fres.GetFilterDecisions().end();
			it ++ )
	{
		if ( it->first == "testfilter" )
			BOOST_CHECK( it->second == true )

			if ( it->first == "testfilter" )
	BOOST_CHECK( it->second == true )
		std::cout << it->first << " : " << it->second << std::endl;
	}*/
}


}


