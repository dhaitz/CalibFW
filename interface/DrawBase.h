
#ifndef __DRAWBASE_H__
#define __DRAWBASE_H__

#include <string>
#include <iostream>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"


class EventSelectionBase
{
public:
    // default implementation, no selection performed
    virtual bool IsEventInSelecction( EventId id )
    {
        return true;
    }
};

class CutSelection : public EventSelectionBase
{
public:
    CutSelection( EventSet * eventsInCut )
    {
        this->m_pEventsInCut = eventsInCut;
    }

    virtual inline bool IsEventInSelecction( EventId id ) {
        return ( m_pEventsInCut->find( id ) != m_pEventsInCut->end() );
    }

private:
    EventSet * m_pEventsInCut;

};

template < class TData >
class CHistDataDrawBase
{
public:
    virtual void Draw( TH1D * pHist, TData data ) = 0;
};

template < class THistType >
class CHistEvtMapTemplate : public  CHistDataDrawBase<  EventVector & >
{
public:
    CHistEvtMapTemplate()
    {
        m_bOnlyEventsInCut = true;
        m_bUsePtCut = true;
        m_binWith = ZPtBinning;
    }

    enum BinWithEnum { ZPtBinning, Jet1PtBinning };

    virtual void HistFill( THistType * pHist,
                   double fillValue,
                   EventResult & Res )
    {
        pHist->Fill( fillValue, Res.m_weight);
    }

    BinWithEnum m_binWith;

    // default behavior, might get more complex later on
    bool IsInSelection ( EventVector::iterator it )
    {
        bool bPass = true;
        // no section here is allowed to set to true again, just to false ! avoids coding errors
        if (this->m_bOnlyEventsInCut)
        {
            if (! it->IsInCut())
                bPass = false;
        }
        else
        {
            // all events which are valid in jSON file
            // does this work for mc ??
            if (! it->IsValidEvent() )
                bPass = false;
        }

        if ( m_bUsePtCut )
        {
            double fBinVal;
            if ( m_binWith == ZPtBinning )
                fBinVal = it->m_pData->Z->Pt();
            else
                fBinVal = it->GetCorrectedJetPt(0);

            if (!( fBinVal >= this->m_dLowPtCut ))
                bPass = false;

            if (!( fBinVal < this->m_dHighPtCut ))
                bPass = false;
        }

        return bPass;
    }

    bool m_bOnlyEventsInCut;
    bool m_bUsePtCut;
    double m_dLowPtCut;
    double m_dHighPtCut;
};

class CHistEvtMapBase : public  CHistEvtMapTemplate < TH1D >
{
  
};

class CHistEvtMapInt : public  CHistEvtMapTemplate < TH1I >
{
public:
    virtual void HistFill( TH1I * pHist,
                   int fillValue,
                   EventResult & Res )
    {
        pHist->Fill( fillValue, Res.m_weight);
    }
};


class CHistEvtDataZMass : public  CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin();
                !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
            {
                HistFill( pHist,it->m_pData->Z->GetCalcMass(), (*it));
            }
        }
    }
};

class CHistEvtDataZPt : public  CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->Z->Pt(), (*it));
        }
    }
};

class CHistEvtDataMet : public  CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->met->Energy(), (*it));
        }
    }
};

class CHistEvtDataTcMet : public  CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->tcmet->Pt(), (*it));
        }
    }
};
class CHistEvtDataZPtCutEff : public  CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            //if (IsInSelection(it))
            if (! it->IsInCut() )
                HistFill( pHist, it->m_pData->Z->Pt(), (*it));
        }
    }
};


class CHistEvtDataZPhi : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->Z->Phi() - TMath::Pi(),(*it));
        }
    }
};

class CHistEvtDataZEta : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->Z->Eta(),(*it));
        }
    }
};



class CHistEvtDataMuMinusPt : public  CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->mu_minus->Pt(), (*it));
        }
    }
};

class CHistEvtDataMuPlusPt : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->mu_plus->Pt(), (*it));
        }
    }
};

class CHistEvtDataMuPlusEta : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->mu_plus->Eta(), (*it));
        }
    }
};

class CHistEvtDataMuMinusEta : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->mu_minus->Eta(), (*it));
        }
    }
};

class CHistEvtDataMuPlusPhi : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->mu_plus->Phi() - TMath::Pi(), (*it));
        }
    }
};

