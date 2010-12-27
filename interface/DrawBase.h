#ifndef __DRAWBASE_H__
#define __DRAWBASE_H__

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


template < class THistType >
class GraphBase: public PlotBase<THistType>
{
};


class GraphErrors: public PlotBase< GraphErrors>
{
public:

	class DataPoint
	{
	public:
		DataPoint ( double x, double y, double xe, double ye)
			: m_fx( x), m_fy(y), m_fxe(xe), m_fye( ye)
		{

		}

		double m_fx, m_fy, m_fxe, m_fye;
	};

	GraphErrors() :
		m_sName("default_graph_name"), m_sCaption("default_graph_caption")
	{
	}

	void Init()
	{
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierBeforeCreation( this );
		m_graph = new TGraphErrors( m_points.size());
		m_graph->SetName( m_sName.c_str() );
		//m_graph->SetCaption( m_sCaption.c_str() );

		this->RunModifierBeforeDataEntry( this );

		unsigned long l = 0;
		for ( boost::ptr_vector<DataPoint>::iterator it = m_points.begin();
			!( it == m_points.end()); it++)
		{
			m_graph->SetPoint(l,  it->m_fx, it->m_fy);
			m_graph->SetPointError(l,  it->m_fxe, it->m_fye);

			l++;
		}

		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		CALIB_LOG( "Storing GraphErrors " + this->m_sRootFileFolder + "/" + this->m_sName + "_graph" )

		if ( pRootFile->cd( this->m_sRootFileFolder.c_str() ) == false)
		{
			pRootFile->mkdir( this->m_sRootFileFolder.c_str() );
			pRootFile->cd( this->m_sRootFileFolder.c_str() );
		}
		m_graph->Write((this->m_sName + "_graph").c_str());
	}

	void AddPoint( double x, double y, double xe, double ye )
	{
		m_points.push_back( new DataPoint( x, y, xe, ye ) );
	}

	std::string m_sName;
	std::string m_sCaption;
	std::string m_sRootFileFolder;
	int m_iBinCount;
	double m_dBinLower;
	double m_dBinUpper;

	boost::ptr_vector<DataPoint> m_points;

	TGraphErrors * m_graph;
};

class Hist1D: public HistBase< Hist1D>
{
public:

	Hist1D() :
		m_sName("default_hist_name"), m_sCaption("default_hist_caption"),
				m_iBinCount(100), m_dBinLower(0.0f), m_dBinUpper(200.0f), m_bUseCustomBin(false)
	{

	}

	void Init()
	{
		this->RunModifierBeforeCreation( this );

		if ( m_bUseCustomBin )
		{
			m_hist = new TH1D(    this->m_sName.c_str(),
				              this->m_sCaption.c_str(),
				              m_iBinCount, &m_dCustomBins[0] );
		}
		else
		{
		m_hist = new TH1D(this->m_sName.c_str(), this->m_sCaption.c_str(),
				this->m_iBinCount, this->m_dBinLower, this->m_dBinUpper);
		}
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
	  double m_dCustomBins[255];


	 bool m_bUseCustomBin;

	TH1D * m_hist;
};

class EventDump: public EventConsumerBase<EventResult>
{
public:
	virtual void Init(EventPipeline * pset)
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
class DrawGraphErrorsConsumerBase: public DrawConsumerBase<TData>
{
public:
	DrawGraphErrorsConsumerBase() :
		m_graph(NULL)
	{
	}

	virtual void Init(EventPipeline * pset)
	{
		DrawConsumerBase<TData>::Init(pset);
		CALIB_LOG( "Initializing GraphErrors for " << this->GetProductName() )

		m_graph->m_sName = m_graph->m_sCaption = this->GetProductName();
		m_graph->m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_graph->Init();
	}

	virtual void Finish()
	{
		// store hist
		// + modifiers
		//CALIB_LOG( "Z mass mean " << m_hist->m_hist->GetMean() )
		m_graph->Store(this->GetPipelineSettings()->GetRootOutFile());
	}
	// already configured histogramm
	GraphErrors * m_graph;
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

class ModHistCustomBinnig : public ModifierBase<Hist1D>
{
public:
	ModHistCustomBinnig( )
	{

	}

   virtual void BeforeCreation(Hist1D * pElem)
   {
	   pElem->m_iBinCount = m_iBinCount;

	      for ( int i = 0; i <= m_iBinCount; i++ )
	      {
	    	  pElem->m_dCustomBins[i] = m_dBins[i];
	      }

	      pElem->m_bUseCustomBin = true;
   }

    int m_iBinCount;
    double m_dBins[255];
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


class DrawEventCount: public DrawHist1dConsumerBase<EventResult>
{ public:
virtual void Init(EventPipeline * pset) {
	ModHistCustomBinnig * cbMod = new ModHistCustomBinnig;

	stringvector sVec = pset->GetSettings()->GetCustomBins();
	std::vector<PtBin> custBins = pset->GetSettings()->GetAsPtBins(  sVec );

    cbMod->m_iBinCount = custBins.size();
    cbMod->m_dBins[0] = custBins[0].GetMin();

	  int i = 1;
	  BOOST_FOREACH( PtBin & bin, custBins )
	  {
		cbMod->m_dBins[i] = bin.GetMax();
		i++;
      }

	m_hist->AddModifier( cbMod );

	DrawHist1dConsumerBase<EventResult>::Init(pset);
}
virtual void ProcessFilteredEvent(EventResult & res)
{
	m_hist->Fill( res.m_pData->Z->Pt(), res.m_weight );
}
};


class GraphXProviderBase
{
public:
	virtual double GetXValue(EventResult & event) = 0;

};

template <class TBinInfo>
class BinnedCounterBase
{
public:
	template <class TInf>
	class Bin
	{
	public:
		PtBin m_bin;
		TInf m_binInfo;
	};

