import sys

import JsonConfigBase as base

#getdefaultconfig(data)
conf = base.GetMcBaseConfig()

#change settings
conf["Algos"] = ["ak5PFJets", "ak5PFJetsL1L2L3", "ak5PFJetsL1L2L3CHS"]
conf["InputFiles"] = "/data/berger/data/DYToMuMu-Summer11/*.root"
conf["OutputPath"] = "/data/berger/data/pythia_Nov1"

#setbins()
conf = base.ExpandDefaultMcConfig(
        [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000], conf)
base.ApplyReweighting(conf, 'summer11pythia')

# variations
back2back_var = base.ExpandRange(conf["Pipelines"], "CutBack2Back", [0.24, 0.34,0.44])
secjet_var = base.ExpandRange(conf["Pipelines"], "CutSecondLeadingToZPt", [0.1, 0.15, 0.2, 0.3])
npu_var = base.ExpandRange2(conf["Pipelines"], 'RecoVert', [0, 2, 6], [1, 5, 100])

etabins = [0.0, 0.261, 0.522, 0.783, 0.957, 1.131, 1.305, 1.479, 1.93, 2.322, 2.411, 2.5, 2.853, 2.964, 3.139, 3.489, 5.191]
jeteta_var = base.ExpandRange2(conf["Pipelines"], 'JetEta', etabins[:-1], etabins[1:])

#included
conf["Pipelines"] = dict( conf["Pipelines"].items() + npu_var.items())
conf["Pipelines"] = dict( conf["Pipelines"].items() + jeteta_var.items())
conf["Pipelines"] = dict( conf["Pipelines"].items() + secjet_var.items())
conf["Pipelines"] = dict( conf["Pipelines"].items() + base.CreateEndcapPipelines( conf["Pipelines"] ).items())

#add statistics
conf["Pipelines"]["default"]["AdditionalConsumer"] = ["cut_statistics"]

base.Run(conf, sys.argv[0] + ".json")
