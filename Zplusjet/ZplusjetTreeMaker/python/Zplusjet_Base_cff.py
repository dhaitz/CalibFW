# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms

def customise_for_gc(process):
	"""
	Customizations for Grid Control follow here
	grid-control: https://ekptrac.physik.uni-karlsruhe.de/trac/grid-control
	"""
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
	process = cms.Process("DATA")
	
	process.load('CommonTools.ParticleFlow.pfNoPileUp_cff')

	# WORK AROUND FOR NEW JEC, dont use if the new global tag is available
#	process.load("CondCore.DBCommon.CondDBCommon_cfi")
#	process.jec = cms.ESSource("PoolDBESSource",
#	      DBParameters = cms.PSet(
#       	messageLevel = cms.untracked.int32(0)
#	        ),
#	timetype = cms.string('runnumber'),
#     	toGet = cms.VPSet(
#    	cms.PSet(
#         record = cms.string('JetCorrectionsRecord'),
#        tag    = cms.string('JetCorrectorParametersCollection_Jec11V2_AK5PF'),
#            label  = cms.untracked.string('AK5PF')
#            ),
#      	cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_Jec11V2_AK5PFchs'),
#            label  = cms.untracked.string('AK5PFchs')
#            ),
#        cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_Jec11V2_AK7PF'),
#            label  = cms.untracked.string('AK7PF')
#            )   
#        ),

      	## here you add as many jet types as you need (AK5PFchs, AK5Calo, AK5JPT, AK7PF, AK7Calo, KT4PF, KT4Calo)
#     	connect = cms.string('sqlite:Jec11V2.db')
#	)
	# Add an es_prefer statement to get your new JEC constants from the sqlite file, rather than from the global tag
#	process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

	
	p = process # shortcut!

	# Count the number of processed events
	p.processedEvents = cms.EDProducer('EventCounter',
		index_label = cms.untracked.string("processedEvents"))
	p.processedEventsPath = cms.Path(process.processedEvents)

	# Analyzer: The Zs  and the Z+jet-analyzer
	import Zplusjet.ZplusjetTreeMaker.Z_collections_noprecuts_cfi as Zs
	p.extend(Zs)

	p.flatTreeMaker = cms.EDAnalyzer('ZplusjetTreeMaker',
		reco_zs_name = cms.string('goodZToMuMuReco'),
		gen_zs_name = cms.string('goodZToMuMuGen'),
		zJetPhiBalancing = cms.double(10.0),			# Z plus Jets
		jetMaxEta = cms.double(8.0),
		zMuMinDr = cms.double(0.5),				# Is muon a seed for the genjet?
		triggerResultsTag = cms.untracked.InputTag("TriggerResults","","HLT"),
		algo_whitelist_vstring = cms.untracked.vstring("ak5"),
		algo_blacklist_vstring = cms.untracked.vstring('CaloJetsL',"JPTJetsL"),
		secondJetZptRatio = cms.double(100.0),
		output_filename = cms.untracked.string('skim.root'),
		JetCorrectionService = cms.string("ak5PFL1FastL2L3"),
	)
	p.flatTreeMakerPath = cms.Path(p.flatTreeMaker)

	# The output module
	# include this only for testing, NEVER use this in grid runs !
	p.out = cms.OutputModule("PoolOutputModule",
			fileName = cms.untracked.string("test_out.root"))
	p.outpath=cms.EndPath(p.out)

	# The JEC module
	### Geometry and Detector Conditions (needed for a few patTuple production steps)
	process.load("Configuration.StandardSequences.Services_cff")
	process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

	##----------Import the JEC services -----------------------
	process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
	process.load("JetMETCorrections.Configuration.JetCorrectionProducers_cff")
	##--------- Import the Jet RECO modules -------------------
	process.load('RecoJets.Configuration.RecoPFJets_cff')
	##--------- Turn-on the FastJet density calculation -------
	process.kt6PFJets.doRhoFastjet = True
	##--------- Turn-on the FastJet jet area calculation for your algorithm ----
	process.ak5PFJets.doAreaFastjet = True
        process.ak7PFJets.doAreaFastjet = True


        # custom FastJet calculation for the CHS set of particles
        p.ak5PFL1FastjetCHS = process.ak5PFL1Fastjet.clone(srcRho = cms.InputTag("kt6PFJetsCHS", "rho"), algorithm = 'AK5PFchs')



	# custom correction services
	p.ak5PFL1FastL2 = cms.ESSource( 'JetCorrectionServiceChain',
				    correctors = cms.vstring('ak5PFL1Fastjet','ak5PFL2Relative')
    		)

        p.ak5PFL1FastL2CHS = cms.ESSource( 'JetCorrectionServiceChain',
                                       correctors = cms.vstring('ak5PFL1FastjetCHS','ak5PFL2Relative')
                )


	# custom L1L2L3 CHS corrections
        p.ak5PFL1FastL2L3CHS = p.ak5PFL1L2L3.clone()
	p.ak5PFL1FastL2L3CHS.correctors[0] = 'ak5PFL1FastjetCHS'

        p.ak5PFL1FastL2L3ResCHS = p.ak5PFL1FastL2L3Residual.clone()
        p.ak5PFL1FastL2L3ResCHS.correctors[0] = 'ak5PFL1FastjetCHS'


        p.ak7PFL1FastL2 = cms.ESSource( 'JetCorrectionServiceChain',
                                    correctors = cms.vstring('ak7PFL1Fastjet','ak7PFL2Relative')
                )


