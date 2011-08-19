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

	virtual void AddPlot( Hist1D * hist )
	{
		m_hist1d.push_back( hist );
	}


	// store all histograms
	virtual void Finish()
	{
		TSettings s = this->GetPipelineSettings();

		BOOST_FOREACH( Hist1D & p, m_hist1d)
		{
			p.Store(s.GetRootOutFile());
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

		m_npv = new Hist1D( "npv_",
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_npv );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		m_npv->Fill( event.m_primaryVertex->nVertices, metaData.GetWeight());
	}

private:
	Hist1D * m_npv;

};


/*
 * Calculates the Response distribution with a Histogram
 */
class BinResponseConsumer: public ZJetMetaConsumer
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_resp = new Hist1D( "balanceresp_",
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetResponseModifier() );
		AddPlot ( m_resp );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);

		KDataLV * jet0 = event.GetPrimaryJet(this->GetPipelineSettings());
		assert( jet0 );
		assert( metaData.HasValidMuons());

		// fill with the Pt Balance Response
		m_resp->Fill( jet0->p4.Pt() / metaData.GetRefZ().p4.Pt(),
				metaData.GetWeight());
	}

	static std::string GetName()
	{
		return "bin_response";
	}

	Hist1D * m_resp;
};

class MetadataConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_run = new Hist1D( "run_",
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

		m_numPU = new Hist1D( "numpu_",
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNRVModifier() );
		AddPlot ( m_numPU );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		MetadataConsumer::ProcessFilteredEvent( event, metaData);
		m_numPU->Fill( event.m_geneventmetadata->numPUInteractions, metaData.GetWeight());
	}

private:
	Hist1D * m_numPU;

};


class MetaConsumerDataLV: public ZJetMetaConsumer
{
public:
	MetaConsumerDataLV()
	 : m_plotMass( false )
	{

	}

	MetaConsumerDataLV(std::string physicsObjectName)
	 : m_plotMass( false )
	{
		SetPhysicsObjectName(physicsObjectName);
		MetaConsumerDataLV();
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );



		m_histPt = new Hist1D(GenName(GetPhysicsObjectName(), "_pt_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetPtModifier());

		m_histEta = new Hist1D(GenName(GetPhysicsObjectName(), "_eta_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetEtaModifier());

		m_histPhi = new Hist1D(GenName(GetPhysicsObjectName(), "_phi_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetPhiModifier());

		if ( m_plotMass )
		{
			m_histMass = new Hist1D(GenName(GetPhysicsObjectName(), "_mass_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetMassModifier());
			AddPlot( m_histMass );
		}

		AddPlot(m_histPt);
		AddPlot(m_histEta);
		AddPlot(m_histPhi);
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline

	virtual void PlotDataLVQuantities(KDataLV const* dataLV,
			ZJetMetaData const& metaData)
	{
		m_histPt->Fill(dataLV->p4.Pt(), metaData.GetWeight());
		m_histEta->Fill(dataLV->p4.Eta(), metaData.GetWeight());
		m_histPhi->Fill(dataLV->p4.Phi(), metaData.GetWeight());

		if ( m_plotMass )
			m_histMass->Fill(dataLV->p4.mass(), metaData.GetWeight());
	}

	std::string GenName(std::string const& sInp, std::string const& sQuant)
	{
		std::string modString(sInp);
		boost::replace_all(modString, "%quant%", sQuant);
		return modString;
	}

	Hist1D * m_histPt;
	Hist1D * m_histEta;
	Hist1D * m_histPhi;
	Hist1D * m_histMass;

	bool m_plotMass;

	std::string m_sObjectName;

	IMPL_PROPERTY(std::string, PhysicsObjectName )

};

class DataZConsumer: public MetaConsumerDataLV
{
public:
	DataZConsumer()
	{
		SetPhysicsObjectName("Z%quant%");
		m_plotMass = true;
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		PlotDataLVQuantities( metaData.GetPtZ(), metaData );
	}
};

class DataMuonConsumer: public MetaConsumerDataLV
{
public:
	DataMuonConsumer( char charge ) : m_charge( charge )
	{
		if ( m_charge > 0)
			SetPhysicsObjectName("mu_plus%quant%");

		if ( m_charge < 0)
			SetPhysicsObjectName("mu_minus%quant%");
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		//todo: somehow filter the good muons
		for ( KDataMuons::iterator it = event.m_muons->begin();
				it != event.m_muons->end(); it ++ )
		{
			if ( it->charge == m_charge)
			{
				PlotDataLVQuantities( &(*it), metaData );
				return;
			}
		}

	}

	char m_charge;
};


/// maybe also used for other stuff, like muons
class DataLVsConsumer: public MetaConsumerDataLV
{
public:

	DataLVsConsumer(
			std::string productName,
			unsigned int productIndex) :
				MetaConsumerDataLV()
				{
		SetProductIndex( productIndex );
		SetProductName( productName );

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
		KDataLV * lv = event.GetJet( GetPipelineSettings(), GetProductIndex());

		if (lv == NULL)
			// no valid entry for us here !

		{
			return;
		}

		PlotDataLVQuantities(lv, metaData);
		ProcessFilteredEvent_specific( event, metaData, lv);
	}

	virtual void ProcessFilteredEvent_specific( ZJetEventData const& event,
			ZJetMetaData const& metaData,
			KDataLV * jet)
	{

	}

	virtual void Finish()
	{
		MetaConsumerDataLV::Finish();
	}

	IMPL_PROPERTY_READONLY( std::string, ProductName )
	IMPL_PROPERTY_READONLY( unsigned int, ProductIndex )

};

class DataPFJetsConsumer:
public DataLVsConsumer
{
public:

	DataPFJetsConsumer( std::string productName,
			unsigned int productIndex) :
		DataLVsConsumer(  productName, productIndex)
		{

		}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		DataLVsConsumer::Init(pset);

		m_neutralEmFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_emfraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_neutralEmFraction );
	}

	virtual void ProcessFilteredEvent_specific( ZJetEventData const& event,
			ZJetMetaData const& metaData,
			KDataLV * jet)
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( jet );
		m_neutralEmFraction->Fill( pfJet->chargedEMFraction, metaData.GetWeight() );
	}

	Hist1D * m_neutralEmFraction;
};

class JetRespConsumer: public ZJetConsumer
{
public:

	JetRespConsumer( boost::property_tree::ptree * ptree , std::string configPath )
	{
		/*
		"Name" : "response_balance",
        "SourceFolder" : [ "Pt30to60_incut", "Pt60to100_incut" ],
        "SourceResponse" : "balresp_AK5PFJets"
		 */
		m_sourceFolder = PropertyTreeSupport::GetAsStringList( ptree, configPath + ".SourceFolder" );
		m_sourceResponse = ptree->get<std::string>( configPath + ".SourceResponse");
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetConsumer::Init( pset );

		m_run = new Hist1D( "run_",
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetNoModifier() );
		//AddPlot ( m_run );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetConsumer::ProcessFilteredEvent( event, metaData);
		m_run->Fill( event.m_eventmetadata->nRun, metaData.GetWeight());
	}

	virtual void Finish()
	{

	}

	static std::string GetName()
	{
		return "response_balance";
	}

private:
	Hist1D * m_run;
	stringvector m_sourceFolder;
	std::string m_sourceResponse;
};

/*
 template<>
 void MetaConsumerDataLV_Vector<KDataPFJet>::ProcessFilteredEvent(ZJetEventData const& event,
 ZJetMetaData const& metaData)
 {
 CALIB_LOG_FATAL("oh ja !")
 }*/

}

