#pragma once

#include "TROOT.h"
#include "TTree.h"
#include "TH1D.h"
#include "TNtuple.h"
#include "TFile.h"
#include "Math/GenVector/VectorUtil.h"

template <class T>
static T SafeGet(TDirectory* pDir, std::string const& objName)
{
	T ob = (T) pDir->Get(objName.c_str());

	if (ob == NULL)
		LOG_FATAL("Cant load " << objName << " from directory " << pDir->GetName());

	return ob;
}

template <class T>
static T SafeGet(TFile* file, std::string const& objName)
{
	T ob = (T)(file->Get(objName.c_str()));

	if (ob == NULL)
		LOG_FATAL("Cant load " << objName << " from directory " << file->GetName());

	return ob;
}
