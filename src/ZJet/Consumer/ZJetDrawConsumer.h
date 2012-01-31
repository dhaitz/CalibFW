#pragma once

#include <string>
#include <iostream>

#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp>
#include <list>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <boost/property_tree/ptree.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"

#include "Draw/DrawBase.h"
#include "Pipeline/JetTools.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

namespace CalibFW
{
/*

Draw Calc  c


DrawBase ( [DataProvider for val, weight], DataSelection )


DrawBase ( sets up histos, graphs ), implements Pipeline,
taken
can add multiple data providers here
DrawHisto
needs: value, weight

DrawGraph
needs: x, y

DrawPolar (3d)
needs: r, phi, rho

DataProvider ( extracts all needed values from data classes )
* linked to tho output the Draw classes
* can also cache the values and insert them in a FINAL STEP into the
graph

DataSelection
* selects which events are included into the draw set

DrawZMassConsumer

to solve:
fukin hell: what about plots which first have to aggregate data of all events. especially cut-efficiency plots
which have to know the percentage of all cuts passed ?
So eine Art Value aggregator? das wird in den meisten fällen ein TH1d sein, kann aber auch eine Custom klasse sein.

Major design change:
DrawProvider ist ein EventConsumer, der wiederum ein DrawHisto verwendet um Plots rauszuschreiben. The other way around.
Der Event Selektor wird nicht im DrawHisto, sondern direkt im EventConsumer angwendet.
So machen, dass der EventConsumer noch die Kontrolle drüber hat, was ihm entzogen wird.
*/
/*
typedef DrawHist2DConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetHist2D;*/
typedef DrawHist1dConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetHist1D;
/*
typedef DrawGraphErrorsConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetGraphErrors;
*/

template<class TData, class TMetaData, class TSettings>
class MetaConsumerBase: public DrawConsumerBase<TData, TMetaData, TSettings>
{
public:

	virtual void AddPlot(Hist1D * hist)
	{
		m_hist1d.push_back(hist);
	}

	// store all histograms
	virtual void Finish()
	{
		TSettings s = this->GetPipelineSettings();

		BOOST_FOREACH( Hist1D & p, m_hist1d)
		{		p.Store(s.GetRootOutFile());
		}
	}

private:
	boost::ptr_list<Hist1D> m_hist1d;

};

typedef MetaConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetMetaConsumer;

typedef DrawConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetConsumer;

class PrimaryVertexConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_npv = new Hist1D( "npv_"  + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_npv );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		m_npv->Fill( event.m_vertexSummary->nVertices, metaData.GetWeight());
	}

private:
	Hist1D * m_npv;

};

class ValidMuonsConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_valid = new Hist1D( "muons_valid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_valid );
		m_invalid = new Hist1D( "muons_invalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_invalid );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		m_valid->Fill( metaData.GetValidMuons().size(), metaData.GetWeight());
		m_invalid->Fill( metaData.GetInvalidMuons().size(), metaData.GetWeight());
	}

private:
	Hist1D * m_valid;
	Hist1D * m_invalid;

};

class ValidJetsConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_valid = new Hist1D( "jets_valid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_valid );

		m_invalid = new Hist1D( "jets_invalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_invalid );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		m_valid->Fill( metaData.GetValidJetCount( this->GetPipelineSettings(), event ), metaData.GetWeight());
		m_invalid->Fill( metaData.GetInvalidJetCount( this->GetPipelineSettings(), event), metaData.GetWeight());
	}

private:
	Hist1D * m_valid;
	Hist1D * m_invalid;

};

class MetadataConsumer: public ZJetMetaConsumer
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_run = new Hist1D( "run_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNoModifier() );
		AddPlot ( m_run );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		m_run->Fill( event.m_eventmetadata->nRun, metaData.GetWeight());
	}

private:
	Hist1D * m_run;

};

class GenMetadataConsumer: public MetadataConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		MetadataConsumer::Init( pset );

		m_numPU = new Hist1D( "numpu_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		m_numPUtruth = new Hist1D( "numputruth_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_numPU );
		AddPlot ( m_numPUtruth );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		MetadataConsumer::ProcessFilteredEvent( event, metaData);
		m_numPU->Fill( event.m_geneventmetadata->numPUInteractions0, metaData.GetWeight());
		m_numPUtruth->Fill( event.m_geneventmetadata->numPUInteractionsTruth, metaData.GetWeight());
	}

private:
	Hist1D * m_numPU;
	Hist1D * m_numPUtruth;

};


/*
template<>
void MetaConsumerDataLV_Vector<KDataPFJet>::ProcessFilteredEvent(ZJetEventData const& event,
ZJetMetaData const& metaData)
{
CALIB_LOG_FATAL("oh ja !")
}*/

}

