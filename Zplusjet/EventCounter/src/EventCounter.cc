// -*- C++ -*-
//
// Package:    EventCounter
// Class:      EventCounter
// 
/**\class EventCounter EventCounter.cc Zplusjet/EventCounter/src/EventCounter.cc

 Description: Count the events processed

 Implementation:
    A really trivial module that produces a double where the number of events 
    processed is stored.
*/
//
// Original Author:  Danilo Piparo
//         Created:  Wed Jan 14 09:37:30 CET 2009
// $Id: EventCounter.cc,v 1.5 2010/11/16 00:23:41 hauth Exp $
//
//
#include "Zplusjet/EventCounter/interface/EventCounter.h"

//
// constructors and destructor
//
EventCounter::EventCounter(const edm::ParameterSet& iConfig)
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
*/
   //now do what ever other initialization is needed

   m_index_label =
   iConfig.getUntrackedParameter<std::string> ("indexLabel","processedEvents");

    produces < double > (m_index_label);
    //produces <string> ("BookkepingString").setBranchAlias("BookkepingString");
    m_events_processed=0;

}


EventCounter::~EventCounter()
{

}

// ------------ method called to produce the data  ------------
void
EventCounter::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);

   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
   std::auto_ptr<ExampleData2> pOut(new ExampleData2(*pIn));
   iEvent.put(pOut);
*/

/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/
   m_events_processed++;
   std::auto_ptr<double> event_index(new double);
   *event_index=m_events_processed;
   iEvent.put(event_index,m_index_label);
    //event.put( value_autoptr, prod_name);
}

// ------------ method called once each job just before starting event loop  ------------
void 
EventCounter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EventCounter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(EventCounter);
