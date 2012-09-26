#pragma once

#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <memory>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"


#include <boost/scoped_ptr.hpp>

#include "DrawModifierBase.h"
#include "DrawBase.h"

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
		ProfileBase< Profile2d>(  sName, sFolder),
		m_iBinCountX( 100),
		m_dBinLowerX( 100),
		m_dBinUpperX( 100)

		{
			//m_points.reserve( 1000 );
		}

	void Init()
	{
		this->RunModifierBeforeCreation( this );

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder() );
		m_profile.reset
				 (
					RootFileHelper::GetStandaloneTProfile(
					GetName().c_str(),GetName().c_str(),
					m_iBinCountX, m_dBinLowerX, m_dBinUpperX )
			);

		this->RunModifierBeforeDataEntry( this );
	}

	void Store(TFile * pRootFile)
	{


		//m_graph->SetCaption( m_sCaption.c_str() );

		/*

		for ( std::vector<DataPoint>::const_iterator it = m_points.begin();
				!( it == m_points.end()); it++)
		{
			m_profile->Fill( it->m_fx, it->m_fy, it->m_fweight);
		}
*/
		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		//CALIB_LOG( "Storing GraphErrors " + this->m_sRootFileFolder + "/" + this->m_sName + "_graph" )
		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_profile->Write(GetName().c_str());
	}

	void AddPoint( double x, double y, double weight)
	{
		//m_points.push_back( DataPoint( x, y, weight ) );
		m_profile->Fill( x, y, weight);
	}

	unsigned int m_iBinCountX;
	double m_dBinLowerX;
	double m_dBinUpperX;

	std::vector<DataPoint> m_points;

	boost::scoped_ptr<TProfile> m_profile;
};




class ModProfileXBinRange : public ModifierBase<Profile2d>
{
public:
	ModProfileXBinRange( double lower, double upper) :
		m_dBinLower(lower),m_dBinUpper(upper)
	{
	}

	virtual void BeforeCreation(Profile2d * pElem)
	{
		pElem->m_dBinLowerX = this->m_dBinLower;
		pElem->m_dBinUpperX = this->m_dBinUpper;
	}

private:
	double m_dBinLower;
	double m_dBinUpper;
};

class ModProfileXBinCount : public ModifierBase<Profile2d>
{
public:
	ModProfileXBinCount( unsigned int binCount) : m_binCount(binCount)
	{
	}

	virtual void BeforeCreation(Profile2d * pElem)
	{
		pElem->m_iBinCountX = m_binCount;
	}

private:
	unsigned int m_binCount;
};






class Profile3d: public ProfileBase< Profile3d >
{
public:

	class DataPoint
	{
	public:
		DataPoint ( double x, double y, double z)
		: m_fx( x), m_fy(y), m_fz( z )
		  {

		  }

		double m_fx, m_fy, m_fz;
	};

	Profile3d( std::string sName, std::string sFolder ) :
		ProfileBase< Profile3d>(  sName, sFolder),
		m_iBinCountX( 100),
		m_dBinLowerX( -5),
		m_dBinUpperX( 5),
		m_iBinCountY( 100),
		m_dBinLowerY( -5),
		m_dBinUpperY( 5)

		{
		
		}

	void Init()
	{
		this->RunModifierBeforeCreation( this );

		RootFileHelper::SafeCd( gROOT, GetRootFileFolder() );
		m_profile.reset
				 (
					RootFileHelper::GetStandaloneTProfile2D(
					GetName().c_str(),GetName().c_str(),
					m_iBinCountX, m_dBinLowerX, m_dBinUpperX,
					m_iBinCountY, m_dBinLowerY, m_dBinUpperY )
			);

		this->RunModifierBeforeDataEntry( this );
	}

	void Store(TFile * pRootFile)
	{

		this->RunModifierAfterDataEntry(this );
		this->RunModifierAfterDraw( this );

		RootFileHelper::SafeCd( pRootFile, GetRootFileFolder() );
		m_profile->Write(GetName().c_str());
	}

	void AddPoint( double x, double y, double z)
	{
		m_profile->Fill( x, y, z);
	}

	unsigned int m_iBinCountX;
	double m_dBinLowerX;
	double m_dBinUpperX;
	unsigned int m_iBinCountY;
	double m_dBinLowerY;
	double m_dBinUpperY;

	std::vector<DataPoint> m_points;

	boost::scoped_ptr<TH2D> m_profile;
};

class ModProfile3dXBinRange : public ModifierBase<Profile3d>
{
public:
	ModProfile3dXBinRange( double lower, double upper) :
		m_dBinLower(lower),m_dBinUpper(upper)
	{
	}

	virtual void BeforeCreation(Profile3d * pElem)
	{
		pElem->m_dBinLowerX = this->m_dBinLower;
		pElem->m_dBinUpperX = this->m_dBinUpper;
	}

private:
	double m_dBinLower;
	double m_dBinUpper;
};

class ModProfile3dXBinCount : public ModifierBase<Profile3d>
{
public:
	ModProfile3dXBinCount( unsigned int binCount) : m_binCount(binCount)
	{
	}

	virtual void BeforeCreation(Profile3d * pElem)
	{
		pElem->m_iBinCountX = m_binCount;
	}

private:
	unsigned int m_binCount;
};

class ModProfile3dYBinRange : public ModifierBase<Profile3d>
{
public:
	ModProfile3dYBinRange( double lower, double upper) :
		m_dBinLower(lower),m_dBinUpper(upper)
	{
	}

	virtual void BeforeCreation(Profile3d * pElem)
	{
		pElem->m_dBinLowerY = this->m_dBinLower;
		pElem->m_dBinUpperY = this->m_dBinUpper;
	}

private:
	double m_dBinLower;
	double m_dBinUpper;
};

class ModProfile3dYBinCount : public ModifierBase<Profile3d>
{
public:
	ModProfile3dYBinCount( unsigned int binCount) : m_binCount(binCount)
	{
	}

	virtual void BeforeCreation(Profile3d * pElem)
	{
		pElem->m_iBinCountY = m_binCount;
	}

private:
	unsigned int m_binCount;
};







}

