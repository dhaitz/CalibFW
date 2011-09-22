#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <cassert>

#include "GlobalInclude.h"




class PtBinWeight
{
public:
    PtBinWeight()
    {
        this->m_entryCount = 0;
    }

    PtBinWeight( PtBin bin, double xsection)
    {
        this->m_bin = bin;
        this->m_xsection = xsection;
        this->m_entryCount = 0;
    }

    PtBin m_bin;
    double m_xsection;
    unsigned long m_entryCount;
};

class PtBinWeighter
{
public:
	
	void Reset()
{
	m_weights.clear();
}

    void ResetEntryCount()
    {
        std::vector<PtBinWeight>::iterator iter;

        for ( iter = this->m_weights.begin();
                iter != this->m_weights.end();
                ++iter)
        {
            (*iter).m_entryCount = 0;
        }
    }

    void IncreaseCountByZpt( double zPt, long incBy )
    {
        std::vector<PtBinWeight>::iterator iter;

        for ( iter = this->m_weights.begin();
                iter != this->m_weights.end();
                ++iter)
        {
            if ( (*iter).m_bin.IsInBin( zPt) )
            {
                (*iter).m_entryCount += incBy;
            }
        }
    }

    void IncreaseCountByXSection( double xsection, long incBy )
    {
        std::vector<PtBinWeight>::iterator iter;

        for ( iter = this->m_weights.begin();
                iter != this->m_weights.end();
                ++iter)
        {
            if ( FLOAT_COMPARE((*iter).m_xsection, xsection ) )
            {
                (*iter).m_entryCount += incBy;
                CALIB_LOG_FILE( "file xsec " << xsection << " count " << incBy )
            }
        }
    }

    void AddBin( PtBin bin, double weight)
    {
        m_weights.push_back( PtBinWeight( bin, weight) );
    }

    double GetWeightByZPt( double zpt )
    {
        std::vector<PtBinWeight>::iterator iter;

        for ( iter = this->m_weights.begin();
                iter != this->m_weights.end();
                ++iter)
        {
            // w = ( xsection * intlumi ) / Nmc in this xsection

            if ( (*iter).m_bin.IsInBin( zpt)  )
                return  ( iter->m_xsection *  1 /* int lumi*/ )  /  ((double) iter->m_entryCount) ;
        }

        std::cout << "No weight for zpt " << zpt << " found." << std::endl;
        exit(0);

    }

    double GetWeightByXSection( double xsection )
    {
        std::vector<PtBinWeight>::iterator iter;

        for ( iter = this->m_weights.begin();
                iter != this->m_weights.end();
                ++iter)
        {
            // w = ( xsection * intlumi ) / Nmc in this xsection
            if ( FLOAT_COMPARE((*iter).m_xsection, xsection )  )
                //return  ( (*iter).m_xsection *  1 /* int lumi*/ )  /  ((double) (*iter).m_entryCount) ;

                // uses
                return  ( xsection *  1 /* int lumi*/ )  /  ((double) (*iter).m_entryCount) ;
        }

        std::cout << "No weight for xsection " << xsection << " found." << std::endl;
        exit(0);
        //return 1.0f;
    }

    void Print()
    {
        std::vector<PtBinWeight>::iterator iter;

        for ( iter = this->m_weights.begin();
                iter != this->m_weights.end();
                ++iter)
        {
            CALIB_LOG_FILE( "pt" << iter->m_bin.m_fLowestPt << "-" << iter->m_bin.m_fHighestPt
                      << "  xsection: " << iter->m_xsection << "  entries: " << iter->m_entryCount << "  weight: " << this->GetWeightByXSection(iter->m_xsection ))
        }
    }


    std::vector<PtBinWeight> m_weights;
};

