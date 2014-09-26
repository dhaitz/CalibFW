#pragma once

#include <boost/algorithm/string.hpp>
#include <string>

/*
taken from https://twiki.cern.ch/twiki/bin/view/Main/EGammaScaleFactors2012

This producer contains some un-intuitive programming constructs, which are
necessary to account for the different formats of the ROOT files containing
the scale factors (different axes, binning in absolute eta)
*/

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;

class ElectronSFProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	ElectronSFProducer(std::string sffile, std::string id) : ZJetGlobalMetaDataProducerBase(),
		m_sffile(sffile), m_id(id)
	{
		std::string histoname;
		//histoname depending on id
		//pt/eta axes are different for mva/cutbased
		// cutbased id has binning in absolute eta
		if (m_id == "mva")
		{
			histoname = "h_electronScaleFactor_IdIsoSip";
			m_etabins = & m_ybins;
			m_ptbins = & m_xbins;
			reversed_axes = true;
			m_absoluteeta = false;
		}
		else if ((m_id == "loose") || (m_id == "medium") || (m_id == "tight") || (m_id == "veto"))
		{
			histoname = ("sf" + boost::to_upper_copy(m_id));
			m_etabins = & m_xbins;
			m_ptbins = & m_ybins;
			reversed_axes = false;
			m_absoluteeta = true;
		}

		LOG("Loading electron scale factors (" << histoname << "):\n  " << sffile)
		TFile file(sffile.c_str(), "READONLY");
		TH1D* sfhisto = (TH1D*) file.Get(histoname.c_str());

		// Get the pT and eta bin borders
		for (int iy = 0; iy <= sfhisto->GetNbinsY(); iy ++)
			m_ybins.emplace_back(2 * sfhisto->GetYaxis()->GetBinCenter(iy) - sfhisto->GetYaxis()->GetBinLowEdge(iy));
		for (int ix = 0; ix <= sfhisto->GetNbinsX(); ix ++)
			m_xbins.emplace_back(2 * sfhisto->GetXaxis()->GetBinCenter(ix) - sfhisto->GetXaxis()->GetBinLowEdge(ix));

		// Fill the m_sf array with the values from the root histo
		for (int iy = 1; iy <= sfhisto->GetNbinsY(); iy ++)
		{
			for (int ix = 1; ix <= sfhisto->GetNbinsX(); ix ++)
			{
				m_sf[ix - 1][iy - 1] = sfhisto->GetBinContent(ix, iy);
			}
		}
		delete sfhisto;
		file.Close();
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		metaData.sfminus = getscalefactor(metaData.leadingeminus);
		metaData.sfplus = getscalefactor(metaData.leadingeplus);
		metaData.scalefactor = metaData.sfplus * metaData.sfminus;
		return true;
	}

	static std::string Name()
	{
		return "electron_sf_producer";
	}

private:
	float m_sf[12][12];
	std::vector<float> m_xbins;
	std::vector<float> m_ybins;
	std::vector<float>* m_etabins;
	std::vector<float>* m_ptbins;
	std::string m_sffile;
	std::string m_id;
	bool reversed_axes;
	bool m_absoluteeta;

	virtual float getscalefactor(KDataLV const& e) const
	{
		for (unsigned int i = 0; i < m_etabins->size() - 1; i++)
		{
			if (GetEta(e) >= m_etabins->at(i) && GetEta(e) < m_etabins->at(i + 1))
			{
				for (unsigned int j = 0; j < m_ptbins->size() - 1; j++)
				{
					if (e.p4.Pt() >= m_ptbins->at(j) && e.p4.Pt() < m_ptbins->at(j + 1))
					{
						if (reversed_axes)
							return m_sf[j][i];
						else
							return m_sf[i][j];
					}
				}
				return 1.; // not in pt bins
			}
		}
		return 1.;
	}

	virtual double GetEta(KDataLV const& e) const
	{
		if (m_absoluteeta)
			return std::abs(e.p4.Eta());
		else
			return e.p4.Eta();
	}

};


}
