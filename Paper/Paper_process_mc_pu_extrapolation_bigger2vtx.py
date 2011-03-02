
# contains methods to create parts of the configuration which are always the same
import JsonConfigBase

# contains configs of the paths used on your machines. in this external config
# so you can run on multiple machines without changing the config file ( .. in theory )
import LocalConfigBase
import sys
import copy

# returns one pipeline with mc configs all set. Default cut values set. etc
# see the GetMcBaseConfig method for details
# also sets the Algos to process
conf = JsonConfigBase.GetMcBaseConfig()

# if you want to change the values of default values, do it now:
#
# conf["Pipelines"]["default"]["CutZMassWindow"] = 23 
# etc

# add the l1 corr to the list
conf["Algos"].append( "ak5PFJetsL1" )
conf["Algos"].append( "ak5PFJetsL1L2L3" )

# easy ...
conf["InputFiles"] = "/afs/naf.desy.de/user/p/piparo/lustre/Paper/ZPJ2011_DY_pu_corrected/dy_fall10_*.root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "extrapolation_dy_pu_more2tvx"

# apply weighting to the events
conf["UseWeighting"] = 1
# use the weghting specified in the event
# if you have PtBins, this must be 0 and the above must be 1. Then the Weights are calculated by the 
# resp_cuts code. But please double check if the xsections in resp_cuts are still right.
conf["UseGlobalWeightBin"] = 1

conf["Pipelines"]["default"]["Filter"].append("recovert")
conf["Pipelines"]["default"]["FilterRecoVertLow"] = 3
conf["Pipelines"]["default"]["FilterRecoVertHigh"] = 100

# this is the magic method. Generates plots for the different pt bins and uses as template
# configuration the one we created so far ( "conf" )
conf = JsonConfigBase.ExpandDefaultMcConfig( [0,15,30,60,100,500], conf, True )
#conf = JsonConfigBase.ExpandDefaultMcConfig( [0,100,500], conf, True )





second_leading_jetpt = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.05, 0.1, 0.13, 0.15, .17, 0.2, 0.3], True, True  )
#CutBack2Back = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [.04,.14,.24,.34], True, True  )

#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
conf["Pipelines"] = dict( conf["Pipelines"].items() + second_leading_jetpt.items() )

# creates the file process_mc_dy.py.json containing all settings and runs bin/resp_cuts.exe 
JsonConfigBase.Run( conf, sys.argv[0] + ".json")

## tada that.s it

