import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3", "ak7PFJetsL1L2L3"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/piparo/Paper/ZPJ2011_DY_pu_corrected_matching/*.root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/mc_fall10_dy_v1"

# apply weighting to the events
conf["UseWeighting"] = 0
# use the weghting specified in the event
# if you have PtBins, this must be 0 and the above must be 1. Then the Weights are calculated by the 
# resp_cuts code. But please double check if the xsections in resp_cuts are still right.
conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