class CHistEvtDataMuMinusPhi : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->m_pData->mu_minus->Phi() - TMath::Pi(), (*it));
        }
    }
};


class CHistEvtDataMuAllPt : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
            {
                HistFill( pHist, it->m_pData->mu_plus->Pt(), (*it));
                HistFill( pHist, it->m_pData->mu_minus->Pt(), (*it));
            }
        }
    }
};

class CHistEvtDataMuAllEta : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
            {
                HistFill( pHist, it->m_pData->mu_plus->Eta(), (*it));
                HistFill( pHist, it->m_pData->mu_minus->Eta(), (*it));
            }
        }
    }
};

class CHistEvtDataMuAllPhi : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
            {
                HistFill( pHist, it->m_pData->mu_plus->Phi() - TMath::Pi(), (*it));
                HistFill( pHist, it->m_pData->mu_minus->Phi() - TMath::Pi(), (*it));
            }

        }
    }
};

class CHistEvtDataJetBase : public CHistEvtMapBase
{
public:
    CHistEvtDataJetBase( int iJetNum )
    {
        m_iJetNum = iJetNum;
    }

    int m_iJetNum;
};


class CHistEvtDataJetEta : public CHistEvtDataJetBase
{
public:
    CHistEvtDataJetEta( int iJetNum ) : CHistEvtDataJetBase( iJetNum)
    {
    }

    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it) && (  it->m_pData->jets[m_iJetNum]->P() > 0.01f ) )
                HistFill( pHist, it->m_pData->jets[m_iJetNum]->Eta(),(*it));
        }
    }
};


class CHistEvtDataJetPt : public CHistEvtDataJetBase
{
public:
    CHistEvtDataJetPt( int iJetNum ) : CHistEvtDataJetBase( iJetNum)
    {
    }

    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        double ptVal;
        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it)&& (  it->m_pData->jets[m_iJetNum]->P() > 0.01f ))
                HistFill( pHist, it->GetCorrectedJetPt( this->m_iJetNum) , (*it));
        }
    }
};

class CHistEvtDataJetPhi : public CHistEvtDataJetBase
{
public:
    CHistEvtDataJetPhi( int iJetNum ) : CHistEvtDataJetBase( iJetNum)
    {
    }

    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it)&& (  it->m_pData->jets[m_iJetNum]->P() > 0.01f ))
                HistFill( pHist, it->m_pData->jets[m_iJetNum]->Phi() - TMath::Pi(),(*it));
        }
    }
};

// cut plots
/*
class CHistJetPtCorrEta : public CHistEvtDataJetBase
{
public:
    CHistJetCorrEta( int iJetNum ) : CHistEvtDataJetBase( iJetNum)
    {
    }

    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it) && (  it->m_pData->jets[m_iJetNum]->P() > 0.01f ))
            	HistFill( pHist,it->m_l2CorrJets[ this->m_iJetNum ] -  it->m_pData->jets[m_iJetNum]->Pt(),(*it));
        }
    }
};
*/

class CHistEvtDataJetPtMinusZPt : public CHistEvtDataJetBase
{
public:
    CHistEvtDataJetPtMinusZPt( int iJetNum ) : CHistEvtDataJetBase( iJetNum)
    {
    }

    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it)&& (  it->m_pData->jets[m_iJetNum]->P() > 0.01f ))
                HistFill( pHist,it->m_pData->Z->Pt() -  it->m_pData->jets[m_iJetNum]->Pt(),(*it));
        }
    }
};


class CHistEvtData2ndJetToZPt : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it)&& (  it->m_pData->jets[1]->P() > 0.01f ))
                HistFill( pHist,it->GetCorrectedJetPt(1) / it->m_pData->Z->Pt(), (*it));
        }
    }
};


class CHistEvtDataBack2Back : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, TMath::Abs( TMath::Abs(it->m_pData->jets[0]->Phi() - it->m_pData->Z->Phi()) - TMath::Pi())
                          , (*it));
        }
    }
};


class CHistEvtDataJetResponse : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin();  !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
                HistFill( pHist, it->GetCorrectedJetPt(0) / it->m_pData->Z->Pt()
                          , (*it));
        }
    }
};

