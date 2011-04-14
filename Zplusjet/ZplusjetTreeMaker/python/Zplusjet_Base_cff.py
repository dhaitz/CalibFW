# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

 # 
    #  Customizations for Grid Control follow here
    #

def customise_for_gc(process):
    import FWCore.ParameterSet.Config as cms
    from IOMC.RandomEngine.RandomServiceHelper import RandomNumberServiceHelper

    try:
	    maxevents =GCMAX_EVENTS__
	    process.maxEvents = cms.untracked.PSet(
		    input = cms.untracked.int32(max(-1, maxevents))
	    )
    except:
	    pass

    # Dataset related setup
    try:
	    tmp = GCSKIP_EVENTS__
	    process.source = cms.Source("PoolSource",
		    skipEvents = cms.untracked.uint32(GCSKIP_EVENTS__),
		    fileNames = cms.untracked.vstring(GCFILE_NAMES__)
	    )
	    try:
		    secondary = GCFILE_NAMES2__
		    process.source.secondaryFileNames = cms.untracked.vstring(secondary)
	    except:
		    pass
	    try:
		    lumirange = [GCLUMI_RANGE__]
		    process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange(lumirange)
	    except:
		    pass
    except:
	    pass

    if hasattr(process, "RandomNumberGeneratorService"):
	    randSvc = RandomNumberServiceHelper(process.RandomNumberGeneratorService)
	    randSvc.populate()

    process.AdaptorConfig = cms.Service("AdaptorConfig",
	    enable = cms.untracked.bool(True),
	    stats = cms.untracked.bool(True),
    )

    # Generator related setup
    try:
	    if hasattr(process, "generator"):
		    process.source.firstLuminosityBlock = cms.untracked.uint32(1+GCMY_JOBID__)
		    print "Generator random seed:", process.RandomNumberGeneratorService.generator.initialSeed
    except:
	    pass

    return (process)



def getBaseConfig( Zlist , is_data):
    # Configuration |---------------------------------------------------------------

    process = cms.Process("DATA")
    p = process # shortcut!

    # Begin of TreeMaker-config ====================================================
    # Message logger and count of processed events 

#    p.MessageLogger = cms.Service("MessageLogger",
#    	destinations   = cms.untracked.vstring('cout'),
#    	cout           = cms.untracked.PSet(
#                           threshold = cms.untracked.string('DEBUG')),
            # enable log debug output for all Modules
