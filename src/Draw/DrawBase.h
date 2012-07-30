#pragma once

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
//#include "EventData.h"
//#include "PtBinWeighter.h"
#include "Pipeline/EventPipeline.h"

#include "DrawModifierBase.h"

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

class ModHistBinRange;
class ModHistBinCount;

template<class TPlotType>
class PlotBase
{
public:

	typedef ModifierBase< TPlotType > Modifier;
	typedef std::list< Modifier * > ModifierList;

	PlotBase( std::string sName, std::string sFolder)
		: m_sName ( sName)
	{
		SetRootFileFolder ( sFolder );
		//SetNameAndCaption(RootNamer::GetTempHistoName());
		SetNameAndCaption(sName);
	}

	void AddModifier(ModifierBase<TPlotType> * mod)
	{
		m_modifiers.push_back(mod);
	}

	void AddModifiers(ModifierList modList)
	{
		BOOST_FOREACH( Modifier * m, modList)
		{
			m_modifiers.push_back(m);
		}
	}

	void RunModifierBeforeCreation(TPlotType * pElem)
	{
		for ( typename ModifierVector::iterator it = m_modifiers.begin() ; it != m_modifiers.end(); ++ it )
		{		it->BeforeCreation(pElem);}
	}
	void RunModifierBeforeDataEntry( TPlotType * pElem )
	{
		for ( typename ModifierVector::iterator it = m_modifiers.begin() ; it != m_modifiers.end(); ++ it )
			{	it->BeforeDataEntry(pElem);}
	}
	void RunModifierAfterDataEntry( TPlotType * pElem )
	{
		for ( typename ModifierVector::iterator it = m_modifiers.begin() ; it != m_modifiers.end(); ++ it )
			{	it->AfterDataEntry(pElem);}
	}
	void RunModifierAfterDraw( TPlotType * pElem )
	{
		for ( typename ModifierVector::iterator it = m_modifiers.begin() ; it != m_modifiers.end(); ++ it )
			{	it->AfterDraw(pElem);}
	}

	typedef ModifierBase<TPlotType> ModifierForThisClass;

	typedef typename boost::ptr_vector<ModifierForThisClass >  ModifierVector;
	ModifierVector m_modifiers;
	//std::vector<ModifierBase<TPlotType> *> m_modifiers;

	void SetNameAndCaption( std::string sName)
	{
		m_sName = m_sCaption = sName;
	}

	std::string GetName() const { return m_sName; }
	std::string GetCaption() const { return m_sCaption; }

private:

	std::string m_sName;
	std::string m_sCaption;

	IMPL_PROPERTY(std::string, RootFileFolder)

};

class PlotSettings
{
	boost::shared_ptr<TFile> m_pOutputFile;
};

template < class THistType >
class HistBase: public PlotBase<THistType>
{
public:
	HistBase( std::string sName, std::string sFolder) :
		PlotBase<THistType>( sName, sFolder)
	{
	}
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

	GraphErrors( std::string sName, std::string sFolder ) :
		PlotBase< GraphErrors>(  sName, sFolder)
		{
		}

	void Init()
	{
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierBeforeCreation( this );

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder() );
		m_graph.reset( RootFileHelper::GetStandaloneTGraphErrors( m_points.size()) );
		m_graph->SetName( GetName().c_str() );
		//m_graph->SetCaption( m_sCaption.c_str() );

		this->RunModifierBeforeDataEntry( this );

		unsigned long l = 0;
		for ( std::list<DataPoint>::const_iterator it = m_points.begin();
				!( it == m_points.end()); it++)
		{
			m_graph->SetPoint(l, it->m_fx, it->m_fy);
			m_graph->SetPointError(l, it->m_fxe, it->m_fye);

			l++;
		}

		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing GraphErrors " + this->m_sRootFileFolder + "/" + this->m_sName + "_graph" )
		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_graph->Write(( GetName() ).c_str());
	}

	void AddPoint( double x, double y, double xe, double ye )
	{
		m_points.push_back( DataPoint( x, y, xe, ye ) );
	}

	int m_iBinCount;
	double m_dBinLower;
	double m_dBinUpper;

	std::list<DataPoint> m_points;

	boost::scoped_ptr< TGraphErrors > m_graph;
};
/*
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

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder());
		m_profile = RootFileHelper::GetStandaloneTProfile(
				GetName(), GetCaption(),
				this->m_iBinXCount, this->m_dBinXLower, this->m_dBinXUpper);
		m_profile->Sumw2();

		this->RunModifierBeforeDataEntry( this );
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing 2d Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )
		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_profile->Write((GetName() + "_profile").c_str());
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

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder() );
		m_hist = RootFileHelper::GetStandaloneTH2D_1(
				GetName(), GetCaption(),
				this->m_iBinXCount, this->m_dBinXLower, this->m_dBinXUpper,
				this->m_iBinYCount, this->m_dBinYLower, this->m_dBinYUpper);
		m_hist->Sumw2();

		this->RunModifierBeforeDataEntry( this );

		if ( m_bDoProfile )
		{
			m_profile.SetNameAndCaption( this->GetName() + "_profiley");

			m_profile.m_iBinXCount = this->m_iBinXCount;

			m_profile.m_dBinXLower = this->m_dBinXLower;
			m_profile.m_dBinXUpper = this->m_dBinXUpper,
					m_profile.SetRootFileFolder( GetRootFileFolder() );
			m_profile.Init();
		}
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing 2d Histogram " + this->m_sRootFileFolder + "/" + this->m_sName + "_hist" )
		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_hist->Write((GetName() + "_hist").c_str());

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
*/
class Hist1D: public HistBase< Hist1D>
{
public:

