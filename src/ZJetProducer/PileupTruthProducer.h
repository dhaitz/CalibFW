#pragma once

// from ROOT
//#include <Math/VectorUtil.h>
#include "ZJetProducer/MetadataProducer.h"
//#include "RootTools/RootIncludes.h"
//#include "ZJetProducer/TriggerWeights.h"
#include <map>


namespace Artus
{
/// Event reweighting depending on pile-up and/or 2nd jet \f$pT\f$.
/** Modify the event weight depending on the number of true interactions per
    event to fit the estimated distribution in data.

    This can only be used on MC.
*/
class PileupTruthProducer: public ZJetGlobalMetaDataProducerBase
{
private:
	std::map<int, std::map<int, double> > m_pumean;

public:
	// load the numbers from a text file
	PileupTruthProducer(std::string file, double minbxsec)
	{
		LOG("Loading pile-up truth from " << file);
		ifstream f(file.c_str(), std::ios::in);
		if (!f.is_open())
			LOG_FATAL("Could not open file " << file);

		int run, ls, cnt(0), cntMax(-1);
		double lum, xsavg, xsrms;
		while (f >> run >> ls >> lum >> xsrms >> xsavg && ++cnt != cntMax)
		{
			if (false && cnt < 10)
				LOG(run << " " << ls << " "
					<< lum << " " << xsavg << " " << xsrms
					<< ": " << (xsavg * minbxsec * 1000) << " +/- " << (xsrms * minbxsec * 1000.0));

			if (unlikely(xsrms < 0))
				LOG_FATAL("RMS = " << xsrms << " < 0");

			m_pumean[run][ls] = xsavg * minbxsec * 1000.0;
		}
	}

	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing todo here
	}

	static std::string Name()
	{
		return "pileuptruth_producer";
	}


	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& m_pipelineSettings) const
	{
		const int run = data.m_eventmetadata->nRun;
		const int ls = data.m_eventmetadata->nLumi;
		double npu = m_pumean.at(run).at(ls);
		metaData.SetNpuTruth(npu);
		LOG("per event: " << run << ":" << ls << " npu = " << npu);
		return true;
	}
};


}
