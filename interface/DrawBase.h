#ifndef __DRAWBASE_H__
#define __DRAWBASE_H__

#include <string>
#include <iostream>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"
#include "EventPipeline.h"

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

#define IMPL_HIST1D_MOD1(CLASSNAME, DATAPATH, MOD1)	\
class CLASSNAME: public DrawHist1dConsumerBase<EventResult>	{ public: \
virtual void Init(EventPipeline * pset) { \
	m_hist->AddModifier( MOD1 );	\
	DrawHist1dConsumerBase<EventResult>::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \


#define IMPL_HIST1D_MOD2(CLASSNAME, DATAPATH, MOD1,  MOD2)	\
class CLASSNAME: public DrawHist1dConsumerBase<EventResult>	{ public: \
virtual void Init(EventPipeline * pset) { \
	m_hist->AddModifier( MOD1 ); m_hist->AddModifier( MOD2 );	\
	DrawHist1dConsumerBase<EventResult>::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \

#define IMPL_HIST1D_JET_MOD1(CLASSNAME, DATAPATH, MOD1)	\
class CLASSNAME: public DrawJetConsumerBase	{ public: \
CLASSNAME ( int jetNum ) : DrawJetConsumerBase( jetNum) {} \
virtual void Init(EventPipeline * pset) { \
	m_hist->AddModifier( MOD1 );	\
	DrawJetConsumerBase::Init(pset); \
}	\
virtual void ProcessFilteredEvent(EventResult & res) { \
DATAPATH  }}; \



template < class TDrawElement >
class ModifierBase
{
public:
	virtual void BeforeCreation(TDrawElement * pElem)
	{
	}
	virtual void BeforeDataEntry(TDrawElement * pElem)
	{
	}
	virtual void AfterDataEntry(TDrawElement * pElem)
	{
	}
	virtual void AfterDraw(	TDrawElement * pElem)
	{
	}
};


template < class TPlotType >
class PlotBase
{
public:
	void AddModifier(ModifierBase<TPlotType> * mod)
	{
		m_modifiers.push_back( mod );
	}

	void RunModifierBeforeCreation( TPlotType * pElem )
	{
		BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers ) { p->BeforeCreation(pElem);}
	}
	void RunModifierBeforeDataEntry( TPlotType * pElem )
	{
		BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers ) { p->BeforeDataEntry(pElem);}
	}
	void RunModifierAfterDataEntry( TPlotType * pElem )
	{
		BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers ) { p->AfterDataEntry(pElem);}
	}
	void RunModifierAfterDraw( TPlotType * pElem )
	{
		BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers ) { p->AfterDraw(pElem);}
	}

	std::vector<ModifierBase<TPlotType> *> m_modifiers;
};

class PlotSettings
{
	boost::shared_ptr<TFile> m_pOutputFile;
};

template < class THistType >
class HistBase: public PlotBase<THistType>
{
};

class Hist1D: public HistBase< Hist1D>
{
public:

	Hist1D() :
		m_sName("default_hist_name"), m_sCaption("default_hist_caption"),
				m_iBinCount(100), m_dBinLower(0.0f), m_dBinUpper(200.0f)
	{

	}

	void Init()
	{
		this->RunModifierBeforeCreation( this );
		m_hist.reset(new TH1D(this->m_sName.c_str(), this->m_sCaption.c_str(),
				this->m_iBinCount, this->m_dBinLower, this->m_dBinUpper));
		this->RunModifierBeforeDataEntry( this );
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		CALIB_LOG( "Storing Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )

		if ( pRootFile->cd( this->m_sRootFileFolder.c_str() ) == false)
		{
			pRootFile->mkdir( this->m_sRootFileFolder.c_str() );
			pRootFile->cd( this->m_sRootFileFolder.c_str() );
		}
		m_hist->Write((this->m_sName + "_hist").c_str());
	}

	void Fill(double val, double weight)
	{
		m_hist->Fill(val, weight);
	}

	std::string m_sName;
	std::string m_sCaption;
	std::string m_sRootFileFolder;
	int m_iBinCount;
	double m_dBinLower;
	double m_dBinUpper;

	boost::scoped_ptr<TH1D> m_hist;
};

class EventDump: public EventConsumerBase<EventResult>
{
public:
	virtual void Init()
	{
		CALIB_LOG("EventDump::Init called")
	}

