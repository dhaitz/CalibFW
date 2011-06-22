# -*- coding: utf-8 -*-

from getROOT import *
import sys, os, math #, kein pylab!
import time
import numpy as np
import matplotlib
matplotlib.rc('text', usetex = True) 
matplotlib.rc('font', size = 16)
#import matplotlib.pyplot as plt
from plotBase import *

print "%1.2f Start with settings" % time.clock()
### SETTINGS
settings = StandardSettings()
settings.outputformats = ['png', 'pdf', 'svg', 'txt', 'dat']
settings.outputformats = ['png']
settings.lumi = 206.26826
settings.verbosity = 2
factor = settings.lumi*0.000713239*1.16105019587
factor10 = 36*0.001*0.75
mc11color = 'FireBrick'
mc10color = 'MidnightBlue'
data11color = 'black'
data10color = 'gray'

###quantity R(ptZ) pt eta phi
###set algo, cone size, #pv, L1L2L3, 
###data/mc(generator,tune)
###document: serif,LaTeX ; slides: sans serif 
#mpl.rc('text', usetex=True)#does not work on the naf! no latex there
#matplotlib.rc('font', family='serif')
#mpl.pylab.rcParams['legend.loc'] = 'best' rc lines.color linewidth font family weight size

#### INPUTFILES
print "%1.2f Open files:" % time.clock()
fdata, filename_data = OpenFile(GetPath() + "chs_May10_data.root", (settings.verbosity>1))
fmc,   filename_mc   = OpenFile(GetPath() + "chs_Summer11_mc_flat.root", (settings.verbosity>1))
fdata10, filename_data10 = OpenFile(GetPath() + "data2010_v8_single_l3.root", (settings.verbosity>1))
fmc10,   filename_mc10   = OpenFile(GetPath() + "mc_fall10_dy_v1.root", (settings.verbosity>1))
#print "and from MC file (Herwig): " + filename_mch
#print "read histos:", time.clock()
#oname = GetNameFromSelection('z_pt')
#histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
#histname = oname[0].replace('data','mc').replace('Res','')
#histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
#histo_mc.scale(factor)
###Test section
#histo_data.write('out/dat/textout.txt')
#histo_data.read('out/dat/textout.txt')
#print histo_data.x
#histo_data.dump('out/dat/zmass_ak7PFJetsL1L2L3Res_Zplusjet_data.dat')
#histo_data.load('out/dat/zmass_ak7PFJetsL1L2L3Res_Zplusjet_data.dat')
#print histo_data
print "%1.2f Do plots ..." % time.clock()

####PLOTS ! use loops !
####std: ak5PFJetsL1L2L3(Res) allPV > compare data/MCs R pt eta phi 
####0. fig (mit ratio?)
####1. standard captions L (always), sqrt(s) (if data) tick_params
####2. captions (quantity) x,y label (kein titel)
####3. add plots (plot and legend) (different) errorbar contour (colormap) fill (histo) fill_between label='text' line. legend: numpoints ncol title
####   scatter subplots(2,1,True)
####4. draw extra lines and texts (common)
####axh/vline/span arrow, annotate line.set_label('') text(0.5, 0.5,'matplotlib', horizontalalignment='center', verticalalignment='center', transform = ax.transAxes)

####5. save


####compare cone sizes, pvs, etabins, 


def zphi():
	print "eta of the Z boson"
	oname = GetNameFromSelection('z_phi')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)
	fzeta, azeta, plt_name = makeplot('z_phi')
	azeta = AxisLabels(azeta,'phi', 'Z')
	#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

	#histo_data.x.pop()
	histo1 = azeta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	histo2 = azeta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')

	#def fitfunc(x):
	#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))
	#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
	#y = fitfunc(x)

	azeta = captions(azeta,settings)
	azeta.set_ylim(top=histo_data.ymax*1.2)
	azeta = tags(azeta, 'Private work', 'Joram Berger')
	azeta.legend(loc='center right', numpoints=1, frameon=False)

	#plt.minorticks_on()

	Save(fzeta,'z_phi', settings)

	########################PT######################
