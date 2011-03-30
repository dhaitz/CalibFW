#pragma once

#include "FilterBase.h"
#include "ZJetPipeline.h"

namespace CalibFW {

typedef FilterBase<EventResult, ZJetPipelineSettings> ZJetFilterBase;

class RecoVertFilter: public  ZJetFilterBase{
public:
	RecoVertFilter() :
		ZJetFilterBase () {

	}

	virtual bool DoesEventPass(EventResult & event) {
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (event.GetRecoVerticesCount()
				>= GetPipelineSettings()->GetFilterRecoVertLow()
				&& event.GetRecoVerticesCount()
						<= GetPipelineSettings()->GetFilterRecoVertHigh());
	}

	virtual std::string GetFilterId() {
		return "recovert";
	}
};

class JetEtaFilter: public ZJetFilterBase {
public:
	JetEtaFilter() :
		ZJetFilterBase () {

	}

	virtual bool DoesEventPass(EventResult & event) {
		//return (  event.GetRecoVerticesCount()  == m_pipelineSettings->GetFilterRecoVertLow() );
		return (TMath::Abs(event.m_pData->jets[0]->Eta())
				>= GetPipelineSettings()->GetFilterJetEtaLow() && TMath::Abs(
				event.m_pData->jets[0]->Eta())
				<= GetPipelineSettings()->GetFilterJetEtaHigh());
	}

	virtual std::string GetFilterId() {
		return "jeteta";
	}
};

class SecondJetRatioFilter: public ZJetFilterBase {
public:
	SecondJetRatioFilter() :
		ZJetFilterBase () {
	}

	virtual bool DoesEventPass(EventResult & event) {
		bool bPass = true;
		double fBinVal = event.GetCorrectedJetPt(1) / event.m_pData->Z->Pt();

		if (!(fBinVal >= GetPipelineSettings()->GetFilterSecondJetRatioLow()))
			bPass = false;

		if (!(fBinVal < GetPipelineSettings()->GetFilterSecondJetRatioHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId() {
		return "secondjetratio";
	}
};

class PtWindowFilter: public ZJetFilterBase{
public:
	PtWindowFilter() :
		ZJetFilterBase (), m_binWith(ZPtBinning) {

	}

	virtual bool DoesEventPass(EventResult & event) {
		bool bPass = true;
		double fBinVal;
		if (m_binWith == ZPtBinning)
			fBinVal = event.m_pData->Z->Pt();
		else
			fBinVal = event.GetCorrectedJetPt(0);

		if (!(fBinVal >= GetPipelineSettings()->GetFilterPtBinLow()))
			bPass = false;

		if (!(fBinVal < GetPipelineSettings()->GetFilterPtBinHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId() {
		return "ptbin";
	}

	virtual std::string ToString(bool bVerbose = false) {
		std::stringstream s;

		if (bVerbose) {
			s << "Binned with ";
			if (m_binWith == ZPtBinning)
				s << " ZPt ";
			else
				s << " Jet1Pt ";

			s << " from " << GetPipelineSettings()->GetFilterPtBinHigh() << " to "
					<< GetPipelineSettings()->GetFilterPtBinLow();
		} else {
			s << "Pt" << std::setprecision(0)
					<< GetPipelineSettings()->GetFilterPtBinLow() << "to"
					<< std::setprecision(0)
					<< GetPipelineSettings()->GetFilterPtBinHigh();
		}
		return s.str();
	}

	enum BinWithEnum {
		ZPtBinning, Jet1PtBinning
	};
	BinWithEnum m_binWith;
};

class InCutFilter: public ZJetFilterBase {
public:

	virtual bool DoesEventPass(EventResult & event) {
		unsigned long ignoredCut = GetPipelineSettings()->GetFilterInCutIgnored();
		// no section here is allowed to set to true again, just to false ! avoids coding errors
		return event.IsInCutWhenIgnoringCut(ignoredCut);
	}

	virtual std::string GetFilterId() {
		return "incut";
	}

	virtual std::string ToString(bool bVerbose = false) {
		return "InCut";
	}

};

}

