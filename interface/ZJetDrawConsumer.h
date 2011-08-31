#pragma once

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"
#include "EventPipeline.h"
#include "CutHandler.h"

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

typedef DrawHist2DConsumerBase<EventResult, EventMetaData,ZJetPipelineSettings> ZJetHist2D;
typedef DrawHist1dConsumerBase<EventResult, EventMetaData, ZJetPipelineSettings> ZJetHist1D;
typedef DrawGraphErrorsConsumerBase<EventResult, EventMetaData, ZJetPipelineSettings> ZJetGraphErrors;

#define IMPL_HIST1D_MOD1(CLASSNAME, DATAPATH, MOD1)	\
class CLASSNAME: public ZJetHist1D	{ public: \
virtual void Init(ZJetPipeline * pset) { \
	m_hist->AddModifier( MOD1 );	\
	ZJetHist1D::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \


#define IMPL_HIST1D_MOD2(CLASSNAME, DATAPATH, MOD1,  MOD2)	\
class CLASSNAME: public ZJetHist1D	{ public: \
virtual ~CLASSNAME() {}\
virtual void Init(ZJetPipeline * pset) { \
	m_hist->AddModifier( MOD1 ); m_hist->AddModifier( MOD2 );	\
	ZJetHist1D::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \

#define IMPL_HIST1D_JET_MOD1(CLASSNAME, DATAPATH, MOD1)	\
class CLASSNAME: public DrawJetConsumerBase	{ public: \
CLASSNAME ( int jetNum ) : DrawJetConsumerBase( jetNum) {} \
virtual ~CLASSNAME() {}\
virtual void Init(ZJetPipeline * pset) { \
	m_hist->AddModifier( MOD1 );	\
	DrawJetConsumerBase::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \


#define IMPL_HIST1D_JET_MOD2(CLASSNAME, DATAPATH, MOD1, MOD2)	\
class CLASSNAME: public DrawJetConsumerBase	{ public: \
CLASSNAME ( int jetNum ) : DrawJetConsumerBase( jetNum) {} \
virtual ~CLASSNAME() {}\
virtual void Init(ZJetPipeline * pset) { \
	m_hist->AddModifier( MOD1 ); m_hist->AddModifier( MOD2 );	\
	DrawJetConsumerBase::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \


class DrawJetConsumerBase: public ZJetHist1D
{
public:
	DrawJetConsumerBase(int jetNum) :
		DrawHist1dConsumerBase<EventResult, EventMetaData, ZJetPipelineSettings> (), m_jetNum(
				jetNum)
	{
	}

	int m_jetNum;
};

// Z STUFF
/*
 IMPL_HIST1D_MOD2(DrawZMassConsumerAlt ,m_hist->Fill(res.m_pData->Z->GetCalcMass() , res.GetWeight( )); ,
 new ModHistBinRange(0.0f, 200.0f),
 new ModHistBinCount(300))*/

/* ok, I investigated the template based customization of the plot
 but this does not result in less code for the drawing code
 -> stick with the macro-based approach, although it is not as "beatiful"
 from a development point of view
 enum ZMassDraw{};

 template <class TDrawType>
 class DrawHistTemplate: public DrawHist1dConsumerBase<EventResult>
 {
 public:
 virtual void Init(EventPipeline * pset)
 {
 //	m_hist->AddModifier( MOD1 );
 DrawHist1dConsumerBase<EventResult>::Init(pset);
 }
 virtual void ProcessFilteredEvent(EventResult & res)
 {
 // do fill here
 }
 };

 template<>
 void DrawHistTemplate<ZMassDraw>::ProcessFilteredEvent(EventResult & res)
 {

 }

 */

// Z STUFF
IMPL_HIST1D_MOD2(DrawZMassConsumer ,m_hist->Fill(res.m_pData->Z->GetCalcMass() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 200.0f),
		new ModHistBinCount(300))
IMPL_HIST1D_MOD2(DrawZPtConsumer ,m_hist->Fill(res.m_pData->Z->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))

IMPL_HIST1D_MOD1(DrawZEtaConsumer ,m_hist->Fill(res.m_pData->Z->Eta() , res.GetWeight( )); ,
		new ModHistBinRange(-5.0f, 5.0f))

IMPL_HIST1D_MOD1(DrawZPhiConsumer ,m_hist->Fill(res.m_pData->Z->Phi() - TMath::Pi(), res.GetWeight( )); ,
		new ModHistBinRange(-3.5f, 3.5f))

// Parton Flavour
IMPL_HIST1D_MOD2(DrawPartonFlavourConsumer ,m_hist->Fill(res.m_pData->partonFlavour, res.GetWeight( )); ,
		new ModHistBinRange(-50.5f, 49.5f),
		new ModHistBinCount(100))

// mus pt
IMPL_HIST1D_MOD2(DrawMuPlusPtConsumer ,m_hist->Fill(res.m_pData->mu_plus->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))
IMPL_HIST1D_MOD2(DrawMuMinusPtConsumer ,m_hist->Fill(res.m_pData->mu_minus->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))
// mus Eta
IMPL_HIST1D_MOD1(DrawMuPlusEtaConsumer ,m_hist->Fill(res.m_pData->mu_plus->Eta() , res.GetWeight( )); ,
		new ModHistBinRange(-5.0f, 5.0f))
IMPL_HIST1D_MOD1(DrawMuMinusEtaConsumer ,m_hist->Fill(res.m_pData->mu_minus->Eta() , res.GetWeight( )); ,
		new ModHistBinRange(-5.0f, 5.0f))

// mus phi
IMPL_HIST1D_MOD1(DrawMuPlusPhiConsumer ,m_hist->Fill(res.m_pData->mu_plus->Phi() - TMath::Pi() , res.GetWeight( )); ,
		new ModHistBinRange(-3.5f, 3.5f))
IMPL_HIST1D_MOD1(DrawMuMinusPhiConsumer ,m_hist->Fill(res.m_pData->mu_minus->Phi() - TMath::Pi(), res.GetWeight( )); ,
		new ModHistBinRange(-3.5f, 3.5f))

// Both mus
IMPL_HIST1D_MOD2(DrawMuAllPtConsumer ,m_hist->Fill(res.m_pData->mu_plus->Pt() , res.GetWeight( )); m_hist->Fill(res.m_pData->mu_minus->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))
IMPL_HIST1D_MOD1(DrawMuAllEtaConsumer ,m_hist->Fill(res.m_pData->mu_plus->Eta() , res.GetWeight( )); m_hist->Fill(res.m_pData->mu_minus->Eta() , res.GetWeight( )); ,
		new ModHistBinRange(-5.0f, 5.0f))
IMPL_HIST1D_MOD1(DrawMuAllPhiConsumer ,m_hist->Fill(res.m_pData->mu_plus->Phi() - TMath::Pi(), res.GetWeight( )); m_hist->Fill(res.m_pData->mu_minus->Phi() - TMath::Pi() , res.GetWeight( )); ,
		new ModHistBinRange(-3.5f, 3.5f))

// MET
IMPL_HIST1D_MOD2(DrawMetConsumer ,m_hist->Fill(res.m_pData->met->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))
IMPL_HIST1D_MOD2(DrawMetResolutionConsumer , m_hist->Fill(res.m_pData->met->Pt()/res.m_pData->genmet->Pt() - 1.0, res.GetWeight( )); ,
		new ModHistBinRange(-1.0f, 50.0f),
		new ModHistBinCount(510))
IMPL_HIST1D_MOD2(DrawTcMetConsumer ,m_hist->Fill(res.m_pData->tcmet->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))
//RECO VERT
IMPL_HIST1D_MOD2(DrawRecoVertConsumer ,m_hist->Fill( (double)res.GetRecoVerticesCount() , res.GetWeight( )); ,
		new ModHistBinRange(-0.5, 25.5),
		new ModHistBinCount(26))

//RECO PU INTERACTIONS
IMPL_HIST1D_MOD2(DrawPUConsumer ,m_hist->Fill( (double)res.GetPileUpInteractions() , res.GetWeight( )); ,
		new ModHistBinRange(-0.5, 25.5),
		new ModHistBinCount(26))

//RECO PU INTERACTIONS BEFORE
IMPL_HIST1D_MOD2(DrawPUBeforeConsumer ,m_hist->Fill( (double)res.GetPileUpInteractionsBefore() , res.GetWeight( )); ,
		new ModHistBinRange(-0.5, 25.5),
		new ModHistBinCount(26))

//RECO PU INTERACTIONS AFTER
IMPL_HIST1D_MOD2(DrawPUAfterConsumer ,m_hist->Fill( (double)res.GetPileUpInteractionsAfter() , res.GetWeight( )); ,
		new ModHistBinRange(-0.5, 25.5),
		new ModHistBinCount(26))

// SecondJet Pt / Z.Pt
IMPL_HIST1D_MOD1(Draw2ndJetPtDivZPtConsumer ,
		{	if ( res.IsJetValid( 1 ))
			{	m_hist->Fill( res.GetCorrectedJetPt(1) / res.m_pData->Z->Pt() , res.GetWeight( ));}},
		new ModHistBinRange(0.0, 2.0))

//JET RESP
IMPL_HIST1D_MOD1(DrawJetRespConsumer ,m_hist->Fill( res.GetCorrectedJetPt(0) / res.m_pData->Z->Pt() , res.GetWeight( )); ,
		new ModHistBinRange(0.0, 2.0))
// MPF RESP
IMPL_HIST1D_MOD1(DrawMpfJetRespConsumer ,
		{
			double scalPtEt = res.m_pData->Z->Px()*res.m_pData->met->Px() +
			res.m_pData->Z->Py()*res.m_pData->met->Py();
			double scalPtSq = res.m_pData->Z->Px()*res.m_pData->Z->Px() +
			res.m_pData->Z->Py()*res.m_pData->Z->Py();

			m_hist->Fill( 1.0 + (scalPtEt /scalPtSq), res.GetWeight( ));
		},
		new ModHistBinRange(0.0, 2.0))

// Matched Z
IMPL_HIST1D_MOD1(DrawZMatchConsumer ,
		{	if (res.m_pData->matched_Z!=NULL and res.m_pData->matched_Z->Pt() >0)
			{	m_hist->Fill( res.m_pData->Z->Pt() / res.m_pData->matched_Z->Pt(), res.GetWeight( ));}},
		new ModHistBinRange(0.0, 6.0))

// Jets
IMPL_HIST1D_JET_MOD2(DrawJetPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill(res.GetCorrectedJetPt( m_jetNum ) , res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))

IMPL_HIST1D_JET_MOD1(DrawJetEtaConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill(res.m_pData->jets[m_jetNum]->Eta() , res.GetWeight( ));
			}
		},
		new ModHistBinRange(-4.0f, 4.0f) )

IMPL_HIST1D_JET_MOD1(DrawJetPhiConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill(res.m_pData->jets[m_jetNum]->Phi() - TMath::Pi() , res.GetWeight( ));
			}
		},
		new ModHistBinRange(-3.5f, 3.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaPhiConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( DeltaHelper::GetDeltaPhiCenterZero( res.m_pData->Z,
								res.m_pData->jets[m_jetNum]),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(-3.5f, 3.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaPhiWrtJet1Consumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( DeltaHelper::GetDeltaPhiCenterZero( res.m_pData->jets[0],
								res.m_pData->jets[m_jetNum]),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(-3.5f, 3.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaEtaWrtJet1Consumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( TMath::Abs( res.m_pData->jets[0]->Eta() -
								res.m_pData->jets[m_jetNum]->Eta()),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(-3.5f, 3.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaRWrtJet1Consumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( DeltaHelper::GetDeltaR(res.m_pData->jets[0],res.m_pData->jets[m_jetNum]),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 6.0f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaEtaConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( TMath::Abs( res.m_pData->Z->Eta() -
								res.m_pData->jets[m_jetNum]->Eta()),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 4.0f) )

// matched jet pf / calo
// Jets
IMPL_HIST1D_JET_MOD1(DrawMatchCaloJetPtRatioConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ) && ( res.m_pData->matched_calo_jets[m_jetNum] != NULL)
					&& (res.m_pData->matched_calo_jets[m_jetNum]->Pt() > 0))
			{
				m_hist->Fill( res.m_pData->matched_calo_jets[m_jetNum]->Pt() / res.GetCorrectedJetPt( m_jetNum ) ,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 2.0f) )

IMPL_HIST1D_JET_MOD2(DrawMatchCaloJetPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ) && ( res.m_pData->matched_calo_jets[m_jetNum] != NULL)
					&& (res.m_pData->matched_calo_jets[m_jetNum]->Pt() > 0))
			{
				m_hist->Fill( res.m_pData->matched_calo_jets[m_jetNum]->Pt() ,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1000.0f),
		new ModHistBinCount(500))

// Energy fractions // Test if jet.Energy = sum (calenergies)
IMPL_HIST1D_JET_MOD1(DrawJetAllEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				/// this is not working correctly. still not sure how to add this

				m_hist->Fill( (  res.m_pData->pfProperties[ m_jetNum]->ChargedHadronEnergyFraction +
								res.m_pData->pfProperties[ m_jetNum]->NeutralHadronEnergyFraction +
								( res.m_pData->pfProperties[ m_jetNum]->ChargedEmEnergy / res.m_pData->jets[m_jetNum]->Energy()) +
								( res.m_pData->pfProperties[ m_jetNum]->NeutralEmEnergy / res.m_pData->jets[m_jetNum]->Energy()) +
								( res.m_pData->pfProperties[ m_jetNum]->ElectronEnergy / res.m_pData->jets[m_jetNum]->Energy()) +
								( res.m_pData->pfProperties[ m_jetNum]->MuonEnergy / res.m_pData->jets[m_jetNum]->Energy()) +
								( res.m_pData->pfProperties[ m_jetNum]->PhotonEnergy / res.m_pData->jets[m_jetNum]->Energy())
								),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 2.0f) )

IMPL_HIST1D_JET_MOD1(DrawJetChargedHadronEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ChargedHadronEnergyFraction,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetNeutralHadronEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->NeutralHadronEnergyFraction,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetChargedEmEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ChargedEmEnergy / res.m_pData->jets[m_jetNum]->Energy(),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetNeutralEmEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->NeutralEmEnergy / res.m_pData->jets[m_jetNum]->Energy(),
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetElectronEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				//{/m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ElectronEnergy / res.m_pData->jets[m_jetNum]->Energy(),
				//*		res.GetWeight( ));
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ElectronEnergyFraction,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetMuonEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->MuonEnergyFraction,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetPhotonEnergyFractionPtConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->PhotonEnergyFraction,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 1.5f) )

// Multiplicities
IMPL_HIST1D_JET_MOD2(DrawJetChargedHadronMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				//					CALIB_LOG( "pf multi " <<  res.m_pData->pfProperties[ m_jetNum]->ChargedHadronMultiplicity )
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ChargedHadronMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(-0.5f, 11.5f),
		new ModHistBinCount(12)

)

IMPL_HIST1D_JET_MOD1(DrawJetNeutralHadronMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->NeutralHadronMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 12.f) )

IMPL_HIST1D_JET_MOD1(DrawJetChargedMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ChargedMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 12.f) )

IMPL_HIST1D_JET_MOD1(DrawJetNeutralMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->NeutralMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 12.f) )

IMPL_HIST1D_JET_MOD1(DrawJetElectronMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->ElectronMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 400.f) )

IMPL_HIST1D_JET_MOD1(DrawJetMuonMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->MuonMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 12.f) )

IMPL_HIST1D_JET_MOD1(DrawJetPhotonMultiplicityConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->PhotonMultiplicity,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 12.f) )

IMPL_HIST1D_JET_MOD1(DrawJetConstituentsConsumer ,
		{
			if ( res.IsJetValid( m_jetNum ))
			{
				m_hist->Fill( res.m_pData->pfProperties[ m_jetNum]->Constituents,
						res.GetWeight( ));
			}
		},
		new ModHistBinRange(0.0f, 100.f) )

class DrawEventCount: public ZJetHist1D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{
		ModHistCustomBinnig * cbMod = new ModHistCustomBinnig(
				pset->GetSettings()->GetCustomBins());
		m_hist->AddModifier(cbMod);

		ZJetHist1D::Init(pset);
	}
	virtual void ProcessFilteredEvent(EventResult & res)
	{
		m_hist->Fill(res.m_pData->Z->Pt(), res.GetWeight());
	}
};

class DrawDeltaRMapConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0f, 5.0f, 0.0, 50.0));
		m_hist->AddModifier(new ModHist2DBinCount(120, 120));

		ZJetHist2D::Init(pset);
	}
};

class DrawBalanceMpfConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0f, 3.0f, 0.0f, 3.0f));
		m_hist->AddModifier(new ModHist2DBinCount(100, 100));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			double balance = res.GetCorrectedJetPt(0) / res.m_pData->Z->Pt();
			double scalPtEt = res.m_pData->Z->Px() * res.m_pData->met->Px()
					+ res.m_pData->Z->Py() * res.m_pData->met->Py();
			double scalPtSq = res.m_pData->Z->Px() * res.m_pData->Z->Px()
					+ res.m_pData->Z->Py() * res.m_pData->Z->Py();
			double mpf = 1.0 + (scalPtEt / scalPtSq);

			m_hist->Fill(balance, mpf, res.GetWeight());
		}
	}

};

class DrawJetActivityRecoVertMapConsumer: public ZJetHist2D
{
public:
	DrawJetActivityRecoVertMapConsumer()
	{
	}

	virtual void Init(ZJetPipeline * pset)
	{
		// magic master switch
		m_hist->m_bDoProfile = true;

		m_hist->AddModifier(new ModHist2DBinRange(-0.5, 14.5, 0.0f, 200.0f));
		m_hist->AddModifier(new ModHist2DBinCount(15, 50));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			m_hist->Fill(res.GetRecoVerticesCount(), res.GetCorrectedJetPt(1)
					+ res.GetCorrectedJetPt(2), res.GetWeight());
		}
	}
};