class CHistEvtDataMetJetResponse : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin();  !(it == data.end()); ++it)
        {
/*            if (IsInSelection(it))
                HistFill( pHist,1.0 + ( (- it->m_pData->met->Energy()
		
		it->GetCorrectedJetPt(0) / it->m_pData->Z->Pt()
                          , (*it));*/
        }
    }
};

class CHistEvtDataRecoVertices : public CHistEvtMapBase
{
public:
    virtual void Draw( TH1D * pHist, EventVector & data )
    {
        EventVector::iterator it;

        for ( it = data.begin();  !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
	    {
		//std::cout << it->GetRecoVerticesCount() << std::endl;
                HistFill( pHist, (double) it->GetRecoVerticesCount(), (*it));
	    }
        }
    }
};

class CDrawBase
{
public:

    CDrawBase( TString cName, TString cCaption)
    {

        m_bSavePng = false;
        m_bSavePdf = false;

        m_sName = cName;
        m_sCaption = cCaption;
    }

    bool m_bSavePng;
    bool m_bSavePdf;

    EventSelectionBase m_eventSelection;

    TString m_sName;
    TString m_sCaption;
    TString m_sOutFolder;
};

class CHistModifierBase
{
public:
    virtual void ModifyBeforeHistCreation( void * pDrawBase  ) {};
    virtual void ModifyBeforeDataEntry( TCanvas * pC, TH1 * pHist ) {}
    virtual void ModifyAfterDataEntry( TCanvas * pC, TH1 * pHist ) {}
    virtual void ModifyAfterHistoDraw( TCanvas * pC, TH1 * pHist ) {}
};

class CGraphModifierBase
{
public:
    virtual void ModifyBeforeCreation( void * pDrawBase  ) {};
    virtual void ModifyBeforeDataEntry( TCanvas * pC, TGraphErrors * pHist ) {}
    virtual void ModifyAfterDataEntry( TCanvas * pC, TGraphErrors * pHist ) {}
    virtual void ModifyAfterDraw( TCanvas * pC, TGraphErrors * pHist ) {}
};


typedef std::vector< CHistModifierBase * > ModifierList;
/*
class CGraphModBinRange : public CGraphModifierBase
{
public:
    CModBinRange( double lower, double upper)
    {

    }

    void ModifyBeforeCreation( TGraphErrors * pB  )
    {

    }

private:
    double m_dBinLower;
    double m_dBinUpper;
};
*/
class CModTdrStyle : public CHistModifierBase
{
public:
    void ModifyBeforeHistCreation( void * pB  );
};

class CModBinRange : public CHistModifierBase
{
public:
    CModBinRange( double lower, double upper);

    void ModifyBeforeHistCreation( void * pB  );

private:
    double m_dBinLower;
    double m_dBinUpper;
};

class CModHorizontalLine : public CHistModifierBase
{
public:
    CModHorizontalLine( double xVal );

    void ModifyAfterHistoDraw( TCanvas * pC, TH1 * pHist );
private:
    double m_xVal;
};


class CModBinCount : public CHistModifierBase
{
public:
    CModBinCount( int binCount);

    void ModifyBeforeHistCreation( void * pDrawBase  );

private:
    int m_iBinCount;
};

class CPlotL2Corr
{
public:
    double GetX( EventResult & res )
    {
        return res.m_pData->jets[0]->Eta();
    }

    double GetY( EventResult & res )
    {
        return res.m_l2CorrPtJets[0];
    }

};

template < class TDataExtractor >
class CGraphDrawEvtMap
{
public:
    CGraphDrawEvtMap()
    {
        m_bOnlyEventsInCut = true;
        m_bUsePtCut = true;
        m_binWith = ZPtBinning;
    }

    enum BinWithEnum { ZPtBinning, Jet1PtBinning };

    unsigned int GetPointCount( EventVector & data )
    {
        unsigned int c = 0;
        for (EventVector::iterator it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
            {
                c++;
            }
        }

        return c;
    }

    void Draw( TGraphErrors * pGraph, EventVector & data )
    {
        int i = 0;
        TDataExtractor tEx;
        for (EventVector::iterator it = data.begin(); !(it == data.end()); ++it)
        {
            if (IsInSelection(it))
            {
                pGraph->SetPoint (i,
                                  tEx.GetX( (*it)),
                                  tEx.GetY( (*it)));
                i++;
            }
        }
    }

    BinWithEnum m_binWith;

