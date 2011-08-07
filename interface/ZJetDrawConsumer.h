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
	MetaConsumerDataLV()
	{

	}

	MetaConsumerDataLV(std::string physicsObjectName)
	{
		SetPhysicsObjectName(physicsObjectName);

		MetaConsumerDataLV();
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		m_histPt = new Hist1D();
		m_histEta = new Hist1D();
		m_histPhi = new Hist1D();

		m_hist1d.push_back(m_histPt);
		m_hist1d.push_back(m_histEta);
		m_hist1d.push_back(m_histPhi);

		m_histPt->SetNameAndCaption(GenName(GetPhysicsObjectName(), "_pt_"));
		m_histPhi->SetNameAndCaption(GenName(GetPhysicsObjectName(), "_phi_"));
		m_histEta->SetNameAndCaption(GenName(GetPhysicsObjectName(), "_eta_"));

		m_pipeline = pset;

		m_histPt->Init( Hist1D::GetPtModifier());
		m_histPt->SetRootFileFolder(GetPipelineSettings()->GetRootFileFolder());

		m_histEta->Init(Hist1D::GetEtaModifier());
		m_histEta->SetRootFileFolder(GetPipelineSettings()->GetRootFileFolder());

		m_histPhi->Init(Hist1D::GetPhiModifier());
		m_histPhi->SetRootFileFolder(GetPipelineSettings()->GetRootFileFolder());
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline

	virtual void PlotDataLVQuantities(KDataLV const& dataLV,
			ZJetMetaData const& metaData)
	{
		m_histPt->Fill(dataLV.p4.Pt(), metaData.GetWeight());
		m_histEta->Fill(dataLV.p4.Eta(), metaData.GetWeight());

		// Kappa->Phi is centered around zero
		m_histPhi->Fill(dataLV.p4.Phi(), metaData.GetWeight());
	}

	virtual void Finish()
	{
		BOOST_FOREACH( Hist1D & p, m_hist1d)
		{		p.Store(GetPipelineSettings()->GetRootOutFile());
		}
	}
	/*
 virtual void SetPhysicsObjectName(std::string sOb)
 {
 CALIB_LOG("Set names " << GenName( sOb ,"_pt_"))

 }*/

	std::string GenName(std::string const& sInp, std::string const& sQuant)
	{
		std::string modString(sInp);
		boost::replace_all(modString, "%quant%", sQuant);
		return modString;
	}

	Hist1D * m_histPt;
	Hist1D * m_histEta;
	Hist1D * m_histPhi;

	boost::ptr_list<Hist1D> m_hist1d;

	std::string m_sObjectName;

	IMPL_PROPERTY(std::string, PhysicsObjectName )

};

/// maybe also used for other stuff, like muons
template<class TJetType>
class DataLVsConsumer: public MetaConsumerDataLV
{
public:

	DataLVsConsumer( vector<TJetType> * jetSource,
			std::string productName,
			unsigned int productIndex) :
				MetaConsumerDataLV()
				{
		SetProductIndex( productIndex );
		SetProductName( productName );
		SetSource( jetSource);

		std::stringstream jetName;
		jetName << "jet" << (GetProductIndex() + 1) << "%quant%"
				<< GetProductName();

		SetPhysicsObjectName( jetName.str() );
				}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		MetaConsumerDataLV::Init(pset);
		Init_specific( pset );
	}

	virtual void Init_specific(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{

	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		//CALIB_LOG( m_source->size() )
		// call sub plots
		if (GetProductIndex() >= GetSource()->size())
			// no valid entry for us here !

		{
			//CALIB_LOG_FATAL( "No entry for the Product " << GetProductName() << " with index " << GetProductID() )
			return;
		}

		TJetType const& jet = GetSource()->at(GetProductIndex());

		PlotDataLVQuantities(jet, metaData);

		// TODO: is jet valid
		//		m_histJetPt.Fill(jet.p4.Pt(), metaData.GetWeight());
		//		m_histJetPt.Fill(jet.p4.Pt(), metaData.GetWeight());
		//CALIB_LOG(pfJet.p4.Pt() );
		ProcessFilteredEvent_specific( event, metaData, jet);
	}

	virtual void ProcessFilteredEvent_specific( ZJetEventData const& event,
			ZJetMetaData const& metaData,
			TJetType const& jet)
	{

	}

	virtual void Finish()
	{
		MetaConsumerDataLV::Finish();
	}

	IMPL_PROPERTY_READONLY( vector<TJetType> *, Source)
	IMPL_PROPERTY_READONLY( std::string, ProductName )
	IMPL_PROPERTY_READONLY( unsigned int, ProductIndex )

};

class DataPFJetsConsumer:
public DataLVsConsumer<KDataPFJet>
{
public:

	DataPFJetsConsumer( vector<KDataPFJet> * jetSource, std::string productName, unsigned int productIndex) :
		DataLVsConsumer<KDataPFJet>( jetSource, productName, productIndex)
		{

		}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		DataLVsConsumer<KDataPFJet>::Init(pset);

		m_neutralEmFraction = new Hist1D();
		m_neutralEmFraction->SetNameAndCaption(GenName(GetPhysicsObjectName(), "_emfraction_"));

		m_neutralEmFraction->Init(Hist1D::GetFractionModifier());
		m_neutralEmFraction->SetRootFileFolder(GetPipelineSettings()->GetRootFileFolder());

		m_hist1d.push_back( m_neutralEmFraction );


	}

	virtual void ProcessFilteredEvent_specific( ZJetEventData const& event,
			ZJetMetaData const& metaData,
			KDataPFJet const& jet)
	{
		m_neutralEmFraction->Fill( jet.chargedEMFraction, metaData.GetWeight() );
	}

	Hist1D * m_neutralEmFraction;
};

/*
 template<>
 void MetaConsumerDataLV_Vector<KDataPFJet>::ProcessFilteredEvent(ZJetEventData const& event,
 ZJetMetaData const& metaData)
 {
 CALIB_LOG_FATAL("oh ja !")
 }*/

}

