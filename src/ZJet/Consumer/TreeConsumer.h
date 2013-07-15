#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "Draw/Profile.h"


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
        std::vector<int> v = this->GetPipelineSettings().GetQuantities();

        std::vector<float> arr;

        // fill the array with values according to the variable-list :
        for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) 
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
    
    float returnvalue(int n, ZJetEventData const& event,
			                    ZJetMetaData const& metaData, ZJetPipelineSettings const& s){

    // general quantities
    if (n ==0) //npv
        return event.m_vertexSummary->nVertices;
    else if (n ==1) //rho
        return event.m_jetArea->median;
    else if (n ==2) //run number
        return event.m_eventmetadata->nRun;
    else if (n ==3) //weight
        return metaData.GetWeight();
    // Z
    else if (n ==4) //zpt
        return metaData.GetRefZ().p4.Pt();
    else if (n ==5) //zeta
        return metaData.GetRefZ().p4.Eta();
    else if (n ==6) //zphi
        return metaData.GetRefZ().p4.Phi();
    else if (n ==7) //zmass
        return metaData.GetRefZ().p4.mass();

    // leading jet
    else if (n ==8) //jet1pt
        return metaData.GetValidPrimaryJet(s, event)->p4.Pt();
    else if (n ==9) //jet1eta
        return metaData.GetValidPrimaryJet(s, event)->p4.Eta();
    else if (n ==10) //jet1phi
        return metaData.GetValidPrimaryJet(s, event)->p4.Phi();

    // leading jet composition
    else if (n ==11)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->photonFraction;
    else if (n ==12)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->chargedEMFraction;
    else if (n ==13)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->chargedHadFraction;
    else if (n ==14)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->neutralHadFraction;
    else if (n ==15)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->muonFraction;
    else if (n ==16)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->HFHadFraction;
    else if (n ==17)
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->HFEMFraction;

    // second jet
    else if (n ==18)
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Pt();
        else
            return 0;
    }
    else if (n ==19)
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Phi();
        else
            return 0;
    }
    else if (n ==20)
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Eta();
        else
            return 0;
    }

    // MET & sumEt
    else if (n ==21) //METpt
        return metaData.GetMet(event, s)->p4.Pt();
    else if (n ==22) //METphi
        return metaData.GetMet(event, s)->p4.Phi();
    else if (n ==23) //sumEt
        return metaData.GetMet(event, s)->sumEt;
    else if (n ==24) //rawMETpt
        return event.GetMet(s)->p4.Pt();
    else if (n ==25) //rawMETphi
        return event.GetMet(s)->p4.Phi();

    else if (n ==26) // UEpt
        return metaData.GetUE(event, s)->p4.Pt();
    else if (n ==27) // UEphi
        return metaData.GetUE(event, s)->p4.Phi();
    else if (n ==28) // UEeta
        return metaData.GetUE(event, s)->p4.Eta();
    else if (n ==29) // MPF
        return metaData.GetMPF(metaData.GetMet(event, s));
    else if (n ==30) // raw MPF
        return metaData.GetMPF(event.GetMet(s));
    else if (n ==31) // other jets pT
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
    else if (n ==32) // other jets phi
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
    else if (n ==33) // other jets eta
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
    else if (n ==34) //genjet1pt
    {
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

        if (metaData.GetValidJetCount(s, event, genName) == 0)
	        return false;

	    return metaData.GetValidJet(s, event, 0, genName)->p4.Pt();
    }
    else if (n ==35) //genjet1eta
    {
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

        if (metaData.GetValidJetCount(s, event, genName) == 0)
	        return false;

	    return metaData.GetValidJet(s, event, 0, genName)->p4.Eta();
    }
    else if (n ==36) //genjet1phi
    {
        std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

        if (metaData.GetValidJetCount(s, event, genName) == 0)
	        return false;

	    return metaData.GetValidJet(s, event, 0, genName)->p4.Phi();
    }
    else if (n ==37) //matchedgenjet1pt
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

    else if (n ==40) //genjet2pt
    {
            std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

	        if (metaData.GetValidJetCount(s, event, genName) < 2)
		        return false;

		    return metaData.GetValidJet(s, event, 1, genName)->p4.Pt();
    }
    else if (n ==42) //genzpt
        return metaData.GetRefGenZ().p4.Pt();
    else if (n ==43) //genmpf
        return metaData.GetGenMPF(metaData.GetPtGenMet()), metaData.GetWeight();
    else if (n ==44) //algo flavour
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
    else if (n ==45) //flavourp
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
    else if (n ==46) // nmatchingpartons
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
    else if (n ==47) // nmatchingpartons with status 3
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
    else if (n ==48) // npartons
    {
        return metaData.m_genPartons.size();
    }
    else if (n ==49) // npartons3
    {

        KGenParticles matching_partons;
      
        for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
        {
            if (it->status() == 3)
                matching_partons.push_back(*it);
        }


        return matching_partons.size();
    }
    else if (n ==50) // partonratio
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
    else if (n ==51) // leadinggenparticle
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
    else if (n ==99) // leadinggenparticle 5
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
    else if (n ==53) // firstpartonflavour
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
    else if (n ==54) // secondpartonflavour
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
    else if (n ==55) // nneutrinos
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
    else if (n ==56) // nmatchningneutrinos
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
    else if (n ==100) // klongpt
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


    else if (n ==101) //first parton pt
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
    else if (n ==102) //parton sum pt
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



    else
        CALIB_LOG_FATAL("TTreeConsumer: Quantity "<< n <<" not available!")

    }

};


}
