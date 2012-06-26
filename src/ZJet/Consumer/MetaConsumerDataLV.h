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
#include "ZJetDrawConsumer.h"

namespace CalibFW
{

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
			std::string algorithm, 
            bool useValidJets,
            std::string namePrefix = "") :
				MetaConsumerDataLV(),
				m_algorithm( algorithm )
				{
        SetUseValidJets( useValidJets );
		SetProductIndex( productIndex );
		SetProductName( productName );

		std::stringstream jetName;
		jetName << namePrefix << "jet" << (GetProductIndex() + 1) << "%quant%"
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

		if (GetProductIndex() >= metaData.GetValidJetCount(this->GetPipelineSettings(), event) )
			// no valid entry for us here !
			return;

		if ( (m_algorithm != "") && (GetProductIndex() >= metaData.GetValidJetCount(this->GetPipelineSettings(), event, m_algorithm)) )
			// no valid GenJet!
			return;
		

		KDataLV * lv;

    
        if ( GetUseValidJets() )
        {
    		if( m_algorithm == "" )
    			lv = metaData.GetValidJet( GetPipelineSettings(), event, GetProductIndex());
    		else
    			lv = metaData.GetValidJet( GetPipelineSettings(), event, GetProductIndex(), m_algorithm);

    		assert (lv != NULL);

    		PlotDataLVQuantities(lv, metaData);
    		ProcessFilteredEvent_specific( event, metaData, lv);

        }
        else
        {
            // plot all invalid jet quantities at once !
    		if( m_algorithm == "" ) { CALIB_LOG_FATAL( "not implemented, please provide jet name") }

            unsigned int invalidJetCount =  metaData.GetInvalidJetCount( GetPipelineSettings(), event, m_algorithm);
            for ( unsigned int i = 0; i < invalidJetCount; ++i )
            {
    			lv = metaData.GetInvalidJet( GetPipelineSettings(), event, GetProductIndex(), m_algorithm);
                assert ( lv != NULL );
        		PlotDataLVQuantities(lv, metaData);
        		ProcessFilteredEvent_specific( event, metaData, lv);                
            }
        }

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
    IMPL_PROPERTY_READONLY( bool, UseValidJets )

	std::string m_algorithm;
};



class DataGenJetConsumer: public DataLVsConsumer
{
public:
	DataGenJetConsumer(   std::string productName,
			unsigned int productIndex, std::string algoName ) :
					DataLVsConsumer( productName, productIndex, algoName, true)
	{
	}
};


class DataPFJetsConsumer: public DataLVsConsumer
{
public:

	DataPFJetsConsumer( std::string productName,
			unsigned int productIndex,
			std::string algoName = "",
			bool onlyBasic = false,
            bool useValidJets = true, 
            std::string namePrefix = "" ) :
				DataLVsConsumer( productName, productIndex, algoName, useValidJets, namePrefix),
				m_onlyBasic( onlyBasic )
				{

				}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		DataLVsConsumer::Init(pset);

		if ( ! m_onlyBasic )
			{
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
	}

	virtual void ProcessFilteredEvent_specific( ZJetEventData const& event,
			ZJetMetaData const& metaData,
			KDataLV * jet)
	{
		if ( ! m_onlyBasic)
			{
			KDataPFJet * pfJet = static_cast<KDataPFJet*>( jet );

			m_neutralEmFraction->Fill( pfJet->neutralEMFraction, metaData.GetWeight() );
			m_chargedEMFraction->Fill( pfJet->chargedEMFraction, metaData.GetWeight() );
			m_chargedHadFraction->Fill( pfJet->chargedHadFraction, metaData.GetWeight() );
			m_neutralHadFraction->Fill( pfJet->neutralHadFraction, metaData.GetWeight() );
			m_muonFraction->Fill( pfJet->muonFraction, metaData.GetWeight() );

			m_const->Fill( pfJet->nConst, metaData.GetWeight() );
			m_charged->Fill( pfJet->nCharged, metaData.GetWeight() );
			m_area->Fill( pfJet->area, metaData.GetWeight() );

			m_summedFraction->Fill( pfJet->neutralEMFraction + pfJet->chargedEMFraction +
					pfJet->chargedHadFraction + pfJet->neutralHadFraction +
					pfJet->muonFraction ,
					metaData.GetWeight() );
		}

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

	bool m_onlyBasic;
};


}

