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

Draw Calc c


DrawBase ([DataProvider for val, weight], DataSelection)


DrawBase (sets up histos, graphs), implements Pipeline,
taken
can add multiple data providers here
DrawHisto
needs: value, weight

DrawGraph
needs: x, y

DrawPolar (3d)
needs: r, phi, rho

DataProvider (extracts all needed values from data classes)
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

	virtual void AddPlot(Hist1D* hist)
	{
		m_hist1d.push_back(hist);
	}

	virtual void AddPlot(Hist2D* hist)
	{
		m_hist2d.push_back(hist);
	}

	// store all histograms
	virtual void Finish()
	{
		TSettings s = this->GetPipelineSettings();

		BOOST_FOREACH(Hist1D& p, m_hist1d)
		{
			p.Store(s.GetRootOutFile());
		}
		BOOST_FOREACH(Hist2D& p, m_hist2d)
		{
			p.Store(s.GetRootOutFile());
		}
	}

private:
	boost::ptr_list<Hist1D> m_hist1d;
	boost::ptr_list<Hist2D> m_hist2d;
};

typedef MetaConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetMetaConsumer;

typedef DrawConsumerBase<ZJetEventData, ZJetMetaData,
ZJetPipelineSettings> ZJetConsumer;

class PrimaryVertexConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_npv = new Hist1D("npv_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(49));
		AddPlot(m_npv);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
		m_npv->Fill(event.m_vertexSummary->nVertices,metaData.GetWeight());
	}

private:
	Hist1D* m_npv;
};

class RhoConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_rho = new Hist1D( "rho_"  + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetRhoModifier() );
		AddPlot ( m_rho );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);
		m_rho->Fill( event.m_jetArea->median, metaData.GetWeight());
	}

private:
	Hist1D * m_rho;

};


class BasicTwoDConsumer: public ZJetMetaConsumer
{
public:
	BasicTwoDConsumer(std::string algoName) : m_algoname(algoName) {}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_2dz = new Hist2D("2D_zeta_zphi_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, -3.14159f, 3.14159f);
		m_2djet1 = new Hist2D("2D_jet1eta_jet1phi_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, -3.14159f, 3.14159f);
		m_2djet2 = new Hist2D("2D_jet2eta_jet2phi_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, -3.14159f, 3.14159f);

		m_2djet1jet2 = new Hist2D("2D_deltaeta-jet1-jet2_deltaphi-jet1-jet2_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 10.0f, 0.0f, 3.14159f);
		m_2dzjet1 = new Hist2D("2D_deltaeta-z-jet1_deltaphi-z-jet1_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 10.0f, 0.0f, 3.14159f);
		m_2dzjet2 = new Hist2D("2D_deltaeta-z-jet2_deltaphi-z-jet2_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 10.0f, 0.0f, 3.14159f);
		m_2dzmet = new Hist2D("2D_deltaeta-z-met_deltaphi-z-met_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 10.0f, 0.0f, 3.14159f);
		m_2djet1met = new Hist2D("2D_deltaeta-jet1-met_deltaphi-jet1-met_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 10.0f, 0.0f, 3.14159f);

		m_2dalphadeltaphi = new Hist2D("2D_alpha_deltaphi-z-jet1_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 5.0f, 0.0f, 3.14159f);

		m_2dnpvzpt = new Hist2D("2D_npv_zpt_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 0.0f, 50.f, 0.0f, 1000.f);

		AddPlot(m_2dz);
		AddPlot(m_2djet1);
		AddPlot(m_2djet2);

		AddPlot(m_2djet1jet2);
		AddPlot(m_2dzjet1);
		AddPlot(m_2dzjet2);
		AddPlot(m_2dzmet);
		AddPlot(m_2djet1met);

		AddPlot(m_2dalphadeltaphi);

		AddPlot(m_2dnpvzpt);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);

        m_2dnpvzpt->Fill(
				event.m_vertexSummary->nVertices,
				metaData.GetRefZ().p4.Pt(),
				metaData.GetWeight());

		if (metaData.GetValidJetCount(this->GetPipelineSettings(), event, m_algoname) > 0)
		{
			m_2dz->Fill(
				metaData.GetRefZ().p4.Eta(),
				metaData.GetRefZ().p4.Phi(),
				metaData.GetWeight());
			m_2djet1->Fill(
				metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4.Eta(),
				metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4.Phi(),
				metaData.GetWeight());

			m_2dzjet1->Fill(
				std::abs(metaData.GetRefZ().p4.Eta() - metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4.Eta()),
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(metaData.GetRefZ().p4, metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4)),
						metaData.GetWeight());
			m_2dzmet->Fill(
				std::abs(metaData.GetRefZ().p4.Eta() - metaData.GetMet(event, GetPipelineSettings())->p4.Eta()),
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(metaData.GetRefZ().p4, metaData.GetMet(event, GetPipelineSettings())->p4)),
				metaData.GetWeight());

			m_2djet1met->Fill(
				std::abs(metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4.Eta() - metaData.GetMet(event, GetPipelineSettings())->p4.Eta()),
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4, metaData.GetMet(event, GetPipelineSettings())->p4)),
				metaData.GetWeight());
		}

		if (metaData.GetValidJetCount(this->GetPipelineSettings(), event, m_algoname) > 1)
		{
			m_2djet2->Fill(
				metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4.Eta(),
				metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4.Phi(),
				metaData.GetWeight());

			m_2djet1jet2->Fill(
				std::abs(metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4.Eta() - metaData.GetValidJet(
						this->GetPipelineSettings(), event, 1, m_algoname)->p4.Eta()),
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4, metaData.GetValidJet(
						this->GetPipelineSettings(), event, 1, m_algoname)->p4)),
				metaData.GetWeight());

			m_2dzjet2->Fill(
				std::abs(metaData.GetRefZ().p4.Eta() - metaData.GetValidJet(
						this->GetPipelineSettings(), event, 1, m_algoname)->p4.Eta()),
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(metaData.GetRefZ().p4, metaData.GetValidJet(
						this->GetPipelineSettings(), event, 1, m_algoname)->p4)),
				metaData.GetWeight());

			m_2dalphadeltaphi->Fill(
				metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4.Pt() / metaData.GetRefZ().p4.Pt(),
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(metaData.GetRefZ().p4, metaData.GetValidJet(
						this->GetPipelineSettings(), event, 0, m_algoname)->p4)),
				metaData.GetWeight());
		}
	}

