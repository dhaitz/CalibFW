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

		m_npv = new Hist1D( "npv_",
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

/*
 * Calculates the Response distribution with a Histogram
 */
class BinResponseConsumer: public ZJetMetaConsumer
{
public:
	enum
	{	MpfResponse, BalResponse}m_respType;

	BinResponseConsumer( boost::property_tree::ptree * ptree , std::string configPath) :
		ZJetMetaConsumer()
		{
		{
			/*
		 "Name" : "response_balance",
		 "ProductName" : "binresp",
		 "ResponseType": "bal"|"mpf",
		 "JetNumber" : 1 to n
			 */
			m_jetnum = ptree->get<unsigned int>( configPath + ".JetNumber", 1 ) -1;
			m_name = ptree->get<std::string>( configPath + ".ProductName");

			if ( ptree->get<std::string>( configPath + ".ResponseType") == "bal" )
				m_respType = BalResponse;
			else if ( ptree->get<std::string>( configPath + ".ResponseType") == "mpf" )
				m_respType = MpfResponse;
			else
			{
				CALIB_LOG_FATAL("Unknown Response type " + ptree->get<std::string>( configPath + ".ResponseType") );
			}
		}
		}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_resp = new Hist1D( m_name,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetResponseModifier() );
		AddPlot ( m_resp );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);

		assert( metaData.HasValidZ());

		if ( m_respType == BalResponse )
		{

			if ( m_jetnum >= metaData.GetValidJetCount( this->GetPipelineSettings(), event))
				// on jet for us here
				return;

			KDataLV * jet0 = metaData.GetValidJet(this->GetPipelineSettings(),
					event,
					m_jetnum);
			assert( jet0 != NULL );
			//std::cout << "Folder: " <<  this->GetPipelineSettings().GetRootFileFolder() << std::endl;


			// fill with the Pt Balance Response
			m_resp->Fill( metaData.GetBalance( jet0 ),
					metaData.GetWeight());
		}

		if ( m_respType == MpfResponse )
		{
			m_resp->Fill( metaData.GetMPF( event.GetMet( this->GetPipelineSettings()) ),
					metaData.GetWeight());

		}
	}

	static std::string GetName()
	{
		return "bin_response";
	}

private:
	Hist1D * m_resp;
	unsigned int m_jetnum;

	std::string m_name;
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
		AddPlot ( m_numPU );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		MetadataConsumer::ProcessFilteredEvent( event, metaData);
		m_numPU->Fill( event.m_geneventmetadata->numPUInteractions0, metaData.GetWeight());
	}

private:
	Hist1D * m_numPU;

};

class MetaConsumerDataLV: public ZJetMetaConsumer
{
public:
	MetaConsumerDataLV( )
	: m_plotMass( false )
	  {

	  }

	MetaConsumerDataLV(std::string physicsObjectName, std::string algoName)
	: m_plotMass( false )
	  {
		SetPhysicsObjectName(physicsObjectName + "_" + algoName);
		//MetaConsumerDataLV( algoName );
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
	DataZConsumer( std::string algoName)
	{
		SetPhysicsObjectName("z%quant%" + algoName);
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
	DataMuonConsumer( char charge, std::string algoName ) : m_charge( charge )
	{
		if ( m_charge > 0)
			SetPhysicsObjectName("mu_plus%quant%" + algoName);

		if ( m_charge < 0)
			SetPhysicsObjectName("mu_minus%quant%" + algoName);
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
			unsigned int productIndex,
			std::string algorithm) :
				MetaConsumerDataLV(),
				m_algorithm( algorithm )
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

		if ( GetProductIndex() >= ( metaData.GetValidJetCount( this->GetPipelineSettings(), event)) )
			// no valid entry for us here !
			return;

		KDataLV * lv;

		if( m_algorithm == "" )
			lv = metaData.GetValidJet( GetPipelineSettings(), event, GetProductIndex());
		else
			lv = metaData.GetValidJet( GetPipelineSettings(), event, GetProductIndex(), m_algorithm);

		assert (lv != NULL);

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

	std::string m_algorithm;
};



class DataGenJetConsumer: public DataLVsConsumer
{
public:
	DataGenJetConsumer(   std::string productName,
			unsigned int productIndex, std::string algoName ) :
					DataLVsConsumer( productName, productIndex, algoName)
	{
	}
};


class DataPFJetsConsumer: public DataLVsConsumer
{
public:

	DataPFJetsConsumer( std::string productName,
			unsigned int productIndex,
			std::string algoName = "") :
				DataLVsConsumer( productName, productIndex, algoName)
				{

				}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		DataLVsConsumer::Init(pset);

		m_neutralEmFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_neutral_em_fraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_neutralEmFraction );

		m_chargedEMFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_charged_em_fraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_chargedEMFraction );

		m_chargedHadFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_charged_had_fraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_chargedHadFraction );

		m_neutralHadFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_neutral_had_fraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_neutralHadFraction );

		m_muonFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_muon_fraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_muonFraction );

		m_const = new Hist1D( GenName(GetPhysicsObjectName(), "_const_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetConstituentsModifier());

		AddPlot( m_const );

		m_charged = new Hist1D( GenName(GetPhysicsObjectName(), "_charged_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetConstituentsModifier());

		AddPlot( m_charged );

		m_summedFraction = new Hist1D( GenName(GetPhysicsObjectName(), "_summed_fractions_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetFractionModifier());

		AddPlot( m_summedFraction );

		m_area = new Hist1D( GenName(GetPhysicsObjectName(), "_area_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetAreaModifier());

		AddPlot( m_area );
	}

	virtual void ProcessFilteredEvent_specific( ZJetEventData const& event,
			ZJetMetaData const& metaData,
			KDataLV * jet)
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( jet );

		m_neutralEmFraction->Fill( pfJet->neutralEMFraction, metaData.GetWeight() );
		m_chargedEMFraction->Fill( pfJet->chargedEMFraction, metaData.GetWeight() );
		m_chargedHadFraction->Fill( pfJet->chargedHadFraction, metaData.GetWeight() );
		m_neutralHadFraction->Fill( pfJet->neutralHadFraction, metaData.GetWeight() );

		//to come in next kappa skim
		// m_muonEtFraction->Fill( pfJet->muonEtFraction, metaData.GetWeight() );

		m_const->Fill( pfJet->nConst, metaData.GetWeight() );
		m_charged->Fill( pfJet->nCharged, metaData.GetWeight() );
		m_area->Fill( pfJet->area, metaData.GetWeight() );

		m_summedFraction->Fill( pfJet->neutralEMFraction + pfJet->chargedEMFraction +
				pfJet->chargedHadFraction + pfJet->neutralHadFraction ,
				metaData.GetWeight() );

	}

	Hist1D * m_summedFraction;
	Hist1D * m_neutralEmFraction;
	Hist1D * m_chargedEMFraction;
	Hist1D * m_chargedHadFraction;
	Hist1D * m_neutralHadFraction;
	Hist1D * m_muonFraction;
	Hist1D * m_const;
	Hist1D * m_charged;
	Hist1D * m_area;
};

/*
 template<>
 void MetaConsumerDataLV_Vector<KDataPFJet>::ProcessFilteredEvent(ZJetEventData const& event,
 ZJetMetaData const& metaData)
 {
 CALIB_LOG_FATAL("oh ja !")
 }*/

}