	virtual void Finish()
	{
		CALIB_LOG("EventDump::Finish called")
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(EventResult & event)
	{
		CALIB_LOG("Filtered EventNum " << event.m_pData->cmsEventNum)
	}

	// this method is called for all events
	virtual void ProcessEvent(EventResult & event, FilterResult & result)
	{
		CALIB_LOG("Unfiltered EventNum " << event.m_pData->cmsEventNum)
	}
};

template<class TData>
class DrawConsumerBase: public EventConsumerBase<TData>
{
public:
	DrawConsumerBase()
	{

	}

	// generates a name for the created product
	std::string GetProductName()
	{
		// check if there is a PtBin Filter which will affect our Product Name
		PtWindowFilter * pwin =
				dynamic_cast<PtWindowFilter *> (this->m_pipeline->FindFilter(
						PtWindowFilter().GetFilterId()));
		PtBin * ptBin = NULL;

		InCutFilter * pcut =
				dynamic_cast<InCutFilter *> (this->m_pipeline->FindFilter(
						InCutFilter().GetFilterId()));
		bool isNoCut = (pcut == NULL);

		if (pwin != NULL)
		{
			ptBin = new PtBin(this->GetPipelineSettings()->GetFilterPtBinLow(),
					this->GetPipelineSettings()->GetFilterPtBinHigh());

		}

		TString sName = RootNamer::GetHistoName(
				this->GetPipelineSettings()->GetAlgoName(), m_sQuantityName,
				this->GetPipelineSettings()->GetInputType(), 0, ptBin, isNoCut);
		return sName.Data();
	}

	std::string m_sQuantityName;
};

template<class TData>
class DrawHist1dConsumerBase: public DrawConsumerBase<TData>
{
public:
	DrawHist1dConsumerBase() :
		m_hist(NULL)
	{
	}

	virtual void Init(EventPipeline * pset)
	{
		DrawConsumerBase<TData>::Init(pset);
		CALIB_LOG( "Initializing Hist for " << this->GetProductName() )

		/* m_hist->m_sName = "nname"; //this->GetProductName();
		 m_hist->m_sCaption = "ccapt" ;//this->GetProductName(); */
		m_hist->m_sName = m_hist->m_sCaption = this->GetProductName();
		m_hist->m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_hist->Init();
	}

	virtual void Finish()
	{
		// store hist
		// + modifiers
		//CALIB_LOG( "Z mass mean " << m_hist->m_hist->GetMean() )
		m_hist->Store(this->GetPipelineSettings()->GetRootOutFile());
	}
	// already configured histogramm
	Hist1D * m_hist;
};

// MODS

class ModHistBinRange : public ModifierBase<Hist1D>
{
public:
	ModHistBinRange( double lower, double upper) : m_dBinLower(lower),m_dBinUpper(upper)
	{
	}

	virtual void BeforeCreation(Hist1D * pElem)
	{
		pElem->m_dBinLower = this->m_dBinLower;
		pElem->m_dBinUpper = this->m_dBinUpper;
	}

private:
	double m_dBinLower;
	double m_dBinUpper;
};

class ModHistBinCount : public ModifierBase<Hist1D>
{
public:
	ModHistBinCount( unsigned int count) : m_iBinCount(count)
	{
	}