    // default behavior, might get more complex later on
    bool IsInSelection ( EventVector::iterator it )
    {
        bool bPass = true;
        // no section here is allowed to set to true again, just to false ! avoids coding errors
        if (this->m_bOnlyEventsInCut)
        {
            if (! it->IsInCut())
                bPass = false;
        }
        else
        {
            // all events which are valid in jSON file
            // does this work for mc ??
            if (! it->IsValidEvent() )
                bPass = false;
        }

        if ( m_bUsePtCut )
        {
            double fBinVal;
            if ( m_binWith == ZPtBinning )
                fBinVal = it->m_pData->Z->Pt();
            else
                fBinVal = it->m_pData->jets[0]->Pt();

            if (!( fBinVal >= this->m_dLowPtCut ))
                bPass = false;

            if (!( fBinVal < this->m_dHighPtCut ))
                bPass = false;
        }

        return bPass;
    }

    bool m_bOnlyEventsInCut;
    bool m_bUsePtCut;
    double m_dLowPtCut;
    double m_dHighPtCut;
};

template < class TDataExtractor >
class CGraphDrawZPtCutEff
{
public:
    CGraphDrawZPtCutEff()
    {
        m_dZPtStart = 0.0f;
        m_dZPtEnd = 400.0f;
        m_dStepSize = 2.0f;
	m_cutBitmask = 0;
    }

    unsigned int GetPointCount( EventVector & data )
    {
        return (unsigned int)( (m_dZPtEnd - m_dZPtStart) / m_dStepSize);
    }

    class LocalBin
    {
    public:
        LocalBin() : m_iRejectedEvents(0), m_iAllEvents(0)
        {

        }

        PtBin m_bin;
        int m_iRejectedEvents;
        int m_iAllEvents;
    };

    void Draw( TGraphErrors * pGraph, EventVector & data, TDataExtractor selektor  )
    {
        int i = 0;
        int iBinCount = (int)( (m_dZPtEnd - m_dZPtStart) / m_dStepSize);

        for (int iBin = 0; iBin < iBinCount; iBin ++ )
        {
            LocalBin locacBin;
            locacBin.m_bin = PtBin( (double) iBin * m_dStepSize + m_dZPtStart,
                                    (double) (iBin + 1) * m_dStepSize + m_dZPtStart);

            for (EventVector::iterator it = data.begin(); !(it == data.end()); ++it)
            {
	      if ( selektor.IsEventIncluded( *it ))
	      {
                if (locacBin.m_bin.IsInBin( it->m_pData->Z->Pt()))
                {
		    if ( m_cutBitmask == 0 )
		    {
                    if (! it->IsInCut())
                        locacBin.m_iRejectedEvents++;
		    }
		    else
		    {
			// check only for 1 cut
		     	unsigned long res =  ( it->m_cutBitmask & m_cutBitmask );
			if ( res > 0  ) 
			  locacBin.m_iRejectedEvents++; 
		    }
                    locacBin.m_iAllEvents++;
		    
                }
	      }
            }
            // gotta be more like undefined and have no value at all...
            double fCutEff = 1.0f;

            // prevent division by zero
            if ( locacBin.m_iAllEvents > 0 )
                fCutEff = (double) locacBin.m_iRejectedEvents / (double) locacBin.m_iAllEvents;

            std::cout << "CuttEff Bin " <<  locacBin.m_bin.good_id()
                      << " EventsDropped: " << fCutEff
                      << " Rejected: " << locacBin.m_iRejectedEvents
                      << " All Events: " << locacBin.m_iAllEvents << std::endl;
            pGraph->SetPoint( iBin,
                              locacBin.m_bin.GetBinCenter(),
                              fCutEff );

        }
    }

    double m_dStepSize;
    double m_dZPtStart;
    double m_dZPtEnd;
    unsigned long m_cutBitmask;
};


/* Plots the Cut Efficiency over the JetResponse of the events to see
 * wether there is a corellation
 */
template < class TEventSelector >
class CGraphDrawJetResponseCutEff
{
public:
    CGraphDrawJetResponseCutEff() :
            m_dJetResponseStart(0.0),
            m_dJetResponseEnd(2.0),
            m_dStepSize( 0.1 )
    {
    }

    unsigned int GetPointCount( EventVector & data )
    {
        return (unsigned int)( (m_dJetResponseEnd - m_dJetResponseStart) / m_dStepSize);
    }

