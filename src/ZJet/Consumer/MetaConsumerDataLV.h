#pragma once

#include <string>
#include <iostream>
#include <list>
#include <typeinfo>

#include <stdio.h>
#include <stdlib.h>

#include <boost/assign/list_of.hpp>
#include <boost/assert.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/ptree.hpp>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "Draw/DrawBase.h"
#include "Pipeline/JetTools.h"
#include "../ZJetPipeline.h"
#include "ZJetDrawConsumer.h"

namespace CalibFW {

class MetaConsumerDataLV: public ZJetMetaConsumer
{
public:
	MetaConsumerDataLV() : m_plotMass(false) {}

	MetaConsumerDataLV(std::string physicsObjectName, std::string algoName):
			m_plotMass(false)
	{
		SetPhysicsObjectName(physicsObjectName + "_" + algoName);
		//MetaConsumerDataLV(algoName);
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_histPt = new Hist1D(GenName(GetPhysicsObjectName(), "pt_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetPtModifier());

		m_histEta = new Hist1D(GenName(GetPhysicsObjectName(), "eta_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetEtaModifier());

		m_histAbsEta = new Hist1D(GenName(GetPhysicsObjectName(), "abseta_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetAbsEtaModifier());

		m_histPhi = new Hist1D(GenName(GetPhysicsObjectName(), "phi_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetPhiModifier());

		if (m_plotMass) {
			m_histMass = new Hist1D(GenName(GetPhysicsObjectName(), "mass_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetMassModifier());
			AddPlot(m_histMass);
		}

		AddPlot(m_histPt);
		AddPlot(m_histEta);
		AddPlot(m_histAbsEta);
		AddPlot(m_histPhi);
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline

	virtual void PlotDataLVQuantities(KDataLV const* dataLV,
			ZJetMetaData const& metaData)
	{
		m_histPt->Fill(dataLV->p4.Pt(), metaData.GetWeight());
		m_histEta->Fill(dataLV->p4.Eta(), metaData.GetWeight());
		m_histAbsEta->Fill(TMath::Abs(dataLV->p4.Eta()), metaData.GetWeight());
		m_histPhi->Fill(dataLV->p4.Phi(), metaData.GetWeight());

		if (m_plotMass)
			m_histMass->Fill(dataLV->p4.mass(), metaData.GetWeight());
	}

	std::string GenName(std::string const& sInp, std::string const& sQuant)
	{
		std::string modString(sInp);
		boost::replace_all(modString, "%quant%", sQuant);
		return modString;
	}

	Hist1D* m_histPt;
	Hist1D* m_histEta;
	Hist1D* m_histAbsEta;
	Hist1D* m_histPhi;
	Hist1D* m_histMass;

	bool m_plotMass;
	std::string m_sObjectName;
	IMPL_PROPERTY(std::string, PhysicsObjectName)
};


class DataZConsumer: public MetaConsumerDataLV
{
public:
	DataZConsumer(std::string algoName)
	{
		SetPhysicsObjectName("z%quant%" + algoName);
		m_plotMass = true;
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		MetaConsumerDataLV::Init(pset);

		m_histY = new Hist1D(GenName(GetPhysicsObjectName(), "Y_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetEtaModifier());

		AddPlot(m_histY);

	}
	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		PlotDataLVQuantities(metaData.GetPtZ(), metaData);
		m_histY->Fill(metaData.GetRefZ().p4.Rapidity(), metaData.GetWeight());
	}
	Hist1D* m_histY;
};


class DataMETConsumer: public MetaConsumerDataLV
{
public:
	DataMETConsumer(std::string algoName)
	{
		SetPhysicsObjectName("MET%quant%" + algoName);
		m_plotMass = false;
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		MetaConsumerDataLV::Init(pset);

		m_sumEt = new Hist1D(GenName(GetPhysicsObjectName(), "sumEt_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetMETModifier());

		AddPlot(m_sumEt);

		m_fraction = new Hist1D(GenName(GetPhysicsObjectName(), "fraction_"),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetMETFractionModifier());

		AddPlot(m_fraction);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		PlotDataLVQuantities(metaData.GetMet(event, GetPipelineSettings()), metaData);
		m_sumEt->Fill(metaData.GetMet(event, GetPipelineSettings())->sumEt, metaData.GetWeight());
		m_fraction->Fill(metaData.GetMet(event, GetPipelineSettings())->p4.Pt() / metaData.GetMet(event, GetPipelineSettings())->sumEt, metaData.GetWeight());
	}

	Hist1D* m_sumEt;
	Hist1D* m_fraction;
};

class DataRawMETConsumer: public MetaConsumerDataLV
{
public:
	DataRawMETConsumer(std::string algoName)
	{
		SetPhysicsObjectName("rawMET%quant%" + algoName);
		m_plotMass = false;
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		MetaConsumerDataLV::Init(pset);

	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		PlotDataLVQuantities(event.m_pfMet, metaData);
	}
};


class DataMuonConsumer: public MetaConsumerDataLV
{
public:
	DataMuonConsumer(char charge, std::string algoName) : m_charge(charge)
	{
		if (m_charge > 0)
			SetPhysicsObjectName("muplus%quant%" + algoName);

		if (m_charge < 0)
			SetPhysicsObjectName("muminus%quant%" + algoName);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				it != metaData.m_listValidMuons.end(); it ++)
		{
			if (it->charge == m_charge)
			{
				PlotDataLVQuantities(&(*it), metaData);
				return;
			}
		}
	}

	char m_charge;
};

class DataGenMuonConsumer: public MetaConsumerDataLV
{
public:
	DataGenMuonConsumer(char charge, std::string algoName) : m_charge(charge)
	{
		if (m_charge > 0)
			SetPhysicsObjectName("genmuplus%quant%" + algoName);

		if (m_charge < 0)
			SetPhysicsObjectName("genmuminus%quant%" + algoName);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		for (KGenParticles::const_iterator it = metaData.m_genMuons.begin();
				it != metaData.m_genMuons.end(); it ++)
		{
			if (it->charge() == m_charge)
			{
				PlotDataLVQuantities(&(*it), metaData);
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
	DataLVsConsumer(std::string productName, unsigned int productIndex,
			std::string algorithm, bool useValidJets, std::string namePrefix = "") :
			MetaConsumerDataLV(), m_algorithm(algorithm)
	{
		SetUseValidJets(useValidJets);
		SetProductIndex(productIndex);
		SetProductName(productName);

		std::stringstream jetName;
		jetName << namePrefix << "jet" << (GetProductIndex() + 1) << "%quant%"
				<< GetProductName();

		SetPhysicsObjectName(jetName.str());
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		MetaConsumerDataLV::Init(pset);
		Init_specific(pset);
	}

	virtual void Init_specific(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		//CALIB_LOG(m_source->size())
		// call sub plots

		if (GetProductIndex() >= metaData.GetValidJetCount(this->GetPipelineSettings(), event))
			// no valid entry for us here !
			return;

		if ((m_algorithm != "") && (GetProductIndex() >= metaData.GetValidJetCount(this->GetPipelineSettings(), event, m_algorithm)))
			// no valid GenJet!
			return;


		KDataLV* lv;

		if (GetUseValidJets())
		{
			if (m_algorithm == "")
				lv = metaData.GetValidJet(GetPipelineSettings(), event, GetProductIndex());
			else
				lv = metaData.GetValidJet(GetPipelineSettings(), event, GetProductIndex(), m_algorithm);
			assert (lv != NULL);

			PlotDataLVQuantities(lv, metaData);
			ProcessFilteredEvent_specific(event, metaData, lv);
		}
		else
		{
			// plot all invalid jet quantities at once !
			if(m_algorithm == "")
				CALIB_LOG_FATAL("not implemented, please provide jet name")

			unsigned int invalidJetCount =  metaData.GetInvalidJetCount(
					GetPipelineSettings(), event, m_algorithm);
			for (unsigned int i = 0; i < invalidJetCount; ++i)
			{
				lv = metaData.GetInvalidJet(GetPipelineSettings(), event, GetProductIndex(), m_algorithm);
				assert (lv != NULL);
				PlotDataLVQuantities(lv, metaData);
				ProcessFilteredEvent_specific(event, metaData, lv);
			}
		}
	}

	virtual void ProcessFilteredEvent_specific(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			KDataLV* jet)
	{}

	virtual void Finish()
	{
		MetaConsumerDataLV::Finish();
	}

	IMPL_PROPERTY_READONLY(std::string, ProductName)
	IMPL_PROPERTY_READONLY(unsigned int, ProductIndex)
	IMPL_PROPERTY_READONLY(bool, UseValidJets)

	std::string m_algorithm;
};



class DataGenJetConsumer: public DataLVsConsumer
{
public:
	DataGenJetConsumer(std::string productName, unsigned int productIndex,
			std::string algoName) :
		DataLVsConsumer(productName, productIndex, algoName, true)
	{}
};


class DataPFJetsConsumer: public DataLVsConsumer
{
public:

	DataPFJetsConsumer(std::string productName, unsigned int productIndex,
			std::string algoName = "", bool onlyBasic = false,
			bool useValidJets = true, std::string namePrefix = "") :
		DataLVsConsumer(productName, productIndex, algoName, useValidJets, namePrefix),
			m_onlyBasic(onlyBasic)
	{}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		DataLVsConsumer::Init(pset);

		if (! m_onlyBasic)
		{
			m_neutralEmFraction = new Hist1D(GenName(GetPhysicsObjectName(), "neutralemfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_neutralEmFraction);

			m_chargedEMFraction = new Hist1D(GenName(GetPhysicsObjectName(), "chargedemfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_chargedEMFraction);

			m_chargedHadFraction = new Hist1D(GenName(GetPhysicsObjectName(), "chargedhadfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_chargedHadFraction);

			m_neutralHadFraction = new Hist1D(GenName(GetPhysicsObjectName(), "neutralhadfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_neutralHadFraction);

			m_HFEMFraction = new Hist1D(GenName(GetPhysicsObjectName(), "HFemfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_HFEMFraction);

			m_HFHadFraction = new Hist1D(GenName(GetPhysicsObjectName(), "HFhadfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_HFHadFraction);

			m_muonFraction = new Hist1D(GenName(GetPhysicsObjectName(), "muonfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_muonFraction);

			m_electronFraction = new Hist1D(GenName(GetPhysicsObjectName(), "electronfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_electronFraction);

			m_photonFraction = new Hist1D(GenName(GetPhysicsObjectName(), "photonfraction_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_photonFraction);


			m_const = new Hist1D(GenName(GetPhysicsObjectName(), "const_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetCountModifier(149));
			AddPlot(m_const);

			m_charged = new Hist1D(GenName(GetPhysicsObjectName(), "charged_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetCountModifier(149));
			AddPlot(m_charged);

			m_summedFraction = new Hist1D(GenName(GetPhysicsObjectName(), "summedfractions_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_summedFraction);

			m_summedFraction2 = new Hist1D(GenName(GetPhysicsObjectName(), "summedfractions2_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetFractionModifier());
			AddPlot(m_summedFraction2);

			m_area = new Hist1D(GenName(GetPhysicsObjectName(), "area_"),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetAreaModifier());
			AddPlot(m_area);
		}
	}

	virtual void ProcessFilteredEvent_specific(ZJetEventData const& event,
			ZJetMetaData const& metaData, KDataLV* jet)
	{
		if (!m_onlyBasic)
		{
			KDataPFJet* pfJet = static_cast<KDataPFJet*>(jet);

			m_neutralEmFraction->Fill(pfJet->neutralEMFraction, metaData.GetWeight());
			m_chargedEMFraction->Fill(pfJet->chargedEMFraction, metaData.GetWeight());
			m_chargedHadFraction->Fill(pfJet->chargedHadFraction, metaData.GetWeight());
			m_HFEMFraction->Fill(pfJet->HFEMFraction, metaData.GetWeight());
			m_HFHadFraction->Fill(pfJet->HFHadFraction, metaData.GetWeight());
			m_neutralHadFraction->Fill(pfJet->neutralHadFraction, metaData.GetWeight());
			m_muonFraction->Fill(pfJet->muonFraction, metaData.GetWeight());


			m_photonFraction->Fill(pfJet->photonFraction, metaData.GetWeight());
			m_electronFraction->Fill(pfJet->electronFraction, metaData.GetWeight());

			m_const->Fill(pfJet->nConst, metaData.GetWeight());
			m_charged->Fill(pfJet->nCharged, metaData.GetWeight());
			m_area->Fill(pfJet->area, metaData.GetWeight());

			m_summedFraction->Fill(
					pfJet->neutralEMFraction + pfJet->chargedEMFraction +
					pfJet->chargedHadFraction + pfJet->neutralHadFraction +
					pfJet->HFHadFraction + pfJet->HFEMFraction +
					pfJet->muonFraction,
					metaData.GetWeight());

			m_summedFraction2->Fill(
					pfJet->photonFraction + pfJet->electronFraction +
					pfJet->chargedHadFraction + pfJet->neutralHadFraction +
					pfJet->HFHadFraction + pfJet->HFEMFraction +
					pfJet->muonFraction,
					metaData.GetWeight());
		}
	}

	Hist1D* m_summedFraction;
	Hist1D* m_neutralEmFraction;
	Hist1D* m_chargedEMFraction;
	Hist1D* m_chargedHadFraction;
	Hist1D* m_HFEMFraction;
	Hist1D* m_HFHadFraction;
	Hist1D* m_neutralHadFraction;
	Hist1D* m_muonFraction;

	Hist1D* m_electronFraction;
	Hist1D* m_photonFraction;
	Hist1D* m_summedFraction2;

	Hist1D* m_const;
	Hist1D* m_charged;
	Hist1D* m_area;

	bool m_onlyBasic;
};


}