#	p.ak5PFJetsL1 = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1Fastjet'])

	# Jet Producer

	# custom L1L2 corrections
	p.ak5PFJetsL1 = cms.EDProducer(
            'PFJetCorrectionProducer',
            src        = cms.InputTag('ak5PFJets'),
            correctors = cms.vstring('ak5PFL1Fastjet')
            )

	# custom L1L2 corrections
	p.ak5PFJetsL1L2 = p.ak5PFJetsL1.clone( correctors = ['ak5PFL1FastL2'])
	p.ak5PFJetsL1L2L3 = p.ak5PFJetsL1.clone( correctors = ['ak5PFL1FastL2L3'])
	p.ak5PFJetsL1L2L3Res = p.ak5PFJetsL1.clone( correctors = ['ak5PFL1FastL2L3Residual'])	

        p.ak7PFJetsL1 = cms.EDProducer(
            'PFJetCorrectionProducer',
            src        = cms.InputTag('ak7PFJets'),
            correctors = cms.vstring('ak7PFL1Fastjet')
            )
 
        # custom L1L2 corrections
        p.ak7PFJetsL1L2 = p.ak7PFJetsL1.clone( correctors = ['ak7PFL1FastL2'])
	p.ak7PFJetsL1L2L3 = p.ak7PFJetsL1.clone( correctors = ['ak7PFL1FastL2L3'])
	p.ak7PFJetsL1L2L3Res = p.ak7PFJetsL1.clone( correctors = ['ak7PFL1FastL2L3Residual'])
	
	
#	p.ak5PFJetsL1L2L3 = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1FastL2L3'])
#	p.ak5PFJetsL1L2L3Res = p.ak5PFJetsL1.clone(correctors = ['ak5PFL1FastL2L3Residual'])

#        p.ak7PFJetsL1L2L3 = p.ak7PFJetsL1.clone(correctors = ['ak7PFL1FastL2L3'])
#        p.ak7PFJetsL1L2L3Res = p.ak7PFJetsL1.clone(correctors = ['ak7PFL1FastL2L3Residual'])



