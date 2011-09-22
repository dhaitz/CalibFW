#pragma once


#include "ZJetFilterBase.h"

namespace CalibFW
{

class ValidZFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		// std::cout << "val z " << metaData.HasValidZ() << std::endl;
		// metaData.HasValidZ();

		// just for debugging !!!
		// remove everything down here !

		for ( KDataMuons::const_iterator it = metaData.GetValidMuons().begin();
				it != metaData.GetValidMuons().end(); it ++)
		{
			if( it->p4.Pt() < 12.0f)
			{
				return false;
			}
			if ( it->p4.Eta() > 8.0f )
				return false;
		}

		if (metaData.HasValidZ())
		{
			 //"60.0 < mass < 120.0"
			if (60.0 > metaData.GetZ().p4.M())
				return false;
			if ( metaData.GetZ().p4.M() > 120)
				return false;

			return true;
		}
		else
			return false;
	}

	virtual std::string GetFilterId()
	{
		return "valid_z";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Valid Z Filter";
	}

};

}