#            debugModules  = cms.untracked.vstring('*')
#    )

    #import Zplusjet.ZplusjetTreeMaker.ZplusJetConfigHelper as ZplusJetConfigHelper

    p.load('RecoJets.Configuration.RecoPFJets_cff')
    p.load('JetMETCorrections.Configuration.JetCorrectionServicesAllAlgos_cff')
    p.load('JetMETCorrections.Configuration.JetCorrectionProducersAllAlgos_cff')

    # setting the global tag in order to get HLT infos    
    process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
    #process.GlobalTag.globaltag = " HAS TO BE SET BY FINAL CONFIG FILE. check: https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideFrontierConditions#Global_Tags_for_Conditions_Data
    #process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
    #process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')

    p.processedEvents = cms.EDProducer('EventCounter',
	     index_label = cms.untracked.string("processedEvents"),
         )	     
    p.processedEventsPath = cms.Path(process.processedEvents)

    # The Zs  and the Z+jet-analyzer
    import  Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs 
    p.extend(Zs)

    p.flatTreeMaker = cms.EDAnalyzer('ZplusjetTreeMaker',
	    reco_zs_name = cms.string('goodZToMuMuReco'),
	    gen_zs_name = cms.string('goodZToMuMuGen'),
	    zJetPhiBalancing = cms.double(10.0),			# Z plus Jets
	    jetMaxEta = cms.double(8.0),
	    zMuMinDr = cms.double(0.5),	 			# Is muon a seed for the genjet?
        triggerResultsTag = cms.untracked.InputTag("TriggerResults","","HLT"),
	    algo_whitelist_vstring = cms.untracked.vstring("ak5","ak7"),
	    algo_blacklist_vstring = cms.untracked.vstring('CaloJetsL',"JPTJetsL"),
	    secondJetZptRatio = cms.double(100.0),
	    output_filename = cms.untracked.string('out.root'),#'ftm_@PTMIN@to@PTMAX@_7TeV.root'),
        
    )

    p.flatTreeMakerPath = cms.Path(p.flatTreeMaker)

    p.out = cms.OutputModule("PoolOutputModule",
		    fileName = cms.untracked.string("test.root")
		    )
    p.outpath=cms.Path(p.out)

    ## configure PU substraction
    #p.ak5PFJets.doAreaFastjet = True
    #p.kt6PFJets.doRhoFastjet = True
    #p.kt6PFJets.doAreaFastjet = True
    
    #p.L1Fastjet.algorithm = cms.string('AK5Calo') #DUMMY THESE are DUMMY
    #p.L1Fastjet.era = 'Spring10' #DUMMY not used 
    #p.L1Fastjet.level = cms.string('L2Relative') #DUMMY
    #p.L1Fastjet.useCondDB = cms.untracked.bool(False)
    #p.L1Fastjet.srcMedianPt = cms.InputTag('kt6PFJets')    

    ##-------------------- Communicate with the DB -----------------------
    process.load('Configuration.StandardSequences.Services_cff')
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    ##-------------------- Import the JEC services -----------------------
    process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

    ##-------------------- Disable the CondDB for the L1Offset (until they are included in a new global tag)
    process.ak5CaloL1Offset.useCondDB = False
    process.ak5PFL1Offset.useCondDB = False
    process.ak5JPTL1Offset.useCondDB = False

    #process.load("JetMETCorrections.Configuration.JetCorrectionProducers_cff")
    p.ak5CaloJetsL1 = p.ak5CaloJetsL1.clone(correctors = ['ak5CaloL1Offset'])
    p.ak5PFJetsL1 = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1Offset'])
    p.ak5JPTJetsL1 = p.ak5JPTJetsL1.clone(correctors = ['ak5JPTL1Offset'])

    p.ak5CaloJetsL1L2 = p.ak5CaloJetsL1.clone(correctors = ['ak5CaloL2Relative'],src='ak5CaloJetsL1')
    p.ak5PFJetsL1L2 = p.ak5PFJetsL1.clone(correctors = ['ak5PFL2Relative'],src='ak5PFJetsL1')
    p.ak5JPTJetsL1L2 = p.ak5JPTJetsL1.clone(correctors = ['ak5JPTL2Relative'],src='ak5JPTJetsL1')

    p.ak5CaloJetsL1L2L3 = p.ak5CaloJetsL1.clone(correctors = ['ak5CaloL1L2L3'])
    p.ak5PFJetsL1L2L3 = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1L2L3'])
    p.ak5JPTJetsL1L2L3 = p.ak5JPTJetsL1.clone(correctors = ['ak5JPTL1L2L3'])

    p.ak5CaloJetsL1L2L3Res = p.ak5CaloJetsL1.clone(correctors = ['ak5CaloL1L2L3Residual'])
    p.ak5PFJetsL1L2L3Res = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1L2L3Residual'])
    p.ak5JPTJetsL1L2L3Res = p.ak5JPTJetsL1.clone(correctors = ['ak5JPTL1L2L3Residual'])


    # NO CORRECTOR FOR AK7!!!!!!!!!!
    p.ak7CaloJetsL1 = p.ak5CaloJetsL1.clone(correctors = ['ak5CaloL1Offset'], src='ak7CaloJets')
    p.ak7PFJetsL1 = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1Offset'], src='ak7PFJets')
    #p.ak7JPTJetsL1 = p.ak57PTJetsL1.clone(correctors = ['ak5JPTL1Offset'])

    p.ak7CaloJetsL1L2 = p.ak7CaloJetsL1.clone(correctors = ['ak7CaloL2Relative'],src='ak7CaloJetsL1')
    p.ak7PFJetsL1L2 = p.ak7PFJetsL1.clone(correctors = ['ak7PFL2Relative'],src='ak7PFJetsL1')
    #p.ak7JPTJetsL1L2 = p.ak7JPTJetsL1.clone(correctors = ['ak7JPTL2Relative'],src='ak7JPTJetsL1')

    p.ak7CaloJetsL1L2L3 = p.ak7CaloJetsL1.clone(correctors = ['ak5CaloL1Offset','ak7CaloL2Relative','ak7CaloL3Absolute'])
    p.ak7PFJetsL1L2L3 = p.ak7PFJetsL1.clone(correctors = ['ak5PFL1Offset','ak7PFL2Relative','ak7PFL3Absolute'])
    #p.ak7JPTJetsL1L2L3 = p.ak7JPTJetsL1.clone(correctors = ['ak7JPTL1L2L3'])

    p.ak7CaloJetsL1L2L3Res = p.ak7CaloJetsL1.clone(correctors = ['ak5CaloL1Offset','ak7CaloL2Relative','ak7CaloL3Absolute','ak5CaloResidual'])
    p.ak7PFJetsL1L2L3Res = p.ak7PFJetsL1.clone(correctors = ['ak5PFL1Offset','ak7PFL2Relative','ak7PFL3Absolute','ak5PFResidual'])
    #p.ak7JPTJetsL1L2L3Res = p.ak7JPTJetsL1.clone(correctors = ['ak7JPTL1L2L3Residual'])


    p.correctionPathLX= cms.Path(p.ak5PFJetsL1 *
                                 p.ak5PFJetsL1L2 *
                                 p.ak5PFJetsL1L2L3 *
                                 p.ak5PFJetsL1L2L3Res *
                                 p.ak5CaloJetsL1 *
                                 p.ak5CaloJetsL1L2 *
                                 p.ak5CaloJetsL1L2L3 *
                                 p.ak5CaloJetsL1L2L3Res *
                                 p.ak5JPTJetsL1 *
                                 p.ak5JPTJetsL1L2 *
                                 p.ak5JPTJetsL1L2L3 *
                                 p.ak5JPTJetsL1L2L3Res *                                 
                                 p.ak7PFJetsL1 *
                                 p.ak7PFJetsL1L2 *
                                 p.ak7PFJetsL1L2L3 *
                                 p.ak7PFJetsL1L2L3Res *
                                 p.ak7CaloJetsL1 *
                                 p.ak7CaloJetsL1L2 *
                                 p.ak7CaloJetsL1L2L3 *
                                 p.ak7CaloJetsL1L2L3Res)
                                 #p.ak7JPTJetsL1 *
                                 #p.ak7JPTJetsL1L2 *
                                 #p.ak7JPTJetsL1L2L3 *
                                 #p.ak7JPTJetsL1L2L3Res)



    # Now the IDs
    
    #p.load("Configuration.StandardSequences.Geometry_cff")

    
    #p.load("RecoJets.JetProducers.JetIDParams_cfi")

    #p.ak5CaloJetsL1ID = cms.EDProducer('JetIDProducer', 
                                       #p.JetIDParams,
                                       #src = cms.InputTag('ak5CaloJetsL1'),
                                       #alias = cms.untracked.string('ak5CaloJetsL1'))

    #p.ak5CaloJetsL1L2ID = cms.EDProducer('JetIDProducer', 
                                       #p.JetIDParams,
                                       #src = cms.InputTag('ak5CaloJetsL1L2'),
                                       #alias = cms.untracked.string('ak5CaloJetsL1L2'))

    #p.ak5CaloJetsL1L2L3ResID = cms.EDProducer('JetIDProducer', 
                                              #p.JetIDParams,
                                              #src = cms.InputTag('ak5CaloJetsL1L2L3Res'),
                                              #alias = cms.untracked.string('ak5CaloJetsL1L2L3Res'))
                                       
                                       

    #p.JetIDsPath = cms.Path(p.ak5CaloJetsL1ID*p.ak5CaloJetsL1L2ID*p.ak5CaloJetsL1L2L3ResID)



    # The gen jets matching --------------------------------------------------------

    #calo_jets_names=("iterativeCone5CaloJets",
                     #"kt4CaloJets",
                     #"kt6CaloJets",
                     #"sisCone5CaloJets",
                     #"sisCone7CaloJets",
                     #"ak5CaloJets")

    #gen_jets_names=("iterativeCone5GenJets",
                    #"kt4GenJets",
                    #"kt6GenJets",
                    #"sisCone5GenJets",
                    #"sisCone7GenJets",
                    #"ak5GenJets")
                    
    #jets_names_dict=zip(gen_jets_names,calo_jets_names)
    
    pf_jets_names=("ak5PFJets",
                   "ak5PFJetsL1",
                   "ak5PFJetsL1L2",
                   "ak5PFJetsL1L2L3",
                   "ak5PFJetsL1L2L3Res",
                   "ak7PFJets",
                   "ak7PFJetsL1",
                   "ak7PFJetsL1L2",
                   "ak7PFJetsL1L2L3",
                   "ak7PFJetsL1L2L3Res")
                   
    calo_jets_names=map(lambda name: name.replace("PF","Calo"), pf_jets_names)

    jets_names_map=zip(pf_jets_names,calo_jets_names)
    #print jets_names_map

    #p.offsetCorrection = cms.Sequence(p.ak5PFJets * p.kt6PFJets * p.ak5PFJetsL1)
    #p.offsetCorrectionPath=cms.Path(p.offsetCorrection)

    calojetsmatch_Paths=[]

    for pf_jets_name,calo_jets_name in jets_names_map:
        module_name="%scalomatch" %pf_jets_name
        setattr(p,module_name,
                cms.EDProducer('MatchRecToGen',
                srcRec = cms.InputTag(pf_jets_name),
                srcGen = cms.InputTag(calo_jets_name)))
        print "Module name %s" %module_name

        exec( "p.%spath=cms.Path(p.%s)" %(module_name,module_name) )
        exec("calojetsmatch_Paths.append(p.%spath)" %module_name)


    genjetsmatch_Paths=[]
    gen_jets_names=("ak5GenJets",
                    "ak5GenJets",
                    "ak5GenJets",
                    "ak5GenJets",
                    "ak5GenJets",
                    "ak7GenJets",
                    "ak7GenJets",
                    "ak7GenJets",
                    "ak7GenJets",
                    "ak7GenJets")   
    jets_names_map=zip(pf_jets_names,gen_jets_names)                    
    for pf_jets_name,gen_jets_name in jets_names_map:
        module_name="%sgenmatch" %pf_jets_name
        setattr(p,module_name,
                cms.EDProducer('MatchRecToGen',
                srcRec = cms.InputTag(pf_jets_name),
                srcGen = cms.InputTag(gen_jets_name)))
        print "Module name %s" %module_name

        exec( "p.%spath=cms.Path(p.%s)" %(module_name,module_name) )
        exec("genjetsmatch_Paths.append(p.%spath)" %module_name)

    
    jpt_jets_names=map(lambda name: name.replace("PF","JPT"), pf_jets_names)

    jets_names_map=zip(pf_jets_names,jpt_jets_names)
    jptjetsmatch_Paths=[]

    for pf_jets_name,jpt_jets_name in jets_names_map:
	if "ak7" in  jpt_jets_name: continue
	if "ak5JPTJets" ==  jpt_jets_name: continue
        module_name="%sjptmatch" %pf_jets_name
        setattr(p,module_name,
                cms.EDProducer('MatchRecToGen',
                srcRec = cms.InputTag(pf_jets_name),
                srcGen = cms.InputTag(jpt_jets_name)))
        print "Module name %s" %module_name

        exec( "p.%spath=cms.Path(p.%s)" %(module_name,module_name) )
        exec("jptjetsmatch_Paths.append(p.%spath)" %module_name)




    # Match the Z Bosons
    p.matchZs=cms.EDProducer('MatchRecToGen',
                             srcRec = cms.InputTag("goodZToMuMuReco"),
                             srcGen = cms.InputTag("goodZToMuMuGen"))
    p.matchZspath=cms.Path(p.matchZs)


    # Schedule
    process.schedule = cms.Schedule(
      p.processedEventsPath,    # TreeMaker events
      Zlist,                    # contains reco and gen events
      #p.offsetCorrectionPath,
      p.correctionPathLX,
      #p.JetIDsPath,          
    #	p.outpath,
    )

  
    for path in calojetsmatch_Paths:
      process.schedule.append(path)
   
    for path in jptjetsmatch_Paths:
      process.schedule.append(path)
      
    if not is_data:
      for path in genjetsmatch_Paths:
        p.schedule.append(path)      
        p.schedule.append(p.matchZspath)
      
    process.schedule.append(p.flatTreeMakerPath)

    process = customise_for_gc(p)
    return process

# grid-control: https://ekptrac.physik.uni-karlsruhe.de/trac/grid-control