	virtual void BeforeCreation(Hist1D * pElem)
	{
		pElem->m_iBinCount = m_iBinCount;
	}

private:
	unsigned int m_iBinCount;

};

class DrawJetConsumerBase: public DrawHist1dConsumerBase<EventResult>
{
public:
	DrawJetConsumerBase( int jetNum ) : DrawHist1dConsumerBase<EventResult>(),
		m_jetNum ( jetNum )
	{
	}

	int m_jetNum;
};


// Z STUFF
IMPL_HIST1D_MOD1(DrawZMassConsumer ,m_hist->Fill(res.m_pData->Z->GetCalcMass() , res.m_weight); ,
		new ModHistBinRange(0.0f, 200.0f))
IMPL_HIST1D_MOD1(DrawZPtConsumer ,m_hist->Fill(res.m_pData->Z->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 200.0f))

IMPL_HIST1D_MOD1(DrawZEtaConsumer ,m_hist->Fill(res.m_pData->Z->Phi() , res.m_weight); ,
		new ModHistBinRange(-3.0f, 3.0f))

IMPL_HIST1D_MOD1(DrawZPhiConsumer ,m_hist->Fill(res.m_pData->Z->Eta() , res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))

// mus pt
IMPL_HIST1D_MOD1(DrawMuPlusPtConsumer ,m_hist->Fill(res.m_pData->mu_plus->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 200.0f))
IMPL_HIST1D_MOD1(DrawMuMinusPtConsumer ,m_hist->Fill(res.m_pData->mu_minus->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 200.0f))

// mus Eta
IMPL_HIST1D_MOD1(DrawMuPlusEtaConsumer ,m_hist->Fill(res.m_pData->mu_plus->Eta() , res.m_weight); ,
		new ModHistBinRange(-3.0f, 3.0f))
IMPL_HIST1D_MOD1(DrawMuMinusEtaConsumer ,m_hist->Fill(res.m_pData->mu_minus->Eta() , res.m_weight); ,
		new ModHistBinRange(-3.0f, 3.0f))

// mus phi
IMPL_HIST1D_MOD1(DrawMuPlusPhiConsumer ,m_hist->Fill(res.m_pData->mu_plus->Phi() , res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))
IMPL_HIST1D_MOD1(DrawMuMinusPhiConsumer ,m_hist->Fill(res.m_pData->mu_minus->Phi() , res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))

// Both mus
IMPL_HIST1D_MOD1(DrawMuAllPtConsumer ,m_hist->Fill(res.m_pData->mu_plus->Pt() , res.m_weight); m_hist->Fill(res.m_pData->mu_minus->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 200.0f))
IMPL_HIST1D_MOD1(DrawMuAllEtaConsumer ,m_hist->Fill(res.m_pData->mu_plus->Eta() , res.m_weight); m_hist->Fill(res.m_pData->mu_minus->Eta() , res.m_weight); ,
		new ModHistBinRange(-3.0f, 3.0f))
IMPL_HIST1D_MOD1(DrawMuAllPhiConsumer ,m_hist->Fill(res.m_pData->mu_plus->Phi() , res.m_weight); m_hist->Fill(res.m_pData->mu_minus->Phi() , res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))


// MET
IMPL_HIST1D_MOD1(DrawMetConsumer ,m_hist->Fill(res.m_pData->met->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 300.0f))
IMPL_HIST1D_MOD1(DrawTcMetConsumer ,m_hist->Fill(res.m_pData->tcmet->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 300.0f))

//RECO VERT
IMPL_HIST1D_MOD2(DrawRecoVertConsumer ,m_hist->Fill( (double)res.GetRecoVerticesCount() , res.m_weight); ,
		new ModHistBinRange(-0.5, 14.5),
		new ModHistBinCount(15))

//JET RESP
IMPL_HIST1D_MOD1(DrawJetRespConsumer ,m_hist->Fill( res.GetCorrectedJetPt(0) / res.m_pData->Z->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0, 2.0))
// MPF RESP
IMPL_HIST1D_MOD1(DrawMpfJetRespConsumer ,
		{
			double scalPtEt = res.m_pData->Z->Px()*res.m_pData->met->Px() +
								res.m_pData->Z->Py()*res.m_pData->met->Py();
			double scalPtSq = res.m_pData->Z->Px()*res.m_pData->Z->Px() +
								res.m_pData->Z->Py()*res.m_pData->Z->Py();

			m_hist->Fill( 1.0 + (scalPtEt /scalPtSq), res.m_weight);
		},
		new ModHistBinRange(0.0, 2.0))

// Jets
IMPL_HIST1D_JET_MOD1(DrawJetPtConsumer ,
		{
				if ( res.IsJetValid( m_jetNum ))
				{
					m_hist->Fill(res.GetCorrectedJetPt( m_jetNum ) , res.m_weight);
				}
		},
		new ModHistBinRange(0.0f, 200.0f) )

IMPL_HIST1D_JET_MOD1(DrawJetEtaConsumer ,
		{
				if ( res.IsJetValid( m_jetNum ))
				{
					m_hist->Fill(res.m_pData->jets[m_jetNum]->Eta() , res.m_weight);
				}
		},
		new ModHistBinRange(-3.0f, 3.0f) )

IMPL_HIST1D_JET_MOD1(DrawJetPhiConsumer ,
		{
				if ( res.IsJetValid( m_jetNum ))
				{
					m_hist->Fill(res.m_pData->jets[m_jetNum]->Phi() , res.m_weight);
				}
		},
		new ModHistBinRange(-3.5f, 3.5f) )


class EventSelectionBase
{
public:
	// default implementation, no selection performed
	virtual bool IsEventInSelecction(EventId id)
	{
		return true;
	}
};

class CutSelection: public EventSelectionBase
{
public:
	CutSelection(EventSet * eventsInCut)
	{
		this->m_pEventsInCut = eventsInCut;
	}

