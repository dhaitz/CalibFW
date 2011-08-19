#pragma once

#include "EventPipeline.h"
#include "PipelineSettings.h"

#include <boost/algorithm/string/predicate.hpp>


#include "DataFormats/interface/Kappa.h"
#include "DataFormats/interface/KDebug.h"

//#include "RootTools/libKRootTools.h"
#include "RootTools/FileInterface.h"

#include "EventData.h"
#include "CutHandler.h"
//#include "ZJetFilter.h"

namespace CalibFW
{





class ZJetPipelineSettings
{

public:
	ZJetPipelineSettings()
	{
		SetSettingsRoot("default");
		//		SetEnableReweighting(false);
	}

	enum WriteEventsEnum
	{
		NoEvents, OnlyInCutEvents, AllEvents
	};

IMPL_PROPERTY(boost::property_tree::ptree * , PropTree)
IMPL_PROPERTY(std::string, SettingsRoot)
IMPL_PROPERTY(unsigned long, OverallNumberOfProcessedEvents )

IMPL_SETTING( bool, EnableCaloMatching)
IMPL_SETTING( bool, EnableReweighting)

	// Filter Settings
IMPL_SETTING(double, FilterPtBinLow)
IMPL_SETTING(double, FilterPtBinHigh)
IMPL_SETTING( int, FilterRecoVertLow)
IMPL_SETTING( int, FilterRecoVertHigh)
IMPL_SETTING(double, FilterJetEtaLow)
IMPL_SETTING(double, FilterJetEtaHigh)

IMPL_SETTING(double, FilterSecondJetRatioLow)
IMPL_SETTING(double, FilterSecondJetRatioHigh)
IMPL_SETTING(unsigned long, FilterInCutIgnored)

IMPL_SETTING(bool, Filter2ndJetPtCutSet)
IMPL_SETTING(bool, FilterDeltaPhiCutSet)

IMPL_SETTING(std::string, JetAlgorithm)
IMPL_SETTING(std::string, RootFileFolder)
IMPL_SETTING(std::string, SecondLevelFolderTemplate)

	// only level 1 runs directly on data
IMPL_SETTING(int, Level)

IMPL_SETTING(std::string, WriteEvents)

	WriteEventsEnum GetWriteEventsEnum()
	{
		if (this->GetWriteEvents() == "all")
			return AllEvents;
		if (this->GetWriteEvents() == "incut")
			return OnlyInCutEvents;

		return NoEvents;
	}

	// Cut settings
IMPL_SETTING(std::string, CutHLT)
IMPL_SETTING(double, CutMuonPt)
IMPL_SETTING(double, CutZMassWindow)
IMPL_SETTING(double, CutMuEta)
IMPL_SETTING(double, CutZPt)
IMPL_SETTING(double, CutMuonEta)
IMPL_SETTING(double, CutLeadingJetEta)
IMPL_SETTING(double, CutSecondLeadingToZPt)
IMPL_SETTING(double, CutSecondLeadingToZPtJet2Threshold)
IMPL_SETTING(double, CutSecondLeadingToZPtDeltaR)
IMPL_SETTING(double, CutBack2Back)
IMPL_SETTING(double, CutJetPt)

	// if a cut is enabled, the bit corresponding to the cut id it set in this integer
IMPL_PROPERTY(unsigned long, CutEnabledBitmask)

IMPL_PROPERTY(TFile *, RootOutFile)

	InputTypeEnum GetInputType()
	{
		std::string sInp = GetPropTree()->get<std::string> ("InputType");

		InputTypeEnum inp = McInput;

		if (sInp == "mc")
			inp = McInput;
		else if (sInp == "data")
			inp = DataInput;
		else
		CALIB_LOG_FATAL("Input type " + sInp + " not supported.")

		return inp;
	}

	VarCache<stringvector> m_filter;

	stringvector GetFilter() const
	{
		RETURN_CACHED( m_filter, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".Filter") )
	}

	VarCache<stringvector> m_L2Corr;

	stringvector GetL2Corr()
	{
		RETURN_CACHED( m_L2Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L2Corr") )
	}

	VarCache<stringvector> m_L3Corr;

	stringvector GetL3Corr()
	{
		RETURN_CACHED( m_L3Corr, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".L3Corr") )
	}

	VarCache<stringvector> m_additionalConsumer;