	BinnedCounterBase( double start, double stepSize, int binCount)
	{
		for (unsigned int iBin = 0; iBin < binCount; iBin ++ )
		{
		     Bin<TBinInfo> * pBin = new Bin<TBinInfo>();
		     pBin->m_bin = PtBin( (double) iBin * stepSize + start,
		                            ((double) (iBin + 1)) * stepSize + start);
		     m_binList.push_back( pBin );
		}
    }

	TBinInfo * GetBinInfo( double val)
	{

		for (int i = 0;	i < m_binList.size(); i++)
		{
			if ( m_binList[i].m_bin.IsInBin( val ))
			{
				return &m_binList[i].m_binInfo;
			}
		}
		return NULL;
	}


	boost::ptr_vector<Bin<TBinInfo > > m_binList;

};

class CutEffBinInfo
{
public:
	CutEffBinInfo() : m_rejected( 0 ), m_accepted(0)
	{

	}

	unsigned long m_rejected;
	unsigned long m_accepted;
};


class CutEffBinnedCounter: public BinnedCounterBase< CutEffBinInfo>
{
public:
	CutEffBinnedCounter( double start, double stepSize, int binCount):
		BinnedCounterBase< CutEffBinInfo>( start, stepSize, binCount)
	{

	}
};

class GraphXProviderZpt : public GraphXProviderBase
{
public:
	virtual double GetLow() { return 0.0f; }
	virtual double GetHigh() { return 500.0f; }
	virtual int GetBinCount() { return 50; }
	virtual double GetXValue(EventResult & event) { return event.m_pData->Z->Pt();}
};

class GraphXProviderRecoVert : public GraphXProviderBase
{
public:
	virtual double GetLow() { return -0.5f; }
	virtual double GetHigh() { return 14.5f; }
	virtual int GetBinCount() { return 15; }
	virtual double GetXValue(EventResult & event) { return event.GetRecoVerticesCount();}
};

template <class TXProvider>
class DrawCutEffGraph: public DrawGraphErrorsConsumerBase<EventResult>
{
public:
	DrawCutEffGraph( int cutId ) : 	DrawGraphErrorsConsumerBase<EventResult>(),
		m_iCutId( cutId)
	{
		m_binCounter.reset( new CutEffBinnedCounter( m_xProvider.GetLow(),
				(  m_xProvider.GetHigh() -  m_xProvider.GetLow()) / (double) m_xProvider.GetBinCount(),
				m_xProvider.GetBinCount()));
	}

	// this method is called for all events
	virtual void ProcessEvent(EventResult & event, FilterResult & result)
	{
		if ( !g_cutHandler.IsValidEvent( &event))
			return;

		CutEffBinInfo * pInfo = m_binCounter->GetBinInfo(	m_xProvider.GetXValue( event ));

		// is null is returned, out of our range, but is fine
		if ( pInfo != NULL)
		{
			if ( g_cutHandler.IsCutInBitmask( m_iCutId, event.m_cutBitmask ))
				pInfo->m_rejected++;
			else
				pInfo->m_accepted++;
		}
	}

	virtual void Finish()
	{
		// plot the efficiency
		for (int i = 0;	i < m_binCounter->m_binList.size(); i++)
		{
			unsigned long overall = m_binCounter->m_binList[i].m_binInfo.m_rejected  + m_binCounter->m_binList[i].m_binInfo.m_accepted;

			if ( overall > 0)
			{
			m_graph->AddPoint( m_binCounter->m_binList[i].m_bin.GetBinCenter(),
					 (double)m_binCounter->m_binList[i].m_binInfo.m_rejected /
					 (double)overall,
					 0.0f, 0.0f);
			}
			 else
			 {
					m_graph->AddPoint( m_binCounter->m_binList[i].m_bin.GetBinCenter(),
							 1.0f,
							 0.0f, 0.0f);
			 }

		}

		// store hist
		// + modifiers
		//CALIB_LOG( "Z mass mean " << m_hist->m_hist->GetMean() )
		DrawGraphErrorsConsumerBase<EventResult>::Finish();
	}

	int m_iCutId;
	TXProvider m_xProvider;
	std::auto_ptr<CutEffBinnedCounter> m_binCounter;
};

class DrawJetRespGraph: public DrawGraphErrorsConsumerBase<EventResult>
{
public:
	DrawJetRespGraph( std::string sInpHist ) : 	DrawGraphErrorsConsumerBase<EventResult>(),
		m_sInpHist( sInpHist)
	{

	}

	virtual void Process()
	{
		// move throug the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector< PtBin > bins = this->GetPipelineSettings()->GetAsPtBins( sv );

		int i = 0;
		for (std::vector< PtBin >::iterator it = bins.begin();
				it != bins.end();
				it ++)
		{
			this->GetPipelineSettings()->GetRootOutFile()->cd( "" );

			TString sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), m_sInpHist.c_str(),
					this->GetPipelineSettings()->GetInputType(), 0, &(*it), false);
			TH1D * hresp = (TH1D * )this->GetPipelineSettings()->GetRootOutFile()->Get( sName );

			sName = RootNamer::GetHistoName(
					this->GetPipelineSettings()->GetAlgoName(), "z_pt",
					this->GetPipelineSettings()->GetInputType(), 0, &(*it), false);
			TH1D * hpt   = (TH1D * )this->GetPipelineSettings()->GetRootOutFile()->Get( sName );

			m_graph->AddPoint( hpt->GetMean(), hresp->GetMean(), hpt->GetMeanError(), hresp->GetMeanError());
			i++;
		}

	}
	std::string m_sInpHist;
};

}
#endif

