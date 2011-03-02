
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
conf = JsonConfigBase.GetDataBaseConfig()

# if you want to change the values of default values, do it now:
#
# conf["Pipelines"]["default"]["CutZMassWindow"] = 23 
# etc

# add the l1 corr to the list
conf["Algos"].append( "ak5PFJetsL1" )

# easy ...
conf["InputFiles"] = "/afs/naf.desy.de/user/p/piparo/lustre/DiplomArbeitThomas/ZPJ2011_data_nov4/*root" 
conf["OutputPath"] = LocalConfigBase.GetLocalOutputPath() + "extrapolation_data"

# apply weighting to the events
#conf["UseWeighting"] = 1
# use the weghting specified in the event
# if you have PtBins, this must be 0 and the above must be 1. Then the Weights are calculated by the 
# resp_cuts code. But please double check if the xsections in resp_cuts are still right.
#conf["UseGlobalWeightBin"] = 1

# this is the magic method. Generates plots for the different pt bins and uses as template
# configuration the one we created so far ( "conf" )
conf = JsonConfigBase.ExpandDefaultDataConfig( [0,30,60,100,140,500], conf, True )
#conf = JsonConfigBase.ExpandDefaultMcConfig( [0,100,500], conf, True )

myown = copy.deepcopy( conf["Pipelines"]["default"] )
myown["Filter"].append("ptbin")
myown["FilterPtBinLow"] = 10.0
myown["FilterPtBinHigh"] = 500.0
myown["RootFileFolder"] = "Pt10to500_incut"

conf["Pipelines"][ "Pt10to500_incut" ] = myown

# this methods can be used to create cut variations on any arbitrary configuration variable
# I can show you how this works face to face or by phone. it is way easier to explain then

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", [10, 15, 20], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
second_leading_jetpt = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1,0.15, 0.2, 0.3, 0.4,0.5], True, True  )
CutBack2Back = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [.04,.14,.24,.34,.44,.55], True, True  )

#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )
conf["Pipelines"] = dict( conf["Pipelines"].items() + second_leading_jetpt.items()+CutBack2Back.items() )

# creates the file process_mc_dy.py.json containing all settings and runs bin/resp_cuts.exe 
JsonConfigBase.Run( conf, sys.argv[0] + ".json")

## tada that.s it
