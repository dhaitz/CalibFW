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
inline void setupFactorProvider(const Tjet &jet, Tprov *prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
}

template<typename Tprov>
inline void setupFactorProvider(const KDataJet &jet, Tprov *prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
	prov->setJetEMF(jet.fEM);
}

template<typename Tprov>
inline void setupFactorProvider(const KDataPFJet &jet, Tprov *prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
	prov->setJetEMF(jet.neutralEMFraction + jet.chargedEMFraction);
}

// Functions to correct a single jet with the FactorizedJetCorrector

template<typename T>
inline void correctSingleJet(T &jet, FactorizedJetCorrector *jec)
{
	setupFactorProvider(jet, jec);
	jet.p4 *= jec->getCorrection();
}

template<>
inline void correctSingleJet(KDataJet &jet, FactorizedJetCorrector *jec)
{
	setupFactorProvider(jet, jec);
	jec->setJetA(jet.area);
	jet.p4 *= jec->getCorrection();
}

template<>
inline void correctSingleJet(KDataPFJet &jet, FactorizedJetCorrector *jec)
{
	setupFactorProvider(jet, jec);
	jec->setJetA(jet.area);
	jet.p4 *= jec->getCorrection();
}

// Functions to apply correction + uncertainty to a single jet:

template<typename T>
inline void applyUncertainty(T &jet, JetCorrectionUncertainty *unc, const JECValueType jv = jec_center)
{
	if ((unc != 0) && (jv != jec_center))
	{
	    // sure you want to do this ?
        assert( false );

		setupFactorProvider(jet, unc);
		if (jv == jec_up)
			jet.p4 *= (1 + unc->getUncertainty(true));
		else

			jet.p4 *= (1 - unc->getUncertainty(false));
	}
}

template<typename T>
inline void correctJets(std::vector<T> *jets,
	FactorizedJetCorrector *jec, JetCorrectionUncertainty *unc,
	const double rho, const int npv, const double area = -1, const JECValueType jv = jec_center)
{
	if (jec == 0)
		return;
	for (size_t idx = 0; idx < jets->size(); ++idx)
	{
		//std::cout << "Rho " << rho << " npv " << npv << std::endl;
		jec->setRho(rho);
		jec->setNPV(npv);
		T &jet = jets->at(idx);
		if (area > 0)
		{
			std::cout << "SETTING FIXED AREA! FAIL !" << std::endl;
			assert(false);
			jet.area = area;
		}
		
		//std::cout << "JetA: " << jet.area << std::endl;
		correctSingleJet(jet, jec);
		applyUncertainty(jet, unc, jv);
	}
	sort_pt(jets);
}

#include <KappaTools/RootTools/FileInterface.h>

class JECService
{
public:
	JECService(FileInterface &fi, const std::string prefix,
			const std::vector<std::string> &level, const std::string algo,
			const double R, const int jeuDir = 0)
		: area(-1), jeuType(jec_center), JEC(0), JEU(0),
			vs(fi.Get<KVertexSummary>("offlinePrimaryVerticesSummary", false)),
			ja(fi.Get<KJetArea>("KT6Area", true, true))
	{
        init(level, jeuDir, prefix, algo);
	}

	JECService(KVertexSummary * vertexSummary,  KJetArea * jetArea, const std::string prefix,
			const std::vector<std::string> &level, const std::string algo,
			const double R, const int jeuDir = 0)
		: area(-1), jeuType(jec_center), JEC(0), JEU(0),
			vs( vertexSummary ),
			ja( jetArea)
    {
        init(level, jeuDir, prefix , algo);
    }

	~JECService()
	{
		delete JEU;
		delete JEC;
	}

	template<typename T>
	inline void correct(T *jets)
	{
		correctJets(jets, JEC, JEU, ja->median, vs->nVertices, area, jeuType);
	}

private:
    void init(const std::vector<std::string> &level, const int jeuDir, const std::string prefix, const std::string algo)
    {
        std::cout << yellow << " * Loading jet energy corrections..." << reset << std::endl << "\t";
		std::vector<JetCorrectorParameters> jecVec;
		for (size_t i = 0; i < level.size(); ++i)
		{
			std::cout << level[i] << " ";
			jecVec.push_back(JetCorrectorParameters(prefix + "_" + level[i] + "_" + algo + ".txt"));
		}
		JEC = new FactorizedJetCorrector(jecVec);
		std::cout << std::endl;

		std::cout << yellow << " * Loading jet energy uncertainties..." << reset << std::endl;
		JEU = new JetCorrectionUncertainty(prefix + "_" + "Uncertainty" + "_" + algo + ".txt");
		if (jeuDir > 0)
			jeuType = jec_up;
		else if (jeuDir < 0)
			jeuType = jec_down;
		else
			jeuType = jec_center;

    }

	double area;
	JECValueType jeuType;
	FactorizedJetCorrector *JEC;
	JetCorrectionUncertainty *JEU;
	KVertexSummary *vs;
	KJetArea *ja;
};

#endif
#endif
