
#ifndef __DRAWBASE_H__
#define __DRAWBASE_H__

#include <string>
#include <iostream>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"


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


class CHistEvtMapBase : public  CHistDataDrawBase<  EventVector & >
{
public:
    CHistEvtMapBase()
    {
    	m_bOnlyEventsInCut = true;
    	m_bUsePtCut = true;
	m_binWith = ZPtBinning;
    }
  
    enum BinWithEnum { ZPtBinning, Jet1PtBinning };
  
    void HistFill( TH1D * pHist, 
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
    virtual void ModifyBeforeHistCreation( void * pDrawBase  ){};
    virtual void ModifyBeforeDataEntry( TCanvas * pC, TH1 * pHist ) {}
    virtual void ModifyAfterDataEntry( TCanvas * pC, TH1 * pHist ) {}
    virtual void ModifyAfterHistoDraw( TCanvas * pC, TH1 * pHist ) {}
};

class CGraphModifierBase
{
public:
    virtual void ModifyBeforeCreation( void * pDrawBase  ){};
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

template < class TData, class TDataDrawer >
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
      

    virtual double GetValue() {};

    void Execute( TData data )
    {
        //int entries=m_pChain->GetEntries();
	ModifierVector::iterator iter;

        // apply modifiers
        for (iter = m_graphMods.begin(); !( iter == m_graphMods.end()); ++iter) {
            iter->ModifyBeforeCreation( this );
        }

	TDataDrawer tdraw;
        //std::cout << "Generating " << this->m_sName << " ... ";*/
        TCanvas *c = new TCanvas( this->m_sName + "_c", this->m_sCaption,200,10,600,600);
        TGraphErrors * resp_h = new TGraphErrors( tdraw.GetPointCount(data ));
	resp_h->SetName(  this->m_sName );
	
/*                                  this->m_sCaption,
                                  m_iBinCount,m_dBinLower,m_dBinUpper);*/
        //this->StyleHisto( resp_h );

        // apply modifiers
        /*
        for (iter = m_graphMods.begin(); iter != m_graphMods.end(); ++iter) {
            iter->ModifyBeforeDataEntry( c, resp_h );
        }*/

	
	tdraw.Draw( resp_h, data );

        // apply modifiers
	/*
        for (iter = m_graphMods.begin(); iter != m_graphMods.end(); ++iter) {
            iter->ModifyAfterDataEntry( c, resp_h );
        }*/
/*
	resp_h->GetYaxis()->SetRangeUser(0.0, resp_h->GetMaximum() * 1.2 );
*/
        resp_h->Draw();
	
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
      

    virtual double GetValue() {};

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