def zpt():
	print "pT of the Z boson"
	oname = GetNameFromSelection('z_pt')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)
	fig, ax, plt_name = makeplot('z_pt')
	ax = AxisLabels(ax,'pt', 'Z')
	#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

	#histo_data.x.pop()
	histo1 = ax.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='Z pt (data)')
	histo2 = ax.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='Z pt (MC)')

	#def fitfunc(x):
	#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))
	#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
	#y = fitfunc(x)

	ax = captions(ax,settings,False)
	ax.set_ylim(top=histo_mc.ymax*1.2)
	ax = tags(ax, 'Private work', 'Joram Berger')
	ax.legend(loc='center right', numpoints=1, frameon=False)

	#plt.minorticks_on()

	Save(fig,'Z_pt', settings)
	ax.set_ylim(bottom=1.0)
	ax.set_yscale('log')
	Save(fig,'Z_pt_log', settings)

def jetpt():
	print "pT of the leading jet"
	oname = GetNameFromSelection('jet1_pt')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = makeplot('jet1_pt')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
	ajet = captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.2)
	ajet = tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='center right', numpoints=1, frameon=False)
	ajet = AxisLabels(ajet, 'pt', 'jet1')
	ajet.xmax = 400

	#plt.minorticks_on()

	Save(fjet,'jet_pt', settings)

def jet2ptall():
	print "pT of the 2nd jet in all events"
	oname = GetNameFromSelection('jet2_pt',{},{'incut':'allevents'})
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = makeplot('jet2_pt')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
	ajet = captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.2)
	ajet = tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='center right', numpoints=1, frameon=False)
	ajet = AxisLabels(ajet, 'pt', 'jet2')
	ajet.xmax = 400

	#plt.minorticks_on()
#	ajet.set_ylim(bottom=1.0)
#	ajet.set_yscale('log')
	Save(fjet,'jet2_pt_all', settings)


def zphi():
	print "phi of the Z boson"
	oname = GetNameFromSelection('z_phi')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = makeplot('z_phi')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	ajet = captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.4)
	ajet = tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='lower center', numpoints=1, frameon=False)
	ajet = AxisLabels(ajet, 'phi', 'Z')

	Save(fjet,'z_phi', settings)

def jetphi():
	print "phi of the leading jet"
	oname = GetNameFromSelection('jet1_phi')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = makeplot('jet1_phi')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fillstyle='full',fmt='-', capsize=0, label ='MC')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	ajet = captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.4)
	ajet = tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='lower center', numpoints=1, frameon=False)
	ajet = AxisLabels(ajet, 'phi', 'jet1')
	Print(histo_data.ysum/histo_mc.ysum)
	Save(fjet,'jet_phi', settings)
	
#def zphi():
#	print "phi of the Z boson"
#	oname = GetNameFromSelection('z_phi')
#	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
#	histname = oname[0].replace('data','mc').replace('Res','')
#	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
#	histo_mc.scale(factor)

#	fjet, ajet, jetname = makeplot('z_phi')
#	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
#	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
#	ajet = captions(ajet,settings)
#	ajet.set_ylim(top=histo_mc.ymax*1.4)
#	ajet = tags(ajet, 'Private work', 'Joram Berger')
#	ajet.legend(loc='lower center', numpoints=1, frameon=False)
#	ajet = AxisLabels(ajet, 'phi', 'Z')
#	Print(histo_data.ysum/histo_mc.ysum)

#	Save(fjet,'z_phi', settings)
	


def jet2pt():
	genericplot('jet2_pt', 'pt', 'jet2',fdata, fmc, factor, settings)
	
def zeta():
	genericplot('z_eta', 'eta', 'Z',fdata, fmc, factor, settings, 'lower center')

def jeteta():
	genericplot('jet1_eta', 'eta', 'jet1',fdata, fmc, factor, settings, 'lower center')
	
