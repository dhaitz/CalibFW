import sys
import JsonConfigBase
import LocalConfigBase
import subprocess
import copy

conf = JsonConfigBase.GetMcBaseConfig()

conf["Algos"] = ["ak5PFJetsL1L2L3", "ak7PFJetsL1L2L3"]

conf["InputFiles"] = "/scratch/hh/lustre/cms/user/berger/skimSpring11_dy_v1/mc_414_berger_2011-05-09_DY_Spring11_powheg*.root"
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "analysis/mc_spring11_dy_v1_rw"

# apply weighting to the events
conf["UseWeighting"] = 1
# use the weghting specified in the event
# if you have PtBins, this must be 0 and the above must be 1. Then the Weights are calculated by the 
# resp_cuts code. But please double check if the xsections in resp_cuts are still right.
conf["UseEventWeight"] = 1
conf["UseGlobalWeightBin"] = 1
conf["GlobalXSection"] = 1614.0


conf = JsonConfigBase.ExpandDefaultMcConfig( [0,15,30,60,100,500], conf, True )

JsonConfigBase.Run( conf, sys.argv[0] + ".json")