	virtual inline bool IsEventInSelecction(EventId id)
	{
		return (m_pEventsInCut->find(id) != m_pEventsInCut->end());
	}

private:
	EventSet * m_pEventsInCut;

};

template<class TData>
class CHistDataDrawBase
{
public:
	virtual void Draw(TH1D * pHist, TData data) = 0;
};

template<class THistType>
class CHistEvtMapTemplate: public CHistDataDrawBase<EventResult>
{
public:
	CHistEvtMapTemplate()
	{
		m_bOnlyEventsInCut = true;
		m_bUsePtCut = true;
		m_binWith = ZPtBinning;
	}

	enum BinWithEnum
	{
		ZPtBinning, Jet1PtBinning
	};

	virtual void HistFill(THistType * pHist, double fillValue,
			EventResult & Res)
	{
		pHist->Fill(fillValue, Res.m_weight);
	}

	BinWithEnum m_binWith;

	// default behavior, might get more complex later on
	bool IsInSelection(EventResult & evRes)
	{
		bool bPass = true;
		// no section here is allowed to set to true again, just to false ! avoids coding errors
		if (this->m_bOnlyEventsInCut)
		{
			if (!evRes.IsInCut())
				bPass = false;
		}
		else
		{
			// all events which are valid in jSON file
			// does this work for mc ??
			if (!evRes.IsValidEvent())
				bPass = false;
		}

		if (m_bUsePtCut)
		{
			double fBinVal;
			if (m_binWith == ZPtBinning)
				fBinVal = evRes.m_pData->Z->Pt();
			else
				fBinVal = evRes.GetCorrectedJetPt(0);

			if (!(fBinVal >= this->m_dLowPtCut))
				bPass = false;

			if (!(fBinVal < this->m_dHighPtCut))
				bPass = false;
		}

		return bPass;
	}

	bool m_bOnlyEventsInCut;
	bool m_bUsePtCut;
	double m_dLowPtCut;
	double m_dHighPtCut;
};

}
#endif