def zmass():
	genericplot('zmass', 'mass', 'Z',fdata, fmc, factor, settings)

def balance():
	print "Response with the balance method"
	oname = GetNameFromSelection('jetrespgraph')
#	histo_data10 = SafeConvert(fdata10,oname[0], settings.lumi,settings.outputformats)
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats)
	histname = mchisto(oname[0])
#	histo_mc10 = SafeConvert(fmc10,histname, settings.lumi,settings.outputformats)
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)

	fresp, aresp, respname = makeplot('jetresp')
#	histo02 = aresp.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color=mc10color,fmt='-', capsize=0, label ='MC Fall10')
	histo03 = aresp.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='Summer11 MC')
#	histo00 = aresp.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
	histo01 = aresp.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data')

	aresp = captions(aresp,settings, False)
	aresp = tags(aresp, 'Private work', 'Joram Berger')
	aresp.legend(loc='lower right', numpoints=1, frameon=False)
	aresp = AxisLabels(aresp, 'jetresp', 'jet')
	#aresp.xmax = 400

	#plt.minorticks_on()

	Save(fresp,'jetresp', settings)

def mpf():
	print "Response with the MPF method"
	oname = GetNameFromSelection('mpfrespgraph')
#	histo_data10 = SafeConvert(fdata10,oname[0], settings.lumi,settings.outputformats)
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats)
	histname = mchisto(oname[0])
#	histo_mc10 = SafeConvert(fmc10,histname, settings.lumi,settings.outputformats)
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)

	fmpf, ampf, mpfname = makeplot('mpfresp')
#	histo02 = ampf.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color=mc10color,fmt='-', capsize=0, label ='MC Fall10')
	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='Summer11 MC')
#	histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
	histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data')

	ampf = captions(ampf,settings, False)
	ampf = tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc='lower right', numpoints=1, frameon=False)
	ampf = AxisLabels(ampf, 'mpfresp', 'jet')
	#ampf.xmax = 400

	#plt.minorticks_on()

	Save(fmpf,'mpfresp', settings)
	
def algocomp():
	print "Comparison of jet algorithms"
	oname = GetNameFromSelection('jetrespgraph',{},{'algo':'ak5'})[0]
	histo_10ak5 = SafeConvert(fdata10,oname, settings.lumi,settings.outputformats)
	histo_11ak5 = SafeConvert(fdata,oname, settings.lumi,settings.outputformats)
	histname = GetNameFromSelection('jetrespgraph',{},{'algo':'ak7'})[0]
	histo_10ak7 = SafeConvert(fdata10,histname, settings.lumi,settings.outputformats)
	histo_11ak7 = SafeConvert(fdata,histname, settings.lumi,settings.outputformats)

	fmpf, ampf, mpfname = makeplot('jetresp')
	histo00 = ampf.errorbar(histo_10ak5.xc, histo_10ak5.y, histo_10ak5.yerr,histo_10ak5.xerr, color=data10color,fmt='^', capsize=0, label ='anti-kt 0.5 (2010)')
	histo01 = ampf.errorbar(histo_11ak5.xc, histo_11ak5.y, histo_11ak5.yerr, histo_11ak5.xerr, color=data11color,fmt='o', capsize=0, label ='anti-kt 0.5 (2011)')
	histo02 = ampf.errorbar(histo_10ak7.xc, histo_10ak7.y, histo_10ak7.yerr, histo_10ak7.xerr, color='red',fmt='v', capsize=0, label ='anti-kt 0.7 (2010)')
	histo03 = ampf.errorbar(histo_11ak7.xc, histo_11ak7.y, histo_11ak7.yerr, histo_11ak7.xerr, color='orange',fmt='o', capsize=0, label ='anti-kt 0.7 (2011)')

	ampf = captions(ampf,settings)
	ampf = tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc='lower right', numpoints=1, frameon=False)
	ampf = AxisLabels(ampf, 'jetresp', 'jet')
	Save(fmpf,'algocomp', settings)