	stringvector GetAdditionalConsumer()
	{
		RETURN_CACHED( m_additionalConsumer, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".AdditionalConsumer") )
	}

	static std::vector<PtBin> GetAsPtBins(stringvector & sv)
	{
		std::vector<PtBin> bins;

		int i = 0;
		for (stringvector::iterator it = (sv.begin() + 1); it < sv.end(); it++)
		{
			int ilow = atoi(sv[i].c_str());
			int ihigh = atoi(sv[i + 1].c_str());

			bins.push_back(PtBin(ilow, ihigh));
			i++;
		}

		return bins;
	}

	VarCache<doublevector> m_recovertWeight;

	doublevector GetRecovertWeight()
	{
		RETURN_CACHED( m_recovertWeight, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetSettingsRoot() + ".RecovertWeight") )
	}

	VarCache<stringvector> m_jetRespBins;

	stringvector GetCustomBins()
	{
		RETURN_CACHED( m_jetRespBins, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetSettingsRoot() + ".CustomBins") )
	}
	// TODO: maybe cache this for better performance

	VarCache<stringvector> m_cuts;

	stringvector GetCuts() const
	{
		RETURN_CACHED( m_cuts,PropertyTreeSupport::GetAsStringList( GetPropTree(),GetSettingsRoot() + ".Cuts" ) )
	}
	/*
	 void AddFilter( std::string sFilterId)
	 {
	 m_filter.push_back( sFilterId);
	 }

	 stringvector m_filter;*/

};




class ZJetMetaData : public CalibFW::EventMetaDataBase
{
public:
	ZJetMetaData()
	{
		SetCutBitmask(0);
		SetValidZ( false);
	}

	double GetWeight() const { return 1.0f; }

	// cutPassed is true, if the event was not dropped by the cut
	void SetCutResult( long cutId, bool cutPassed )
	{
		// ensure the bit is removed if it was set before
		this->SetCutBitmask (  (  (! cutPassed) * cutId ) | (  GetCutBitmask() & ( ~ cutId ) ));
	}

	bool HasValidZ() const
	{
		return this->GetValidZ();
	}

	bool IsAllCutsPassed() const
	{
		return ( this->GetCutBitmask() == 0 );
	}

	bool IsCutPassed( long cutId ) const
	{
		return ( this->GetCutBitmask() & cutId ) == 0;
	}

	IMPL_PROPERTY_READONLY(long, CutBitmask);
	IMPL_PROPERTY(bool, ValidZ);
	IMPL_PROPERTY(KDataLV, Z);

	KDataMuons const& GetValidMuons() const
	{
		return m_listValidMuons;
	}

	KDataMuons m_listValidMuons;
	KDataMuons m_listInvalidMuons;
};



class ZJetEventData
{


public:
	KDataPFJets * PF_jets;// = fi.Get<KDataPFJets>(names[0]);

	// contains
	KDataPFJets * m_primaryJetCollection;
	KDataMuons * m_muons;

	typedef std::map<std::string, KDataPFJets * > PfMap;
	typedef std::map<std::string, KDataJets * > JetMap;
//typedef std::map<std::string, K * > GenMap;
	PfMap m_pfJets;
	JetMap m_jets;


	// May return null, if no primary jet is available
	virtual KDataLV * GetPrimaryJet ( ZJetPipelineSettings const& psettings ) const
		{
			return GetJet( psettings, 0);
		}

	// May return null, if the jet is not available
	virtual KDataLV * GetJet(ZJetPipelineSettings const& psettings,
			unsigned int index ) const
	{
		if ( boost::algorithm::starts_with( psettings.GetJetAlgorithm(), "AK5PF" ) ||
			boost::algorithm::starts_with( psettings.GetJetAlgorithm(), "AK7PF" ) ||
			boost::algorithm::starts_with( psettings.GetJetAlgorithm(), "KT4PF" ) ||
			boost::algorithm::starts_with( psettings.GetJetAlgorithm(), "KT6PF" ))
		{
			KDataPFJets * pfJets = m_pfJets.at(psettings.GetJetAlgorithm());

			if (pfJets->size() <= index )
				return NULL;

			return &pfJets->at(index);
		}
		else
		{
			KDataJets * jets = m_jets.at(psettings.GetJetAlgorithm());

			if (jets->size() >= index )
				return NULL;

			return &jets->at(index);
		}
	}

	KDataMuons * Muons;
	KEventMetadata * m_eventmetadata;
	KGenEventMetadata * m_geneventmetadata;

	KVertexSummary * m_primaryVertex;

	// convinience functions
	//double GetPrimaryJetPt() const { return PF_Jets->at(0).Pt(); }
	//KDataPFJet const& GetPrimaryJet() const { return PF_Jets->at(0); }

/*
	template <class TJetType>
	std::vector<TJetType> * GetJets(std::string jetAlgo) const
	{
		return m_fi->Get< std::vector<TJetType> >( jetAlgo );
	}*/
/*
	KDataLV GetPrimaryJet( std::string sJetAlgo)
	{
		return m_fi->Get<KDataLVs>(sJetAlgo)->at(0);
	}
*/
private:


};

typedef EventPipeline<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetPipeline;
typedef FilterBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetFilterBase;

