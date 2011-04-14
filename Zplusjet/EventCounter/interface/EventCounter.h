// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


//
// class decleration
//

class EventCounter : public edm::EDProducer {
   public:
    explicit EventCounter(const edm::ParameterSet&);
    ~EventCounter();

   private:
    virtual void beginJob() ;
    virtual void produce(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

      // ----------member data ---------------------------
    double m_events_processed;
    std::string m_index_label;
};