	static ModifierList GetPtModifier();
	static ModifierList GetPtModifier(double max);
	static ModifierList GetPhiModifier();
	static ModifierList GetEtaModifier();
	static ModifierList GetFractionModifier();
	static ModifierList GetConstituentsModifier();
	static ModifierList GetNoModifier();
	static ModifierList GetNRVModifier();
    static ModifierList GetCountModifier( unsigned int max );
	static ModifierList GetResponseModifier();
	static ModifierList GetMassModifier();
	static ModifierList GetAreaModifier();
	static ModifierList GetMETModifier();
	static ModifierList GetMETFractionModifier();
	static ModifierList GetJetMatchingModifier();
	static ModifierList GetRunModifier();

	Hist1D( std::string sName, std::string sFolder, Hist1D::ModifierList l ) :
		HistBase< Hist1D>(sName, sFolder),
		m_iBinCount(100),
		m_dBinLower(0.0f),
		m_dBinUpper(200.0f),
		m_bUseCustomBin(false)
	{
		this->AddModifiers( l);
		this->RunModifierBeforeCreation( this );

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder() );
		if ( m_bUseCustomBin )
		{
			m_hist.reset( RootFileHelper::GetStandaloneTH1D_1( GetName(),
					GetCaption(),
					m_iBinCount, &m_dCustomBins[0]) );
			/*new TH1D(    this->m_sName.c_str(),
		 this->m_sCaption.c_str(),
		 m_iBinCount, &m_dCustomBins[0] );*/
		}
		else
		{
			m_hist.reset( RootFileHelper::GetStandaloneTH1D_2( GetRootFileFolder() + "_" + GetName(),
					GetCaption(), this->m_iBinCount, this->m_dBinLower, this->m_dBinUpper)  );

		}
		m_hist->Sumw2();

		this->RunModifierBeforeDataEntry( this );
	}

	void Init( )
	{
		Hist1D::ModifierList modList;

		CALIB_LOG_FATAL("not implemented")

		this->Init(modList);
	}

	void Init( ModifierList l )
	{

	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing Histogram " + this->GetRootFileFolder() + "/" + this->GetName()  )

		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_hist->Write((this->GetName() ).c_str());
	}

	void Fill(double val, double weight)
	{
		m_hist->Fill(val, weight);
	}
/*
	TH1D * GetRawHisto()
		{	return m_hist;}*/

	int m_iBinCount;
	double m_dBinLower;
	double m_dBinUpper;
	double m_dCustomBins[255];

	bool m_bUseCustomBin;

	boost::scoped_ptr<TH1D> m_hist;
};

template<class TData, class TMetaData, class TSettings>
class DrawConsumerBase: public EventConsumerBase<TData, TMetaData,  TSettings>
{
public:
	DrawConsumerBase()
	{
	}
	virtual ~DrawConsumerBase()
	{}


};

template<class TData, class TMetaData,class TSettings>
class DrawHist1dConsumerBase: public DrawConsumerBase<TData, TMetaData,TSettings>
{
public:
	DrawHist1dConsumerBase() :
		m_hist(NULL)
		{
		}

	virtual void Init(EventPipeline<TData, TMetaData,TSettings> * pset)
	{
		DrawConsumerBase<TData, TMetaData,TSettings>::Init(pset);
		//CALIB_LOG( "Initializing Hist for " << this->GetProductName() )


		//m_hist->SetNameAndCaption( this->GetProductName());
		m_hist->SetRootFileFolder (this->GetPipelineSettings().GetRootFileFolder());
		m_hist->Init( Hist1D::ModifierList()  );
	}

	virtual void Finish()
	{
		// store hist
		// + modifiers
		//CALIB_LOG( "Storing Hist for " << this->GetProductName() )
		m_hist->Store(this->GetPipelineSettings().GetRootOutFile());
	}
	// already configured histogramm
	Hist1D * m_hist;
};


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

class ModHistCustomBinning : public ModifierBase<Hist1D>
{
public:
	ModHistCustomBinning( )
	{

	}

	ModHistCustomBinning ( stringvector customBins )
	{/*
		std::vector<PtBin> custBins = ZJetPipelineSettings::GetAsPtBins(  customBins );

		if ( custBins.size() == 0 )
			CALIB_LOG_FATAL("No bins specified for Plot !")

		this->m_iBinCount = custBins.size();
		this->m_dBins[0] = custBins[0].GetMin();

		  int i = 1;
		  BOOST_FOREACH( PtBin & bin, custBins )
		  {
			this->m_dBins[i] = bin.GetMax();
			i++;
		  }*/
		CALIB_LOG_FATAL("not implmented")
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

}