class Draw2ndJetCutNRVMapConsumer: public ZJetHist2D
{
public:
	Draw2ndJetCutNRVMapConsumer()
	{
	}

	virtual void Init(ZJetPipeline * pset)
	{
		// magic master switch
		m_hist->m_bDoProfile = true;

		m_hist->AddModifier(new ModHist2DBinRange(-0.5, 14.5, 0.0f, 3.0f));
		m_hist->AddModifier(new ModHist2DBinCount(15, 30));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			m_hist->Fill(res.GetRecoVerticesCount(), res.GetCorrectedJetPt(1) / res.m_pData->Z->Pt(), res.GetWeight());
		}
	}
};

class DrawDeltaRJetMapConsumer: public DrawDeltaRMapConsumer
{
public:
	DrawDeltaRJetMapConsumer(int jetNum) :
		m_jetNum(jetNum)
	{
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(m_jetNum))
		{
			m_hist->Fill(DeltaHelper::GetDeltaR(res.m_pData->jets[0],
					res.m_pData->jets[m_jetNum]), res.GetCorrectedJetPt(
					m_jetNum), res.GetWeight());
		}
	}

	int m_jetNum;
};

class DrawDeltaRJetRatioJetMapConsumer: public DrawDeltaRMapConsumer
{
public:
	DrawDeltaRJetRatioJetMapConsumer(int jetNum) :
		m_jetNum(jetNum)
	{
	}

	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0f, 5.0f, 0.0, 1.2));
		m_hist->AddModifier(new ModHist2DBinCount(120, 120));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(m_jetNum))
		{
			m_hist->Fill(DeltaHelper::GetDeltaR(res.m_pData->jets[0],
					res.m_pData->jets[m_jetNum]), res.GetCorrectedJetPt(
					m_jetNum) / res.GetCorrectedJetPt(0), res.GetWeight());
		}
	}

	int m_jetNum;
};

