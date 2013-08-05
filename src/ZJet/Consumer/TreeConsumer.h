#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "Draw/Profile.h"

#include "src/Pipeline/SourceBase.h"
#include "RootTools/RootIncludes.h"




namespace CalibFW
{

template < class TEvent, class TMetaData, class TSettings >
class TreeConsumerBase : public EventConsumerBase< TEvent, TMetaData, TSettings>
{
public:
	typedef SourceBase<TEvent, TMetaData, TSettings> SourceTypeForThis;
	typedef EventPipeline<TEvent, TMetaData, TSettings> PipelineTypeForThis;
};

class TreeConsumer : public TreeConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings >
{

    static std::string GetName(){return "tree_consumer";}

	void Init(PipelineTypeForThis * pset)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::Init( pset );
        std::string quantities = this->GetPipelineSettings().GetQuantitiesString();
        m_tree = new TNtuple("NTuple", "NTuple", quantities.c_str());
    }

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::ProcessFilteredEvent( event, metaData);
        
        // get the list (stringvector) of variables 
        std::vector<std::string> v = this->GetPipelineSettings().GetQuantities();

        std::vector<float> arr;

        // fill the array with values according to the variable-list :
        for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) 
            arr.push_back(returnvalue(*it, event, metaData, this->GetPipelineSettings()));

        // add the array to the tree
        m_tree->Fill(&arr[0]);
	}

	virtual void Finish()
	{
        ZJetPipelineSettings s = this->GetPipelineSettings();
        RootFileHelper::SafeCd( s.GetRootOutFile(), 
                            s.GetRootFileFolder());
        m_tree->Write((s.GetTreename()+"_" + s.GetJetAlgorithm()).c_str());
	}