/*
class RecoVertFilter: public ZJetFilterBase
{
public:
	RecoVertFilter() :
		ZJetFilterBase()
	{

	}

	virtual bool DoesEventPass(const ZJetEventData & event)
	{
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (event.GetRecoVerticesCount()
				>= GetPipelineSettings()->GetFilterRecoVertLow()
				&& event.GetRecoVerticesCount()
						<= GetPipelineSettings()->GetFilterRecoVertHigh());
		// todo
		return true;
	}

	virtual std::string GetFilterId()
	{
		return "recovert";
	}
};*/
/*
class JetEtaFilter: public ZJetFilterBase
{
public:
	JetEtaFilter() :
		ZJetFilterBase()
	{

	}

	virtual bool DoesEventPass(const ZJetEventData & event)
	{
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (TMath::Abs(event.m_pData->jets[0]->Eta())
				>= GetPipelineSettings()->GetFilterJetEtaLow() && TMath::Abs(
				event.m_pData->jets[0]->Eta())
				<= GetPipelineSettings()->GetFilterJetEtaHigh());
		// todo
		return true;
	}

	virtual std::string GetFilterId()
	{
		return "jeteta";
	}
};
*/
/*
class SecondJetRatioFilter: public ZJetFilterBase
{
public:
	SecondJetRatioFilter() :
		ZJetFilterBase()
	{
	}

	virtual bool DoesEventPass(const ZJetEventData & event)
	{
		bool bPass = true;
		double fBinVal = 0.0f; // todo  event.GetCorrectedJetPt(1) / event.m_pData->Z->Pt();

/*		if (!(fBinVal >= GetPipelineSettings()->GetFilterSecondJetRatioLow()))
			bPass = false;

		if (!(fBinVal < GetPipelineSettings()->GetFilterSecondJetRatioHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "secondjetratio";
	}
};
*/
class PtWindowFilter: public ZJetFilterBase
{
public:

	enum BinWithEnum
	{
		ZPtBinning, Jet1PtBinning
	};
	BinWithEnum m_binWith;

	PtWindowFilter() :
		ZJetFilterBase(), m_binWith(ZPtBinning)
	{
	}

	PtWindowFilter( BinWithEnum binValue  ) :
		ZJetFilterBase(), m_binWith( binValue)
	{
	}


	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		double fBinVal;

		if (m_binWith == ZPtBinning)
		{
			if ( !metaData.HasValidZ() )
				return false;
			fBinVal = metaData.GetRefZ().p4.Pt();
		}
		else
		{
			KDataLV * pJet = event.GetPrimaryJet( settings );
			if ( pJet == NULL)
				return false;

			fBinVal = pJet->p4.Pt();
		}


		if (!(fBinVal >= settings.GetFilterPtBinLow()))
			bPass = false;

		if (!(fBinVal < settings.GetFilterPtBinHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "ptbin";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		std::stringstream s;

		if (bVerbose)
		{
			s << "Binned with ";
			if (m_binWith == ZPtBinning)
				s << " ZPt ";
			else
				s << " Jet1Pt ";
/*
			s << " from " << GetPipelineSettings()->GetFilterPtBinHigh()
					<< " to " << GetPipelineSettings()->GetFilterPtBinLow();*/
		}
		else
		{/*
			s << "Pt" << std::setprecision(0)
					<< GetPipelineSettings()->GetFilterPtBinLow() << "to"
					<< std::setprecision(0)
					<< GetPipelineSettings()->GetFilterPtBinHigh();*/
		}
		return s.str();
	}


};
/*
// Allows to select only events with a specific cut signature
class CutSelectionFilter: public ZJetFilterBase
{
public:
	CutSelectionFilter() :
		ZJetFilterBase()
	{

	}

	virtual bool DoesEventPass( const  ZJetEventData & event)
	{/*
		event.IsCutInBitmask(16);

		if (GetPipelineSettings()->GetFilter2ndJetPtCutSet()
				!= event.IsCutInBitmask(16))
			return false;

		if (GetPipelineSettings()->GetFilterDeltaPhiCutSet()
				!= event.IsCutInBitmask(32))
			return false;

		// todo
		return true;
	}

	virtual std::string GetFilterId()
	{
		return "cutselection";
	}

};*/

class InCutFilter: public ZJetFilterBase
{
public:

	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings)
	{
		//unsigned long ignoredCut = settings.GetFilterInCutIgnored();
		// no section here is allowed to set to true again, just to false ! avoids coding errors
		//return event.IsInCutWhenIgnoringCut(ignoredCut);
		// todo
		return metaData.IsAllCutsPassed();
	}

	virtual std::string GetFilterId()
	{
		return "incut";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "InCut";
	}
};

class ValidMuonsFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings)
	{
		return metaData.HasValidZ();
	}

	virtual std::string GetFilterId()
	{
		return "valid_muons";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Valid Muons Filter";
	}

};

class ZJetPipelineInitializer: public PipelineInitilizerBase<ZJetEventData,
		ZJetMetaData, ZJetPipelineSettings>
{
public:
	virtual void InitPipeline(ZJetPipeline * pLine, ZJetPipelineSettings const& pset) const;
};

}