#	p.ak7PFJetsL1 = p.ak7PFJetsL1.clone(correctors = ['ak7PFL1Offset'])
#	p.ak7PFJetsL1L2 = p.ak7PFJetsL1.clone(correctors = ['ak7PFL2Relative'],src='ak7PFJetsL1')
#	p.ak7PFJetsL1L2L3 = p.ak7PFJetsL1.clone(correctors = ['ak7PFL1L2L3'])
#	p.ak7PFJetsL1L2L3Res = p.ak7PFJetsL1.clone(correctors = ['ak5PFL1L2L3Residual'])

	#process.load("JetMETCorrections.Configuration.JetCorrectionProducers_cff")

	# apply PF no PU 
	process.ak5PFJetsCHS = p.ak5PFJets.clone()
	process.ak5PFJetsCHS.src = cms.InputTag("pfNoPileUp")

        process.ak5PFJetsL1CHS = p.ak5PFJetsL1.clone()
        process.ak5PFJetsL1CHS.src = cms.InputTag("ak5PFJetsCHS")
	process.ak5PFJetsL1CHS.correctors = cms.vstring('ak5PFL1FastjetCHS')

        process.ak5PFJetsL1L2CHS = p.ak5PFJetsL1CHS.clone()
        process.ak5PFJetsL1L2CHS.correctors = cms.vstring('ak5PFL1FastL2CHS')

	process.ak5PFJetsL1L2L3CHS = p.ak5PFJetsL1CHS.clone()
	process.ak5PFJetsL1L2L3CHS.correctors = cms.vstring("ak5PFL1FastL2L3CHS")

        process.ak5PFJetsL1L2L3ResCHS = p.ak5PFJetsL1CHS.clone()
        process.ak5PFJetsL1L2L3ResCHS.correctors = cms.vstring("ak5PFL1FastL2L3ResCHS")


	process.kt6PFJetsCHS = p.kt6PFJets.clone()
	process.kt6PFJetsCHS.src = cms.InputTag("pfNoPileUp")

	# no chs available for ak7 yet

        #process.ak5PFJetsL1L2NoPU = p.ak5PFJetsL1L2.clone()
        #process.ak5PFJetsL1L2NoPU.src = cms.InputTag("ak5PFJetsL1NoPU")

        #process.ak5PFJetsL1L2L3NoPU = p.ak5PFJetsL1L2L3.clone()
        #process.ak5PFJetsL1L2L3NoPU.src = cms.InputTag("ak5PFJetsNoPU")

        #process.ak7PFJetsNoPU = p.ak7PFJets.clone()
        #process.ak7PFJetsNoPU.src = 'pfNoPileUp'

        #process.ak7PFJetsL1NoPU = p.ak7PFJetsL1.clone()
        #process.ak7PFJetsL1NoPU.src = 'pfNoPileUp' 

        #process.ak7PFJetsL1L2NoPU = p.ak7PFJetsL1L2.clone()
        #process.ak7PFJetsL1L2NoPU.src = 'pfNoPileUp' 

        #process.ak7PFJetsL1L2L3NoPU = p.ak7PFJetsL1L2L3.clone()
        #process.ak7PFJetsL1L2L3NoPU.src = 'pfNoPileUp'

	p.correctionPathLX= cms.Path(
		p.kt6PFJets     	* p.kt6PFJetsCHS *
		p.ak5PFJets     	* p.ak5PFJetsCHS *
		p.ak5PFJetsL1   	* p.ak5PFJetsL1CHS *
		p.ak5PFJetsL1L2		* p.ak5PFJetsL1L2CHS *
		p.ak5PFJetsL1L2L3 	* p.ak5PFJetsL1L2L3CHS *

                p.ak7PFJets             * #p.ak7PFJetsCHS *
                p.ak7PFJetsL1           * #p.ak7PFJetsL1CHS *
                p.ak7PFJetsL1L2         * #p.ak7PFJetsL1L2CHS *
                p.ak7PFJetsL1L2L3       #* p.ak7PFJetsL1L2L3CHS
#		p.ak5PFJetsL1L2L3Res 
#	        p.ak5PFJetsL1NoPU * p.ak5PFJetsL1L2NoPU * p.ak5PFJetsL1L2L3NoPU 
		#p.ak7PFJetsNoPU * p.ak7PFJetsL1NoPU * p.ak7PFJetsL1L2NoPU * p.ak7PFJetsL1L2L3NoPU *
#		p.ak5PFJetsL1 * p.ak5PFJetsL1L2 * p.ak5PFJetsL1L2L3 * #p.ak5PFJetsL1L2L3Res *
#		p.ak5CaloJetsL1 * p.ak5CaloJetsL1L2 * p.ak5CaloJetsL1L2L3 * p.ak5CaloJetsL1L2L3Res *
#		p.ak5JPTJetsL1 * p.ak5JPTJetsL1L2 * p.ak5JPTJetsL1L2L3 * p.ak5JPTJetsL1L2L3Res *
#		p.ak7PFJetsL1 * p.ak7PFJetsL1L2 * p.ak7PFJetsL1L2L3 #* p.ak7PFJetsL1L2L3Res
#		p.ak7CaloJetsL1 * p.ak7CaloJetsL1L2 * p.ak7CaloJetsL1L2L3 * p.ak7CaloJetsL1L2L3Res)
		#p.ak7JPTJetsL1 * p.ak7JPTJetsL1L2 * p.ak7JPTJetsL1L2L3 * p.ak7JPTJetsL1L2L3Res)
	)
	

	if is_data:
		p.correctionPathLX *= p.ak5PFJetsL1L2L3Res * p.ak5PFJetsL1L2L3ResCHS
		p.correctionPathLX *= p.ak7PFJetsL1L2L3Res
		## do ak7

