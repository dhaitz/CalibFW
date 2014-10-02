#pragma once

namespace Artus
{

/** Combine valid muons to a Z boson.

    There must be 2 or 3 valid muons that fullfil the Z requirements.
    This requires the @see ValidMuonProducer before.
*/
class ZProducer: public ZJetGlobalProductProducerBase
{
public:

	ZProducer() : ZJetGlobalProductProducerBase(),
		zmassRangeMin(71.19), zmassRangeMax(111.19)
	{}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const
	{
		KDataMuons const& valid_muons = product.GetValidMuons();

		if (valid_muons.size() < 2)
		{
			// no Z to produce here
			product.SetValidZ(false);
			return false;
		}

		// old code uses	Z_mass_cut = "60.0 < mass < 120.0"
		std::vector<KDataLV> z_cand;

		if (valid_muons.size() > 3)
		{
			LOG("Warning: 4 valid muons? Skipping Event.")
			product.SetValidZ(false);
			return false;
		}


		// Create all possible Z combinations
		// Note: If we have more than 3 muons in an event, this may produce double counting
		for (unsigned int i = 0; i < valid_muons.size() ; ++i)
		{
			for (unsigned int j = i + 1; j < valid_muons.size(); ++j)
			{
				KDataMuon const& m1 = valid_muons.at(i);
				KDataMuon const& m2 = valid_muons.at(j);

				if (m1.charge + m2.charge == 0)
				{
					KDataLV z;
					z.p4 = m1.p4 + m2.p4;

					if (z.p4.mass() > zmassRangeMin && z.p4.mass() < zmassRangeMax)
						z_cand.emplace_back(z);
				}
			}
		}

		if (z_cand.size() > 1)
		{
			LOG(" -- more than 1 Z boson candidate found." << z_cand.size() << " in an event. Not sure how to combine this. Dropping event for now.")
			product.SetValidZ(false);
			return false;
		}

		if (z_cand.size() == 0)
		{
			product.SetValidZ(false);
			return false;
		}

		product.SetZ(z_cand[0]);
		product.SetValidZ(true);

		return true;
	}

	static std::string Name()
	{
		return "z_producer";
	}

private:
	const double zmassRangeMin;
	const double zmassRangeMax;
};

}
