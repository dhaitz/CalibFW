# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

import  Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs
import Zplusjet.ZplusjetTreeMaker.Zplusjet_Base_cff as TreeBaseCfg

defaultGlobalTag = "FT_R_38X_V14A::All"
useDefaultGlobalTag = 0


try:
    TreeBaseCfg.GCFILE_NAMES__ = __FILE_NAMES__
    TreeBaseCfg.GCSKIP_EVENTS__ = __SKIP_EVENTS__
    TreeBaseCfg.GCMAX_EVENTS__ = __MAX_EVENTS__
    TreeBaseCfg.GCMY_JOBID__ = __MY_JOBID__
except NameError:
    print "NOTE: Not run by grid-control, substituting data files."
    TreeBaseCfg.GCFILE_NAMES__=[
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010B/Mu/RECO/PromptReco-v2/000/146/511/D80834F4-A2C7-DF11-84CA-0030487A3DE0.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010B/Mu/RECO/PromptReco-v2/000/146/511/D075F740-B0C7-DF11-9FA5-001D09F291D7.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010B/Mu/RECO/PromptReco-v2/000/146/511/CA381CC5-86C7-DF11-8570-00304879FA4A.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/data/Run2010B/Mu/RECO/PromptReco-v2/000/146/511/B43BD1D0-8DC7-DF11-9FEE-001D09F26C5C.root']
    TreeBaseCfg.GCSKIP_EVENTS__ = 0
    TreeBaseCfg.GCMAX_EVENTS__ = 10000
    TreeBaseCfg.GCMY_JOBID__ = "23" 
    useDefaultGlobalTag = 1

# use only RECO Path for processing 
process = TreeBaseCfg.getBaseConfig(Zs.goodZToMuMuRecoPath)

if ( useDefaultGlobalTag ):
    process.GlobalTag.globaltag = defaultGlobalTag
else:
    process.GlobalTag.globaltag = "@GLOBALTAG@"