#	print p.pfNoPileUp
	

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
#				"ak5PFJetsL1L2L3Res",
				"ak7PFJets",
				"ak7PFJetsL1",
				"ak7PFJetsL1L2",
				"ak7PFJetsL1L2L3")
#				"ak7PFJetsL1L2L3Res")

#	calo_jets_names=map(lambda name: name.replace("PF","Calo"), pf_jets_names)
	calo_jets_names=[]

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
#					"ak5GenJets",
					"ak7GenJets",
					"ak7GenJets",
					"ak7GenJets",
#					"ak7GenJets",
					"ak7GenJets")
	jets_names_map=zip(pf_jets_names,gen_jets_names)
	for pf_jets_name,gen_jets_name in jets_names_map:
		module_name="%sgenmatch" %pf_jets_name
		setattr(p,module_name,
				cms.EDProducer('MatchRecToGen',
				srcRec = cms.InputTag(pf_jets_name),
				srcGen = cms.InputTag(gen_jets_name)))
		#print "Module name %s" %module_name

		exec( "p.%spath=cms.Path(p.%s)" %(module_name,module_name) )
		exec("genjetsmatch_Paths.append(p.%spath)" %module_name)


#	jpt_jets_names=map(lambda name: name.replace("PF","JPT"), pf_jets_names)
	jpt_jets_names=[]

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
		#print "Module name %s" %module_name

		exec( "p.%spath=cms.Path(p.%s)" %(module_name,module_name) )
		exec("jptjetsmatch_Paths.append(p.%spath)" %module_name)

	# Match the Z Bosons
	p.matchZs=cms.EDProducer('MatchRecToGen',
							srcRec = cms.InputTag("goodZToMuMuReco"),
							srcGen = cms.InputTag("goodZToMuMuGen"))
	p.matchZspath=cms.Path(p.matchZs)

	p.pfCHS = cms.Path( p.pfNoPileUpSequence )

	# Schedule
	process.schedule = cms.Schedule(
		p.processedEventsPath,	# TreeMaker events
		p.pfCHS,
		Zlist,					# contains reco and gen events
		p.correctionPathLX,
		#p.JetIDsPath,
		#p.CHSpath
	)
	#process.schedule.append(p.outpath)

	for path in calojetsmatch_Paths:
		process.schedule.append(path)

	for path in jptjetsmatch_Paths:
		process.schedule.append(path)

	if not is_data:
		for path in genjetsmatch_Paths:
			p.schedule.append(path)
			p.schedule.append(p.matchZspath)

	process.schedule.append(p.flatTreeMakerPath) #	p.outpath

	
	process = customise_for_gc(p)
	return process