class DrawDeltaRJetRatioZMapConsumer: public DrawDeltaRMapConsumer
{
public:
	DrawDeltaRJetRatioZMapConsumer(int jetNum) :
		m_jetNum(jetNum)
	{
	}

	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0f, 5.0f, 0.0, 1.2));
		m_hist->AddModifier(new ModHist2DBinCount(120, 120));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(m_jetNum))
		{
			m_hist->Fill(DeltaHelper::GetDeltaR(res.m_pData->jets[0],
					res.m_pData->jets[m_jetNum]), res.GetCorrectedJetPt(
					m_jetNum) / res.m_pData->Z->Pt(), res.GetWeight());
		}
	}

	int m_jetNum;
};

class DrawEtaPhiMapConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0f, 4.0f, -3.2, 3.2));
		m_hist->AddModifier(new ModHist2DBinCount(40, 40));

		ZJetHist2D::Init(pset);
	}

};

class DrawPhiJet2PtConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(-3.2, 3.2, 0.0, 50.0f));
		m_hist->AddModifier(new ModHist2DBinCount(40, 40));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			m_hist->Fill(DeltaHelper::GetDeltaPhiCenterZero(
					res.m_pData->jets[1], res.m_pData->jets[0]),
					res.GetCorrectedJetPt(1), res.GetWeight());
		}
	}
};

class DrawPhiJet2RatioConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(-3.2, 3.2, 0.0, 1.2f));
		m_hist->AddModifier(new ModHist2DBinCount(40, 40));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			m_hist->Fill(DeltaHelper::GetDeltaPhiCenterZero(
					res.m_pData->jets[1], res.m_pData->jets[0]),
					res.GetCorrectedJetPt(1) / res.m_pData->Z->Pt(),
					res.GetWeight());
		}
	}
};

