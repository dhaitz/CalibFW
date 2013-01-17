/*
 * Sources.h
 *
 *  Created on: Sep 21, 2011
 *      Author: poseidon
 */

#ifndef SOURCES_H_
#define SOURCES_H_

#include "Pipeline/SourceBase.h"
#include "../ZJetEventData.h"
#include "../ZJetMetaData.h"
#include "../ZJetPipelineSettings.h"

namespace CalibFW
{
typedef SourceBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetSourceBase;

class SourceRecoVert: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = (double) event.m_vertexSummary->nVertices;
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 50;
	}
	virtual double GetDefaultLowBin() const
	{
		return -0.5f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 49.5;
	}
};

class SourceTrueVert: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = (double) event.m_geneventmetadata->numPUInteractionsTruth;
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 50;
	}
	virtual double GetDefaultLowBin() const
	{
		return -0.5f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 49.5;
	}
};

class SourceDiMuonPtCombinedEBEB: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		for (unsigned int i = 0; i < metaData.GetValidMuons().size() ; ++i)
		{
			for (unsigned int j = i + 1; j < metaData.GetValidMuons().size(); ++j)
			{
				KDataMuon const& m1 = metaData.GetValidMuons().at(i);
				KDataMuon const& m2 = metaData.GetValidMuons().at(j);
				if (m1.charge + m2.charge == 0)
				{
					KDataLV z;
					z.p4 = m1.p4 + m2.p4;
					if (z.p4.mass() > 71.19 && z.p4.mass() < 111.19)
						if (m1.p4.Eta() < 1.48 && m2.p4.Eta() < 1.48)
						{
							val = (1 / m1.p4.Pt()) + (1 / m2.p4.Pt());
							return true;
						}
				}
			}
		}
	return false;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.0f; }
	virtual double GetDefaultHighBin() const { return 0.2f; }
};

class SourceDiMuonPtCombinedECEC: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		for (unsigned int i = 0; i < metaData.GetValidMuons().size() ; ++i)
		{
			for (unsigned int j = i + 1; j < metaData.GetValidMuons().size(); ++j)
			{
				KDataMuon const& m1 = metaData.GetValidMuons().at(i);
				KDataMuon const& m2 = metaData.GetValidMuons().at(j);
				if (m1.charge + m2.charge == 0)
				{
					KDataLV z;
					z.p4 = m1.p4 + m2.p4;
					if (z.p4.mass() > 71.19 && z.p4.mass() < 111.19)
						if (m1.p4.Eta() > 1.48 && m1.p4.Eta() < 3. &&
							m2.p4.Eta() > 1.48 && m2.p4.Eta() < 3.)
						{
							val = 1.0 / m1.p4.Pt() + 1.0 / m2.p4.Pt();
							return true;
						}
				}
			}
		}
	return false;
	}
	virtual unsigned int GetDefaultBinCount() const
	    {return 100;}
	virtual double GetDefaultLowBin() const
	    {return 0.0f;}
	virtual double GetDefaultHighBin() const
	{return 0.2f;}
};

class SourceDiMuonPtCombinedEBEC: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		for (unsigned int i = 0; i < metaData.GetValidMuons().size() ; ++i)
		{
			for (unsigned int j = i + 1; j < metaData.GetValidMuons().size(); ++j)
			{
				KDataMuon const& m1 = metaData.GetValidMuons().at(i);
				KDataMuon const& m2 = metaData.GetValidMuons().at(j);
				if (m1.charge + m2.charge == 0)
				{
					KDataLV z;
					z.p4 = m1.p4 + m2.p4;
					if (z.p4.mass() > 71.19 && z.p4.mass() < 111.19)
					{
						if (  (m1.p4.Eta() > 1.48 && m1.p4.Eta() < 3. && m2.p4.Eta() < 1.48 )
							||(m1.p4.Eta() < 1.48 && m2.p4.Eta() > 1.48 && m2.p4.Eta() < 3.)  )
						{
							val = (1 / m1.p4.Pt()) + (1 / m2.p4.Pt());
							return true;
						}
					}
				}
			}
		}
	return false;
	}
	virtual unsigned int GetDefaultBinCount() const
	    {return 100;}
	virtual double GetDefaultLowBin() const
	    {return 0.0f;}
	virtual double GetDefaultHighBin() const
	{return 0.2f;}
};



