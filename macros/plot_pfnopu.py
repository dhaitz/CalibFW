# -*- coding: utf-8 -*-

import getROOT
import sys, os, math #, kein pylab!
import time
import numpy as np
import matplotlib
matplotlib.rc('text', usetex = True) 
matplotlib.rc('font', size = 16)
#import matplotlib.pyplot as plt
import plotBase

print "%1.2f Start with settings" % time.clock()
### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png', 'pdf', 'txt', 'dat']
settings.lumi = 206.26826
settings.verbosity = 2
#factor = global_factor = 0.158781242981 # qualitycuts
factor = global_factor = 0.141210916022 # incuts

factor10 = 36*0.001*0.75
mc11color = 'FireBrick'
mc10color = 'MidnightBlue'
data11color = 'black'
data10color = 'gray'

from_folder = "NoBinning_incut/"

###quantity R(ptZ) pt eta phi
###set algo, cone size, #pv, L1L2L3, 
###data/mc(generator,tune)
###document: serif,LaTeX ; slides: sans serif 
#mpl.rc('text', usetex=True)#does not work on the naf! no latex there
#matplotlib.rc('font', family='serif')
#mpl.pylab.rcParams['legend.loc'] = 'best' rc lines.color linewidth font family weight size

#### INPUTFILES
print "%1.2f Open files:" % time.clock()
fdata = getROOT.OpenFile(plotBase.GetPath() + "Run2011A-May10ReReco.root", (settings.verbosity>1))[0]
fmc  = getROOT.OpenFile(plotBase.GetPath() + "DYToMuMu_Summer11-PU.root", (settings.verbosity>1))[0]

#def mostgenerictest():
#	
#	def mytest( tf, ta, tname, plot_collection, caption, settings ):
#		ta = plotBase.AxisLabels(ta,'pt', 'Z')
#		return None
#	
#	plotBase.hist_baseplot( [(from_folder + 'jet1_pt_ak5PFJetsNoPU_Zplusjet_data_hist',  
#			    fdata, None)], 
#				"j_pt_test", settings, mytest )

def jet2pt():
	plotBase.genericplot('jet2_pt', 'pt', 'jet2', 
						fdata, { 'correction': 'NoPU'}, 
						fmc, { 'correction': 'NoPU'}, 
						factor, settings)

def jet_constituents( jetnum):
	
	def mytest( tf, ta, tname, plot_collection, caption, settings ):
		ta = plotBase.AxisLabels(ta, 'jetconstituents', 'jet')
		ta.legend(loc='upper right', numpoints=1, frameon=False)
		
		return None
	
	#def DataDots( histo, settings ):
	#	histo.
		
	def ScaleMc( rootinput, settings ):
		rootinput.scale( global_factor )
	
	plotBase.hist_baseplot( [(from_folder + 'jet' + str(jetnum) + '_constituents_ak5PFJetsNoPU_Zplusjet_data_hist',  
			    			fdata, 
			    			{ "label":'Data NoPU', "fmt":"o", "color":'#000000'},  
			    			None, None),
							(from_folder + 'jet' + str(jetnum) + '_constituents_ak5PFJets_Zplusjet_data_hist',  
			    			fdata, 
			    			{ "label":'Data', "fmt":'^', "color":'#999999'},  
			    			None, None),
						 	(from_folder + 'jet' + str(jetnum) + '_constituents_ak5PFJetsNoPU_Zplusjet_mc_hist',  
			    			fmc, 
			    			{"label":'MC NoPU', "color":'#580000'},  
			    			None, ScaleMc),
						 	(from_folder + 'jet' + str(jetnum) + '_constituents_ak5PFJets_Zplusjet_mc_hist',  
			    			fmc, 
			    			{"label":'MC', "color":'#ff5151'},  
			    			None, ScaleMc)],
						    "jet" + str(jetnum) + "_constituents", settings, mytest )

