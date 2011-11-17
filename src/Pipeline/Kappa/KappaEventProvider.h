
#include <cassert>

#include "DataFormats/interface/Kappa.h"
#include "DataFormats/interface/KDebug.h"

#include "Pipeline/EventPipelineRunner.h"



namespace CalibFW
{

template < class TEventType >
class KappaEventProvider: public EventProvider< TEventType >
{
public:
	KappaEventProvider(FileInterface & fi, InputTypeEnum inpType) :
		m_prevRun(-1), m_prevLumi(-1), m_inpType(inpType), m_fi(fi)
		{

		// setup pointer to collections
		m_event.m_eventmetadata = fi.Get<KEventMetadata> ();

		if (inpType == McInput)
		{
			m_event.m_geneventmetadata = fi.Get<KGenEventMetadata> ();
		}


		//m_event.m_fi = &fi;

        WireEvent();

		m_fi.SpeedupTree();

        // auto-delete objects when moving to a new object. Not defult root behaviour
        //fi.eventdata.SetAutoDelete(kTRUE);

		m_mon.reset(new ProgressMonitor(GetOverallEventCount()));
		}

    // overwrite using template specialization
    void WireEvent() { assert(false);}

	virtual bool GotoEvent(long long lEvent, HLTTools * hltInfo )
	{
		m_mon->Update( );
		m_fi.eventdata.GetEntry(lEvent);
        //std::cout << "goto event" << std::endl;
		if (m_prevRun != m_event.m_eventmetadata->nRun)
		{
			m_prevRun = m_event.m_eventmetadata->nRun;
			m_prevLumi = -1;
		}

		if (m_prevLumi != m_event.m_eventmetadata->nLumi)
		{
			m_prevLumi = m_event.m_eventmetadata->nLumi;

			// load the correct lumi information
			if (m_inpType == McInput)
			{
				m_event.m_lumimetadata = m_fi.Get<KGenLumiMetadata> (
						m_event.m_eventmetadata->nRun,
						m_event.m_eventmetadata->nLumi);
			}
			else if (m_inpType == DataInput)
			{
				m_event.m_lumimetadata = m_fi.Get<KDataLumiMetadata> (
						m_event.m_eventmetadata->nRun,
						m_event.m_eventmetadata->nLumi);
			}
			else
			{
                CALIB_LOG_FATAL("Unknown input type" )
			}

			//std::cout << "Loading new lumi info" << std::endl;
			// reload the HLT information associated with this lumi
			hltInfo->setLumiMetadata( m_event.m_lumimetadata );
		}
        //std::cout << "DONE: goto event" << std::endl;

		return true;
	}

	virtual TEventType const& GetCurrentEvent() const
			{
		return m_event;
			}

	virtual long long GetOverallEventCount() const
			{
		return m_fi.eventdata.GetEntries();
			}

protected:
	void InitPFJets(ZJetEventData & event, std::string algoName)
	{
		event.m_pfJets[algoName] = m_fi.Get<KDataPFJets> (algoName);
	}

	void InitGenJets(ZJetEventData & event, std::string algoName)
	{
		event.m_genJets[algoName] = m_fi.Get<KDataLVs> (algoName);
	}

	long m_prevRun, m_prevLumi;
	TEventType m_event;

	InputTypeEnum m_inpType;
	boost::scoped_ptr<ProgressMonitor> m_mon;

	FileInterface & m_fi;
};

}
