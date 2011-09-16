#pragma once

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootIncludes.h"


#include "../DrawModifierBase.h"
#include "../DrawBase.h"

namespace CalibFW
{


template < class THistType >
class ProfileBase: public PlotBase<THistType>
{
public:
	ProfileBase( std::string sName, std::string sFolder ) :
			PlotBase< THistType>(  sName, sFolder)
		{
		}
};

class Profile2d: public ProfileBase< Profile2d >
{
public:

	class DataPoint
	{
	public:
		DataPoint ( double x, double y, double weight)
		: m_fx( x), m_fy(y), m_fweight( weight )
		  {

		  }

		double m_fx, m_fy, m_fweight;
	};

	Profile2d( std::string sName, std::string sFolder ) :
			ProfileBase< Profile2d>(  sName, sFolder)
		{
		}

	void Init()
	{
	}

	void Store(TFile * pRootFile)
	{
		this->RunModifierBeforeCreation( this );

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder() );
		m_graph = RootFileHelper::GetStandaloneTProfile(
				GetName().c_str(),GetName().c_str(),
				m_iBinCountX, m_dBinLowerX, m_dBinLowerX );

		//m_graph->SetCaption( m_sCaption.c_str() );

		this->RunModifierBeforeDataEntry( this );

		for ( std::list<DataPoint>::const_iterator it = m_points.begin();
				!( it == m_points.end()); it++)
		{
			m_graph->Fill( it->m_fx, it->m_fy, it->m_fweight);

		}

		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing GraphErrors " + this->m_sRootFileFolder + "/" + this->m_sName + "_graph" )
		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_graph->Write(( GetName() + "_profile").c_str());
	}

	void AddPoint( double x, double y, double weight)
	{
		m_points.push_back( DataPoint( x, y, weight ) );
	}

	int m_iBinCountX;
	double m_dBinLowerX;
	double m_dBinUpperX;

	std::list<DataPoint> m_points;

	TProfile * m_graph;
};

}

