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

		//CALIB_LOG( "Storing GraphErrors " + this->m_sRootFileFolder + "/" + this->m_sName + "_graph" )

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


class Hist2D: public HistBase< Hist2D>
{
public:

	Hist2D() :
		m_sName("default_hist_name"), m_sCaption("default_hist_caption"),
				m_iBinXCount(100), m_dBinXLower(0.0f), m_dBinXUpper(200.0f),
				m_iBinYCount(100), m_dBinYLower(0.0f), m_dBinYUpper(200.0f)
	{

	}

	void Init()
	{
		this->RunModifierBeforeCreation( this );

		m_hist = new TH2D(this->m_sName.c_str(), this->m_sCaption.c_str(),
				this->m_iBinXCount, this->m_dBinXLower, this->m_dBinXUpper,
				this->m_iBinYCount, this->m_dBinYLower, this->m_dBinYUpper);
		m_hist->Sumw2();

		this->RunModifierBeforeDataEntry( this );
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing 2d Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )

		if ( pRootFile->cd( this->m_sRootFileFolder.c_str() ) == false)
		{
			pRootFile->mkdir( this->m_sRootFileFolder.c_str() );
			pRootFile->cd( this->m_sRootFileFolder.c_str() );
		}
		m_hist->Write((this->m_sName + "_hist").c_str());
	}

	void Fill(double x, double y, double weight)
	{
		m_hist->Fill(x, y, weight);
	}

	TH2D * GetRawHisto(){ return m_hist; }

	std::string m_sName;
	std::string m_sCaption;
	std::string m_sRootFileFolder;
	int m_iBinXCount;
	double m_dBinXLower;
	double m_dBinXUpper;
	int m_iBinYCount;
	double m_dBinYLower;
	double m_dBinYUpper;

	TH2D * m_hist;
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
		m_hist->Sumw2();

		this->RunModifierBeforeDataEntry( this );
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )

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

	TH1D * GetRawHisto(){ return m_hist; }

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
		//CALIB_LOG( "Initializing GraphErrors for " << this->GetProductName() )

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
		//CALIB_LOG( "Initializing Hist for " << this->GetProductName() )

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

template<class TData>
class DrawHist2DConsumerBase: public DrawConsumerBase<TData>
{
public:
	DrawHist2DConsumerBase() :
		m_hist(NULL)
	{
	}