    class LocalResponseBin
    {
    public:
        LocalResponseBin() : m_iRejectedEvents(0), m_iAllEvents(0)
        {

        }

        PtBin m_bin;
        int m_iRejectedEvents;
        int m_iAllEvents;
    };

    void Draw( TGraphErrors * pGraph, EventVector & data, TEventSelector eventSelector )
    {
        int i = 0;
        unsigned int iBinCount = GetPointCount( data );

        for (unsigned int iBin = 0; iBin < iBinCount; iBin ++ )
        {
            LocalResponseBin locacBin;
            locacBin.m_bin = PtBin( (double) iBin * m_dStepSize + m_dJetResponseStart,
                                    ((double) (iBin + 1)) * m_dStepSize + m_dJetResponseStart);

            for (EventVector::iterator it = data.begin(); !(it == data.end()); ++it)
            {
                if ( eventSelector.IsEventIncluded( *it ))
                {
                    if (locacBin.m_bin.IsInBin( it->GetCorrectedJetResponse()))
                    {
                        if (! it->IsInCut())
                            locacBin.m_iRejectedEvents++;

                        locacBin.m_iAllEvents++;
                    }
                }
            }
            // gotta be more like undefined and have no value at all...
            double fCutEff = 1.0f;

            // prevent division by zero
            if ( locacBin.m_iAllEvents > 0 )
                fCutEff = (double) locacBin.m_iRejectedEvents / (double) locacBin.m_iAllEvents;

            std::cout << "CuttEff Bin " <<  locacBin.m_bin.good_id()
                      << " EventsDropped: " << fCutEff
                      << " Rejected: " << locacBin.m_iRejectedEvents
                      << " All Events: " << locacBin.m_iAllEvents << std::endl;
            pGraph->SetPoint( iBin,
                              locacBin.m_bin.GetBinCenter(),
                              fCutEff );

        }
    }


    double m_dStepSize;
    double m_dJetResponseStart;
    double m_dJetResponseEnd;
};


class PassAllEventSelector
{
public:
    inline bool IsEventIncluded( EventResult & evt )
    {
        return true;
    }
};
/*
class BitmaskCutsEventSelector
{
public:

   // bitmap has got to be 1 at all places where the events must pass the cuts !
    BitmaskCutsEventSelector ( unsigned long bitmap )
    {
	m_bitmap = bitmap;
    }
      
  
    bool IsEventIncluded( EventResult & evt )
    {
	unsigned long res =  ( evt.m_cutBitmask & m_bitmap );
        return ( res > 0 );
    }
    
    unsigned long m_bitmap;
};
*/

class PtBinEventSelector
{
public:
    enum BinWithEnum { ZPtBinning, Jet1PtBinning };

    PtBinEventSelector( bool onlyEventsInCut = true,
                        bool usePtCut = false,
                        double lowPtCut = 0.0f,
                        double highPtCut = 0.0f,
                        BinWithEnum binWith = ZPtBinning )
    {
        m_bOnlyEventsInCut = onlyEventsInCut;
        m_bUsePtCut = usePtCut;
        m_dLowPtCut = lowPtCut;
        m_dHighPtCut = highPtCut;
        m_binWith = binWith;
    }

    inline bool IsEventIncluded( EventResult & evt )
    {
        bool bPass = true;
        // no section here is allowed to set to true again, just to false ! avoids coding errors
        if (this->m_bOnlyEventsInCut)
        {
            if (! evt.IsInCut())
                bPass = false;
        }
        else
        {
            // all events which are valid in jSON file
            // does this work for mc ??
            if (! evt.IsValidEvent() )
                bPass = false;
        }

        if ( m_bUsePtCut )
        {
            double fBinVal;
            if ( m_binWith == ZPtBinning )
                fBinVal = evt.m_pData->Z->Pt();
            else
                fBinVal = evt.GetCorrectedJetPt(0);

            if (!( fBinVal >= this->m_dLowPtCut ))
                bPass = false;

            if (!( fBinVal < this->m_dHighPtCut ))
                bPass = false;
        }

        return bPass;
    }

    bool m_bOnlyEventsInCut;
    bool m_bUsePtCut;
    double m_dHighPtCut;
    double m_dLowPtCut;




    BinWithEnum m_binWith;

};


template < class TData, class TDataDrawer, class TEventSelector >
class CGrapErrorDrawBase : public CDrawBase
{
public:
    typedef boost::ptr_vector<CGraphModifierBase > ModifierVector;