def jet_pt( jetnum):
	
	def mytest( tf, ta, tname, plot_collection, caption, settings ):
		ta = plotBase.AxisLabels(ta, 'pt', 'jet')
		ta.legend(loc='upper right', numpoints=1, frameon=False)
		
		if jetnum == 2:
			print "ieieieieie"
			ta.set_xlim(0,80)
			#ta.autoscale()
	#def DataDots( histo, settings ):
	#	histo.
		
	def ScaleMc( rootinput, settings ):
		rootinput.scale( global_factor )
	
	plotBase.hist_baseplot( [(from_folder + 'jet' + str(jetnum) + '_pt_ak5PFJetsNoPU_Zplusjet_data_hist',  
			    			fdata, 
			    			{ "label":'Data NoPU', "fmt":"o", "color":'#000000'},  
			    			None, None),
							(from_folder + 'jet' + str(jetnum) + '_pt_ak5PFJets_Zplusjet_data_hist',  
			    			fdata, 
			    			{ "label":'Data',  "fmt":'^', "color":'#999999'},  
			    			None, None),
						 	(from_folder + 'jet' + str(jetnum) + '_pt_ak5PFJetsNoPU_Zplusjet_mc_hist',  
			    			fmc, 
			    			{"label":'MC NoPU', "color":'#580000'},  
			    			None, ScaleMc),
						 	(from_folder + 'jet' + str(jetnum) + '_pt_ak5PFJets_Zplusjet_mc_hist',  
			    			fmc, 
			    			{"label":'MC', "color":'#ff5151'},  
			    			None, ScaleMc)],
						    "jet" + str(jetnum) + "_pt", settings, mytest )


def resp_mpf():
	
	def mytest( tf, ta, tname, plot_collection, caption, settings ):
		ta = plotBase.AxisLabels(ta, 'mpfresp', 'jet')
		ta.legend(loc='lower right', numpoints=1, frameon=False)
		
	#def DataDots( histo, settings ):
	#	histo.
	
	plotBase.hist_baseplot( [('mpfresp_ak5PFJetsNoPU_Zplusjet_data_graph',  
			    			fdata, 
			    			{ "label":'Data NoPU', "fmt":"o", "color":'#000000'},  
			    			None, None),
							('mpfresp_ak5PFJets_Zplusjet_data_graph',  
			    			fdata, 
			    			{ "label":'Data', "fmt":'^', "color":'#999999'},  
			    			None, None),
						 	('mpfresp_ak5PFJetsNoPU_Zplusjet_mc_graph',  
			    			fmc, 
			    			{"label":'MC NoPU', "fmt":"o","color":'#580000'},  
			    			None, None),
						 	('mpfresp_ak5PFJets_Zplusjet_mc_graph',  
			    			fmc, 
			    			{"label":'MC', "fmt":'^', "color":'#ff5151'},  
			    			None, None)],
						    "resp_mpf", settings, mytest, False )

#cut_ineff_secondleading_to_zpt_nrv
def cufineff( cut ):
	
	def RemoveNpvZero(rootHisto, settings):
		rootHisto.dropbin ( 0)
	
	def mytest( tf, ta, tname, plot_collection, caption, settings ):
		ta = plotBase.AxisLabels(ta, 'cutineff', 'jet')
		ta.legend(loc='lower right', numpoints=1, frameon=False)
	
	plotBase.hist_baseplot( [(from_folder + '' + cut + '_ak5PFJetsNoPU_Zplusjet_data_graph',  
			    			fdata, 
			    			{ "label":'Data NoPU', "fmt":"o", "color":'#000000'},  
			    			None, RemoveNpvZero),
							(from_folder + '' + cut + '_ak5PFJets_Zplusjet_data_graph',  
			    			fdata, 
			    			{ "label":'Data', "fmt":'^', "color":'#999999'},  
			    			None, RemoveNpvZero),
						 	(from_folder + '' + cut + '_ak5PFJetsNoPU_Zplusjet_mc_graph',  
			    			fmc, 
			    			{"label":'MC NoPU', "fmt":"o", "color":'#580000'},  
			    			None, RemoveNpvZero),
						 	(from_folder + '' + cut + '_ak5PFJets_Zplusjet_mc_graph',  
			    			fmc, 
			    			{"label":'MC', "fmt":'^',"color":'#ff5151'},  
			    			None, RemoveNpvZero)],
						    cut, settings, mytest, False )

