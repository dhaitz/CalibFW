
#include <cassert>

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Kappa/DataFormats/interface/KDebug.h"

#include "EventPipeline/EventPipelineRunner.h"



namespace Artus
{

template <class TEventType>
class KappaEventProvider: public EventProvider<TEventType>
{
public:
	KappaEventProvider(FileInterface2& fi, InputTypeEnum inpType, boost::property_tree::ptree propTree) :
		m_prevRun(-1), m_prevLumi(-1), m_inpType(inpType), m_fi(fi), samplenames(PropertyTreeSupport::GetAsStringList(&propTree, "SampleNames"))
	{
		// setup pointer to collections
		m_event.m_eventproduct = fi.Get<KEventMetadata>();

		if (inpType == McInput)
		{
			m_event.m_geneventproduct = fi.Get<KGenEventMetadata>();
			m_event.m_pthatbin = -2;  // sample reweighting might be enabled
		}
		else
		{
			m_event.m_pthatbin = -1;  // sample reweighting is off
		}
		WireEvent(propTree);
		m_fi.SpeedupTree();

		// auto-delete objects when moving to a new object. Not defult root behaviour
		//fi.eventdata.SetAutoDelete(kTRUE);

		m_mon.reset(new ProgressMonitor(GetOverallEventCount()));
	}

	// overwrite using template specialization
	void WireEvent(boost::property_tree::ptree propTree)
	{
		assert(false);
	}

	virtual bool GotoEvent(long long lEvent, HLTTools* hltInfo, int sampleinit)
	{
		if (!m_mon->Update())
			return false;
		m_fi.eventdata.GetEntry(lEvent);
		m_event.m_pthatbin = sampleinit;

		// this should be avoided, weights should be in skim!
		//m_event.m_pthatbin = -1; set to -1 somewhere once if sample rew. is off set to -2 if enabled
		if (m_event.m_pthatbin != -1)
		{
		    if (samplenames.size() < 1)
    		    LOG_FATAL("Sample reweighting enabled but no names given?")
		
			std::string filename = m_fi.eventdata.GetFile()->GetName();
            bool weight_found = false;
			for (unsigned int i = 0; i < samplenames.size(); i++)
			{
				if (boost::algorithm::contains(filename, samplenames.at(i)) && !weight_found)
    			{
    				m_event.m_pthatbin = i;
    				weight_found = true;
    			}
			}
			if (!weight_found)
			LOG_FATAL("No pthat bin found but sample weights expected: " << filename << " (bin: " << m_event.m_pthatbin << ")");
		}

		if (m_prevRun != m_event.m_eventproduct->nRun)
		{
			m_prevRun = m_event.m_eventproduct->nRun;
			m_prevLumi = -1;
		}

		if (m_prevLumi != m_event.m_eventproduct->nLumi)
		{
			m_prevLumi = m_event.m_eventproduct->nLumi;
			m_fi.GetMetaEntry();

			// load the correct lumi information
			if (m_inpType == McInput)
			{
				m_event.m_genlumiproduct = m_fi.GetMeta<KGenLumiMetadata>("KLumiMetadata");
				m_event.m_lumiproduct = m_fi.GetMeta<KGenLumiMetadata>("KLumiMetadata");
			}
			else if (m_inpType == DataInput)
			{
				m_event.m_lumiproduct = m_fi.GetMeta<KDataLumiMetadata>("KLumiMetadata");
			}
			else
			{
				LOG_FATAL("Unknown input type");
			}

			//std::cout << "Loading new lumi info" << std::endl;
			// reload the HLT information associated with this lumi
			hltInfo->setLumiMetadata(m_event.m_lumiproduct);
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
	boost::scoped_ptr<ProgressMonitor> m_mon;

	FileInterface2& m_fi;

	std::vector<std::string> samplenames;
};

}
