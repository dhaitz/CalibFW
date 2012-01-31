#pragma once

#include <string>
#include <iostream>

#include <boost/assign/list_of.hpp> // for 'list_of()'
#include <boost/assert.hpp>
#include <list>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <boost/property_tree/ptree.hpp>

#include <typeinfo>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"

#include "Draw/DrawBase.h"
#include "Pipeline/JetTools.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

namespace CalibFW
{

/*
* Calculates the Response distribution with a Histogram
*/
class BinResponseConsumer: public ZJetMetaConsumer
{
public:
	enum
	{	MpfResponse, BalResponse}m_respType;

	BinResponseConsumer( boost::property_tree::ptree * ptree , std::string configPath) :
		ZJetMetaConsumer(), m_useGenJet ( false ), m_useGenJetAsReference( false )
		{
		{
			/*
		"Name" : "response_balance",
		"ProductName" : "binresp",
		"ResponseType": "bal"|"mpf",
		"JetNumber" : 1 to n
			*/
			m_jetnum = ptree->get<unsigned int>( configPath + ".JetNumber", 1 ) -1;
			m_name = ptree->get<std::string>( configPath + ".ProductName");

			if ( ptree->get<std::string>( configPath + ".ResponseType") == "bal" )
				m_respType = BalResponse;
			else if ( ptree->get<std::string>( configPath + ".ResponseType") == "mpf" )
				m_respType = MpfResponse;
			else
			{
				CALIB_LOG_FATAL("Unknown Response type " + ptree->get<std::string>( configPath + ".ResponseType") );
			}
		}
		}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetMetaConsumer::Init( pset );

		m_resp = new Hist1D( m_name,
				GetPipelineSettings().GetRootFileFolder(),
				Hist1D::GetResponseModifier() );
		AddPlot ( m_resp );
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData)
	{
		ZJetMetaConsumer::ProcessFilteredEvent( event, metaData);

		assert( metaData.HasValidZ());

		if ( m_respType == BalResponse )
		{

			if ( m_jetnum >= metaData.GetValidJetCount( this->GetPipelineSettings(), event))
				// on jet for us here
				return;

			KDataLV * jet0 = metaData.GetValidJet(this->GetPipelineSettings(), event,  m_jetnum);
			assert( jet0 != NULL );
			
			
			KDataLV * gen_jet = NULL;
			
			// do we also need to load the matched gen jet ?
			if ( m_useGenJet || m_useGenJetAsReference )
			{
			    std::string genName ( JetType::GetGenName ( this->GetPipelineSettings().GetJetAlgorithm() ) );
                			    
                //std::cout << std::endl << "Loading genJets " << genName;
                std::vector < int > const& matchList = metaData.m_matchingResults.at( genName );
                                
 			    int iMatchedGen = -1;
 			    if ( matchList.size() > m_jetnum )
 			    {
 			        iMatchedGen = matchList.at(m_jetnum);
 			    }
            
                // std::cout << std::endl << "Loading genJet with num " << iMatchedGen;
                // do we even have a matched gen jet ??			    
			    if ( iMatchedGen > -1 )
			    {
			        unsigned int count = 
                         metaData.GetValidJetCount(this->GetPipelineSettings(),
                                          event,
                                          genName );
                    if ( iMatchedGen < count)
    			    {    gen_jet = metaData.GetValidJet(this->GetPipelineSettings(),
					                event,
					                iMatchedGen,
					                genName );
                    }
                    //else
                    //{ std::cout << std::endl << "No jet found, count is " << count;}
					            
				}
                //std::cout << "done";
			}
			
			// can we get 
			
			// do we do gen jet response to z.pt ?
			if ( ! m_useGenJet )
			{
       			  // check which reference object to use
       			  if ( ! m_useGenJetAsReference )
       			  {
       			    // use z as reference
			          m_resp->Fill( metaData.GetBalance( jet0 ),
					            metaData.GetWeight());
			      }
			      else
			      {
			            if ( gen_jet != NULL )
			            {
			                m_resp->Fill( metaData.GetBalanceBetweenJets( jet0, gen_jet ),
					            metaData.GetWeight());
					    }
					    else
					    {
					        std::cout << "no gen jet for balance reference found ? RECO JetNum " << m_jetnum << std::endl;
					    }
			      }
			}
			else
			{
			// plot the gen.pt over z.pt
			  if (gen_jet != NULL )
			  {
			   // std::cout << "Filing Balance with genjet";
			    m_resp->Fill( metaData.GetBalance( gen_jet ),
					    metaData.GetWeight());
					    //std::cout << "-- done";
			  }
			  else 
			    std::cout << "no gen jet for balance found ? RECO JetNum " << m_jetnum << std::endl;
			}
        }

		if ( m_respType == MpfResponse )
		{
		    // todo: get Gen MPF
			m_resp->Fill( metaData.GetMPF( event.GetMet( this->GetPipelineSettings()) ),
					metaData.GetWeight());

		}
	}

	static std::string GetName()
	{
		return "bin_response";
	}

	
	// if true, the GenJet.Pt is used to calculate the balance instead of the RECO Jet
	bool m_useGenJet;
	// if true, the GenJet.Pt will be used as reference object instead of the Z.Pt()
	bool m_useGenJetAsReference;
	std::string m_name;
	unsigned int m_jetnum;
private:
	Hist1D * m_resp;
	

	
	
};

}