    CGrapErrorDrawBase( TString cName,
                        TFile * pResFile,
                        ModifierVector mods = ModifierVector() ) :  CDrawBase( cName,  cName)
    {
        m_iBinCount = 100;
        m_dBinLower = 0.0;
        m_dBinUpper = 200.0;
        m_lineColor = kBlack;

        m_bDrawLegend = false;

        m_pStoreToFile = pResFile;

        //this->AddModifier(mods);
    }

    ~CGrapErrorDrawBase()
    {
    }


    TDataDrawer m_tdraw;
    
    void Execute( TData data, TEventSelector selector )
    {
        //int entries=m_pChain->GetEntries();
        ModifierVector::iterator iter;

        // apply modifiers
        for (iter = m_graphMods.begin(); !( iter == m_graphMods.end()); ++iter) {
            iter->ModifyBeforeCreation( this );
        }

        ;
        //std::cout << "Generating " << this->m_sName << " ... ";*/
        TCanvas *c = new TCanvas( this->m_sName + "_c", this->m_sCaption,200,10,600,600);
        TGraphErrors * resp_h = new TGraphErrors( m_tdraw.GetPointCount(data ));
        resp_h->SetName(  this->m_sName );
        resp_h->SetMarkerStyle(kFullDotMedium);

        /*                                  this->m_sCaption,
                                          m_iBinCount,m_dBinLower,m_dBinUpper);*/
        //this->StyleHisto( resp_h );

        // apply modifiers
        /*
        for (iter = m_graphMods.begin(); iter != m_graphMods.end(); ++iter) {
            iter->ModifyBeforeDataEntry( c, resp_h );
        }*/
        m_tdraw.Draw( resp_h, data, selector );

        // apply modifiers
        /*
            for (iter = m_graphMods.begin(); iter != m_graphMods.end(); ++iter) {
                iter->ModifyAfterDataEntry( c, resp_h );
            }*/
        /*
        	resp_h->GetYaxis()->SetRangeUser(0.0, resp_h->GetMaximum() * 1.2 );
        */
        resp_h->Draw("pa");

        // apply modifiers
        /* for (iter = m_graphMods.begin(); iter != m_graphMods.end(); ++iter) {
             iter->ModifyAfterDraw( c, resp_h );
         }
        */
        /*
                if (m_bDrawLegend)
                {
                    TLegend *leg = new TLegend(0.66, 0.71, 0.86, 0.81);
                    leg->AddEntry(resp_h,this->m_sCaption,"l");
                }
        */
        if (this->m_bSavePng)
        {
            c->Print( this->m_sOutFolder + this->m_sName + ".png");
        }

        if (this->m_bSavePdf)
        {
            c->Print(this->m_sOutFolder + this->m_sName + ".pdf");
        }

        if (this->m_pStoreToFile != NULL)
        {
            this->m_pStoreToFile->cd();
            resp_h->Write( this->m_sOutFolder + this->m_sName + "_hist");
        }

        //std::cout << "done" << std::endl;
    }
    /*
        void AddModifier( ModifierVector mods)
        {
            ModifierVector::iterator iter;
            for (iter = mods.begin(); iter != mods.end(); ++iter)
            {
                m_graphMods.push_back (*iter);
            }
        }
    */
    void AddModifier( CGraphModifierBase * mod)
    {
        m_graphMods.push_back( mod );

    }

    bool m_bDrawLegend;
    TFile * m_pStoreToFile;

    unsigned int m_iBinCount;
    double m_dBinLower;
    double m_dBinUpper;
    Color_t m_lineColor;


    ModifierVector m_graphMods;
};

class CHistDrawBase : public CDrawBase
{
public:
    CHistDrawBase( TString cName,
                   TFile * pResFile,
                   std::vector<CHistModifierBase *> mods = std::vector<CHistModifierBase *>() ) :  CDrawBase( cName,  cName)
    {
        m_iBinCount = 100;
        m_dBinLower = 0.0;
        m_dBinUpper = 200.0;
        m_lineColor = kBlack;

        m_bDrawLegend = false;

        m_pStoreToFile = pResFile;

        this->AddModifier(mods);
    }

