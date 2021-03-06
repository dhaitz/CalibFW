#ifdef USE_JEC
#ifndef KAPPA_JECTOOLS_H
#define KAPPA_JECTOOLS_H

#include <CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h>
#include <CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h>
#include <CondFormats/JetMETObjects/interface/JetCorrectorParameters.h>
#include <Kappa/DataFormats/interface/Kappa.h>
#include <KappaTools/RootTools/SortTools.h>
#include <KappaTools/Toolbox/IOHelper.h>
//#include "../Toolbox/Math.h"

enum JECValueType { jec_center, jec_up, jec_down };

// Functions to setup the FactorizedJetCorrector / JetCorrectionUncertainty object

template<typename Tprov, typename Tjet>
inline void setupFactorProvider(const Tjet& jet, Tprov* prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
}

template<typename Tprov>
inline void setupFactorProvider(const KDataJet& jet, Tprov* prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
	prov->setJetEMF(jet.fEM);
}

template<typename Tprov>
inline void setupFactorProvider(const KDataPFTaggedJet& jet, Tprov* prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
	prov->setJetEMF(jet.neutralEMFraction + jet.chargedEMFraction);
}

// Functions to correct a single jet with the FactorizedJetCorrector

template<typename T>
inline void correctSingleJet(T& jet, FactorizedJetCorrector* jec, double rundiff)
{
	setupFactorProvider(jet, jec);
	jet.p4 *= jec->getCorrection();
}

template<>
inline void correctSingleJet(KDataJet& jet, FactorizedJetCorrector* jec, double rundiff)
{
	setupFactorProvider(jet, jec);
	jec->setJetA(jet.area);
	jet.p4 *= jec->getCorrection();
}

template<>
inline void correctSingleJet(KDataPFTaggedJet& jet, FactorizedJetCorrector* jec, double rundiff)
{
	setupFactorProvider(jet, jec);
	jec->setJetA(jet.area);
	// double timedependenceCorrectionFactor(double jeteta, int runnumber) {
	const double corrs[5] = {0.0, -0.454e-6, -0.952e-6, 1.378e-6, 0.0};
	double eta = std::abs(jet.p4.Eta());
	double corr = 0.0;
	if (eta < 1.3)
		corr = corrs[0];
	else if (eta < 2.0)
		corr = corrs[1];
	else if (eta < 2.5)
		corr = corrs[2];
	else if (eta < 3.0)
		corr = corrs[3];
	else
		corr = corrs[4];
	double c = 1.0 + corr * rundiff;
	//	std::cout << "eta=" << eta << ", rundiff=" << rundiff << ", corr=" << corr << ", c=" << c << std::endl;
	//	double cfraction = (1.0 + hcalcorr) / c;

	//	jet.HFEMFraction /= c;
	//	jet.HFHadFraction /= c;
	//	jet.chargedEMFraction /= c;
	//	jet.chargedHadFraction /= c;
	//	jet.electronFraction /= c;
	//	jet.muonFraction /= c;
	//	jet.neutralEMFraction /= c;
	//	jet.photonFraction /= c;

	//	jet.neutralHadFraction *= cfraction;
	jet.p4 *= jec->getCorrection() * c;
}

// Functions to apply correction + uncertainty to a single jet:

template<typename T>
inline void applyUncertainty(T& jet, JetCorrectionUncertainty* unc, const JECValueType jv = jec_center)
{
	if ((unc != 0) && (jv != jec_center))
	{
		// sure you want to do this ?
		assert(false);

		setupFactorProvider(jet, unc);
		if (jv == jec_up)
			jet.p4 *= (1 + unc->getUncertainty(true));
		else
			jet.p4 *= (1 - unc->getUncertainty(false));
	}
}