class DrawEtaJet2PtConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0f, 5.0f, 0.0, 50.0f));
		m_hist->AddModifier(new ModHist2DBinCount(40, 40));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			m_hist->Fill(TMath::Abs(res.m_pData->jets[0]->Eta()
					- res.m_pData->jets[1]->Eta()), res.GetCorrectedJetPt(1),
					res.GetWeight());
		}
	}
};

class DrawEtaJet2RatioConsumer: public ZJetHist2D
{
public:
	virtual void Init(ZJetPipeline * pset)
	{

		m_hist->AddModifier(new ModHist2DBinRange(0.0, 5.0f, 0.0, 1.2f));
		m_hist->AddModifier(new ModHist2DBinCount(40, 40));

		ZJetHist2D::Init(pset);
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(1))
		{
			m_hist->Fill(TMath::Abs(res.m_pData->jets[0]->Eta()
					- res.m_pData->jets[1]->Eta()), res.GetCorrectedJetPt(1)
					/ res.m_pData->Z->Pt(), res.GetWeight());
		}
	}
};

class DrawEtaPhiJetMapConsumer: public DrawEtaPhiMapConsumer
{
public:
	DrawEtaPhiJetMapConsumer(int jetNum) :
		m_jetNum(jetNum)
	{
	}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		if (res.IsJetValid(m_jetNum))
		{
			m_hist->Fill(TMath::Abs(res.m_pData->Z->Eta()
					- res.m_pData->jets[m_jetNum]->Eta()),
					DeltaHelper::GetDeltaPhiCenterZero(res.m_pData->Z,
							res.m_pData->jets[m_jetNum]), res.GetWeight());
		}
	}

	int m_jetNum;
};

class GraphXProviderBase
{
public:
	virtual ~GraphXProviderBase()
	{
	}
	virtual double GetXValue(EventResult & event) = 0;

};

class GraphXProviderZpt: public GraphXProviderBase
{
public:
	virtual double GetLow()
	{
		return 0.0f;
	}
	virtual double GetHigh()
	{
		return 300.0f;
	}
	virtual int GetBinCount()
	{
		return 45;
	}
	virtual double GetXValue(EventResult & event)
	{
		return event.m_pData->Z->Pt();
	}
};

class GraphXProviderRecoVert: public GraphXProviderBase
{
public:
	virtual double GetLow()
	{
		return -0.5f;
	}
	virtual double GetHigh()
	{
		return 24.5f;
	}
	virtual int GetBinCount()
	{
		return 25;
	}
	virtual double GetXValue(EventResult & event)
	{
		return event.GetRecoVerticesCount();
	}
};

template<int TJetNum>
class GraphXProviderJetPhiDeltaZ: public GraphXProviderBase
{
public:
	virtual double GetXValue(EventResult & event)
	{
		return DeltaHelper::GetDeltaPhiCenterZero(event.m_pData->Z,
				event.m_pData->jets[TJetNum]);
	}
};

class DrawDeltaPhiRange: public ZJetGraphErrors
{
public:
	DrawDeltaPhiRange() :
		ZJetGraphErrors()
	{
	}
	virtual void Finish()
	{
		// plot the efficiency
		for (double i = 0; i < 2 * TMath::Pi(); i += 0.1)
		{
			TVector3 v1 = TVector3(1.0, 0.0, 0.0);
			TVector3 v2 = TVector3(TMath::Cos(i), TMath::Sin(i), 0.0);

			m_graph->AddPoint(i, DeltaHelper::GetDeltaPhiCenterZero(v1, v2),
					0.0f, 0.0f);
		}

		ZJetGraphErrors::Finish();
	}
};

class DrawJetGraphBase: public ZJetGraphErrors
{
public:
	DrawJetGraphBase(int jetNum) :
		ZJetGraphErrors(), m_jetNum(jetNum)
	{

	}
	int m_jetNum;

};

template<class TXProvider>
class DrawJetPt: public DrawJetGraphBase
{
public:
	DrawJetPt(int jetNum) :
		DrawJetGraphBase(jetNum)
	{
	}

	// this method is called for all events
	virtual void ProcessFilteredEvent(EventResult & event)
	{
		m_graph->AddPoint(m_xProvider.GetXValue(event),
				event.GetCorrectedJetPt(this->m_jetNum), 0.0f, 0.0f);
	}
	TXProvider m_xProvider;
};

template<class TXProvider>
class DrawCutIneffGraph: public ZJetGraphErrors
{
public:
	DrawCutIneffGraph(int cutId) :
		ZJetGraphErrors(), m_iCutId(cutId)
	{
		m_hist_rejected.AddModifier(new ModHistBinRange(m_xProvider.GetLow(),
				m_xProvider.GetHigh()));
		m_hist_rejected.AddModifier(new ModHistBinCount(
				m_xProvider.GetBinCount()));

		m_hist_overall.AddModifier(new ModHistBinRange(m_xProvider.GetLow(),
				m_xProvider.GetHigh()));
		m_hist_overall.AddModifier(new ModHistBinCount(
				m_xProvider.GetBinCount()));

		m_hist_rejected.Init();
		m_hist_overall.Init();
	}

	// this method is called for all events
	virtual void ProcessEvent(EventResult & event, FilterResult & result)
	{
		if ( ! event.IsValidEvent())
			return;

		if ( event.IsCutInBitmask( m_iCutId ))
			m_hist_rejected.Fill( m_xProvider.GetXValue( event ), event.GetWeight( ) );

		m_hist_overall.Fill(m_xProvider.GetXValue(event), event.GetWeight());
	}