	static std::string GetName()
	{
		return "basic_twod_consumer";
	}

private:
	Hist2D* m_2dz;
	Hist2D* m_2djet1;
	Hist2D* m_2djet2;

	Hist2D* m_2djet1jet2;
	Hist2D* m_2dzjet1;
	Hist2D* m_2dzjet2;
	Hist2D* m_2dzmet;
	Hist2D* m_2djet1met;

	Hist2D* m_2dalphadeltaphi;

	Hist2D* m_2dnpvzpt;

	std::string m_algoname;
};

class GenZTwoDConsumer: public ZJetMetaConsumer
{
public:
	GenZTwoDConsumer(std::string algoName) : m_algoname(algoName) {}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_2dgenz = new Hist2D("2D_genzeta_genzphi_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, -3.14159f, 3.14159f);
		m_2dgenz2 = new Hist2D("2D_genzeta_genzpt_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, 0.0f, 1000.f);
		m_2dgenz3 = new Hist2D("2D_genzmass_genzpt_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), 71.19f, 111.19f, 0.f, 1000.f);
		m_2dgenzetarap = new Hist2D("2D_genzeta_genzrapidity_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, -5.0f, 5.0f);
		m_2dgenzrappt = new Hist2D("2D_genzrapidity_genzpt_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(), -5.0f, 5.0f, 0.0f, 1000.f);


		AddPlot(m_2dgenz);
		AddPlot(m_2dgenz2);
		AddPlot(m_2dgenz3);
		AddPlot(m_2dgenzetarap);
		AddPlot(m_2dgenzrappt);

	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);


			m_2dgenz->Fill(
				metaData.GetRefGenZ().p4.Eta(),
				metaData.GetRefGenZ().p4.Phi(),
				metaData.GetWeight());
			m_2dgenz2->Fill(
				metaData.GetRefGenZ().p4.Eta(),
				metaData.GetRefGenZ().p4.Pt(),
				metaData.GetWeight());
			m_2dgenz3->Fill(
				metaData.GetRefGenZ().p4.mass(),
				metaData.GetRefGenZ().p4.Pt(),
				metaData.GetWeight());
			m_2dgenzetarap->Fill(
				metaData.GetRefGenZ().p4.eta(),
				metaData.GetRefGenZ().p4.Rapidity(),
				metaData.GetWeight());
			m_2dgenzrappt->Fill(
				metaData.GetRefGenZ().p4.Rapidity(),
				metaData.GetRefGenZ().p4.Pt(),
				metaData.GetWeight());

	}

	static std::string GetName()
	{
		return "genz_twod_consumer";
	}

private:
	Hist2D* m_2dgenz;
	Hist2D* m_2dgenz2;
	Hist2D* m_2dgenz3;
	Hist2D* m_2dgenzetarap;
	Hist2D* m_2dgenzrappt;


	std::string m_algoname;
};



class DeltaConsumer: public ZJetMetaConsumer
{
public:
	DeltaConsumer(std::string algoName): m_algoname(algoName)
	{}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_deltaRsecondjettoZ = new Hist1D("deltar-secondjet-z_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaRModifier());
		m_deltaRsecondjettoleadingjet = new Hist1D("deltar-secondjet-leadingjet_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaRModifier());

		m_deltaPhiLeadingJetToZ = new Hist1D("deltaphi-leadingjet-z_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaPhiModifier());
		m_deltaPhiLeadingJetToSecondJet = new Hist1D("deltaphi-leadingjet-secondjet_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaPhiModifier());
		m_deltaPhiZToSecondJet = new Hist1D("deltaphi-z-secondjet_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaPhiModifier());
		m_deltaPhiLeadingJetToMET = new Hist1D("deltaphi-leadingjet-MET_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaPhiModifier());
		m_deltaPhiZToMET = new Hist1D("deltaphi-z-MET_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaPhiModifier());

		m_deltaEtaLeadingJetToZ = new Hist1D("deltaeta-leadingjet-z_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaEtaModifier());
		m_deltaEtaLeadingJetToSecondJet = new Hist1D("deltaeta-leadingjet-secondjet_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaEtaModifier());
		m_deltaEtaZToSecondJet = new Hist1D("deltaeta-z-secondjet_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaEtaModifier());
		m_deltaEtaZToMET = new Hist1D("deltaeta-z-met_" + m_algoname,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetDeltaEtaModifier());

		AddPlot(m_deltaRsecondjettoZ);
		AddPlot(m_deltaRsecondjettoleadingjet);

		AddPlot(m_deltaPhiLeadingJetToZ);
		AddPlot(m_deltaPhiLeadingJetToSecondJet);
		AddPlot(m_deltaPhiZToSecondJet);
		AddPlot(m_deltaPhiLeadingJetToMET);
		AddPlot(m_deltaPhiZToMET);

		AddPlot(m_deltaEtaLeadingJetToZ);
		AddPlot(m_deltaEtaLeadingJetToSecondJet);
		AddPlot(m_deltaEtaZToSecondJet);
		AddPlot(m_deltaEtaZToMET);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);

		if (metaData.GetValidJetCount(this->GetPipelineSettings(), event, m_algoname) > 1)
		{
			m_deltaRsecondjettoZ->Fill(
				ROOT::Math::VectorUtil::DeltaR(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4,
					metaData.GetRefZ().p4),
				metaData.GetWeight());
			m_deltaRsecondjettoleadingjet->Fill(
				ROOT::Math::VectorUtil::DeltaR(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4,
					metaData.GetValidJet(this->GetPipelineSettings(), event,0, m_algoname)->p4),
				metaData.GetWeight());

			m_deltaPhiLeadingJetToZ->Fill(
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4,
					metaData.GetRefZ().p4)),
				metaData.GetWeight());
			m_deltaPhiLeadingJetToSecondJet->Fill(
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4,
					metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4)),
				metaData.GetWeight());
			m_deltaPhiZToSecondJet->Fill(
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(
					metaData.GetRefZ().p4,
					metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4)),
				metaData.GetWeight());
			m_deltaPhiLeadingJetToMET->Fill(
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4,
					metaData.GetMet(event, GetPipelineSettings())->p4)),
				metaData.GetWeight());
			m_deltaPhiZToMET->Fill(
				std::abs(ROOT::Math::VectorUtil::DeltaPhi(
					metaData.GetRefZ().p4,
					metaData.GetMet(event, GetPipelineSettings())->p4)),
				metaData.GetWeight());

			m_deltaEtaLeadingJetToZ->Fill(
				std::abs(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4.Eta() -
					metaData.GetRefZ().p4.eta()),
				metaData.GetWeight());
			m_deltaEtaLeadingJetToSecondJet->Fill(
				std::abs(
					metaData.GetValidJet(this->GetPipelineSettings(), event, 0, m_algoname)->p4.Eta() -
					metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4.Eta()),
				metaData.GetWeight());
			m_deltaEtaZToSecondJet->Fill(
				std::abs(
					metaData.GetRefZ().p4.Eta() -
					metaData.GetValidJet(this->GetPipelineSettings(), event, 1, m_algoname)->p4.Eta()),
				metaData.GetWeight());
			m_deltaEtaZToMET->Fill(
				std::abs(metaData.GetRefZ().p4.Eta() - metaData.GetMet(event, GetPipelineSettings())->p4.Eta()),
				metaData.GetWeight());
		}
	}

private:
	Hist1D* m_deltaRsecondjettoZ;
	Hist1D* m_deltaRsecondjettoleadingjet;

	Hist1D* m_deltaPhiLeadingJetToZ;
	Hist1D* m_deltaPhiLeadingJetToSecondJet;
	Hist1D* m_deltaPhiZToSecondJet;
	Hist1D* m_deltaPhiLeadingJetToMET;
	Hist1D* m_deltaPhiZToMET;

	Hist1D* m_deltaEtaLeadingJetToZ;
	Hist1D* m_deltaEtaLeadingJetToSecondJet;
	Hist1D* m_deltaEtaZToSecondJet;
	Hist1D* m_deltaEtaZToMET;

	std::string m_algoname;
};


class ValidMuonsConsumer: public ZJetMetaConsumer
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_valid = new Hist1D("muonsvalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(49));
		AddPlot(m_valid);
		m_invalid = new Hist1D("muonsinvalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(49));
		AddPlot(m_invalid);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
		m_valid->Fill(metaData.GetValidMuons().size(), metaData.GetWeight());
		m_invalid->Fill(metaData.GetInvalidMuons().size(), metaData.GetWeight());
	}

private:
	Hist1D* m_valid;
	Hist1D* m_invalid;
};


class ValidJetsConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_valid = new Hist1D("jetsvalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(199));
		AddPlot(m_valid);

		// only plot the invalid jets for the initial algos (AK5PFJets)
		if (JetType :: IsRaw(this->GetPipelineSettings().GetJetAlgorithm()))
		{
			m_invalid = new Hist1D("jetsinvalid_" + this->GetPipelineSettings().GetJetAlgorithm(),
					GetPipelineSettings().GetRootFileFolder(),
					Hist1D::GetCountModifier(50));
			AddPlot(m_invalid);
		}
		else
		{
			m_invalid = NULL;
		}
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
		m_valid->Fill(metaData.GetValidJetCount(this->GetPipelineSettings(), event), metaData.GetWeight());

		if (m_invalid != NULL)
			m_invalid->Fill(metaData.GetInvalidJetCount(this->GetPipelineSettings(), event), metaData.GetWeight());
	}

private:
	Hist1D* m_valid;
	Hist1D* m_invalid;
};

class FlavorConsumer: public ZJetMetaConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_flavor = new Hist1D("flavor_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(25, -1));
		AddPlot(m_flavor);

	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
		int pdg = std::abs(metaData.GetBalancedParton().pdgId());
		if (pdg > 25)
			pdg = -1;
		m_flavor->Fill(pdg, metaData.GetWeight());
	}

private:
	Hist1D* m_flavor;
};


class MetadataConsumer: public ZJetMetaConsumer
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init(pset);

		m_run = new Hist1D("run_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetRunModifier());
		AddPlot(m_run);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
		m_run->Fill(event.m_eventmetadata->nRun, metaData.GetWeight());
	}

private:
	Hist1D* m_run;
};


class GenMetadataConsumer: public MetadataConsumer
{
public:

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		MetadataConsumer::Init(pset);