template<typename T>
inline void correctJets(std::vector<T>* jets,
						FactorizedJetCorrector* jec, JetCorrectionUncertainty* unc,
						const double rho, const int npv, const double area = -1, const JECValueType jv = jec_center,
						double hcalcorr = 0.0)
{
	if (jec == 0)
		return;
	for (size_t idx = 0; idx < jets->size(); ++idx)
	{
		//std::cout << "Rho " << rho << " npv " << npv << std::endl;
		jec->setRho(rho);
		jec->setNPV(npv);
		T& jet = jets->at(idx);
		if (area > 0)
		{
			LOG_FATAL("SETTING FIXED AREA!");
			jet.area = area;
		}

		correctSingleJet(jet, jec, hcalcorr);
		applyUncertainty(jet, unc, jv);
	}
	//sort_pt(jets);
}

#include <KappaTools/RootTools/FileInterface.h>

class JECService
{
public:
	JECService(FileInterface& fi,
			   const std::string prefix, const std::vector<std::string>& level, const std::string algo,
			   const double R, const int jeuDir = 0, double rcorr = 0.0)
		: vs(fi.Get<KVertexSummary>("goodOfflinePrimaryVerticesSummary", false)),
		  area(-1), ja(fi.Get<KJetArea>("KT6Area", true, true)),
		  jeuType(jec_center), JEC(0), JEU(0), runCorr(rcorr)
	{
		init(level, jeuDir, prefix, algo);
	}

	JECService(KVertexSummary* vertexSummary, KJetArea* jetArea, KEventMetadata* eventproduct,
			   const std::string prefix, const std::vector<std::string>& level, const std::string algo,
			   const double R, const int jeuDir = 0, double rcorr = 0.0,
			   unsigned int* run = 0)
		: vs(vertexSummary), area(-1), ja(jetArea), evtMeta(eventproduct),
		  jeuType(jec_center), JEC(0), JEU(0), runCorr(rcorr), nRun0(201000)
	{
		init(level, jeuDir, prefix , algo);
		if (rcorr != 0.0)
			LOG("HCAL correction = " << rcorr);
	}

	~JECService()
	{
		delete JEC;
		delete JEU;
	}

	template<typename T>
	inline void correct(T* jets)
	{
		//LOG("Correct jets: (" << evtMeta->nRun << "-" << nRun0 << ") * " << hcalCorr
		//		<< " = " << ((signed int) evtMeta->nRun - nRun0) << " * " << hcalCorr
		//		<< " = " << (((signed int) evtMeta->nRun - nRun0) * hcalCorr))
		correctJets(jets, JEC, JEU, ja->median, vs->nVertices, area, jeuType,
					((signed int) evtMeta->nRun - nRun0)*runCorr);

	}

private:
	void init(const std::vector<std::string>& level, const int jeuDir, const std::string prefix, const std::string algo)
	{
		assert(vs != NULL);
		assert(ja != NULL);

		//std::cout << yellow << " * Loading jet energy corrections..." << reset << std::endl << "\t";
		std::vector<JetCorrectorParameters> jecVec;
		for (size_t i = 0; i < level.size(); ++i)
		{
			std::cout << level[i] << " ";
			jecVec.push_back(JetCorrectorParameters(prefix + "_" + level[i] + "_" + algo + ".txt"));
		}
		JEC = new FactorizedJetCorrector(jecVec);

		//std::cout << yellow << " * Loading jet energy uncertainties..." << reset << std::endl << "\t";
		JEU = new JetCorrectionUncertainty(prefix + "_" + "Uncertainty" + "_" + algo + ".txt");
		if (jeuDir > 0)
			jeuType = jec_up;
		else if (jeuDir < 0)
			jeuType = jec_down;
		else
			jeuType = jec_center;
	}

	KVertexSummary* vs;
	double area;
	KJetArea* ja;
	KEventMetadata* evtMeta;
	JECValueType jeuType;
	FactorizedJetCorrector* JEC;
	JetCorrectionUncertainty* JEU;
	double runCorr;
	signed int nRun0;
};

#endif
#endif
