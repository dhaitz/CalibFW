# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

import Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs
import Zplusjet.ZplusjetTreeMaker.Zplusjet_Base_cff as TreeBaseCfg

defaultGlobalTag = "START38_V12::All"
useDefaultGlobalTag = 0

# Check if run by grid-control, if not, define some varibles
try:
    TreeBaseCfg.GCFILE_NAMES__ = __FILE_NAMES__
    TreeBaseCfg.GCSKIP_EVENTS__ = __SKIP_EVENTS__
    TreeBaseCfg.GCMAX_EVENTS__ = __MAX_EVENTS__
    TreeBaseCfg.GCMY_JOBID__ = __MY_JOBID__

except NameError:
    print "NOTE: Not run by grid-control, substituting MC files."
    TreeBaseCfg.GCFILE_NAMES__=["dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2//store/mc/Fall10/ZJetToMuMu_Pt_50to80_TuneZ2_7TeV_pythia6/GEN-SIM-RECO/START38_V12-v1/0001/545E823B-3AC9-DF11-A441-0017A4770438.root"]
    TreeBaseCfg.GCSKIP_EVENTS__ = 0
    TreeBaseCfg.GCMAX_EVENTS__ = 50
    TreeBaseCfg.GCMY_JOBID__ = "23" 
    useDefaultGlobalTag = 1


# use Path combining RECO and GEN for processing 
process = TreeBaseCfg.getBaseConfig(Zs.allgoodZToMuMuPath)

if ( useDefaultGlobalTag ):
    process.GlobalTag.globaltag = defaultGlobalTag
else:
    process.GlobalTag.globaltag = "@GLOBALTAG@"
