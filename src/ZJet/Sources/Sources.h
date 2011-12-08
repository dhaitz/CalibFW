/*
 * Sources.h
 *
 *  Created on: Sep 21, 2011
 *      Author: poseidon
 */

#ifndef SOURCES_H_
#define SOURCES_H_

#include "Pipeline/SourceBase.h"
#include "../ZJetEventData.h"
#include "../ZJetMetaData.h"
#include "../ZJetPipelineSettings.h"

namespace CalibFW
{
typedef SourceBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetSourceBase;

class SourceRecoVert: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = (double) event.m_vertexSummary->nVertices;
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 51;
	}
	virtual double GetDefaultLowBin() const
	{
		return -0.5f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 50.5;
	}
};

class SourceJetEta: public ZJetSourceBase
{
public:
	SourceJetEta() // All Jets
	{

	}
	/*
	 SourceJetEta( int JetNum ) : m_JetNum ( JetNum)
	 {

	 }
	 */

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Eta();
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -10.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceJetPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceRunNumber: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.m_eventmetadata->nRun;
		return true;
	}
/*
	virtual bool HasDefaultBinCount() const	{ return false;	}
	virtual double HasDefaultBins() const { return false; }*/

	// hardcoded for now ...
	virtual unsigned int GetDefaultBinCount() const
	{
		return 19848;
	}
	virtual double GetDefaultLowBin() const
	{
	  
		return 160404.0;
	}
	virtual double GetDefaultHighBin() const
	{
		return 180252.0;
	}
};



class SourceEventcount: public ZJetSourceBase
{
public:
	SourceEventcount(  )
	{
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = 1.0f;

		return true;
	}

	/*virtual bool HasDefaultBinCount() const	{ return false;	}
	virtual double HasDefaultBins() const { return false; }*/
	// hardcoded for now ...
	virtual unsigned int GetDefaultBinCount() const
	{
		return 10000;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0;
	}
	virtual double GetDefaultHighBin() const
	{
		return  1000000.0;
	}	
};

class SourceIntegratedLumi: public ZJetSourceBase
{
public:
	SourceIntegratedLumi(  )
	{
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		assert ( settings. Global()-> GetInputType() == DataInput ) ;
		val = event.GetDataLumiMetadata()->getLumi();

		return true;
	}
/*
	virtual bool HasDefaultBinCount() const	{ return false;	}
	virtual double HasDefaultBins() const { return false; }
	*/

	// hardcoded for now ...
	virtual unsigned int GetDefaultBinCount() const
	{
		return 2000;
	}
	virtual double GetDefaultLowBin() const
	{
	  
		return 0.0;
	}
	virtual double GetDefaultHighBin() const
	{
		//return 0.000000000000006; // lumi in year 2011
		return 6.0;
	}
};

class SourceHltPrescale: public ZJetSourceBase
{
public:
	SourceHltPrescale( std::string conf )
	{
		m_hltName = conf;
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		std::string curName = metaData.m_hltInfo->getHLTName( m_hltName);

		if ( metaData.m_hltInfo->isAvailable( curName ) )
			val = metaData.m_hltInfo->getPrescale( curName );
		else
			val = 0.0f;

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}

	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}

	virtual double GetDefaultHighBin() const
	{
		return 200.0;
	}



private:
	std::string m_hltName;
};

class SourceSelectedHltPrescale: public ZJetSourceBase
{
public:
  
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		assert ( metaData.GetSelectedHlt() != "") ;

		val = metaData.m_hltInfo->getPrescale( metaData.GetSelectedHlt() );

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}

	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}

	virtual double GetDefaultHighBin() const
	{
		return 200.0;
	}



private:
	std::string m_hltName;
};

class SourceZPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceCutValue: public ZJetSourceBase
{
public:
	SourceCutValue(long cutToScore) :
		m_cutToScore(cutToScore)
	{

	}

	// returns 	1 = cut not passed
	// or 		0 = cut passed
	// The Profile plot, for example, can calculate a mean then
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		bool bPassed;
		if (m_cutToScore > -1)
			bPassed = metaData.IsCutPassed(m_cutToScore);
		else
			bPassed = metaData.IsAllCutsPassed();

		if (bPassed)
			val = 0.0f;
		else
			val = 1.0f;

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}

private:
	// if -1 => check for all cuts
	long m_cutToScore;
};

class SourceJetPtRatio: public ZJetSourceBase
{
public:

	// jet1->Pt / jet2->Pt
	SourceJetPtRatio(std::string jetAlgo1, std::string jetAlgo2,
			unsigned int jet1Num, unsigned int jet2Num) :
		m_jetAlgo1(jetAlgo1), m_jetAlgo2(jetAlgo2), m_jet1Num(jet1Num),
				m_jet2Num(jet2Num)
	{

	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataLV * jet1 = metaData.GetValidJet(settings, event, m_jet1Num,
				m_jetAlgo1);
		KDataLV * jet2 = metaData.GetValidJet(settings, event, m_jet2Num,
				m_jetAlgo2);
		val = (jet1->p4.Pt() / jet2->p4.Pt());
		return true;
	}

	
	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 100.0;
	}

private:
	std::string m_jetAlgo1, m_jetAlgo2;
	unsigned int m_jet1Num, m_jet2Num;
};

}

#endif /* SOURCES_H_ */
