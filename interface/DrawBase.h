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

#include "DrawModifierBase.h"
#include "ZJetFilter.h"

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

template<class TPlotType>
class PlotBase
{
public:

	PlotBase()
	{
		SetNameAndCaption(RootNamer::GetTempHistoName());
	}

	void AddModifier(ModifierBase<TPlotType> * mod)
	{
		m_modifiers.push_back(mod);
	}

	void RunModifierBeforeCreation(TPlotType * pElem)
	{
		BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers )
{		p->BeforeCreation(pElem);}
}
void RunModifierBeforeDataEntry( TPlotType * pElem )
{
	BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers )
	{	p->BeforeDataEntry(pElem);}
}
void RunModifierAfterDataEntry( TPlotType * pElem )
{
	BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers )
	{	p->AfterDataEntry(pElem);}
}
void RunModifierAfterDraw( TPlotType * pElem )
{
	BOOST_FOREACH( ModifierBase<TPlotType> * p, m_modifiers )
	{	p->AfterDraw(pElem);}
}

std::vector<ModifierBase<TPlotType> *> m_modifiers;

void SetNameAndCaption( std::string sName)
{
	m_sName = m_sCaption = sName;
}
std::string m_sName;
std::string m_sCaption;
std::string m_sRootFileFolder;
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

GraphErrors() : PlotBase< GraphErrors>()
{
}

void Init()
{
}

void Store(TFile * pRootFile)
{
	this->RunModifierBeforeCreation( this );

	RootFileHelper::SafeCd( gROOT, this->m_sRootFileFolder );
	m_graph = RootFileHelper::GetStandaloneTGraphErrors( m_points.size());
	m_graph->SetName( m_sName.c_str() );
	//m_graph->SetCaption( m_sCaption.c_str() );

	this->RunModifierBeforeDataEntry( this );

	unsigned long l = 0;
	for ( std::vector<DataPoint>::iterator it = m_points.begin();
			!( it == m_points.end()); it++)
	{
		m_graph->SetPoint(l, it->m_fx, it->m_fy);
		m_graph->SetPointError(l, it->m_fxe, it->m_fye);

		l++;
	}

	this->RunModifierAfterDataEntry(this );
	this->RunModifierAfterDraw( this );

	//CALIB_LOG( "Storing GraphErrors " + this->m_sRootFileFolder + "/" + this->m_sName + "_graph" )
	RootFileHelper::SafeCd( pRootFile, this->m_sRootFileFolder );
	m_graph->Write((this->m_sName + "_graph").c_str());
}

void AddPoint( double x, double y, double xe, double ye )
{
	m_points.push_back( DataPoint( x, y, xe, ye ) );
}

int m_iBinCount;
double m_dBinLower;
double m_dBinUpper;

std::vector<DataPoint> m_points;

TGraphErrors * m_graph;
};

class Profile2D: public PlotBase< Profile2D>
{
public:
Profile2D() : PlotBase< Profile2D>(),
m_iBinXCount(100), m_dBinXLower(0.0f), m_dBinXUpper(200.0f)
{
}

void Init()
{
	this->RunModifierBeforeCreation( this );

	RootFileHelper::SafeCd( gROOT, this->m_sRootFileFolder );
	m_profile = RootFileHelper::GetStandaloneTProfile(
			this->m_sName, this->m_sCaption,
			this->m_iBinXCount, this->m_dBinXLower, this->m_dBinXUpper);
	m_profile->Sumw2();

	this->RunModifierBeforeDataEntry( this );
}

void Store(TFile * pRootFile)
{
	this->RunModifierAfterDataEntry(this );
	this->RunModifierAfterDraw( this );

	//CALIB_LOG( "Storing 2d Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )
	RootFileHelper::SafeCd( pRootFile, this->m_sRootFileFolder );
	m_profile->Write((this->m_sName + "_profile").c_str());
}

void Fill(double x, double y, double weight)
{
	m_profile->Fill(x, y, weight);
}

TProfile * GetRawProfile()
{	return m_profile;}

int m_iBinXCount;
double m_dBinXLower;
double m_dBinXUpper;

TProfile* m_profile;
};