private:

	std::string m_name;
    TNtuple * m_tree;

    enum type {
        npv, rho, run, weight, zpt, zeta, zphi, zmass, jet1pt, jet1eta, jet1phi,
        jet1photonfraction, jet1chargedemfraction, jet1chargedhadfraction, 
        jet1neutralhadfraction, jet1muonfraction, jet1HFhadfraction, 
        jet1HFemfraction, jet2pt, jet2phi, jet2eta, METpt, METphi, sumEt, 
        rawMETpt, rawMETphi, uept, uephi, ueeta, mpf, rawmpf, otherjetspt, 
        otherjetsphi, otherjetseta, genjet1pt, genjet1eta, genjet1phi, 
        matchedgenjet1pt, genjet2pt, genzpt, genmpf, algoflavour, physflavour
    } var;
    
    float returnvalue(std::string string, ZJetEventData const& event,
			                    ZJetMetaData const& metaData, ZJetPipelineSettings const& s){

    int n;
	if (string=="npv") var=npv;
    else if (string=="rho") var=rho;
    else if (string=="run") var=run;
    else if (string=="weight") var=weight;
    else if (string=="zpt") var=zpt;
    else if (string=="zeta") var=zeta;
    else if (string=="zphi") var=zphi;
    else if (string=="zmass") var=zmass;
    else if (string=="jet1pt") var=jet1pt;
    else if (string=="jet1eta") var=jet1eta;
    else if (string=="jet1phi") var=jet1phi;
    else if (string=="jet1photonfraction") var=jet1photonfraction;
    else if (string=="jet1chargedemfraction") var=jet1chargedemfraction;
    else if (string=="jet1chargedhadfraction") var=jet1chargedhadfraction;
    else if (string=="jet1neutralhadfraction") var=jet1neutralhadfraction;
    else if (string=="jet1muonfraction") var=jet1muonfraction;
    else if (string=="jet1HFhadfraction") var=jet1HFhadfraction;
    else if (string=="jet1HFemfraction") var=jet1HFemfraction;
    else if (string=="jet2pt") var=jet2pt;
    else if (string=="jet2phi") var=jet2phi;
    else if (string=="jet2eta") var=jet2eta;
    else if (string=="METpt") var=METpt;
    else if (string=="METphi") var=METphi;
    else if (string=="sumEt") var=sumEt;
    else if (string=="rawMETpt") var=rawMETpt;
    else if (string=="rawMETphi") var=rawMETphi;
    else if (string=="uept") var=uept;
    else if (string=="uephi") var=uephi;
    else if (string=="ueeta") var=ueeta;
    else if (string=="mpf") var=mpf;
    else if (string=="rawmpf") var=rawmpf;
    else if (string=="otherjetspt") var=otherjetspt;
    else if (string=="otherjetsphi") var=otherjetsphi;
    else if (string=="otherjetseta") var=otherjetseta;
    else if (string=="genjet1pt") var=genjet1pt;
    else if (string=="genjet1eta") var=genjet1eta;
    else if (string=="genjet1phi") var=genjet1phi;
    else if (string=="matchedgenjet1pt") var=matchedgenjet1pt;
    else if (string=="genjet2pt") var=genjet2pt;
    else if (string=="genzpt") var=genzpt;
    else if (string=="genmpf") var=genmpf;
    else if (string=="algoflavour") var=algoflavour;
    else if (string=="physflavour") var=physflavour;

    // general quantities
    if (var == npv)
        return event.m_vertexSummary->nVertices;
    else if (var == rho)
        return event.m_jetArea->median;
    else if (var == run)
        return event.m_eventmetadata->nRun;
    else if (var == weight)
        return metaData.GetWeight();
    // Z
    else if (var == zpt)
        return metaData.GetRefZ().p4.Pt();
    else if (var == zeta)
        return metaData.GetRefZ().p4.Eta();
    else if (var == zphi)
        return metaData.GetRefZ().p4.Phi();
    else if (var == zmass)
        return metaData.GetRefZ().p4.mass();

    // leading jet
    else if (var == jet1pt)
        return metaData.GetValidPrimaryJet(s, event)->p4.Pt();
    else if (var == jet1eta)
        return metaData.GetValidPrimaryJet(s, event)->p4.Eta();
    else if (var == jet1phi)
        return metaData.GetValidPrimaryJet(s, event)->p4.Phi();

    // leading jet composition
    else if (var == jet1photonfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->photonFraction;
    else if (var == jet1chargedemfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->chargedEMFraction;
    else if (var == jet1chargedhadfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->chargedHadFraction;
    else if (var == jet1neutralhadfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->neutralHadFraction;
    else if (var == jet1muonfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->muonFraction;
    else if (var == jet1HFhadfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->HFHadFraction;
    else if (var == jet1HFemfraction)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->HFEMFraction;

    // second jet
    else if (var == jet2pt)
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Pt();
        else
            return 0;
    }
    else if (var == jet2phi)
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Phi();
        else
            return 0;
    }
    else if (var == jet2eta)
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Eta();
        else
            return 0;
    }

    // MET & sumEt
    else if (var == METpt)
        return metaData.GetMet(event, s)->p4.Pt();
    else if (var == METphi)
        return metaData.GetMet(event, s)->p4.Phi();
    else if (var == sumEt)
        return metaData.GetMet(event, s)->sumEt;
    else if (var == rawMETpt)
        return event.GetMet(s)->p4.Pt();
    else if (var == rawMETphi)
        return event.GetMet(s)->p4.Phi();

    else if (var == uept)
        return metaData.GetUE(event, s)->p4.Pt();
    else if (var == uephi)
        return metaData.GetUE(event, s)->p4.Phi();
    else if (var == ueeta)
        return metaData.GetUE(event, s)->p4.Eta();
    else if (var == mpf)
        return metaData.GetMPF(metaData.GetMet(event, s));
    else if (var == rawmpf)
        return metaData.GetMPF(event.GetMet(s));
    else if (var == otherjetspt)
    {
        if (metaData.GetValidJetCount(s, event) < 2)
            return 0;
        else 
            return (-( metaData.GetRefZ().p4 
                + metaData.GetValidPrimaryJet(s, event)->p4
                + metaData.GetMet(event, s)->p4
                + metaData.GetValidJet(s, event, 1)->p4
                + metaData.GetUE(event, s)->p4
                    )).Pt();
    }
    else if (var == otherjetsphi)
    {
        if (metaData.GetValidJetCount(s, event) < 2)
            return 0;
        else 
            return (-( metaData.GetRefZ().p4 
                + metaData.GetValidPrimaryJet(s, event)->p4
                + metaData.GetMet(event, s)->p4
                + metaData.GetValidJet(s, event, 1)->p4
                + metaData.GetUE(event, s)->p4
                    )).Phi();
    }
    else if (var == otherjetseta)
    {
        if (metaData.GetValidJetCount(s, event) < 2)
            return 0;
        else 
            return (-( metaData.GetRefZ().p4 
                + metaData.GetValidPrimaryJet(s, event)->p4
                + metaData.GetMet(event, s)->p4
                + metaData.GetValidJet(s, event, 1)->p4
                + metaData.GetUE(event, s)->p4
                    )).Eta();
    }
    //gen jets
    else if (var == genjet1pt)
    {
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

        if (metaData.GetValidJetCount(s, event, genName) == 0)
	        return false;

	    return metaData.GetValidJet(s, event, 0, genName)->p4.Pt();
    }
    else if (var == genjet1eta)
    {
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

        if (metaData.GetValidJetCount(s, event, genName) == 0)
	        return false;

	    return metaData.GetValidJet(s, event, 0, genName)->p4.Eta();
    }
    else if (var == genjet1phi)
    {
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

        if (metaData.GetValidJetCount(s, event, genName) == 0)
	        return false;

	    return metaData.GetValidJet(s, event, 0, genName)->p4.Phi();
    }
    else if (var == matchedgenjet1pt)
    {
  		std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

		KDataLV* matched_genjet = NULL;
    
    	if (0 >= metaData.GetValidJetCount(s, event, genName)
					|| 0 >= metaData.GetValidJetCount(s, event))
				return false;

		if (metaData.m_matchingResults.find(genName) == metaData.m_matchingResults.end())
			return false;
		std::vector<int> const& matchList = metaData.m_matchingResults.at(genName);

		if (unlikely(0 >= matchList.size()))
			return false;

		int iMatchedGen = matchList.at(0);
		if (iMatchedGen <= -1)
			return false;

		if (iMatchedGen >= metaData.GetValidJetCount(s, event, genName))
		{
			CALIB_LOG_FATAL("Reco to gen matching: No reference gen jet found! "
				<< iMatchedGen <<" >= " << metaData.GetValidJetCount(s, event, genName))
			return false;
		}
		return metaData.GetValidJet(s, event, iMatchedGen, genName)->p4.Pt();
    }

    else if (var == genjet2pt)
    {
            std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

	        if (metaData.GetValidJetCount(s, event, genName) < 2)
		        return false;

		    return metaData.GetValidJet(s, event, 1, genName)->p4.Pt();
    }
    else if (var == genzpt)
        return metaData.GetRefGenZ().p4.Pt();
    else if (var == genmpf)
        return metaData.GetGenMPF(metaData.GetPtGenMet()), metaData.GetWeight();
    else if (var == algoflavour)
    {
        const float dist = 0.3;
        int flavour = 0;

        KGenParticles matching_partons;
        KGenParticle hardest_parton;
        KGenParticle hardest_b_quark;
        KGenParticle hardest_c_quark;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);

        // iterate over all partons and select the ones close to the leading jet
        for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
        {
            if (it->status() == 3)
               continue;


            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                matching_partons.push_back(*it);
                if (std::abs(it->pdgId()) == 5 && it->p4.Pt() > hardest_b_quark.p4.Pt())
                    hardest_b_quark = *it;
                else if (std::abs(it->pdgId()) == 4 && it->p4.Pt() > hardest_c_quark.p4.Pt())
                    hardest_c_quark = *it;
                else if (it->p4.Pt() > hardest_parton.p4.Pt())
                    hardest_parton = *it;
            }
        }
        if (matching_partons.size() == 0)           // no matches
            flavour = 0;
        else if (matching_partons.size() == 1)      // exactly one match
            flavour = matching_partons[0].pdgId();
        else if (hardest_b_quark.p4.Pt() > 0.)
            flavour = hardest_b_quark.pdgId();
        else if (hardest_c_quark.p4.Pt() > 0.)
            flavour = hardest_c_quark.pdgId();
        else
            flavour = hardest_parton.pdgId();

        return std::abs(flavour);
    }
    else if (var == physflavour)
    {
        const float dist = 0.3;
        int flavour = 0;
        KGenParticles matching_partons;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);


        // iterate over all partons and select the ones close to the leading jet
        for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
        {
            if (it->status() != 3)
                continue;

            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
                matching_partons.push_back(*it);
        }
        // flavour is only well defined if exactly ONE matching parton!
        if (matching_partons.size() == 1)
            flavour = matching_partons[0].pdgId();

        return std::abs(flavour);
    }
    else
        CALIB_LOG_FATAL("TTreeConsumer: Quantity "<< n <<" not available!")
};

};

}