def npv():
	print "Number of primary vertices"
	oname = GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
	print oname
#	histo_data10 = SafeConvert(fdata10,oname[0], settings.lumi,settings.outputformats)
	histo_data = SafeConvert(fdata,oname, settings.lumi,settings.outputformats)
	histname = mchisto(oname)
	print histname
#	histo_mc10 = SafeConvert(fmc10,histname, settings.lumi,settings.outputformats)
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)
	histo_mc.scale(factor)
#	histo_mc10.scale(factor10)
#	histo_data10.dropbin(0)
	histo_data.dropbin(0)
#	histo_mc10.dropbin(0)
	histo_mc.dropbin(0)
#	histo_data10.dropbin(-1)
	histo_data.dropbin(-1)
#	histo_mc10.dropbin(-1)
	histo_mc.dropbin(-1)
	oname.replace('recovert','')

	fmpf, ampf, mpfname = makeplot('recovert')
#	histo02 = ampf.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color=mc10color,fmt='-', capsize=0, label ='MC Fall10')
	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='MC Summer11')
#	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='MC Summer11')
#	histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
	histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data 2011')

#	Print( histo_mc.ymax/histo_data.ymax)
#	Print( histo_mc.y[2]/histo_data.y[2])
#	Print( histo_mc.y[5]/histo_data.y[5])

	ampf = captions(ampf,settings, False)
	ampf = tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc = 'upper right', bbox_to_anchor = (0.98, 0.92), numpoints=1, frameon=False)
	ampf = AxisLabels(ampf, 'recovert')
	#ampf.xmax = 400

	#plt.minorticks_on()

	Save(fmpf,'recovert', settings)
	ampf.set_ylim(bottom=1.0)
	ampf.set_yscale('log')
	Save(fmpf,'recovert_log', settings)


dapv = GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
mcpv = mchisto(dapv)
histo_mcpv = SafeConvert(fmc,mcpv, settings.lumi,settings.outputformats)
histo_dapv = SafeConvert(fdata,dapv, settings.lumi,settings.outputformats)
histo_mcpv.scale(factor)
print len(histo_mcpv)
print len(histo_dapv)
print "Reweighting factors 2011: "
print GetReweighting(histo_dapv,histo_mcpv)

#dapv = GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
#mcpv = mchisto(dapv)
#histo_mcpv = SafeConvert(fmc10,mcpv, settings.lumi,settings.outputformats)
#histo_dapv = SafeConvert(fdata10,dapv, settings.lumi,settings.outputformats)
#histo_mcpv.scale(factor10)
#print "Reweighting factors 2010: "
#print GetReweighting(histo_dapv,histo_mcpv)

def wolke():
	print "Single data points"
	dpoints = npHisto()
	dpoints.read('Datapoints_jetresp_ak5PFJetsL1L2L3.txt')
	oname = GetNameFromSelection('jetrespgraph')
	histname = mchisto(oname[0])
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)

	fmpf, ampf, mpfname = makeplot('jetresp')
	histo02 = ampf.errorbar(dpoints.xc, dpoints.y, color='black',fmt='o', ms = 1.0, capsize=0, label ='single events')
	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='MC Summer11')
#	histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
#	histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data 2011')


	ampf = captions(ampf,settings,False)
	ampf = tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc='lower right', numpoints=1, frameon=False)
	ampf = AxisLabels(ampf, 'jetresp')
	#ampf.xmax = 400
	ampf.set_xlim(20.0,300.0)
	ampf.set_ylim(0.0,2.0)
	#plt.minorticks_on()

	Save(fmpf,'cloud', settings)