class SourceDiGenMuonPtCombined: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.m_genMuons.size() != 2)
			return false;

		if (metaData.m_genZs.size() >= 1)
		{
			KDataLV test;
			test.p4 = metaData.m_genMuons[0].p4 + metaData.m_genMuons[1].p4 - metaData.m_genZs[0].p4;
			if (test.p4.Pt() < 1e-3)	// differs less than a MeV
			{
				val = 1.0 / metaData.m_genMuons[0].p4.Pt() + 1.0 / metaData.m_genMuons[1].p4.Pt();
				return true;
			}
		}
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 0.2f;
	}
};

class SourceJetEta: public ZJetSourceBase
{
public:
	SourceJetEta() {}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Eta();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -10.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceJetAbsEta: public ZJetSourceBase
{
public:
	SourceJetAbsEta() {}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = std::abs(metaData.GetValidPrimaryJet(settings, event)->p4.Eta());
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceJet2Eta: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2)
			return false;

		val = metaData.GetValidJet(settings, event, 1)->p4.Eta();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -10.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceJet2AbsEta: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2)
			return false;

		val = std::abs(metaData.GetValidJet(settings, event, 1)->p4.Eta());
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceJetPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceJet2Pt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2)
			return false;

		val = metaData.GetValidJet(settings, event, 1)->p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceJetPhi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidPrimaryJet(settings, event)->p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -3.14159f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 3.14159f;
	}
};

class SourceJet2Phi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		
		if (metaData.GetValidJetCount(settings, event) < 2)
			return false;

		val = metaData.GetValidJet(settings, event, 1)->p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -3.14159f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 3.14159f;
	}
};

class SourcePtBalance: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataLV* jet = metaData.GetValidJet(settings, event, 0);
		val = metaData.GetBalance(jet);
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2.0;
	}
};


class SourceGenBalance: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		std::string genName(JetType::GetGenName(settings.GetJetAlgorithm()));

	if (metaData.GetValidJetCount(settings, event, genName) == 0)
		return false;

		KDataLV* jet = metaData.GetValidJet(settings, event, 0, genName);
		val = metaData.GetGenBalance(jet);
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2.0;
	}
};

class SourceDeltaRSecondJetToZ: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2) 
			return false;

		val = ROOT::Math::VectorUtil::DeltaR(metaData.GetValidJet(settings, event, 1)->p4,metaData.GetRefZ().p4);
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 100.0;
	}
};

class SourceDeltaRSecondJetToLeadingJet: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2) 
			return false;

		val = ROOT::Math::VectorUtil::DeltaR(metaData.GetValidJet(settings, event, 1)->p4,metaData.GetValidPrimaryJet(settings, event)->p4);
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 100.0;
	}
};

class SourceMPFresponse: public ZJetSourceBase
{
public:
	virtual bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMPF(metaData.GetMet(event, settings));
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2.0;
	}
};

class SourceMPFResponseDiff: public ZJetSourceBase
{
public:
	virtual bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMPF(metaData.GetMet(event, settings)) - metaData.GetMPF(event.m_pfMet) ;
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2.0;
	}
};

class SourceMPFresponse_notypeI: public SourceMPFresponse
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMPF(event.m_pfMet);
		return true;
	}
};

class SourceValidJets: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetValidJetCount(settings, event, settings.GetJetAlgorithm());
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -0.5f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 99.5;
	}
};

class SourceSumEt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->sumEt;
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 250;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2500.0;
	}
};

class SourceMETPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceMETPtDiff: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->p4.Pt()-event.m_pfMet->p4.Pt();
		return true;
	}
/*
	virtual bool HasDefaultBinCount() const	{ return true;	}
	virtual double HasDefaultBins() const { return true; }*/

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceMETEta: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->p4.Eta();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -10.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceMETPhi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -3.14159f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 3.14159f;
	}
};


class SourceMETPhiDiff: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->p4.Phi()-event.m_pfMet->p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -3.14159f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 3.14159f;
	}
};

class SourceMETFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetMet(event, settings)->p4.Pt() / metaData.GetMet(event, settings)->sumEt;
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 0.5;
	}
};