		m_numPU = new Hist1D("numpu_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(49));
		m_numPUtruth = new Hist1D("numputruth_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(49));
		m_weight = new Hist1D("weights_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(0));
		AddPlot(m_numPU);
		AddPlot(m_numPUtruth);
		AddPlot(m_weight);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		MetadataConsumer::ProcessFilteredEvent(event, metaData);
		m_numPU->Fill(event.m_geneventmetadata->numPUInteractions0, metaData.GetWeight());
		m_numPUtruth->Fill(event.m_geneventmetadata->numPUInteractionsTruth, metaData.GetWeight());
		m_weight->Fill(0.0, metaData.GetWeight());
	}

private:
	Hist1D* m_numPU;
	Hist1D* m_numPUtruth;
	Hist1D* m_weight;
};

class FilterSummaryConsumer: public ZJetMetaConsumer
{
public:
	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>* pset)
	{
		ZJetMetaConsumer::Init(pset);
		m_firstEvent = true;
		m_filters = new Hist1D("filters_" + this->GetPipelineSettings().GetJetAlgorithm(),
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetCountModifier(16));
		AddPlot(m_filters);
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent(event, metaData);
//		if (m_firstEvent)
//		{
//			for (auto it = event.m_filtermetadata->filternames.begin();
//					it != event.m_filtermetadata->filternames.begin(); it++)
//				xtitle += " " + *it;
//			m_filters->AddModifier(new ModHistLabels("Filters3"));
//			m_firstEvent = false;
//			CALIB_LOG(xtitle)
//		}
		const int maxfilters = 12;
		for (int i = 0; i < maxfilters; i++)
			if (!event.m_filter->passedFilter(i))
				m_filters->Fill(i, metaData.GetWeight());

		if (!event.m_filter->passedFilters())
			m_filters->Fill(maxfilters, metaData.GetWeight());
	}

private:
	Hist1D* m_filters;
	bool m_firstEvent;
	std::string xtitle;
};

/*
template<>
void MetaConsumerDataLV_Vector<KDataPFJet>::ProcessFilteredEvent(ZJetEventData const& event,
ZJetMetaData const& metaData)
{
CALIB_LOG_FATAL("oh ja !")
}
*/

}

