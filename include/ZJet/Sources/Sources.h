/*
 * Sources.h
 *
 *  Created on: Sep 21, 2011
 *      Author: poseidon
 */

#ifndef SOURCES_H_
#define SOURCES_H_


#include "../../Pipeline/SourceBase.h"
#include "../ZJetEventData.h"
#include "../ZJetMetaData.h"
#include "../ZJetPipelineSettings.h"

namespace CalibFW
{
typedef SourceBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings > ZJetSourceBase;

class SourceRecoVert : public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings,
			double & val) const
	{
		val = (double)event.m_vertexSummary->nVertices;
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const	{ return 51; }
	virtual double GetDefaultLowBin() const { return -0.5f; }
	virtual double GetDefaultHighBin() const {	return 50.5; }
};


class SourceJetEta : public ZJetSourceBase
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

	bool GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings,
			double & val) const
	{
		val = metaData.GetValidPrimaryJet( settings, event )->p4.Eta();
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const	{ return 100; }
	virtual double GetDefaultLowBin() const { return -10.0f; }
	virtual double GetDefaultHighBin() const {	return 10.0; }
};

class SourceJetPt : public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings,
			double & val) const
	{
		val = metaData.GetValidPrimaryJet( settings, event )->p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const	{ return 200; }
	virtual double GetDefaultLowBin() const { return 0.0f; }
	virtual double GetDefaultHighBin() const {	return 1000.0; }
};


class SourceCutValue : public ZJetSourceBase
{
public:
	SourceCutValue( long cutToScore) : m_cutToScore ( cutToScore )
	{

	}

	// returns 	1 = cut not passed
	// or 		0 = cut passed
	// The Profile plot, for example, can calculate a mean then
	bool GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings,
			double & val) const
	{
		bool bPassed;
		if ( m_cutToScore > -1 )
			bPassed = metaData.IsCutPassed( m_cutToScore );
		else
			bPassed = metaData.IsAllCutsPassed();

		if ( bPassed)
			val =  0.0f;
		else
			val = 1.0f;

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const	{ return 200; }
	virtual double GetDefaultLowBin() const { return 0.0f; }
	virtual double GetDefaultHighBin() const {	return 1000.0; }

private:
	// if -1 => check for all cuts
	long m_cutToScore;
};

class SourceJetPtRatio : public ZJetSourceBase
{
public:

	// jet1->Pt / jet2->Pt
	SourceJetPtRatio( std::string jetAlgo1, std::string jetAlgo2,
			unsigned int jet1Num , unsigned int jet2Num)
	: m_jetAlgo1(jetAlgo1), m_jetAlgo2(jetAlgo2),
			m_jet1Num(jet1Num), m_jet2Num(jet2Num)
	{

	}

	bool GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings,
			double & val) const
	{
		KDataLV * jet1 =	metaData.GetValidJet(settings, event, m_jet1Num, m_jetAlgo1 );
		KDataLV * jet2 =	metaData.GetValidJet(settings, event, m_jet2Num, m_jetAlgo2 );
		val =  (jet1->p4.Pt() / jet2->p4.Pt() );
		return true;
	}


private:
	std::string m_jetAlgo1, m_jetAlgo2;
	unsigned int m_jet1Num , m_jet2Num;
};

}

#endif /* SOURCES_H_ */
