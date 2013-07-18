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
        matchedgenjet1pt, genjet2pt, genzpt, genmpf, algoflavour, physflavour, 
        nmatchingpartons, nmatchingpartons3, npartons, npartons3, partonratio, 
        leadinggenparticle, firstpartonflavour, secondpartonflavour, nneutrinos, 
        nmatchingneutrinos, leadinggenparticle5, klongpt, firstpartonpt, 
        partonsumpt, kaonpt, neutrinopt, jet1npt, mpfn, lambdapt, neutronpt, 
        pionpt, kshortpt 
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
    else if (string=="nmatchingpartons") var=nmatchingpartons;
    else if (string=="nmatchingpartons3") var=nmatchingpartons3;
    else if (string=="npartons") var=npartons;
    else if (string=="npartons3") var=npartons3;
    else if (string=="partonratio") var=partonratio;
    else if (string=="leadinggenparticle") var=leadinggenparticle;
    else if (string=="firstpartonflavour") var=firstpartonflavour;
    else if (string=="secondpartonflavour") var=secondpartonflavour;
    else if (string=="nneutrinos") var=nneutrinos;
    else if (string=="nmatchingneutrinos") var=nmatchingneutrinos;
    else if (string=="leadinggenparticle5") var=leadinggenparticle5;
    else if (string=="klongpt") var=klongpt;
    else if (string=="firstpartonpt") var=firstpartonpt;
    else if (string=="partonsumpt") var=partonsumpt;
    else if (string=="kaonpt") var=kaonpt;
    else if (string=="neutrinopt") var=neutrinopt;
    else if (string=="jet1npt") var=jet1npt;
    else if (string=="mpfn") var=mpfn;
    else if (string=="lambdapt") var=lambdapt;
    else if (string=="neutronpt") var=neutronpt;
    else if (string=="pionpt") var=pionpt;
    else if (string=="kshortpt") var=kshortpt;



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

        //else
        /* if (matching_partons.size() == 2){
            CALIB_LOG("")
            CALIB_LOG("  ZpT: " << metaData.GetRefZ().p4.Pt())

            CALIB_LOG("  jet pT: " << metaData.GetValidPrimaryJet(s, event)->p4.Pt())

            KDataLV sum;
            sum.p4 = matching_partons[0].p4 + matching_partons[1].p4;
            CALIB_LOG("  parton sum: " << sum.p4.Pt())

            for (auto it = event.m_particles->begin(); it != event.m_particles->end(); ++it)
            {
                if (it-event.m_particles->begin() < 6
                     || it->status()!=1
                     //|| it->pdgId()==23
                     //|| std::abs(it->pdgId())==13
                     //|| it->p4.Pt()<1
                    )
                    continue;

                 CALIB_LOG(it-event.m_particles->begin() << "  " << it->status() 
                            << "  " << it->pdgId() << "  dR: " << 
                    std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4) )
                << "  p4: " << it->p4
                )
            }
            CALIB_LOG("")
        }*/

        return std::abs(flavour);
    }
    else if (var == nmatchingpartons)
    {
        const float dist = 0.3;
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
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                matching_partons.push_back(*it);
                if (it->status() > 3 || it->status() < 2)
                    CALIB_LOG("Status of matching parton: " << it->status())
            }
        }
        return matching_partons.size();

    }
    else if (var == nmatchingpartons3)
    {
        const float dist = 0.3;
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
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() == 3)
                    matching_partons.push_back(*it);
            }
        }
        return matching_partons.size();

    }
    else if (var == npartons)
    {
        return metaData.m_genPartons.size();
    }
    else if (var == npartons3)
    {

        KGenParticles matching_partons;
      
        for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
        {
            if (it->status() == 3)
                matching_partons.push_back(*it);
        }


        return matching_partons.size();
    }
    else if (var == partonratio)
    {
        const float dist = 0.3;
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
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() != 3)
                    continue;
                matching_partons.push_back(*it);
            }
        }
        if (matching_partons.size() == 2)
        {

        KGenParticles status3_partons;
      
        for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
        {
            if (it->status() == 3)
                status3_partons.push_back(*it);
        }

            if (matching_partons[0].p4.Pt() > matching_partons[1].p4.Pt())
                return (matching_partons[1].p4.Pt() / matching_partons[0].p4.Pt());
            else
                return (matching_partons[0].p4.Pt() / matching_partons[1].p4.Pt());
        }
        else 
            return 0;
    }
    else if (var == leadinggenparticle)
    {
        const float dist = 0.3;
        KGenParticles matching_particles;
        KGenParticle hardest_particle;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);


        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() != 1)
                    continue;
                matching_particles.push_back(*it);
                if (it->p4.Pt() > hardest_particle.p4.Pt())
                    hardest_particle = *it;
            }
        }
        return hardest_particle.pdgId();
    }
    else if (var == leadinggenparticle5)
    {
        const float dist = 0.5;
        KGenParticles matching_particles;
        KGenParticle hardest_particle;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);


        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() != 1)
                    continue;
                matching_particles.push_back(*it);
                if (it->p4.Pt() > hardest_particle.p4.Pt())
                    hardest_particle = *it;
            }
        }
        return hardest_particle.pdgId();
    }
    else if (var == firstpartonflavour)
    {
        const float dist = 0.3;
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
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() != 3)
                    continue;
                matching_partons.push_back(*it);
            }
        }
        // if there are two partons: return the flavour of the harder one
        if (matching_partons.size() == 2)
        {
            if (matching_partons[0].p4.Pt() > matching_partons[1].p4.Pt())
                return matching_partons[0].pdgId();
            else
                return matching_partons[1].pdgId();

        }
        else 
            return 0;
    }
    else if (var == secondpartonflavour)
    {
        const float dist = 0.3;
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
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() != 3)
                    continue;
                matching_partons.push_back(*it);
            }
        }
        // if there are two partons: return the flavour of the softer one
        if (matching_partons.size() == 2)
        {
            if (matching_partons[0].p4.Pt() > matching_partons[1].p4.Pt())
                return matching_partons[1].pdgId();
            else
                return matching_partons[0].pdgId();

        }
        else 
            return 0;
    }
    else if (var == nneutrinos)
    {
        KGenParticles neutrinos;

        // iterate over all partons and select the ones close to the leading jet
        for (auto it = event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (it->status()==1 && (std::abs(it->pdgId())==12 || std::abs(it->pdgId())==14 ||
                 std::abs(it->pdgId())==16) )
            {
                neutrinos.push_back(*it);
            }
        }
        return neutrinos.size();
    }
    else if (var == nmatchingneutrinos)
    {
        const float dist = 0.3;
        KGenParticles matching_neutrinos;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);

        // iterate over all partons and select the ones close to the leading jet
        for (auto it = event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (it->status()==1 && (std::abs(it->pdgId())==12 || std::abs(it->pdgId())==14 ||
                 std::abs(it->pdgId())==16) &&
                (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist))
            {
                matching_neutrinos.push_back(*it);
            }
        }
        return matching_neutrinos.size();

    }
    else if (var == klongpt)
    {
        const float dist = 0.5;
        KDataLV klongsum;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);


        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
            {
                if (it->status() != 1)
                    continue;
                if (std::abs(it->pdgId())==130)
                    klongsum.p4 += it->p4;
            }
        }
        return klongsum.p4.Pt();
    }
    else if (n ==150) // neutrino pt
    {
        KDataLV neutrinosum;

        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (it->status()==1 && (std::abs(it->pdgId())==12 || std::abs(it->pdgId())==14
                || std::abs(it->pdgId())==16))
                neutrinosum.p4 += it->p4;
        }
        return neutrinosum.p4.Pt();
    }
    else if (n ==151) // neutrino phi
    {
        KDataLV neutrinosum;

        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (it->status()==1 && (std::abs(it->pdgId())==12 || std::abs(it->pdgId())==14
                || std::abs(it->pdgId())==16))
                neutrinosum.p4 += it->p4;
        }
        return neutrinosum.p4.Phi();
    }


    else if (var == firstpartonpt)
    {
        const float dist = 0.3;
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
        if (matching_partons.size() ==1)
            return matching_partons[0].p4.Pt();
        else if (matching_partons.size() == 2)
        {
            if (matching_partons[0].p4.Pt() > matching_partons[1].p4.Pt())
                return matching_partons[0].p4.Pt();
            else
                return matching_partons[1].p4.Pt();
        }
        else return 0;
    }    
    else if (var == partonsumpt)
    {
        const float dist = 0.3;
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
        if (matching_partons.size() == 2)
        {
            return (matching_partons[0].p4 + matching_partons[1].p4).Pt();
        }
        else return 0;
    }