	virtual void Finish()
	{
		m_hist_rejected.GetRawHisto()->Divide(m_hist_overall.GetRawHisto());

		// plot the efficiency
		for (int i = 0; i < m_hist_rejected.GetRawHisto()->GetNbinsX(); i++)
		{
			m_graph->AddPoint(m_hist_rejected.GetRawHisto()->GetBinCenter(i),
					m_hist_rejected.GetRawHisto()->GetBinContent(i), 0.0f,
					m_hist_rejected.GetRawHisto()->GetBinError(i));
		}

		// store hist
		// + modifiers
		//CALIB_LOG( "Z mass mean " << m_hist->m_hist->GetMean() )
		ZJetGraphErrors::Finish();
	}

	int m_iCutId;
	TXProvider m_xProvider;

	// only used for the internal binning and not stored to root file
	Hist1D m_hist_rejected;
	Hist1D m_hist_overall;
};

class DrawJetRespGraph: public ZJetGraphErrors
{
public:
	DrawJetRespGraph(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}

	virtual void Process()
	{
		Hist1D m_histResp;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histResp.m_sCaption = m_histResp.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histResp.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histResp.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histResp.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histResp.GetRawHisto()->SetBinContent(i + 1, hresp->GetMean());
			m_histResp.GetRawHisto()->SetBinError(i + 1, hresp->GetMeanError());
			i++;
		}

		m_histResp.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};
/*
 class DrawPartonFlavourGraph: public DrawGraphErrorsConsumerBase<EventResult>
 {
 public:
 DrawPartonFlavourGraph() :
 ZJetGraphErrors
 {
 }

 // The Gen Z ratio

 class DrawZMatchConsumerGraph: public DrawGraphErrorsConsumerBase<EventResult>
 {
 public:
 DrawZMatchConsumerGraph( std::string sInpHist ) :
 ZJetGraphErrors,
 m_sInpHist( sInpHist)
 {

 }

 virtual void Process()
 {
 Hist1D m_histResp;

 // move through the histos
 stringvector sv = this->GetPipelineSettings()->GetCustomBins();
 std::vector< PtBin > bins = this->GetPipelineSettings()->GetAsPtBins( sv );

 m_histResp.m_sCaption = m_histResp.m_sName = this->GetPipelineSettings()->GetRootFileFolder() + this->GetProductName();
 //m_histResp.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
 m_histResp.AddModifier( new ModHistCustomBinnig( this->GetPipelineSettings()->GetCustomBins()) );
 m_histResp.Init();


 int i = 0;
 for (std::vector< PtBin >::iterator it = bins.begin();
 it != bins.end();
 it ++)
 {
 TString sfolder = this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

 sfolder.ReplaceAll( "XXPT_BINXX", (*it).id() );

 // cd to root folder
 this->GetPipelineSettings()->GetRootOutFile()->cd(
 TString( this->GetPipelineSettings()->GetRootOutFile()->GetName()) + ":" );

 TString sName = RootNamer::GetHistoName(
 this->GetPipelineSettings()->GetAlgoName(), m_sInpHist.c_str(),
 this->GetPipelineSettings()->GetInputType(), 0, &(*it), false) + "_hist";
 TH1D * hresp = (TH1D * )this->GetPipelineSettings()->GetRootOutFile()->Get(sfolder + "/" + sName );

 if (hresp == NULL)
 {
 CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
 }

 sName = RootNamer::GetHistoName(
 this->GetPipelineSettings()->GetAlgoName(), "z_pt",
 this->GetPipelineSettings()->GetInputType(), 0, &(*it), false) + "_hist";
 TH1D * hpt   = (TH1D * )this->GetPipelineSettings()->GetRootOutFile()->Get(sfolder + "/" + sName  );

 if (hpt == NULL)
 {
 CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
 }


 m_graph->AddPoint( hpt->GetMean(),
 hresp->GetMean(),
 hpt->GetMeanError(),
 hresp->GetMeanError());

 m_histResp.GetRawHisto()->SetBinContent(i +1, hresp->GetMean() );
 m_histResp.GetRawHisto()->SetBinError(i +1, hresp->GetMeanError() );
 i++;
 }

 m_histResp.Store(this->GetPipelineSettings()->GetRootOutFile());

 }
 std::string m_sInpHist;
 std::string m_sFolder;


 };





 virtual void Process()
 {
 Hist1D m_histResp;

 // move through the histos
 stringvector sv = this->GetPipelineSettings()->GetCustomBins();
 std::vector< PtBin > bins = this->GetPipelineSettings()->GetAsPtBins( sv );

 int i = 0;
 for (std::vector< PtBin >::iterator it = bins.begin();
 it != bins.end();
 it ++)
 {
 TString sfolder = this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

 sfolder.ReplaceAll( "XXPT_BINXX", (*it).id() );

 // cd to root folder
 this->GetPipelineSettings()->GetRootOutFile()->cd(
 TString( this->GetPipelineSettings()->GetRootOutFile()->GetName()) + ":" );

 TString sName = RootNamer::GetHistoName(
 this->GetPipelineSettings()->GetAlgoName(), "partonflavour",
 this->GetPipelineSettings()->GetInputType(), 0, &(*it), false) + "_hist";

 RootFileHelper::SafeGet<TH1D *>( this->GetPipelineSettings()->GetRootOutFile(),
 sfolder + "/" + sName );
 TH1D * hflavourflav = (TH1D * )this->GetPipelineSettings()->GetRootOutFile()->Get(sfolder + "/" + sName );

 sName = RootNamer::GetHistoName(
 this->GetPipelineSettings()->GetAlgoName(), "jet1_pt",
 this->GetPipelineSettings()->GetInputType(), 0, &(*it), false) + "_hist";
 TH1D * hpt   = (TH1D * )this->GetPipelineSettings()->GetRootOutFile()->Get(sfolder + "/" + sName  );

 if (hpt == NULL)
 {
 CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
 }

 double fR = 1.0 / hresp->GetMean();
 double fRError = hresp->GetMeanError() / (TMath::Power(hresp->GetMean(), 2.0));

 m_graph->AddPoint( hpt->GetMean(),
 fR,
 hpt->GetMeanError(),
 fRError);

 m_histResp.GetRawHisto()->SetBinContent(i +1, fR );
 m_histResp.GetRawHisto()->SetBinError(i +1, fRError );
 i++;
 }

 m_histResp.Store(this->GetPipelineSettings()->GetRootOutFile());

 }
 std::string m_sInpHist;
 std::string m_sFolder;
 };
 */
