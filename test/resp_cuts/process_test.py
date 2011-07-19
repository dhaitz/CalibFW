
import JsonConfigBase
import LocalConfigBase
import sys
import copy
import subprocess

doCompare = False

gitrev = subprocess.Popen("git log -n 1 | head -n 1" , stdout=subprocess.PIPE, shell=True).stdout.read()
#create an output file with the current git revision
gitrev = gitrev.split(" ")[1].strip()
print "HEAD git revision: " + gitrev

conf = JsonConfigBase.GetMcBaseConfig()
conf["Algos"] = [ "ak5PFJets" ]
conf["InputFiles"] = "data/eval/mc_cmssw42.root"

if len(sys.argv) > 1 and sys.argv[1] == "compareToHead":
     conf["OutputPath"] = "test/resp_cuts/mc_cmssw42_current"
     doCompare = True
else:
     conf["OutputPath"] = "test/resp_cuts/mc_cmssw42_git_" + gitrev 

print "writing to " + conf["OutputPath"] 

conf["UseWeighting"] = 0
#conf["UseEventWeight"] = 1

conf = JsonConfigBase.ExpandDefaultMcConfig( [0,30,60,100,140,300], conf, True )

#conf["Pipelines"]["default"]["CutSecondLeadingToZPtJet2Threshold"] = 0

#SecondJetNoCut = copy.deepcopy( conf["Pipelines"]["default"] )
#SecondJetNoCut["FilterInCutIgnored"] = 16
#SecondJetNoCut["RootFileFolder"] = "NoSecondJetCut"

#conf["Pipelines"]["NoSecondJetCut"] = SecondJetNoCut

#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutMuonPt", range(10, 20), True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutZMassWindow", [17, 20, 23], True, True  )
#muon_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44], True, True  ) 
#zmass_var = JsonConfigBase.ExpandRange( conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3], True, True  )


#conf["Pipelines"] = dict( conf["Pipelines"].items() + muon_var.items() )




#conf["Pipelines"] = dict( conf["Pipelines"].items() + zmass_var.items() )

# additional consumer
conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

JsonConfigBase.Run( conf, sys.argv[0] + ".json")

# compare somewhere down here 
