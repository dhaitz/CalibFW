#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "../EventPipeline.h"

#define BOOST_TEST_MODULE EventPipeline
#include <boost/test/included/unit_test.hpp>

namespace Artus
{
class Testproduct
{
public:
	Testproduct() :  iGlobalProduct(0) {}

	struct LocalData
	{
		LocalData() : iproduct(0) {}

		int iproduct;
	};


	typedef LocalData  LocalproductType;


	void SetLocalproduct(LocalproductType* pipelineproduct)
	{
		m_localData = pipelineproduct;
	}

	void ClearContent() {}

	LocalproductType* GetLocalproduct() const
	{
		assert(m_localData != NULL);
		return m_localData;
	}
	int iGlobalProduct;


private:

	LocalproductType*   m_localData;
};


class TestData
{
public:
	TestData() : iVal(0) {}

	int iVal;

};

class TestSettings
{
public:

	std::string ToString() const
	{
		return "Test setting";
	}

	IMPL_PROPERTY(unsigned int, Level)


};

class TestFilter: public FilterBase<TestData, Testproduct, TestSettings>
{
public:

	virtual std::string GetFilterId()
	{
		return "testfilter";
	}

	virtual bool DoesEventPass(const TestData& event,  Testproduct const& product, TestSettings const& settings)
	{
		return (event.iVal < 2);
	}
};

class TestFilter2: public FilterBase<TestData, Testproduct, TestSettings>
{
public:

	virtual std::string GetFilterId()
	{
		return "testfilter2";
	}

	virtual bool DoesEventPass(const TestData& event,  Testproduct const& product, TestSettings const& settings)
	{
		return false;
	}
};


class TestLocalProducer : public LocalProducerBase<TestData, Testproduct, TestSettings>
{
public:

	// for each pipeline
	virtual void PopulateLocal(TestData const& data, Testproduct const& product,
							   typename Testproduct::LocalproductType& localproduct,
							   TestSettings const& m_pipelineSettings) const
	{
		localproduct.iproduct =  data.iVal + 1;
	}
};

class TestGlobalProductProducer : public GlobalProductProducerBase<TestData, Testproduct, TestSettings>
{
public:

	// for the global product producer
	virtual bool  PopulateGlobalProduct(TestData const& data, Testproduct& product,
										TestSettings const& m_pipelineSettings) const
	{
		product.iGlobalProduct = data.iVal + 5;
		return true;
	}

};


class TestEventConsumer: public EventConsumerBase< TestData, Testproduct, TestSettings>
{
public:
	TestEventConsumer():
		iFinish(0), iInit(0), iProcessFilteredEvent(0), iProcessEvent(0)
	{}


	virtual void Init(EventPipeline<TestData, Testproduct, TestSettings>* pset)
	{
		m_pipeline = pset;
		iInit++;
	}

	virtual void Finish()
	{
		iFinish ++ ;
	}

	virtual void ProcessFilteredEvent(TestData const& event,
									  Testproduct const& product)
	{
		iProcessFilteredEvent++;
	}

	// this method is called for all events
	virtual void ProcessEvent(TestData const& event,
							  Testproduct const& product,
							  FilterResult& result)
	{
		// did product work ?
		BOOST_CHECK_EQUAL(event.iVal + 1, product.GetLocalproduct()->iproduct);
		BOOST_CHECK_EQUAL(event.iVal + 5, product.iGlobalProduct);

		iProcessEvent++;
		fres = result;
	}

	void CheckCalls(int ProcessFilteredEvent, int ProcessEvent)
	{
		BOOST_CHECK_EQUAL(iInit, 1);
		BOOST_CHECK_EQUAL(iFinish, 1);

		BOOST_CHECK_EQUAL(iProcessFilteredEvent, ProcessFilteredEvent);
		BOOST_CHECK_EQUAL(iProcessFilteredEvent, ProcessFilteredEvent);
	}

	int iFinish;
	int iInit;
	int iProcessFilteredEvent;
	int iProcessEvent;
	FilterResult fres;
};

class TestPipelineInitilizer: public PipelineInitilizerBase< TestData, Testproduct, TestSettings>
{
public:
	virtual void InitPipeline(
		EventPipeline<TestData, Testproduct, TestSettings>* pLine,
		TestSettings const& pset) const {}

};

BOOST_AUTO_TEST_CASE(test_event_pipeline)
{
	TestEventConsumer* pCons1 = new TestEventConsumer();
	TestEventConsumer* pCons2 = new TestEventConsumer();

	EventPipeline<TestData, Testproduct, TestSettings> pline;

	pline.AddConsumer(pCons1);
	pline.AddConsumer(pCons2);

	pline.AddProducer(new TestLocalProducer());

	TestPipelineInitilizer init;

	TestSettings settings;
	pline.InitPipeline(settings, init);

	TestGlobalProductProducer globalProducer;
	Testproduct global;
	TestData td;
	td.iVal = 23;

	// run global meta data
	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);

	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);

	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);

	pline.FinishPipeline();

	pCons1->CheckCalls(3, 3);
	pCons2->CheckCalls(3, 3);
}

BOOST_AUTO_TEST_CASE(test_add_one_filter2times)
{
	EventPipeline<TestData, Testproduct, TestSettings> pline;

	pline.AddFilter(new TestFilter());
	BOOST_CHECK_THROW(pline.AddFilter(new TestFilter()),
					  std::exception);

}


BOOST_AUTO_TEST_CASE(test_event_filter)
{
	TestEventConsumer* pCons1 = new TestEventConsumer();
	TestEventConsumer* pCons2 = new TestEventConsumer();

	EventPipeline<TestData, Testproduct, TestSettings> pline;

	pline.AddConsumer(pCons1);
	pline.AddConsumer(pCons2);

	pline.AddFilter(new TestFilter());
	pline.AddProducer(new TestLocalProducer());

	TestPipelineInitilizer init;

	TestSettings settings;
	pline.InitPipeline(settings, init);

	TestData td;
	Testproduct global;
	TestGlobalProductProducer globalProducer;

	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);
	td.iVal++;
	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);
	td.iVal++;
	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);

	pline.FinishPipeline();

	pCons1->CheckCalls(2, 3);
	pCons2->CheckCalls(2, 3);
}


BOOST_AUTO_TEST_CASE(test_event_multiplefilter)
{
	TestEventConsumer* pCons1 = new TestEventConsumer();

	TestGlobalProductProducer globalProducer;
	EventPipeline<TestData, Testproduct, TestSettings> pline;

	pline.AddConsumer(pCons1);

	pline.AddFilter(new TestFilter2());
	pline.AddFilter(new TestFilter());

	pline.AddProducer(new TestLocalProducer());

	TestPipelineInitilizer init;
	TestSettings settings;
	pline.InitPipeline(settings, init);

	TestData td;
	Testproduct global;

	globalProducer.PopulateGlobalProduct(td, global, settings);
	pline.RunEvent(td, global);

	pline.FinishPipeline();

	BOOST_CHECK(pCons1->fres.GetFilterDecisions().at("testfilter") == true);
	BOOST_CHECK(pCons1->fres.GetFilterDecisions().at("testfilter2") == false);
	BOOST_CHECK(pCons1->fres.HasPassed() == false);
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


