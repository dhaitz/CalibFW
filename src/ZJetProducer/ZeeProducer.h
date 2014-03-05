#pragma once

// from ROOT
#include <Math/VectorUtil.h>
#include "ZJetEventPipeline/Pipeline.h"

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;

class ValidElectronProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		for (KDataElectrons::iterator it = data.m_electrons->begin();
			 it != data.m_electrons->end(); it++)
		{
			bool good_electron = true;

			// Own loose cuts on electrons and electron isolation
			good_electron = good_electron
							&& it->p4.Pt() > 2.0
							&& std::abs(it->p4.Eta()) < 5.0;
			//&& it->trackIso03 < 3.0
			//&& it->idMvaNonTrigV0 > 0;

			// Electron ID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentificatio
			// different thresholds depending on electron pT, eta
			good_electron = good_electron &&
							(
								(
									(it->p4.Pt() < 10)
									&&
									(
										(abs(it->p4.Eta()) < 0.8 && it->idMvaNonTrigV0 > 0.47)
										|| (abs(it->p4.Eta()) > 0.8 && abs(it->p4.Eta()) < 1.479 && it->idMvaNonTrigV0 > 0.004)
										|| (abs(it->p4.Eta()) > 1.479 && abs(it->p4.Eta()) < 2.5 && it->idMvaNonTrigV0 > 0.295)
									)
								)
								||
								(
									(it->p4.Pt() > 10) &&
									(
										(abs(it->p4.Eta()) < 0.8 && it->idMvaNonTrigV0 > -0.34)
										|| (abs(it->p4.Eta()) > 0.8 && abs(it->p4.Eta()) < 1.479 && it->idMvaNonTrigV0 > -0.65)
										|| (abs(it->p4.Eta()) > 1.479 && abs(it->p4.Eta()) < 2.5 && it->idMvaNonTrigV0 > 0.6)
									)
								)
							);

			good_electron = good_electron
							&& it->track.nInnerHits <= 1
							&& (it->trackIso04 / it->p4.Pt()) < 0.4;

			if (good_electron)
				metaData.m_listValidElectrons.push_back(*it);
			else
				metaData.m_listInvalidElectrons.push_back(*it);
		}

		return true;
	}

	static std::string Name()
	{
		return "valid_electron_producer";
	}
};


class ZEEProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	ZEEProducer() : ZJetGlobalMetaDataProducerBase(),
		//zmassRangeMin(71.19), zmassRangeMax(111.19)
		zmassRangeMin(.0), zmassRangeMax(1000.)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		KDataElectrons const& valid_electrons = metaData.GetValidElectrons();

		if (valid_electrons.size() < 2)
		{
			// no Z to produce here
			metaData.SetValidZ(false);
			return false;
		}

		float pt_minus = 0.;
		float pt_plus = 0.;
		int leading_plus = -1;
		int leading_minus = -1;
		for (unsigned int i = 0; i < valid_electrons.size() ; ++i)
		{
			if (valid_electrons.at(i).p4.Pt() > pt_plus && valid_electrons.at(i).charge == 1)
			{
				leading_plus = i;
				pt_plus = valid_electrons.at(i).p4.Pt();
			}

			else if (valid_electrons.at(i).p4.Pt() > pt_minus && valid_electrons.at(i).charge == -1)
			{
				leading_minus = i;
				pt_minus = valid_electrons.at(i).p4.Pt();
			}

		}

		if (leading_minus == -1 || leading_plus == -1)
		{
			// no Z to produce here
			metaData.SetValidZ(false);
			return false;
		}


		metaData.leadingeminus = valid_electrons.at(leading_minus);
		metaData.leadingeplus = valid_electrons.at(leading_plus);

		if (metaData.leadingeplus.p4.Pt() > metaData.leadingeminus.p4.Pt())
			metaData.leadinge = metaData.leadingeplus;
		else
			metaData.leadinge = metaData.leadingeminus;


		KDataLV z;
		z.p4 = valid_electrons.at(leading_plus).p4 + valid_electrons.at(leading_minus).p4;

		metaData.SetZ(z);
		metaData.SetValidZ(true);
		return true;
	}

	static std::string Name()
	{
		return "zee_producer";
	}

private:
	const double zmassRangeMin;
	const double zmassRangeMax;
};

}
