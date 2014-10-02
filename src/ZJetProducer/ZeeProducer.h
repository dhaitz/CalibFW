#pragma once

namespace Artus
{

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

class ValidElectronProducer: public ZJetGlobalProductProducerBase
{
public:

	ValidElectronProducer(std::string id, bool excludeecalgap) : ZJetGlobalProductProducerBase(),
		m_idstr(id), m_excludeecalgap(excludeecalgap)
	{
		m_electronID = ToElectronID(m_idstr);
	}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
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

			// ECAL gap
			if (m_excludeecalgap)
				good_electron = good_electron && (std::abs(it->p4.Eta()) < 1.4442 || std::abs(it->p4.Eta()) > 1.566);

			if (good_electron) //only do the ID if not already vetoed before
			{
				// Electron ID
				// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentificatio
				// different thresholds depending on electron pT, eta
				if (m_electronID == ElectronID::MVA)
				{
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
				}
				else if (m_electronID == ElectronID::LOOSE)
					good_electron = good_electron && it->cutbasedIDloose;
				else if (m_electronID == ElectronID::MEDIUM)
					good_electron = good_electron && it->cutbasedIDmedium;
				else if (m_electronID == ElectronID::TIGHT)
					good_electron = good_electron && it->cutbasedIDtight;
				else if (m_electronID == ElectronID::VETO)
					good_electron = good_electron && it->cutbasedIDveto;
			}

			if (good_electron)
				product.m_listValidElectrons.emplace_back(*it);
			else
				product.m_listInvalidElectrons.emplace_back(*it);
		}

		return true;
	}

	static std::string Name()
	{
		return "valid_electron_producer";
	}
private:
	std::string m_idstr;
	bool m_excludeecalgap;

	enum class ElectronID : int
	{
		NONE  = -1,
		MVA = 0,
		LOOSE = 1,
		MEDIUM = 2,
		TIGHT = 3,
		VETO = 4
	};

	static ElectronID ToElectronID(std::string const& electronID)
	{
		LOG("Using " << electronID << " ElectronID")
		if (electronID == "mva") return ElectronID::MVA;
		else if (electronID == "loose") return ElectronID::LOOSE;
		else if (electronID == "medium") return ElectronID::MEDIUM;
		else if (electronID == "tight") return ElectronID::TIGHT;
		else if (electronID == "veto") return ElectronID::VETO;
		else if (electronID == "none") return ElectronID::NONE;
		else LOG_FATAL("Electron ID of type '" << electronID << "' not known!")
		}

	ElectronID m_electronID;

};


class ZEEProducer: public ZJetGlobalProductProducerBase
{
public:

	ZEEProducer() : ZJetGlobalProductProducerBase(),
		zmassRangeMin(.0), zmassRangeMax(1000.)
	{}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const
	{
		KDataElectrons const& valid_electrons = product.GetValidElectrons();

		if (valid_electrons.size() < 2)
		{
			// no Z to produce here
			product.SetValidZ(false);
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
			product.SetValidZ(false);
			return false;
		}


		product.leadingeminus = valid_electrons.at(leading_minus);
		product.leadingeplus = valid_electrons.at(leading_plus);

		if (product.leadingeplus.p4.Pt() > product.leadingeminus.p4.Pt())
			product.leadinge = product.leadingeplus;
		else
			product.leadinge = product.leadingeminus;


		KDataLV z;
		z.p4 = valid_electrons.at(leading_plus).p4 + valid_electrons.at(leading_minus).p4;

		product.SetZ(z);
		product.SetValidZ(true);
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

/*
    This Producer combines an electron and a muon to a Z-boson.
    To be used for data-driven background estimation studies.
*/
class ZEMuProducer: public ZJetGlobalProductProducerBase
{
public:

	ZEMuProducer() : ZJetGlobalProductProducerBase(),
		//zmassRangeMin(71.19), zmassRangeMax(111.19)
		zmassRangeMin(.0), zmassRangeMax(1000.)
	{}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const
	{
		KDataElectrons const& valid_electrons = product.GetValidElectrons();
		KDataMuons const& valid_muons = product.GetValidMuons();

		if (valid_muons.size() < 1 || valid_electrons.size() < 1 || valid_muons.size() + valid_electrons.size() < 2)
		{
			// too few muons or electrons - no Z to produce here!
			product.SetValidZ(false);
			return false;
		}

		std::vector<KDataLV> z_cand;

		// iterate over all electron-muon combination and look for valid combinations
		for (const auto & m : valid_muons)
		{
			for (const auto & e : valid_electrons)
			{
				if (e.charge + m.charge == 0)
				{
					KDataLV z;
					z.p4 = e.p4 + m.p4;

					if (z.p4.mass() > zmassRangeMin && z.p4.mass() < zmassRangeMax)
						z_cand.emplace_back(z);
				}
			}
		}

		//only return true if unambiguous result (= exactly one candidate found)
		if (z_cand.size() == 1)
		{
			product.SetZ(z_cand[0]);
			product.SetValidZ(true);
			return true;
		}
		else
		{
			product.SetValidZ(false);
			return false;
		}
	}

	static std::string Name()
	{
		return "zemu_producer";
	}

private:
	const double zmassRangeMin;
	const double zmassRangeMax;
};

}
