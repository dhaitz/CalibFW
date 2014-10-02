#pragma once

#include <map>


namespace Artus
{
/// Event reweighting depending on pile-up and/or 2nd jet \f$pT\f$.
/** Modify the event weight depending on the number of true interactions per
    event to fit the estimated distribution in data.

    This can only be used on MC.
*/
class PileupTruthProducer: public ZJetGlobalProductProducerBase
{
private:
	std::map<int, std::map<int, double> > m_pumean;
	mutable int lastrun;
	mutable int lastls;

public:
	// load the numbers from a text file
	PileupTruthProducer(std::string file, double minbxsec) : lastrun(-1), lastls(-1)
	{
		LOG("Loading pile-up truth from " << file);
		ifstream f(file.c_str(), std::ios::in);
		if (!f.is_open())
			LOG_FATAL("Error in PileupTruthProducer: Could not open luminosity file: " << file);

		int run, ls, cnt(0), cntMax(-1);
		double lum, xsavg, xsrms;
		while (f >> run >> ls >> lum >> xsrms >> xsavg && ++cnt != cntMax)
		{
			if (false && cnt < 10)  // debug
				LOG(run << " " << ls << " "
					<< lum << " " << xsavg << " " << xsrms
					<< ": " << (xsavg * minbxsec * 1000) << " +/- " << (xsrms * minbxsec * 1000.0));

			if (unlikely(xsrms < 0))
				LOG_FATAL("Error in PileupTruthProducer: RMS = " << xsrms << " < 0");

			m_pumean[run][ls] = xsavg * minbxsec * 1000.0;
		}
	}

	virtual void PopulateProduct(ZJetEventData const& data,
								 ZJetProduct& product,
								 ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing todo here
	}

	static std::string Name()
	{
		return "pileuptruth_producer";
	}


	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& m_pipelineSettings) const
	{
		const int run = data.m_eventproduct->nRun;
		const int ls = data.m_eventproduct->nLumi;
		double npu = 0;

		try
		{
			npu = m_pumean.at(run).at(ls);
		}
		catch (const std::out_of_range& oor)
		{
			// warn once per lumi section if npu is unknown
			if (ls != lastls || run != lastrun)
				LOG("Warning in PileupTruthProducer: No luminosity for this run and ls: " << run << ":" << ls);
			lastrun = run;
			lastls = ls;
		}

		//LOG("per event: " << run << ":" << ls << " npu = " << npu);
		product.SetNpuTruth(npu);
		return true;
	}
};


}