    ~CHistDrawBase()
    {
        std::vector< CHistModifierBase *>::iterator iter;
        for (iter = m_histMods.begin(); iter != m_histMods.end(); ++iter) {
            delete (*iter);
        }
    }

    virtual void StyleHisto( TH1D * pHist)
    {
        pHist->SetLineColor( this->m_lineColor );
    }

    template < class TData >
    void Execute( TData data, CHistDataDrawBase< TData> * pDrawer)
    {
        //int entries=m_pChain->GetEntries();

        std::vector< CHistModifierBase *>::iterator iter;

        // apply modifiers
        for (iter = m_histMods.begin(); iter != m_histMods.end(); ++iter) {
            (*iter)->ModifyBeforeHistCreation( this );
        }

        //std::cout << "Generating " << this->m_sName << " ... ";

        TCanvas *c = new TCanvas( this->m_sName + "_c", this->m_sCaption,200,10,600,600);
        TH1D * resp_h = new TH1D(	this->m_sName,
                                  this->m_sCaption,
                                  m_iBinCount,m_dBinLower,m_dBinUpper);
        resp_h->Sumw2();
        this->StyleHisto( resp_h );

        // apply modifiers
        for (iter = m_histMods.begin(); iter != m_histMods.end(); ++iter) {
            (*iter)->ModifyBeforeDataEntry( c, resp_h );
        }

        pDrawer->Draw( resp_h, data );

        // apply modifiers
        for (iter = m_histMods.begin(); iter != m_histMods.end(); ++iter) {
            (*iter)->ModifyAfterDataEntry( c, resp_h );
        }

        resp_h->GetYaxis()->SetRangeUser(0.0, resp_h->GetMaximum() * 1.2 );

        resp_h->Draw();

        // apply modifiers
        for (iter = m_histMods.begin(); iter != m_histMods.end(); ++iter) {
            (*iter)->ModifyAfterHistoDraw( c, resp_h );
        }

        /*
                if (m_bDrawLegend)
                {
                    TLegend *leg = new TLegend(0.66, 0.71, 0.86, 0.81);
                    leg->AddEntry(resp_h,this->m_sCaption,"l");
                }
        */
        if (this->m_bSavePng)
        {
            c->Print( this->m_sOutFolder + this->m_sName + ".png");
        }

        if (this->m_bSavePdf)
        {
            c->Print(this->m_sOutFolder + this->m_sName + ".pdf");
        }

        if (this->m_pStoreToFile != NULL)
        {
            this->m_pStoreToFile->cd();
            resp_h->Write( this->m_sOutFolder + this->m_sName + "_hist");
        }

        //std::cout << "done" << std::endl;
    }

    void AddModifier( std::vector<CHistModifierBase *> mods)
    {
        std::vector<CHistModifierBase *>::iterator iter;
        for (iter = mods.begin(); iter != mods.end(); ++iter)
        {
            m_histMods.push_back (*iter);
        }
    }

    void AddModifier( CHistModifierBase * mod)
    {
        std::vector<CHistModifierBase *> lonelyVec;

        lonelyVec.push_back( mod );
        this->AddModifier( lonelyVec );
    }

    bool m_bDrawLegend;
    TFile * m_pStoreToFile;

    unsigned int m_iBinCount;
    double m_dBinLower;
    double m_dBinUpper;
    Color_t m_lineColor;

    std::vector<CHistModifierBase *> m_histMods;
};


#pragma region HistMods


//
// predefined modifier lists
//
class CHistEtaMod
{
public:
    static std::vector<CHistModifierBase *> DefaultModifiers ()
    {
        std::vector<CHistModifierBase* > l;

        l.push_back( new CModBinRange( -3.0, 3.0 ));
        return l;
    }
};

class CHistPhiMod
{
public:
    static std::vector<CHistModifierBase *> DefaultModifiers ()
    {
        std::vector<CHistModifierBase* > l;

        l.push_back( new CModBinRange( -3.5, 3.5 ));
        return l;
    }
};
/*
class CHistResponse : public CHistDrawBase
{
public:
    CHistResponse( TString cHistName, TString cHistCaption, TChain * pChain, evtData * pEvtData,
                   TFile * pResFile ) :
            CHistDrawBase( cHistName,cHistCaption, pChain, pEvtData, pResFile) {}

    virtual double GetValue()
    {
        return m_pEvtData->jets[0]->Pt() / m_pEvtData->Z->Pt();
    }
};
*/

#endif