def resp_balance():
	
	def mytest( tf, ta, tname, plot_collection, caption, settings ):
		ta = plotBase.AxisLabels(ta, 'jetresp', 'jet')
		ta.legend(loc='lower right', numpoints=1, frameon=False)
		
	#def DataDots( histo, settings ):
	#	histo.
	
	plotBase.hist_baseplot( [('jetresp_ak5PFJetsNoPU_Zplusjet_data_graph',  
			    			fdata, 
			    			{ "label":'Data NoPU', "fmt":"o", "color":'#000000'},  
			    			None, None),
							('jetresp_ak5PFJets_Zplusjet_data_graph',  
			    			fdata, 
			    			{ "label":'Data', "fmt":'^', "color":'#999999'},  
			    			None, None),
						 	('jetresp_ak5PFJetsNoPU_Zplusjet_mc_graph',  
			    			fmc, 
			    			{"label":'MC NoPU', "fmt":"o", "color":'#580000'},  
			    			None, None),
						 	('jetresp_ak5PFJets_Zplusjet_mc_graph',  
			    			fmc, 
			    			{"label":'MC', "fmt":'^', "color":'#ff5151'},  
			    			None, None)],
						    "resp_balance", settings, mytest, False )
		
#def npv_qualitycuts():
#	plotBase.genericplot('recovert', 'recovert', 'NPV', 
#						fdata, { 'correction': 'NoPU', 'incut':'qualitycuts'}, 
#						fmc, { 'correction': 'NoPU', 'incut':'qualitycuts'}, 
#						factor, settings)


#def npv_qualitycuts():
#	plotBase.genericplot('recovert', 'recovert', 'NPV', 
#						fdata, { 'correction': 'NoPU', 'incut':'qualitycuts'}, 
#						fmc, { 'correction': 'NoPU', 'incut':'qualitycuts'}, 
#						factor, settings)

def npv_incuts():
	plotBase.genericplot('recovert', 'recovert', 'NPV', 
						fdata, { 'correction': 'NoPU'}, 
						fmc, { 'correction': 'NoPU'}, 
						factor, settings)

def zeta():
	plotBase.genericplot('z_eta', 'eta', 'eta', 
						fdata, { 'correction': 'NoPU'}, 
						fmc, { 'correction': 'NoPU'}, 
						factor, settings)
def jeteta():
	genericplot('jet1_eta', 'eta', 'jet1',
			fdata, fmc, factor, settings, 'lower center')
	
def zmass():
	plotBase.genericplot('zmass', 'mass', 'Z',
						 fdata, { 'correction': 'NoPU',}, 
						 fmc,{ 'correction': 'NoPU'}, factor, settings)

def calcScaling():
	oname = plotBase.GetNameFromSelection('jet1_phi', {},  {  'correction': ""})
	histo_data = getROOT.SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = getROOT.SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)

	print("Scaling Factor: " + str(histo_data.ysum/histo_mc.ysum))



calcScaling()

# The actual plotting starts here:
#List of plots to do - leave empty for all plots
plots = [  zeta, zmass ]



resp_balance()
resp_mpf()

jet_constituents(1)
jet_constituents(2)

jet_pt(1)
jet_pt(2)

cufineff( "cut_ineff_secondleading_to_zpt_nrv" )
cufineff( "cut_ineff_back_to_back_nrv" )
cufineff( "cut_ineff_zmass_window_nrv" )
cufineff( "cut_ineff_muon_pt_nrv" )
cufineff( "cut_ineff_muon_eta_nrv" )


if len(plots)==0:
	plots = [zeta, jeteta, zpt, jetpt, balance, mpf, npv, zphi,jetphi, jet2pt,zmass, scaleres, wolke,jet2ptall]
for plot in plots:
	print "New plot:",
	plot()

print "%1.2f Done." % time.clock()
