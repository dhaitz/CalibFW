#pragma once

namespace Artus
{

/** Select only valid muons.

    The muons are valid if the pass these criteria
    1. soft cuts on muons
    1. muon isolation
    1. tight muonID
*/
class ValidMuonProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		// Apply muon isolation and MuonID
		for (KDataMuons::iterator it = data.m_muons->begin();
			 it != data.m_muons->end(); it++)
		{
			bool good_muon = true;

			// Own loose cuts on muons and muon isolation
			good_muon = good_muon
						&& it->p4.Pt() > 12.0
						&& std::abs(it->p4.Eta()) < 5.0
						&& it->trackIso03 < 3.0;

			// Tight MuonID 2012
			// [twiki](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon)
			// The comments describe, how CMSSW treats the recoMu.
			/// version of MuonID
			bool is2011 = globalSettings.Global()->GetMuonID2011();
			good_muon = good_muon
						&& it->isGlobalMuon()
						// use PF muons
						&& it->isPFMuon()
						// normalizedChi2
						&& it->globalTrack.chi2 / it->globalTrack.nDOF < 10.
						// hitPattern().numberOfValidMuonHits
						&& it->globalTrack.nValidMuonHits > 0
						// numberOfMatchedStations
						&& it->nMatches > 1
						// fabs(muonBestTrack()->dxy(vertex->position))
						&& std::abs(it->bestTrack.getDxy(&data.m_vertexSummary->pv)) < 0.2
						// fabs(muonBestTrack()->dz(vertex->position)) // not in 2011
						&& std::abs(it->bestTrack.getDz(&data.m_vertexSummary->pv)) < 0.5 + 99999. * is2011
						// hitPattern().numberOfValidPixelHits()
						&& it->innerTrack.nValidPixelHits > 0
						// hitPattern().trackerLayersWithMeasurement() // 8 in 2011
						&& it->track.nPixelLayers + it->track.nStripLayers > 5 + 3 * is2011;

			if (good_muon)
				metaData.m_listValidMuons.emplace_back(*it);
			else
				metaData.m_listInvalidMuons.emplace_back(*it);
		}

		return true;
	}

	static std::string Name()
	{
		return "valid_muon_producer";
	}
};

}
