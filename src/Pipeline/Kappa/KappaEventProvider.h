
#include <cassert>

#include "DataFormats/interface/Kappa.h"
#include "DataFormats/interface/KDebug.h"

#include "Pipeline/EventPipelineRunner.h"



namespace CalibFW
{

template <class TEventType>
class KappaEventProvider: public EventProvider<TEventType>
{
public:
	KappaEventProvider(FileInterface2& fi, InputTypeEnum inpType, bool phicorrection, bool tagged) :
		m_prevRun(-1), m_prevLumi(-1), m_inpType(inpType), m_fi(fi)
	{
		// setup pointer to collections
		m_event.m_eventmetadata = fi.Get<KEventMetadata>();

		if (inpType == McInput)
		{
			m_event.m_geneventmetadata = fi.Get<KGenEventMetadata>();
			m_event.m_pthatbin = -2;  // sample reweighting might be enabled
		}
		else
		{
			m_event.m_pthatbin = -1;  // sample reweighting is off
		}
		WireEvent(phicorrection, tagged);
		m_fi.SpeedupTree();

		// auto-delete objects when moving to a new object. Not defult root behaviour
		//fi.eventdata.SetAutoDelete(kTRUE);

		m_mon.reset(new ProgressMonitor(GetOverallEventCount()));
	}

	// overwrite using template specialization
	void WireEvent(bool phicorrection, bool tagged) {assert(false);}

	virtual bool GotoEvent(long long lEvent, HLTTools* hltInfo, int sampleinit)
	{
		m_mon->Update();
		m_fi.eventdata.GetEntry(lEvent);
		m_event.m_pthatbin = sampleinit;

		// this should be avoided, weights should be in skim!
		if (m_event.m_pthatbin != -1)
		{
			//m_event.m_pthatbin = -1; set to -1 somewhere once if sample rew. is off set to -2 if enabled
			std::string filename = m_fi.eventdata.GetFile()->GetName();
			if (boost::algorithm::contains(filename, "_Pt-0to15_"))
				m_event.m_pthatbin = 0;
			else if (boost::algorithm::contains(filename, "_Pt-15to20_"))
				m_event.m_pthatbin = 1;
			else if (boost::algorithm::contains(filename, "_Pt-20to30_"))
				m_event.m_pthatbin = 2;
			else if (boost::algorithm::contains(filename, "_Pt-30to50_"))
				m_event.m_pthatbin = 3;
			else if (boost::algorithm::contains(filename, "_Pt-50to80_"))
				m_event.m_pthatbin = 4;
			else if (boost::algorithm::contains(filename, "_Pt-80to120_"))
				m_event.m_pthatbin = 5;
			else if (boost::algorithm::contains(filename, "_Pt-120to170_"))
				m_event.m_pthatbin = 6;
			else if (boost::algorithm::contains(filename, "_Pt-170to230_"))
				m_event.m_pthatbin = 7;
			else if (boost::algorithm::contains(filename, "_Pt-230to300_"))
				m_event.m_pthatbin = 8;
			else if (boost::algorithm::contains(filename, "_Pt-300_"))
				m_event.m_pthatbin = 9;
			else
				CALIB_LOG_FATAL("No pthat bin found but sample weights expected: " << filename << " (bin: " << m_event.m_pthatbin << ")");
		}

		if (m_prevRun != m_event.m_eventmetadata->nRun)
		{
			m_prevRun = m_event.m_eventmetadata->nRun;
			m_prevLumi = -1;
		}

		if (m_prevLumi != m_event.m_eventmetadata->nLumi)
		{
			m_prevLumi = m_event.m_eventmetadata->nLumi;
			m_fi.GetMetaEntry();

			// load the correct lumi information
			if (m_inpType == McInput)
			{
				m_event.m_genlumimetadata = m_fi.GetMeta<KGenLumiMetadata>("KLumiMetadata");
				m_event.m_lumimetadata = m_fi.GetMeta<KGenLumiMetadata>("KLumiMetadata");
			}
			else if (m_inpType == DataInput)
			{
				m_event.m_lumimetadata = m_fi.GetMeta<KDataLumiMetadata>("KLumiMetadata");
			}
			else
			{
				CALIB_LOG_FATAL("Unknown input type");
			}

			//std::cout << "Loading new lumi info" << std::endl;
			// reload the HLT information associated with this lumi
			hltInfo->setLumiMetadata(m_event.m_lumimetadata);
		}

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
	void InitPFJets(ZJetEventData& event, std::string algoName)
	{
		event.m_pfJets[algoName] = m_fi.Get<KDataPFJets> (algoName);
	}

	void InitCaloJets(ZJetEventData& event, std::string algoName)
	{
		event.m_caloJets[algoName] = m_fi.Get<KDataJets>(algoName);
	}

	void InitGenJets(ZJetEventData& event, std::string algoName)
	{
		event.m_genJets[algoName] = m_fi.Get<KDataLVs> (algoName);
	}

	long m_prevRun, m_prevLumi;
	TEventType m_event;

	InputTypeEnum m_inpType;
	bool phicorrection;
	bool tagged;
	boost::scoped_ptr<ProgressMonitor> m_mon;

	FileInterface2& m_fi;
};

}