<<<<<<< HEAD

=======
    else if (var == lambdapt)
    {
        const float dist = 0.5;
        KDataLV sum;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);

        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist 
                && it->status()==1 
                && (std::abs(it->pdgId())==3122))
                sum.p4 += it->p4;
        }
        return sum.p4.Pt();
    }

    else if (var == neutronpt)
    {
        const float dist = 0.5;
        KDataLV sum;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);

        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist 
                && it->status()==1 
                && (std::abs(it->pdgId())==2112))
                sum.p4 += it->p4;
        }
        return sum.p4.Pt();
    }

    else if (var == pionpt)
    {
        bool show1 = 0;
        bool show2 = 0;
        bool show3 = 1;

        int nr = 0;
        int nr_n = 0;

        const float dist = 0.5;
        KDataLV sum;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);

        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist 
                && it->status()==1 
                && (std::abs(it->pdgId())==111 ||std::abs(it->pdgId())==211))
                sum.p4 += it->p4;

            if (it->status()==1
                && std::abs(it->pdgId())==130
                && nr==0)
                {show1 = 1; nr = it-event.m_particles->begin();}
            if (it->status()==1
                && nr != 0
                && nr_n == 0
                && (std::abs(it->pdgId())==12 || std::abs(it->pdgId())==14 
                || std::abs(it->pdgId())==16))
                {show2 = 1; nr_n = it-event.m_particles->begin();}
            /*if (it->status()==1
                && std::abs(it->pdgId())==310)
                {show3 = 0;}*/



                /*(std::abs(it->pdgId())!=111 &&std::abs(it->pdgId())!=211
                && std::abs(it->pdgId())!=3122
                && std::abs(it->pdgId())!=2112
                && std::abs(it->pdgId())!=11
                && std::abs(it->pdgId())!=12
                && std::abs(it->pdgId())!=13
                && std::abs(it->pdgId())!=14
                && std::abs(it->pdgId())!=15
                && std::abs(it->pdgId())!=16
                && std::abs(it->pdgId())!=22
                && std::abs(it->pdgId())!=23
                && std::abs(it->pdgId())!=130
                && std::abs(it->pdgId())!=310
                && std::abs(it->pdgId())!=2212
                && std::abs(it->pdgId())!=2224
                && std::abs(it->pdgId())!=2214
                && std::abs(it->pdgId())!=2114
                && std::abs(it->pdgId())!=1114
                && std::abs(it->pdgId())!=321

                &&std::abs(it->pdgId())!=21
                &&std::abs(it->pdgId())>5


                && std::abs(it->pdgId())!=3222
                && std::abs(it->pdgId())!=3212
                && std::abs(it->pdgId())!=3112

                && std::abs(it->pdgId())!=3322
                && std::abs(it->pdgId())!=3312


                && std::abs(it->pdgId())!=3334 */
                //))

        }

        if (show1 && show2 && show3 && (nr_n-nr < 20))
        {
            CALIB_LOG(" ")
            CALIB_LOG(nr << " -> " << nr_n)
            for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
            {

                if (it-event.m_particles->begin()>=nr
                    && it-event.m_particles->begin()<=nr_n+5)//it->status()==1 && it->pdgId()!=22)
                CALIB_LOG(it-event.m_particles->begin() << "  " << it->status() 
                                << "  " << it->pdgId() 
                    << "  p4: " << it->p4
                    )
            }
        }
        return sum.p4.Pt();
    }

    else if (var ==  kshortpt)
    {
        const float dist = 0.5;
        KDataLV sum;

        //get the reference jet:genjet by default, reco jet if no genjet available
        KDataLV * ref_jet;
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
        if (metaData.GetValidJetCount(s, event, genName) > 0)
            ref_jet = metaData.GetValidJet(s, event, 0, (JetType::GetGenName(s.GetJetAlgorithm())));
        else
            ref_jet = metaData.GetValidPrimaryJet(s, event);

        for (auto it =event.m_particles->begin(); it != event.m_particles->end(); ++it)
        {
            if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist 
                && it->status()==1 
                && (std::abs(it->pdgId())==310))
                sum.p4 += it->p4;
        }
        return sum.p4.Pt();
    }
>>>>>>> bb6f118... enum to choose the variable


    else
        CALIB_LOG_FATAL("TTreeConsumer: Quantity "<< n <<" not available!")

    }

};


}
