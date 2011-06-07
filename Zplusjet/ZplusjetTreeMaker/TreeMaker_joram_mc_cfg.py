# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

import Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs
import Zplusjet.ZplusjetTreeMaker.Zplusjet_Base_cff as TreeBaseCfg

defaultGlobalTag = "START42_V12::All"
useDefaultGlobalTag = 0

# Check if run by grid-control, if not, define some varibles
try:
    TreeBaseCfg.GCFILE_NAMES__ = __FILE_NAMES__
    TreeBaseCfg.GCSKIP_EVENTS__ = __SKIP_EVENTS__
    TreeBaseCfg.GCMAX_EVENTS__ = __MAX_EVENTS__
    TreeBaseCfg.GCMY_JOBID__ = __MY_JOBID__

except NameError:
    print "NOTE: Not run by grid-control, substituting MC files."
    TreeBaseCfg.GCFILE_NAMES__= ["file:/scratch/hh/lustre/cms/user/berger/temp/edm_MC_423.root"]
    TreeBaseCfg.GCSKIP_EVENTS__ = 0
    TreeBaseCfg.GCMAX_EVENTS__ = 50
    TreeBaseCfg.GCMY_JOBID__ = "23" 
    useDefaultGlobalTag = 1


# use Path combining RECO and GEN for processing 
process = TreeBaseCfg.getBaseConfig(Zs.allgoodZToMuMuPath, False)

if ( useDefaultGlobalTag ):
    process.GlobalTag.globaltag = defaultGlobalTag
else:
    process.GlobalTag.globaltag = "@GLOBALTAG@"