class SourceRunNumber: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.m_eventmetadata->nRun;
		return true;
	}

	// hardcoded for now ...
	virtual unsigned int GetDefaultBinCount() const
	{
		return 10000;
	}
	virtual double GetDefaultLowBin() const
	{
	  
		return 190000.0;
	}
	virtual double GetDefaultHighBin() const
	{
		return 215000.0;
	}
};

class SourceEventcount: public ZJetSourceBase
{
public:
	SourceEventcount() {}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = 1.0f;
		return true;
	}

	// hardcoded for now ...
	virtual unsigned int GetDefaultBinCount() const
	{
		return 2;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2;
	}
};

class SourceAlpha: public ZJetSourceBase
{
public:
	SourceAlpha() {}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event) < 2)
			return false;

		val = metaData.GetValidJet(settings, event, 1)->p4.Pt() / metaData.GetRefZ().p4.Pt();
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0;
	}
	virtual double GetDefaultHighBin() const
	{
		return  0.5;
	}	
};

class SourceGenAlpha: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		std::string genName(JetType::GetGenName(settings.GetJetAlgorithm()));

		if (metaData.GetValidJetCount(settings, event, genName) < 2) 
			return false;

		KDataLV* jet = metaData.GetValidJet(settings, event, 1, genName);
		val = metaData.GetGenBalance(jet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0;
	}
	virtual double GetDefaultHighBin() const
	{
		return  0.5;
	}	
};

class SourceIntegratedLumi: public ZJetSourceBase
{
public:
	SourceIntegratedLumi() {}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		assert(settings.Global()->GetInputType() == DataInput);
		val = event.GetDataLumiMetadata()->getLumi();

		return true;
	}

	// hardcoded for now ...
	virtual unsigned int GetDefaultBinCount() const
	{
		return 2000;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0;
	}
	virtual double GetDefaultHighBin() const
	{
		//return 0.06; // lumi of year 2011 in atto barn
		return 0.01;
	}
};

class SourceHltPrescale: public ZJetSourceBase
{
public:
	SourceHltPrescale(std::string conf)
	{
		m_hltName = conf;
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		std::string curName = metaData.m_hltInfo->getHLTName(m_hltName);

		if (metaData.m_hltInfo->isAvailable(curName))
			val = metaData.m_hltInfo->getPrescale(curName);
		else
			val = 0.0f;

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}

	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}

	virtual double GetDefaultHighBin() const
	{
		return 200.0;
	}

private:
	std::string m_hltName;
};

class SourceSelectedHltPrescale: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		assert(metaData.GetSelectedHlt() != "");
		val = metaData.m_hltInfo->getPrescale(metaData.GetSelectedHlt());
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}

	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}

	virtual double GetDefaultHighBin() const
	{
		return 200.0;
	}

private:
	std::string m_hltName;
};

class SourceGenMuonMinusPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		for (int i = 0; i < metaData.m_genMuons.size(); ++i)
			if (metaData.m_genMuons[i].charge() < 0)
			{
				val = metaData.m_genMuons[i].p4.Pt();
				return true;
			}
		val = 0.0;
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const { return 200; }
	virtual double GetDefaultLowBin() const { return 0.0f; }
	virtual double GetDefaultHighBin() const { return 1000.0; }
};

class SourceGenMuonPlusPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		for (int i = 0; i < metaData.m_genMuons.size(); ++i)
			if (metaData.m_genMuons[i].charge() > 0)
			{
				val = metaData.m_genMuons[i].p4.Pt();
				return true;
			}
		val = 0.0;
		return false;
	}

	virtual unsigned int GetDefaultBinCount() const { return 200; }
	virtual double GetDefaultLowBin() const { return 0.0f; }
	virtual double GetDefaultHighBin() const { return 1000.0; }
};

class SourceZPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceGenZPt: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefGenZ().p4.Pt();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}
};

class SourceGenZRapidity: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefGenZ().p4.Rapidity();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -5.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 5.0f;
	}
};

class SourceGenZEtaRapidityRatio: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefGenZ().p4.Eta() / metaData.GetRefGenZ().p4.Rapidity();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -2.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 2.0f;
	}
};

class SourceZEta: public ZJetSourceBase
{
public:

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.Eta();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -10.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};

