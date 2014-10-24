#pragma once

namespace Artus
{

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

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
