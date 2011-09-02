#pragma once

#include <string>


#include "GlobalInclude.h"
#include "Draw/Profile.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

namespace CalibFW
{

class SourceRecoVert
{
public:
	double GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings)
	{
		return (double)event.m_primaryVertex->nVertices;
	}
};

template < unsigned int TJetNum >
class SourceL1Correction
{
public:
	double GetValue(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings)
	{
		// todo: get raw and l1 jet here !
		KDataLV * jet =	metaData.GetValidJet(settings, event, TJetNum );
		return (jet->p4.Pt() / jet->p4.Pt() );
	}
};


template < class TXSource, class TYSource >
class ProfileConsumerBase : public ZJetConsumerBase
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetConsumerBase::Init( pset );

		// init the profile plot
/*
		m_profile = new Profile2D();
		m_valid = new Hist1D( "muons_valid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_valid );
		m_invalid = new Hist1D( "muons_invalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_invalid );*/
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetConsumerBase::ProcessFilteredEvent( event, metaData);

		m_profile->AddPoint(	m_xsource.GetValue( event, metaData, this->GetPipelineSettings() ),
							m_ysource.GetValue( event, metaData, this->GetPipelineSettings() ),
							metaData.GetWeight() );
	}

	// store all histograms
	virtual void Finish()
	{
		m_profile->Store( this->GetPipelineSettings().GetRootOutFile());
	}


private:
	TXSource m_xsource;
	TYSource m_ysource;

	Profile2d * m_profile;

};

}