def scaleres():
	print "Data/MC comparison plot for scale and resolution"
	para1s, para1r = GetScaleResolution('sr_d2011_423_l3_bal.txt')
	para2s, para2r = GetScaleResolution('sr_d2011_423_l3_mpf.txt')
	Print(para1s)
	Print(para2s)
	fsr, asr, nsr = makeplot('scaleres')
	asr.set_xlabel(r"jet energy scale $s_\mathrm{data/MC}$", ha = "right", x = 1)
	asr.set_xlim(0.95,1.03)
	asr.set_ylabel(r"jet energy resolution $r_\mathrm{data/MC}$", va = "top", y = 1)
	asr.set_ylim(0.95,1.15)

	def ellipsePlot(s=[1,0,0],r=[1,0,0], colour='gray', Label='', shift=0):
		Print(Label)
		# Zeilenabstand in relativen Achsenkoordinaten
		z=0.05
		# jet energy scale (x)
		plt.axvline(s[0], color=colour)
		plt.axvspan(s[0]-s[1], s[0] + s[2], facecolor=colour, edgecolor='none', alpha=0.2)
		# jet energy resolution (y)
		plt.axhline(r[0], color=colour)
		plt.axhspan(r[0]-r[1], r[0] + r[2], facecolor=colour, edgecolor='none', alpha=0.2)
		# point and ellipse
		asr.plot(s[0], r[0], 'o', color = colour, label=Label)  
		el1 = matplotlib.patches.Ellipse((s[0],r[0]), s[1]+s[2], r[1]+r[2], edgecolor=colour, facecolor = 'none')
		el2 = matplotlib.patches.Ellipse((s[0],r[0]), 2.0*(s[1]+s[2]), 2.0*(r[1]+r[2]), edgecolor=colour, facecolor = 'none')
		asr.add_artist(el1)
		asr.add_artist(el2)
		# text
		asr.text(0.02, 0.01+2*z+3*z*shift, Label, va='bottom', ha='left',  transform=asr.transAxes, fontsize=15)
		if True:#round(1000*s[1])==round(1000*s[2]) and round(1000*r[1])==round(1000*r[2]): #gleiche fehler auf 3 stellen
			asr.text(0.02, 0.01+3*z*shift+z, '$s_\mathrm{data/MC} = %1.3f \pm %1.3f$' % (s[0], s[1]),
				va='bottom', ha='left', transform=asr.transAxes, fontsize=15)
			asr.text(0.02, 0.01+3*z*shift, '$r_\mathrm{data/MC} = %1.3f \pm %1.3f$' % (r[0], r[1]),
				va='bottom', ha='left',  transform=asr.transAxes, fontsize=15)
		else:
			asr.text(0.02, 0.01+3*z*shift+z, '$s_\mathrm{data/MC} = %1.3f^{+%1.3f}_{-%1.3f}$' % (s[0], s[1], s[2]),
				va='bottom', ha='left', transform=asr.transAxes, fontsize=15)
			asr.text(0.02, 0.01+3*z*shift, '$r_\mathrm{data/MC} = %1.3f^{+%1.3f}_{-%1.3f}$' % (r[0], r[1], r[2]),
				va='bottom', ha='left',  transform=asr.transAxes, fontsize=15)

	ellipsePlot(para1s, para1r, 'FireBrick','$p_{T}$ balance')
	ellipsePlot(para2s, para2r, mc10color,'MPF', 1)
	#ellipsePlot([0.98, 0.02], [0.98, 0.02], 'red',1) # second dataset 
	asr.legend(loc='lower right', numpoints=1, frameon=False)
#	asr.plot(1.0, 1.0, 'o', color ='black')
#	asr.text(0.02,0.98,'$p_{T}$ balance, L1L2L3, 423',
#				va='top', ha='left', transform=asr.transAxes, fontsize=15)
	asr = captions(asr,settings, False)
#	asr = tags(asr, 'Private work', 'Joram Berger')

	Save(fsr,'scaleres', settings)

# The actual plotting starts here:
#List of plots to do - leave empty for all plots
plots = []

if len(plots)==0:
	plots = [zeta, jeteta, zpt, jetpt, balance, mpf, npv, zphi,jetphi, jet2pt,zmass, scaleres, wolke,jet2ptall]
for plot in plots:
	print "New plot:",
	plot()

print "%1.2f Done." % time.clock()
