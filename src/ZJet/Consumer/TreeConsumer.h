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
        stringvector v = this->GetPipelineSettings().GetQuantities();

        std::vector<float> arr;

        // fill the array with values according to the variable-list :
        for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) 
        {
            arr.push_back(returnvalue(*it, event, metaData, this->GetPipelineSettings()));
        }

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
    
    float returnvalue(std::string string, ZJetEventData const& event,
			                    ZJetMetaData const& metaData, ZJetPipelineSettings const& s){

    // general quantities
    if (string =="npv")
        return event.m_vertexSummary->nVertices;
    else if (string =="rho")
        return event.m_jetArea->median;
    else if (string =="run")
        return event.m_eventmetadata->nRun;
    else if (string =="weight")
        return metaData.GetWeight();
    // Z
    else if (string =="zpt")
        return metaData.GetRefZ().p4.Pt();
    else if (string =="zeta")
        return metaData.GetRefZ().p4.Eta();
    else if (string =="zphi")
        return metaData.GetRefZ().p4.Phi();
    else if (string =="zmass")
        return metaData.GetRefZ().p4.mass();

    // leading jet
    else if (string =="jet1pt")
        return metaData.GetValidPrimaryJet(s, event)->p4.Pt();
    else if (string =="jet1eta")
        return metaData.GetValidPrimaryJet(s, event)->p4.Eta();
    else if (string =="jet1phi")
        return metaData.GetValidPrimaryJet(s, event)->p4.Phi();

    // leading jet composition
    else if (string =="jet1photonfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->photonFraction;
    else if (string =="jet1chargedemfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->chargedEMFraction;
    else if (string =="jet1chargedhadfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->chargedHadFraction;
    else if (string =="jet1neutralhadfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->neutralHadFraction;
    else if (string =="jet1muonfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->muonFraction;
    else if (string =="jet1HFhadfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->HFHadFraction;
    else if (string =="jet1HFemfraction")
        return static_cast<KDataPFJet*>( metaData.GetValidPrimaryJet(s, event) )->HFEMFraction;

    // second jet
    else if (string =="jet2pt")
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Pt();
        else
            return 0;
    }
    else if (string =="jet2phi")
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Phi();
        else
            return 0;
    }
    else if (string =="jet2eta")
    {
        if (metaData.GetValidJetCount(s, event) > 1)
            return metaData.GetValidJet(s, event, 1)->p4.Eta();
        else
            return 0;
    }

    // MET & sumEt
    else if (string =="METpt")
        return metaData.GetMet(event, s)->p4.Pt();
    else if (string =="METphi")
        return metaData.GetMet(event, s)->p4.Phi();
    else if (string =="sumEt")
        return metaData.GetMet(event, s)->sumEt;
    else if (string =="rawMETpt")
        return event.GetMet(s)->p4.Pt();
    else if (string =="rawMETphi")
        return event.GetMet(s)->p4.Phi();

    else if (string =="mpf")
        return metaData.GetMPF(metaData.GetMet(event, s));
    else if (string =="rawmpf")
        return metaData.GetMPF(event.GetMet(s));

    //gen jets
    else if (string =="genjet1pt")
    {
            std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

	        if (metaData.GetValidJetCount(s, event, genName) == 0)
		        return false;

		    return metaData.GetValidJet(s, event, 0, genName)->p4.Pt();
    }
    else if (string =="genjet1eta")
    {
            std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

	        if (metaData.GetValidJetCount(s, event, genName) == 0)
		        return false;

		    return metaData.GetValidJet(s, event, 0, genName)->p4.Eta();
    }
    else if (string =="genjet1phi")
    {
            std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

	        if (metaData.GetValidJetCount(s, event, genName) == 0)
		        return false;

		    return metaData.GetValidJet(s, event, 0, genName)->p4.Phi();
    }
    else if (string =="matchedgenjet1pt")
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

    else if (string =="genjet2pt")
    {
            std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

	        if (metaData.GetValidJetCount(s, event, genName) < 2)
		        return false;

		    return metaData.GetValidJet(s, event, 1, genName)->p4.Pt();
    }
    else if (string =="flavour")
    {
        if (!metaData.GetRefValidParton())
            return false;

        return std::abs(metaData.GetBalancedParton().pdgId());
    }
    else if (string =="flavour")
        return metaData.GetRefGenZ().p4.Pt();
    else if (string =="genmpf")
        return metaData.GetGenMPF(metaData.GetPtGenMet()), metaData.GetWeight();
    else
        CALIB_LOG_FATAL("TTreeConsumer: Quantity "<< string <<" not available!")

    }

};


}
