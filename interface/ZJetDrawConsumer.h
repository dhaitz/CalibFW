#pragma once

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"
#include "EventPipeline.h"
#include "CutHandler.h"
#include "DrawBase.h"

#include "DrawModifier.h"
#include "ZJetPipeline.h"
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

typedef DrawHist2DConsumerBase<ZJetEventData, ZJetMetaData,
		ZJetPipelineSettings> ZJetHist2D;
typedef DrawHist1dConsumerBase<ZJetEventData, ZJetMetaData,
		ZJetPipelineSettings> ZJetHist1D;
typedef DrawGraphErrorsConsumerBase<ZJetEventData, ZJetMetaData,
		ZJetPipelineSettings> ZJetGraphErrors;

class DrawJetConsumerBase: public ZJetHist1D
{
public:
	DrawJetConsumerBase(int jetNum) :
		DrawHist1dConsumerBase<ZJetEventData, ZJetMetaData,
				ZJetPipelineSettings> (), m_jetNum(jetNum)
	{
	}

	int m_jetNum;
};

template<class TData, class TMetaData, class TSettings>
class MetaConsumerBase: public DrawConsumerBase<TData, TMetaData, TSettings>
{

};

class MetaConsumerDataLV: public MetaConsumerBase<ZJetEventData, ZJetMetaData,
		ZJetPipelineSettings>
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		m_pipeline = pset;

		m_histPt.Init();

		m_histPt.AddModifier(new ModHistBinRange(0.0f, 1000.0f));
		m_histPt.AddModifier(new ModHistBinCount(500));

		m_histPt.SetRootFileFolder(GetPipelineSettings()->GetRootFileFolder());

		m_histEta.Init();

		m_histEta.AddModifier(new ModHistBinRange(-10.f, +10.0f));
		m_histEta.AddModifier(new ModHistBinCount(200));

		m_histEta.SetRootFileFolder(GetPipelineSettings()->GetRootFileFolder());
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline

	virtual void PlotDataLVQuantities(KDataLV const& dataLV,
			ZJetMetaData const& metaData)
	{
		m_histPt.Fill(dataLV.p4.Pt(), metaData.GetWeight());
		m_histEta.Fill(dataLV.p4.Eta(), metaData.GetWeight());
	}

	virtual void Finish()
	{
		m_histPt.Store(GetPipelineSettings()->GetRootOutFile());
		m_histEta.Store(GetPipelineSettings()->GetRootOutFile());
	}

	virtual void SetPhysicsObjectName(std::string sOb)
	{
		CALIB_LOG("Set names " << GenName( sOb ,"_pt_"))
		m_histPt.SetNameAndCaption(GenName(sOb, "_pt_"));
		m_histEta.SetNameAndCaption(GenName(sOb, "_eta_"));
	}

	std::string GenName(std::string const& sInp, std::string const& sQuant)
	{
		std::string modString(sInp);
		boost::replace_all(modString, "%quant%", sQuant);
		return modString;
	}

	Hist1D m_histPt;
	Hist1D m_histEta;
	std::string m_sObjectName;
};

/// maybe also used for other stuff, like muons
template<class TJetType>
class MetaConsumerDataLV_Vector: public MetaConsumerDataLV
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		std::stringstream jetName;
		jetName << "jet" << (GetProductID() + 1) << "%quant%"
				<< GetProductName();

		SetPhysicsObjectName(jetName.str());

		MetaConsumerDataLV::Init(pset);
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		if (m_source->size() > 0)
		{

			CALIB_LOG_FATAL( "ieiae" )
		}
		//CALIB_LOG( m_source->size() )
		// call sub plots
		if (GetProductID() >= m_source->size())
		// no valid entry for us here !
		{
			//CALIB_LOG_FATAL( "No entry for the Product " << GetProductName() << " with index " << GetProductID() )
			return;
		}

		CALIB_LOG_FATAL( "ieiae" )

		TJetType & jet = m_source->at(GetProductID());

		PlotDataLVQuantities(jet, metaData);

		// TODO: is jet valid
		//		m_histJetPt.Fill(jet.p4.Pt(), metaData.GetWeight());
		//		m_histJetPt.Fill(jet.p4.Pt(), metaData.GetWeight());
		//CALIB_LOG(pfJet.p4.Pt() );

	}

	virtual void SetSource(vector<TJetType> * jetSource)
	{
		m_source = jetSource;
	}

	virtual void Finish()
	{
		MetaConsumerDataLV::Finish();
	}

	vector<TJetType> * m_source;

IMPL_PROPERTY( std::string, ProductName )
IMPL_PROPERTY( unsigned int, ProductID )

};
}