	virtual void Init(EventPipeline * pset)
	{
		DrawConsumerBase<TData>::Init(pset);
		//CALIB_LOG( "Initializing 2d Hist for " << this->GetProductName() )

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
	Hist2D * m_hist;
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

class ModHist2DBinRange : public ModifierBase<Hist2D>
{
public:
	ModHist2DBinRange( double lowerx, double upperx, double lowery, double uppery) :
		m_dBinXLower(lowerx),m_dBinXUpper(upperx),
		m_dBinYLower(lowery),m_dBinYUpper(uppery)
	{
	}

	virtual void BeforeCreation(Hist2D * pElem)
	{
		pElem->m_dBinXLower = this->m_dBinXLower;
		pElem->m_dBinXUpper = this->m_dBinXUpper;
		pElem->m_dBinYLower = this->m_dBinYLower;
		pElem->m_dBinYUpper = this->m_dBinYUpper;
	}

private:
	double m_dBinXLower;
	double m_dBinXUpper;
	double m_dBinYLower;
	double m_dBinYUpper;
};

class ModHistCustomBinnig : public ModifierBase<Hist1D>
{
public:
	ModHistCustomBinnig( )
	{

	}

	ModHistCustomBinnig ( stringvector customBins )
	{
		std::vector<PtBin> custBins = PipelineSettings::GetAsPtBins(  customBins );

		if ( custBins.size() == 0 )
			CALIB_LOG_FATAL("No bins specified for Plot !")

		this->m_iBinCount = custBins.size();
		this->m_dBins[0] = custBins[0].GetMin();

		  int i = 1;
		  BOOST_FOREACH( PtBin & bin, custBins )
		  {
			this->m_dBins[i] = bin.GetMax();
			i++;
		  }
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

class ModHist2DBinCount : public ModifierBase<Hist2D>
{
public:
	ModHist2DBinCount( unsigned int countx, unsigned int county) :
		m_iBinXCount(countx), m_iBinYCount(county)
	{
	}

	virtual void BeforeCreation(Hist2D * pElem)
	{
		pElem->m_iBinXCount = m_iBinXCount;
		pElem->m_iBinYCount = m_iBinYCount;
	}

private:
	unsigned int m_iBinXCount;
	unsigned int m_iBinYCount;
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

IMPL_HIST1D_MOD1(DrawZEtaConsumer ,m_hist->Fill(res.m_pData->Z->Phi() - TMath::Pi() , res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))

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
IMPL_HIST1D_MOD1(DrawMuPlusPhiConsumer ,m_hist->Fill(res.m_pData->mu_plus->Phi() - TMath::Pi() , res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))
IMPL_HIST1D_MOD1(DrawMuMinusPhiConsumer ,m_hist->Fill(res.m_pData->mu_minus->Phi()  - TMath::Pi(), res.m_weight); ,
		new ModHistBinRange(-3.5f, 3.5f))

// Both mus
IMPL_HIST1D_MOD1(DrawMuAllPtConsumer ,m_hist->Fill(res.m_pData->mu_plus->Pt() , res.m_weight); m_hist->Fill(res.m_pData->mu_minus->Pt() , res.m_weight); ,
		new ModHistBinRange(0.0f, 200.0f))
IMPL_HIST1D_MOD1(DrawMuAllEtaConsumer ,m_hist->Fill(res.m_pData->mu_plus->Eta() , res.m_weight); m_hist->Fill(res.m_pData->mu_minus->Eta() , res.m_weight); ,
		new ModHistBinRange(-3.0f, 3.0f))
IMPL_HIST1D_MOD1(DrawMuAllPhiConsumer ,m_hist->Fill(res.m_pData->mu_plus->Phi() - TMath::Pi(), res.m_weight); m_hist->Fill(res.m_pData->mu_minus->Phi() - TMath::Pi() , res.m_weight); ,
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
					m_hist->Fill(res.m_pData->jets[m_jetNum]->Phi() - TMath::Pi() , res.m_weight);
				}
		},
		new ModHistBinRange(-3.5f, 3.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaPhiConsumer ,
		{
				if ( res.IsJetValid( m_jetNum ))
				{
					m_hist->Fill( DeltaHelper::GetDeltaPhiCenterZero( res.m_pData->Z,
							res.m_pData->jets[m_jetNum]),
							res.m_weight);
				}
		},
		new ModHistBinRange(-3.5f, 3.5f) )

IMPL_HIST1D_JET_MOD1(DrawJetDeltaEtaConsumer ,
		{
				if ( res.IsJetValid( m_jetNum ))
				{
					m_hist->Fill( TMath::Abs(  res.m_pData->Z->Eta() -
							res.m_pData->jets[m_jetNum]->Eta()),
							res.m_weight);
				}
		},
		new ModHistBinRange(0.0f, 4.0f) )


class DrawEventCount: public DrawHist1dConsumerBase<EventResult>
{ public:
virtual void Init(EventPipeline * pset) {
	ModHistCustomBinnig * cbMod = new ModHistCustomBinnig( pset->GetSettings()->GetCustomBins());
	m_hist->AddModifier( cbMod );

	DrawHist1dConsumerBase<EventResult>::Init(pset);
}
virtual void ProcessFilteredEvent(EventResult & res)
{
	m_hist->Fill( res.m_pData->Z->Pt(), res.m_weight );
}
};

class DrawEtaPhiMapConsumer: public DrawHist2DConsumerBase<EventResult>
{
	public:
	virtual void Init(EventPipeline * pset) {

	m_hist->AddModifier( new ModHist2DBinRange(0.0f, 4.0f, -3.2, 3.2 ));
	m_hist->AddModifier( new ModHist2DBinCount(40, 40));

	DrawHist2DConsumerBase<EventResult>::Init(pset);
}/*
virtual void ProcessFilteredEvent(EventResult & res)
{
	m_hist->Fill( res.m_pData->Z->Pt(), res.m_weight );
}*/
};

class DrawEtaPhiJetMapConsumer: public DrawEtaPhiMapConsumer
{
	public:
	DrawEtaPhiJetMapConsumer( int jetNum) : m_jetNum(jetNum)
	{}

	virtual void ProcessFilteredEvent(EventResult & res)
	{
		m_hist->Fill(
				TMath::Abs(res.m_pData->Z->Eta()- res.m_pData->jets[m_jetNum]->Eta()),
				DeltaHelper::GetDeltaPhiCenterZero(res.m_pData->Z, res.m_pData->jets[m_jetNum]),
				res.m_weight );
	}

	int m_jetNum;
};

class GraphXProviderBase
{
public:
	virtual double GetXValue(EventResult & event) = 0;

};


class GraphXProviderZpt : public GraphXProviderBase
{
public:
	virtual double GetLow() { return 0.0f; }
	virtual double GetHigh() { return 300.0f; }
	virtual int GetBinCount() { return 45; }
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

template < int TJetNum >
class GraphXProviderJetPhiDeltaZ : public GraphXProviderBase
{
public:
	virtual double GetXValue(EventResult & event)
	{
		return  DeltaHelper::GetDeltaPhiCenterZero( event.m_pData->Z,
				event.m_pData->jets[TJetNum]);
	}
};

class DrawDeltaPhiRange: public DrawGraphErrorsConsumerBase<EventResult>
{
public:
	DrawDeltaPhiRange( ) : 	DrawGraphErrorsConsumerBase<EventResult>()
	{
	}
	virtual void Finish()
	{
		// plot the efficiency
		for (double i = 0;	i < 2 * TMath::Pi(); i+= 0.1 )
		{
			TVector3 v1 = TVector3(1.0,0.0,0.0);
			TVector3 v2 = TVector3( TMath::Cos(i) , TMath::Sin(i) ,0.0);

			m_graph->AddPoint(
					i,
					DeltaHelper::GetDeltaPhiCenterZero(v1, v2),
					 0.0f,
					 0.0f);
		}

		// store hist
		// + modifiers
		//CALIB_LOG( "Z mass mean " << m_hist->m_hist->GetMean() )
		DrawGraphErrorsConsumerBase<EventResult>::Finish();
	}
};


class DrawJetGraphBase:public DrawGraphErrorsConsumerBase<EventResult>
{
public:
	DrawJetGraphBase( int jetNum) : DrawGraphErrorsConsumerBase<EventResult>(), m_jetNum (jetNum)
		{

		}
	int m_jetNum;

};

template <class TXProvider>
class DrawJetPt: public DrawJetGraphBase
{
public:
	DrawJetPt( int jetNum ) : DrawJetGraphBase( jetNum)
	{
	}

	// this method is called for all events
	virtual void ProcessFilteredEvent(EventResult & event )
	{
		m_graph->AddPoint( 	m_xProvider.GetXValue( event),
							event.GetCorrectedJetPt(this->m_jetNum),
							0.0f, 0.0f );
	}
	TXProvider m_xProvider;
};

template <class TXProvider>
class DrawCutEffGraph: public DrawGraphErrorsConsumerBase<EventResult>
{
public:
	DrawCutEffGraph( int cutId ) : 	DrawGraphErrorsConsumerBase<EventResult>(),
		m_iCutId( cutId)
	{
		m_hist_rejected.AddModifier( new ModHistBinRange( m_xProvider.GetLow(),m_xProvider.GetHigh()));
		m_hist_rejected.AddModifier( new ModHistBinCount( m_xProvider.GetBinCount() ));


		m_hist_overall.AddModifier( new ModHistBinRange( m_xProvider.GetLow(),m_xProvider.GetHigh()));
		m_hist_overall.AddModifier( new ModHistBinCount( m_xProvider.GetBinCount() ));


		m_hist_rejected.Init();
		m_hist_overall.Init();
		/*
		m_binCounter.reset( new CutEffBinnedCounter( m_xProvider.GetLow(),
				(  m_xProvider.GetHigh() -  m_xProvider.GetLow()) / (double) m_xProvider.GetBinCount(),
				m_xProvider.GetBinCount()));*/
	}

	// this method is called for all events
	virtual void ProcessEvent(EventResult & event, FilterResult & result)
	{
		if ( !g_cutHandler.IsValidEvent( &event))
			return;
/*
		CutEffBinInfo * pInfo = m_binCounter->GetBinInfo(	m_xProvider.GetXValue( event ));
*/
		// is null is returned, out of our range, but is fine
		//if ( pInfo != NULL)
		{
			if ( g_cutHandler.IsCutInBitmask( m_iCutId, event.m_cutBitmask ))
				m_hist_rejected.Fill( m_xProvider.GetXValue( event ), event.m_weight );


			m_hist_overall.Fill( m_xProvider.GetXValue( event ), event.m_weight );
		}
	}

	virtual void Finish()
	{
		m_hist_rejected.GetRawHisto()->Divide( m_hist_overall.GetRawHisto() );

		// plot the efficiency
		for (int i = 0;	i < m_hist_rejected.GetRawHisto()->GetNbinsX(); i++)
		{
			m_graph->AddPoint( m_hist_rejected.GetRawHisto()->GetBinCenter(i),
					m_hist_rejected.GetRawHisto()->GetBinContent(i),
					 0.0f,
					 m_hist_rejected.GetRawHisto()->GetBinError(i));

			/*
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
			 }*/

		}

		// store hist
		// + modifiers
		//CALIB_LOG( "Z mass mean " << m_hist->m_hist->GetMean() )
		DrawGraphErrorsConsumerBase<EventResult>::Finish();
	}

	int m_iCutId;
	TXProvider m_xProvider;

	// only used for the internal binning and not stored to root file
	Hist1D m_hist_rejected;
	Hist1D m_hist_overall;

	//std::auto_ptr<CutEffBinnedCounter> m_binCounter;
};

class DrawJetRespGraph: public DrawGraphErrorsConsumerBase<EventResult>
{
public:
	DrawJetRespGraph( std::string sInpHist ) :
		DrawGraphErrorsConsumerBase<EventResult>(),
		m_sInpHist( sInpHist)
	{

	}

	virtual void Process()
	{
		// move through the histos
		stringvector sv = this->GetPipelineSettings()->GetCustomBins();
		std::vector< PtBin > bins = this->GetPipelineSettings()->GetAsPtBins( sv );

		m_histResp.m_sCaption = m_histResp.m_sName = this->GetProductName();
		m_histResp.m_sRootFileFolder = this->GetPipelineSettings()->GetRootFileFolder();
		m_histResp.AddModifier( new ModHistCustomBinnig( this->GetPipelineSettings()->GetCustomBins()) );
		m_histResp.Init();

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

	Hist1D m_histResp;
};

}
#endif

