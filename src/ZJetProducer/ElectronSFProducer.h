#pragma once

#include <boost/algorithm/string.hpp>
#include <string>

/*
taken from https://twiki.cern.ch/twiki/bin/view/Main/EGammaScaleFactors2012
*/

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;

class ElectronSFProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	ElectronSFProducer(std::string sffile, std::string id) : ZJetGlobalMetaDataProducerBase(),
		m_sffile(sffile), m_id("sf" + boost::to_upper_copy(id))
	{
		LOG("Loading electron scale factors (" << m_id << "):\n  " << sffile)
		TFile file(sffile.c_str(), "READONLY");
		TH1D* sfhisto = (TH1D*) file.Get(m_id.c_str());

		// Get the pT and eta bin borders
		for (int iy = 0; iy <= sfhisto->GetNbinsY(); iy ++)
			ptbins.push_back(2 * sfhisto->GetYaxis()->GetBinCenter(iy) - sfhisto->GetYaxis()->GetBinLowEdge(iy));
		for (int ix = 0; ix <= sfhisto->GetNbinsX(); ix ++)
			etabins.push_back(2 * sfhisto->GetXaxis()->GetBinCenter(ix) - sfhisto->GetXaxis()->GetBinLowEdge(ix));

		// Fill the m_sf array with the values from the root histo
		for (int iy = 1; iy <= sfhisto->GetNbinsY(); iy ++) //pt
		{
			for (int ix = 1; ix <= sfhisto->GetNbinsX(); ix ++) // eta
			{
				float sf = sfhisto->GetBinContent(ix, iy);
				m_sf[ix - 1][iy - 1] = sf;
			}
		}
		delete sfhisto;
		file.Close();
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		metaData.sfplus = 0;
		metaData.sfminus = 0;

		// e minus
		KDataElectron electron = metaData.leadingeminus;
		for (unsigned int i = 0; i < etabins.size(); i++)
		{
			if (std::abs(electron.p4.Eta()) > etabins[i] && std::abs(electron.p4.Eta()) < etabins[i + 1])
			{
				for (unsigned int j = 0; j < ptbins.size(); j++)
				{
					if (electron.p4.Pt() > ptbins[j] && electron.p4.Pt() < ptbins[j + 1])
					{
						metaData.sfminus = m_sf[i][j];
						continue;
					}
				}
				continue;
			}
		}

		// e plus
		electron = metaData.leadingeplus;
		for (unsigned int i = 0; i < etabins.size(); i++)
		{
			if (std::abs(electron.p4.Eta()) > etabins[i] && std::abs(electron.p4.Eta()) < etabins[i + 1])
			{
				for (unsigned int j = 0; j < ptbins.size(); j++)
				{
					if (electron.p4.Pt() > ptbins[j] && electron.p4.Pt() < ptbins[j + 1])
					{
						metaData.sfplus = m_sf[i][j];
						continue;
					}
				}
				continue;
			}
		}


		metaData.scalefactor = metaData.sfplus * metaData.sfminus;
		return true;
	}

	static std::string Name()
	{
		return "electron_sf_producer";
	}

private:
	float m_sf[5][6];
	std::vector<float> etabins;
	std::vector<float> ptbins;
	std::string m_sffile;
	std::string m_id;
};


}
