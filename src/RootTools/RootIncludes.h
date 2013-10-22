#pragma once

#include "TROOT.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TFile.h"
#include "Math/GenVector/VectorUtil.h"

class RootFileHelper
{
public:
	template <class T>
	static T SafeGet( TDirectory * pDir, std::string const& objName  )
	{
		T ob = (T)pDir->Get( objName.c_str() );

		if ( ob == NULL)
		{
			std::cout << std::endl << "Cant load " << objName << " from directory " << pDir->GetName() << std::endl;
			exit(0);
		}
		return ob;
	}

    static void SafeCd( TDirectory * pDir, std::string const& dirName )
    {
    	assert( pDir );

        if ( pDir->GetDirectory( dirName.c_str() ) == 0)
		{
			pDir->mkdir( dirName.c_str() );
		}
        pDir->cd( dirName.c_str() );
    }   
};