class Hist2D: public HistBase< Hist2D>
{
public:

Hist2D() : HistBase< Hist2D>(),
m_iBinXCount(100), m_dBinXLower(0.0f), m_dBinXUpper(200.0f),
m_iBinYCount(100), m_dBinYLower(0.0f), m_dBinYUpper(200.0f),
m_bDoProfile( false )
{
}

void Init()
{
	this->RunModifierBeforeCreation( this );

	RootFileHelper::SafeCd( gROOT, this->m_sRootFileFolder );
	m_hist = RootFileHelper::GetStandaloneTH2D_1(
			this->m_sName, this->m_sCaption,
			this->m_iBinXCount, this->m_dBinXLower, this->m_dBinXUpper,
			this->m_iBinYCount, this->m_dBinYLower, this->m_dBinYUpper);
	m_hist->Sumw2();

	this->RunModifierBeforeDataEntry( this );

	if ( m_bDoProfile )
	{
		m_profile.m_sName = this->m_sName + "_profiley";
		m_profile.m_sCaption = this->m_sCaption + "_profiley";

		m_profile.m_iBinXCount = this->m_iBinXCount;

		m_profile.m_dBinXLower = this->m_dBinXLower;
		m_profile.m_dBinXUpper = this->m_dBinXUpper,
		m_profile.m_sRootFileFolder = this->m_sRootFileFolder;
		m_profile.Init();
	}
}

void Store(TFile * pRootFile)
{
	this->RunModifierAfterDataEntry(this );
	this->RunModifierAfterDraw( this );

	//CALIB_LOG( "Storing 2d Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )
	RootFileHelper::SafeCd( pRootFile, this->m_sRootFileFolder );
	m_hist->Write((this->m_sName + "_hist").c_str());

	if ( m_bDoProfile )
	m_profile.Store( pRootFile );
}

void Fill(double x, double y, double weight)
{
	m_hist->Fill(x, y, weight);

	if (m_bDoProfile)
	m_profile.Fill(x, y, weight);
}

TH2D * GetRawHisto()
{	return m_hist;}

int m_iBinXCount;
double m_dBinXLower;
double m_dBinXUpper;
int m_iBinYCount;
double m_dBinYLower;
double m_dBinYUpper;

bool m_bDoProfile;

Profile2D m_profile;

TH2D * m_hist;
};

class Hist1D: public HistBase< Hist1D>
{
public:

Hist1D() : HistBase< Hist1D>(),
m_iBinCount(100), m_dBinLower(0.0f), m_dBinUpper(200.0f), m_bUseCustomBin(false)
{

}

void Init()
{
	this->RunModifierBeforeCreation( this );

	RootFileHelper::SafeCd( gROOT, this->m_sRootFileFolder );
	if ( m_bUseCustomBin )
	{
		m_hist = RootFileHelper::GetStandaloneTH1D_1( this->m_sName,
				this->m_sCaption,
				m_iBinCount, &m_dCustomBins[0]);
		/*new TH1D(    this->m_sName.c_str(),
		 this->m_sCaption.c_str(),
		 m_iBinCount, &m_dCustomBins[0] );*/
	}
	else
	{
		m_hist = RootFileHelper::GetStandaloneTH1D_2( this->m_sName,
				this->m_sCaption, this->m_iBinCount, this->m_dBinLower, this->m_dBinUpper);

	}
	m_hist->Sumw2();

	this->RunModifierBeforeDataEntry( this );
}

void Store(TFile * pRootFile)
{
	this->RunModifierAfterDataEntry(this );
	this->RunModifierAfterDraw( this );

	//CALIB_LOG( "Storing Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )

	RootFileHelper::SafeCd( pRootFile, this->m_sRootFileFolder );
	m_hist->Write((this->m_sName + "_hist").c_str());
}

void Fill(double val, double weight)
{
	m_hist->Fill(val, weight);
}

TH1D * GetRawHisto()
{	return m_hist;}

int m_iBinCount;
double m_dBinLower;
double m_dBinUpper;
double m_dCustomBins[255];

bool m_bUseCustomBin;

TH1D * m_hist;
};

template<class TData, class TSettings>
class DrawConsumerBase: public EventConsumerBase<TData, TSettings>
{
public:
DrawConsumerBase()
{
}
virtual ~DrawConsumerBase()
{}

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

template<class TData, class TSettings>
class DrawGraphErrorsConsumerBase: public DrawConsumerBase<TData, TSettings>
{
public:
DrawGraphErrorsConsumerBase() :
m_graph(NULL)
{
}

virtual void Init(EventPipeline<TData, TSettings> * pset)
{
	DrawConsumerBase<TData, TSettings>::Init(pset);
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

template<class TData, class TSettings>
class DrawHist1dConsumerBase: public DrawConsumerBase<TData, TSettings>
{
public:
DrawHist1dConsumerBase() :
m_hist(NULL)
{
}

virtual void Init(EventPipeline<TData, TSettings> * pset)
{
	DrawConsumerBase<TData, TSettings>::Init(pset);
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
	//CALIB_LOG( "Storing Hist for " << this->GetProductName() )
	m_hist->Store(this->GetPipelineSettings()->GetRootOutFile());
}
// already configured histogramm
Hist1D * m_hist;
};

template<class TData, class TSettings>
class DrawHist2DConsumerBase: public DrawConsumerBase<TData, TSettings>
{
public:
DrawHist2DConsumerBase() :
m_hist(NULL)
{
}

virtual void Init(EventPipeline<TData, TSettings> * pset)
{
	DrawConsumerBase<TData, TSettings>::Init(pset);
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

}