class SourceGenZEta: public ZJetSourceBase
{
public:

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefGenZ().p4.Eta();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -10.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};
class SourceZAbsEta: public ZJetSourceBase
{
public:

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = std::abs(metaData.GetRefZ().p4.Eta());
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}
};
class SourceZPhi: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.Phi();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return -3.14159f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 3.14159f;
	}
};

class SourceZMass: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefZ().p4.mass();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 500;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 500.f;
	}
};
class SourceGenZMass: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = metaData.GetRefGenZ().p4.mass();
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 500;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 500.f;
	}
};

class SourceRho: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		val = event.m_jetArea->median;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 50.f; }
};

class SourceCutValue: public ZJetSourceBase
{
public:
	SourceCutValue(long cutToScore) : m_cutToScore(cutToScore)
	{
	}

	// returns 	1 = cut not passed
	// or 		0 = cut passed
	// The Profile plot, for example, can calculate a mean then
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		bool bPassed;
		if (m_cutToScore > -1)
			bPassed = metaData.IsCutPassed(m_cutToScore);
		else
			bPassed = metaData.IsAllCutsPassed();

		if (bPassed)
			val = 0.0f;
		else
			val = 1.0f;

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 1000.0;
	}

private:
	// if -1 => check for all cuts
	long m_cutToScore;
};

/// jet1->Pt / jet2->Pt
class SourceJetPtRatio: public ZJetSourceBase
{
public:
	SourceJetPtRatio(std::string jetAlgo1, std::string jetAlgo2,
			unsigned int jet1Num, unsigned int jet2Num) :
		m_jetAlgo1(jetAlgo1), m_jetAlgo2(jetAlgo2), m_jet1Num(jet1Num),
				m_jet2Num(jet2Num)
	{
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if (metaData.GetValidJetCount(settings, event, m_jetAlgo1) < m_jet1Num + 1
				|| metaData.GetValidJetCount(settings, event, m_jetAlgo2) < m_jet2Num + 1)
			return false;

		KDataLV * jet1 = metaData.GetValidJet(settings, event, m_jet1Num, m_jetAlgo1);
		KDataLV * jet2 = metaData.GetValidJet(settings, event, m_jet2Num, m_jetAlgo2);
		val = jet1->p4.Pt() / jet2->p4.Pt();

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 100.0;
	}
private:
	std::string m_jetAlgo1, m_jetAlgo2;
	unsigned int m_jet1Num, m_jet2Num;
};


class SourceJetPtAbsDiff: public ZJetSourceBase
{
public:
	SourceJetPtAbsDiff(std::string jetAlgo1, std::string jetAlgo2,
			unsigned int jet1Num, unsigned int jet2Num) :
		m_jetAlgo1(jetAlgo1), m_jetAlgo2(jetAlgo2), m_jet1Num(jet1Num),
				m_jet2Num(jet2Num)
	{}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		if ((metaData.GetValidJetCount(settings, event, m_jetAlgo1) < (m_jet1Num+1)) || (metaData.GetValidJetCount(settings, event, m_jetAlgo2) < (m_jet2Num+1)))
			return false;
		
		KDataLV * jet1 = metaData.GetValidJet(settings, event, m_jet1Num, m_jetAlgo1);
		KDataLV * jet2 = metaData.GetValidJet(settings, event, m_jet2Num, m_jetAlgo2);
		val = jet1->p4.Pt() - jet2->p4.Pt();

		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 200;
	}
	virtual double GetDefaultLowBin() const
	{
		return -100.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 100.0;
	}
private:
	std::string m_jetAlgo1, m_jetAlgo2;
	unsigned int m_jet1Num, m_jet2Num;
};