class DrawJetCorrGraph: public ZJetGraphErrors
{
public:
	DrawJetCorrGraph(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}

	virtual void Process()
	{
		Hist1D m_histResp;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histResp.m_sCaption = m_histResp.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histResp.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histResp.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histResp.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "jet1_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			double fR = 1.0 / hresp->GetMean();
			double fRError = hresp->GetMeanError() / (TMath::Power(
					hresp->GetMean(), 2.0));

			m_graph->AddPoint(hpt->GetMean(), fR, hpt->GetMeanError(), fRError);

			m_histResp.GetRawHisto()->SetBinContent(i + 1, fR);
			m_histResp.GetRawHisto()->SetBinError(i + 1, fRError);
			i++;
		}

		m_histResp.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;
};

// DP: New plot classes for ChargedHadronEnergy, Neutral and Photon fraction

class DrawJetChargedHadronEnergy: public ZJetGraphErrors
{
public:
	DrawJetChargedHadronEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}

	virtual void Process()
	{
		Hist1D m_histChargedHadrFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histChargedHadrFrac.m_sCaption = m_histChargedHadrFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histChargedHadrFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histChargedHadrFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histChargedHadrFrac.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histChargedHadrFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histChargedHadrFrac.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetNeutralHadronEnergy: public ZJetGraphErrors
{
public:
	DrawJetNeutralHadronEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histNeutralHadrFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histNeutralHadrFrac.m_sCaption = m_histNeutralHadrFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histNeutralHadrFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histNeutralHadrFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histNeutralHadrFrac.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histNeutralHadrFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histNeutralHadrFrac.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetChargedEmEnergy: public ZJetGraphErrors
{
public:
	DrawJetChargedEmEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}

	virtual void Process()
	{
		Hist1D m_histChargedEmFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histChargedEmFrac.m_sCaption = m_histChargedEmFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedEmFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histChargedEmFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histChargedEmFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histChargedEmFrac.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histChargedEmFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histChargedEmFrac.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetNeutralEmEnergy: public ZJetGraphErrors
{
public:
	DrawJetNeutralEmEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histNeutralEmFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histNeutralEmFrac.m_sCaption = m_histNeutralEmFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histNeutralEmFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histNeutralEmFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histNeutralEmFrac.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histNeutralEmFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histNeutralEmFrac.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetElectronEnergy: public ZJetGraphErrors
{
public:
	DrawJetElectronEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histElectronFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histElectronFrac.m_sCaption = m_histElectronFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histElectronFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histElectronFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histElectronFrac.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histElectronFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histElectronFrac.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetMuonEnergy: public ZJetGraphErrors
{
public:
	DrawJetMuonEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histMuonFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histMuonFrac.m_sCaption = m_histMuonFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histMuonFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histMuonFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histMuonFrac.GetRawHisto()->SetBinContent(i + 1, hresp->GetMean());
			m_histMuonFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histMuonFrac.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetPhotonEnergy: public ZJetGraphErrors
{
public:
	DrawJetPhotonEnergy(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histPhotonFrac;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histPhotonFrac.m_sCaption = m_histPhotonFrac.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histPhotonFrac.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histPhotonFrac.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histPhotonFrac.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histPhotonFrac.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histPhotonFrac.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawConstituents: public ZJetGraphErrors
{
public:
	DrawConstituents(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histConstituents;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histConstituents.m_sCaption = m_histConstituents.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histConstituents.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histConstituents.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histConstituents.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histConstituents.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histConstituents.Store(this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetChargedHadronMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetChargedHadronMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histChargedHadronMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histChargedHadronMultiplicity.m_sCaption
				= m_histChargedHadronMultiplicity.m_sName
						= this->GetPipelineSettings()->GetRootFileFolder()
								+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histChargedHadronMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histChargedHadronMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histChargedHadronMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histChargedHadronMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histChargedHadronMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetNeutralHadronMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetNeutralHadronMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histNeutralHadronMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histNeutralHadronMultiplicity.m_sCaption
				= m_histNeutralHadronMultiplicity.m_sName
						= this->GetPipelineSettings()->GetRootFileFolder()
								+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histNeutralHadronMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histNeutralHadronMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histNeutralHadronMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histNeutralHadronMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histNeutralHadronMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetChargedMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetChargedMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histChargedMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histChargedMultiplicity.m_sCaption
				= m_histChargedMultiplicity.m_sName
						= this->GetPipelineSettings()->GetRootFileFolder()
								+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histChargedMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histChargedMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histChargedMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histChargedMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histChargedMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetNeutralMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetNeutralMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histNeutralMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histNeutralMultiplicity.m_sCaption
				= m_histNeutralMultiplicity.m_sName
						= this->GetPipelineSettings()->GetRootFileFolder()
								+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histNeutralMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histNeutralMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histNeutralMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histNeutralMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histNeutralMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetElectronMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetElectronMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histElectronMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histElectronMultiplicity.m_sCaption
				= m_histElectronMultiplicity.m_sName
						= this->GetPipelineSettings()->GetRootFileFolder()
								+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histElectronMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histElectronMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histElectronMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histElectronMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histElectronMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetMuonMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetMuonMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histMuonMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histMuonMultiplicity.m_sCaption = m_histMuonMultiplicity.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histMuonMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histMuonMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histMuonMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histMuonMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histMuonMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawJetPhotonMultiplicity: public ZJetGraphErrors
{
public:
	DrawJetPhotonMultiplicity(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histPhotonMultiplicity;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histPhotonMultiplicity.m_sCaption = m_histPhotonMultiplicity.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histPhotonMultiplicity.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histPhotonMultiplicity.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hconst =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hconst == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hconst->GetMean(),
					hpt->GetMeanError(), hconst->GetMeanError());

			m_histPhotonMultiplicity.GetRawHisto()->SetBinContent(i + 1,
					hconst->GetMean());
			m_histPhotonMultiplicity.GetRawHisto()->SetBinError(i + 1,
					hconst->GetMeanError());
			i++;
		}

		m_histPhotonMultiplicity.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawMatchAvgCaloJetPtRatio: public ZJetGraphErrors
{
public:
	DrawMatchAvgCaloJetPtRatio(std::string sInpHist) :
		ZJetGraphErrors(), m_sInpHist(sInpHist)
	{

	}
	virtual void Process()
	{
		Hist1D m_histCaloPFAvgRatio;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histCaloPFAvgRatio.m_sCaption = m_histCaloPFAvgRatio.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histCaloPFAvgRatio.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histCaloPFAvgRatio.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresp =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hresp == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "jet1_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			m_graph->AddPoint(hpt->GetMean(), hresp->GetMean(),
					hpt->GetMeanError(), hresp->GetMeanError());

			m_histCaloPFAvgRatio.GetRawHisto()->SetBinContent(i + 1,
					hresp->GetMean());
			m_histCaloPFAvgRatio.GetRawHisto()->SetBinError(i + 1,
					hresp->GetMeanError());
			i++;
		}

		m_histCaloPFAvgRatio.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist;
	std::string m_sFolder;

};

class DrawMatchAvgAvgCaloJetPtRatio: public ZJetGraphErrors
{
public:
	DrawMatchAvgAvgCaloJetPtRatio(std::string sInpHist1, std::string sInpHist2) :
		ZJetGraphErrors(), m_sInpHist1(sInpHist1), m_sInpHist2(sInpHist2)
	{

	}

	virtual void Process()
	{
		Hist1D m_histCaloPFAvgAvgRatio;

		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector<PtBin> bins = this->GetPipelineSettings()->GetAsPtBins(sv);

		m_histCaloPFAvgAvgRatio.m_sCaption = m_histCaloPFAvgAvgRatio.m_sName
				= this->GetPipelineSettings()->GetRootFileFolder()
						+ this->GetProductName();
		//m_histChargedHadrFrac.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histCaloPFAvgAvgRatio.AddModifier(new ModHistCustomBinnig(
				this->GetPipelineSettings()->GetCustomBins()));
		m_histCaloPFAvgAvgRatio.Init();

		int i = 0;
		for (std::vector<PtBin>::iterator it = bins.begin(); it != bins.end(); it++)
		{
			TString sfolder =
					this->GetPipelineSettings()->GetSecondLevelFolderTemplate();

			sfolder.ReplaceAll("XXPT_BINXX", (*it).id());

			// cd to root folder
			this->GetPipelineSettings()->GetRootOutFile()->cd(TString(
					this->GetPipelineSettings()->GetRootOutFile()->GetName())
					+ ":");

			TString sNamecalo = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist1.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hrespcalo =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sNamecalo);

			TString sNamepf = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(),
					m_sInpHist2.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hresppf =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sNamepf);

			if (hrespcalo == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sNamecalo + " from folder " + sfolder.Data())
			}

			if (hresppf == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sNamepf + " from folder " + sfolder.Data())
			}

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it),
					false) + "_hist";
			TH1D
					* hpt =
							(TH1D *) this->GetPipelineSettings()->GetRootOutFile()->Get(
									sfolder + "/" + sName);

			if (hpt == NULL)
			{
				CALIB_LOG_FATAL( "Can't load TH1D " + sName + " from folder " + sfolder.Data())
			}

			double ratio = hrespcalo->GetMean() / hresppf->GetMean();
			double part1 = hrespcalo->GetMeanError() / hrespcalo->GetMean();
			double part2 = hresppf->GetMeanError() / hresppf->GetMean();
			double ratioerr = ratio * sqrt(part1 * part1 + part2 * part2);

			m_graph->AddPoint(hpt->GetMean(), ratio, hpt->GetMeanError(),
					ratioerr);

			m_histCaloPFAvgAvgRatio.GetRawHisto()->SetBinContent(i + 1, ratio);
			m_histCaloPFAvgAvgRatio.GetRawHisto()->SetBinError(i + 1, ratioerr);
			i++;
		}

		m_histCaloPFAvgAvgRatio.Store(
				this->GetPipelineSettings()->GetRootOutFile());

	}
	std::string m_sInpHist1;
	std::string m_sInpHist2;
	std::string m_sFolder;

};

}

