#pragma once

#include "DrawBase.h"
#include "ZJetPipeline.h"
// MODS

namespace CalibFW
{

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


}