class SourcePhiAbsDiff: public ZJetSourceBase
{
public:
	SourcePhiAbsDiff(std::string Name1, std::string Name2) :
		m_name1(Name1), m_name2(Name2)
	{
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		float m_val1, m_val2;

		if (m_name1 == "jet1")
			m_val1 = metaData.GetValidPrimaryJet(settings, event)->p4.Phi();
		else if (m_name1 == "jet2")
		{
			if (metaData.GetValidJetCount(settings, event) < 2)
				return false;
			m_val1 = metaData.GetValidJet(settings, event, 1)->p4.Phi();
		}
		else if (m_name1 == "z")
			m_val1 = metaData.GetRefZ().p4.Phi();
		else if (m_name1 == "MET")
			m_val1 = metaData.GetMet(event, settings)->p4.Phi();

		if (m_name2 == "jet1")
			m_val2 = metaData.GetValidPrimaryJet(settings, event)->p4.Phi();
		else if (m_name2 == "jet2")
		{
			if (metaData.GetValidJetCount(settings, event) < 2)
				return false;
			m_val2=metaData.GetValidJet(settings, event, 1)->p4.Phi();
		}
		else if (m_name2 == "z")
			m_val2 = metaData.GetRefZ().p4.Phi();
		else if (m_name2 == "MET")
			m_val2 = metaData.GetMet(event, settings)->p4.Phi();

		val = (ROOT::Math::Pi() - std::abs(m_val1 - m_val2 - ROOT::Math::Pi()));
		return true;
	}

	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 3.14159f;
	}
private:
	std::string m_name1, m_name2;
};

class SourceEtaAbsDiff: public ZJetSourceBase
{
public:

	SourceEtaAbsDiff(std::string Name1, std::string Name2) :
		m_name1(Name1), m_name2(Name2)
	{
	}

	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{

		float m_val1, m_val2;

		if (m_name1 == "jet1")
			m_val1 = metaData.GetValidPrimaryJet(settings, event)->p4.Eta();
		else if (m_name1 == "jet2")
		{
			if (metaData.GetValidJetCount(settings, event) < 2)
				return false;
			m_val1=metaData.GetValidJet(settings, event, 1)->p4.Eta();
		}
		else if (m_name1=="z")
			m_val1 = metaData.GetRefZ().p4.Eta();
		else if (m_name1=="MET")
			m_val1 = metaData.GetMet(event, settings)->p4.Eta();

		if (m_name2 == "jet1")
			m_val2 = metaData.GetValidPrimaryJet(settings, event)->p4.Eta();
		else if (m_name2 == "jet2")
		{
			if (metaData.GetValidJetCount(settings, event) < 2)
			{
				return false;
			}
			m_val2=metaData.GetValidJet(settings, event, 1)->p4.Eta();
		}
		else if (m_name2=="z")
			m_val2 = metaData.GetRefZ().p4.Eta();
		else if (m_name2=="MET")
			m_val2 = metaData.GetMet(event, settings)->p4.Eta();

		val = std::abs(m_val1 - m_val2);

		return true;
	}

	
	virtual unsigned int GetDefaultBinCount() const
	{
		return 100;
	}
	virtual double GetDefaultLowBin() const
	{
		return 0.0f;
	}
	virtual double GetDefaultHighBin() const
	{
		return 10.0;
	}

private:
	std::string m_name1, m_name2;
};

class SourceJet1NeutralEMFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet* pfJet = static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(settings, event));
		val = pfJet->neutralEMFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1NeutralHadFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->neutralHadFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1ChargedEMFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->chargedEMFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1ChargedHadFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->chargedHadFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1HFHadFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->HFHadFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1HFEMFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(settings, event));
		val = pfJet->HFEMFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1electronFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->electronFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

class SourceJet1photonFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->photonFraction;
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 1.f; }
};

//Fractions multiplied by response:

class SourceJet1NeutralEMResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet* pfJet = static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(settings, event));
		val = pfJet->neutralEMFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1NeutralHadResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->neutralHadFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1ChargedEMResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->chargedEMFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1ChargedHadResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->chargedHadFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1HFHadResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->HFHadFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1HFEMResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(settings, event));
		val = pfJet->HFEMFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1electronResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->electronFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

class SourceJet1photonResponseFraction: public ZJetSourceBase
{
public:
	bool GetValue(ZJetEventData const& event, ZJetMetaData const& metaData,
			ZJetPipelineSettings const& settings, double & val) const
	{
		KDataPFJet * pfJet = static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(settings, event) );
		val = pfJet->photonFraction * metaData.GetBalance(pfJet);
		return true;
	}
	virtual unsigned int GetDefaultBinCount() const { return 100; }
	virtual double GetDefaultLowBin() const { return 0.f; }
	virtual double GetDefaultHighBin() const { return 2.f; }
};

}

#endif /* SOURCES_H_ */
