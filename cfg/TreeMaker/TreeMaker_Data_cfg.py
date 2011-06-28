# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

import Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs
import Zplusjet.ZplusjetTreeMaker.Zplusjet_Base_cff as TreeBaseCfg

defaultGlobalTag = "FT_R_42_V13A::All"
useDefaultGlobalTag = 0


try:
    TreeBaseCfg.GCFILE_NAMES__ = __FILE_NAMES__
    TreeBaseCfg.GCSKIP_EVENTS__ = __SKIP_EVENTS__
    TreeBaseCfg.GCMAX_EVENTS__ = __MAX_EVENTS__
    TreeBaseCfg.GCMY_JOBID__ = __MY_JOBID__
except NameError:
    print "NOTE: Not run by grid-control, substituting data files."
    TreeBaseCfg.GCFILE_NAMES__=["file:/scratch/hh/lustre/cms/user/hauth/ZPJ2011/examples/data_DoubleMu_Run2011A_ReReco_May10_AOD.root"]
    TreeBaseCfg.GCSKIP_EVENTS__ = 0
    TreeBaseCfg.GCMAX_EVENTS__ = 10000
    TreeBaseCfg.GCMY_JOBID__ = "23" 
    useDefaultGlobalTag = 1

# use only RECO Path for processing 
process = TreeBaseCfg.getBaseConfig(Zs.goodZToMuMuRecoPath,True)

if ( useDefaultGlobalTag ):
    process.GlobalTag.globaltag = defaultGlobalTag
else:
    process.GlobalTag.globaltag = "@GLOBALTAG@"

