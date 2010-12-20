import copy

def GetBaseConfig():
    d = dict()
    
    d["Algos"] = ["ak5PF", "ak5Calo"]
    d["Pipelines"] = { "default": {
            "CutMuonEta": 2.3,
            "CutMuonPt": 15,
            "CutZMassWindow": 20,
            "CutLeadingJetEta": 1.3,
            "CutSecondLeadingToZPt": 0.2,
            "CutBack2Back": 0.34,
            "Cuts": ["json", 
                     "muon_pt",
                     "zmass_window",
                     "zmass_window",
                     "back_to_back",
                     "secondleading_to_zpt",
                     "leadingjet_eta",
                     "muon_eta"],
            "Filter":[]
                      }
            }
            
    
    return d

def GetMcBaseConfig():
    d = GetBaseConfig()
    
    d["UseWeighting"] = 1
    d["UseEventWeight"] = 0
    d["InputType"] = "mc"

    return d

def GetDataBaseConfig():
    d = GetBaseConfig()
    
    d["JsonFile"] = "data/json/Cert_132440-149442_7TeV_StreamExpress_Collisions10_JSON_v3.txt"
    d["UseWeighting"] = 0
    d["UseEventWeight"] = 0
    d["InputType"] = "data"

    return d

def ExpandCutNoCut( pipelineDict):
    newDict = dict()

    for name, elem in pipelineDict.items():
        nocutPipe = copy.deepcopy(elem)
        cutPipe = copy.deepcopy(elem)
        
        cutPipe["Filter"].append ("incut")

        newDict[name + "nocuts" ] = nocutPipe
        newDict[name] = cutPipe

    return newDict

def Expand( pipelineDict, expandCount):
    newDict = dict()

    for name, elem in pipelineDict.items():
        for i in range( expandCount):
            newPipe = copy.deepcopy(elem)
            newDict[name + str(i) ] = newPipe

    return newDict

def ExpandPtBins( pipelineDict, ptbins):
    newDict = dict()
    
    for name, elem in pipelineDict.items():
        i = 0
        for upper in ptbins[1:]:
            ptbinsname =  str(ptbins[i]) + "to" + str(upper)

            newPipe = copy.deepcopy(elem)
            
            newPipe["Filter"].append( "ptbin")
            
            newPipe["FilterPtBinLow"] = ptbins[i]
            newPipe["FilterPtBinHigh"] = upper

            newDict[name + ptbinsname ] = newPipe
            i = i + 1

    return newDict
    
    
    

