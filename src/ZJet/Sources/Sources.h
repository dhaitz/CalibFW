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

class SourceJet2Eta: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2)
		{
			return false;
		}
		val = metaData.GetValidJet(settings, event, 1)->p4.Eta();
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

class SourceJet2Pt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2)
		{
			return false;
		}
		val = metaData.GetValidJet(settings, event, 1)->p4.Pt();
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

class SourceJetPhi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -5.f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 5.f;
	}
};

class SourceJet2Phi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		
		if (metaData.GetValidJetCount(settings, event) < 2)
		{
			return false;
		}
		val = metaData.GetValidJet(settings, event, 1)->p4.Phi();
		
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -5.f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 5.f;
	}
};


class SourcePtBalance: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Pt()/metaData.GetRefZ().p4.Pt();
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
		return 2.0;
	}
};

class SourceMPFresponse: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMPF(event.GetMet(settings));
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
		return 2.0;
	}
};
class SourceValidJets: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidJetCount(settings, event, settings.GetJetAlgorithm());
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 101;
	}
	virtual double GetDefaultLowBin() const
	{
		return -0.5f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 100.5;
	}
};

class SourceSumEt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.GetMet(settings)->sumEt;
		return true;
	}
/*
	virtual bool HasDefaultBinCount() const	{ return true;	}
	virtual double HasDefaultBins() const { return true; }*/

	virtual unsigned int GetDefaultBinCount() const
	{
		return 250;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2500.0;
	}
};

class SourceMETPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.GetMet(settings)->p4.Pt();
		return true;
	}
/*
	virtual bool HasDefaultBinCount() const	{ return true;	}
	virtual double HasDefaultBins() const { return true; }*/

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

class SourceMETEta: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.GetMet(settings)->p4.Eta();
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


class SourceMETPhi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.GetMet(settings)->p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -5.f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 5.f;
	}
};

class SourceMETFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.GetMet(settings)->p4.Pt() / event.GetMet(settings)->sumEt;
		return true;
	}
/*
	virtual bool HasDefaultBinCount() const	{ return true;	}
	virtual double HasDefaultBins() const { return true; }*/

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
		return 0.5;
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
		return 10000;
	}
	virtual double GetDefaultLowBin() const
	{
	  
		return 190000.0;
	}
	virtual double GetDefaultHighBin() const
	{
		return 200000.0;
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
		//return 0.06; // lumi of year 2011 in atto barn
		return 0.01;
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
class SourceZEta: public ZJetSourceBase
{
public:

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.Eta();
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
class SourceZPhi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -5.f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 5.f;
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
		
// debug stuff, remove !
		/*
		if ( val > 3.5)
		{
		    CALIB_LOG_FILE ( "jet1pt " << jet1->p4.Pt()  << " jet2pt " << jet2->p4.Pt() )
		    CALIB_LOG_FILE ( "freak event ? update: " << val << " algo1: " << m_jetAlgo1 << " algo: " << m_jetAlgo2 )
		    CALIB_LOG_FILE ( "jet1Num: " << m_jet1Num << " jet2Num: " << m_jet2Num )
		    CALIB_LOG_FILE ( "Jet1 " << *jet1 )
		    CALIB_LOG_FILE ( "Jet2 " << *jet2 )
		    CALIB_LOG_FILE( event.GetContent( settings ) )
		}*/
		
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

class SourceJetPtAbsDiff: public ZJetSourceBase
{
public:

	// jet1->Pt / jet2->Pt
	SourceJetPtAbsDiff(std::string jetAlgo1, std::string jetAlgo2,
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
		val = (jet1->p4.Pt() - jet2->p4.Pt());

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
