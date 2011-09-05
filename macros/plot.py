# -*- coding: utf-8 -*-
from getROOT import *
import sys, os, math #, kein pylab!
import time
import numpy as np
import matplotlib
matplotlib.rc('text', usetex = False) 
matplotlib.rc('font', size = 16)
#matplotlib.rc('axes', linewidth = 20)
matplotlib.rcParams.update({
	'axes.linewidth': 0.8 # thickness of main box lines 
#	'patch.linewidth': 1.5, # thickness of legend pictures and border
#	'grid.linewidth': 1.3, # thickness of grid lines
#	'lines.linewidth': 2.5, # thickness of error bars
#	'lines.markersize': 2.5, # size of markers
#	'legend.fontsize': 20,
#	'xtick.labelsize': 18,
#	'ytick.labelsize': 18,
#	'text.fontsize': 16,
#	'font.size': 18
})
#import matplotlib.pyplot as plt
import plotBase

print "%1.2f Start with settings" % time.clock()
### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png', 'pdf', 'svg', 'txt', 'dat']
settings.outputformats = ['png','pdf']
settings.lumi = 1079.0
settings.verbosity = 2
factor = settings.lumi # is being overwritten
factor10 = 36*0.001*0.75
mc11color = '#CBDBF9'#FireBrick'
edgeColor = '#0033CC'
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
fdata = OpenFile(plotBase.GetPath() + "data_July22.root", (settings.verbosity>1))
fmc   = OpenFile(plotBase.GetPath() + "MC_July22.root", (settings.verbosity>1))
#fmcflat = OpenFile(plotBase.GetPath() + "chs_Summer11_mc_withoutrw.root", (settings.verbosity>1))
#fdata10 = OpenFile(plotBase.GetPath() + "data2010_v8_single_l3.root", (settings.verbosity>1))
#fmc10   = OpenFile(plotBase.GetPath() + "mc_fall10_dy_v1.root", (settings.verbosity>1))
print "%1.2f Do plots ..." % time.clock()

####PLOTS ! use loops !
####std: ak5PFJetsL1L2L3(Res) allPV > compare data/MCs R pt eta phi 
####0. fig (mit ratio?)
####1. standard plotBase.captions L (always), sqrt(s) (if data) tick_params
####2. plotBase.captions (quantity) x,y label (kein titel)
####3. add plots (plot and legend) (different) errorbar contour (colormap) fill (histo) fill_between label='text' line. legend: numpoints ncol title
####   scatter subplots(2,1,True)
####4. draw extra lines and texts (common)
####axh/vline/span arrow, annotate line.set_label('') text(0.5, 0.5,'matplotlib', horizontalalignment='center', verticalalignment='center', transform = ax.transAxes)

####5. save

# function to be moved to plotBase:
def get_factor(quantity='z_phi'):
	hdata = plotBase.GetNameFromSelection(quantity)[0]
	hmc = hdata.replace('Res','')
	histo_data = SafeConvert(fdata,hdata, settings.lumi,settings.outputformats,5)
	histo_mc = SafeConvert(fmc,hmc, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)
	print "    >>> The additional scaling factor is:", histo_data.ysum/histo_mc.ysum
	return histo_data.ysum/histo_mc.ysum

factor *= get_factor()

####compare cone sizes, pvs, etabins, 


def zphia():
	print "eta of the Z boson"
	oname = plotBase.GetNameFromSelection('z_phi')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)
	fzeta, azeta, plt_name = plotBase.makeplot('z_phi')
	azeta = plotBase.AxisLabels(azeta,'phi', 'Z')
	#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

	#histo_data.x.pop()
#	histo0 = azeta.fill_between(histo_mc.x,histo_mc.y, facecolor=settings.mcColor, label ='')
	histo0 = azeta.bar(histo_mc.x, histo_mc.y,(histo_mc.x[2]-histo_mc.x[1]),fill=True, facecolor=settings.mcColor, edgecolor=settings.mcColor)
	histo2 = azeta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=edgeColor, fmt='-', capsize=0, label ='MC')
	histo1 = azeta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	#def fitfunc(x):
	#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))
	#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
	#y = fitfunc(x)

	azeta = plotBase.captions(azeta,settings)
	azeta.set_ylim(top=histo_data.ymax*1.2)
	azeta = plotBase.tags(azeta, 'Private work', 'Joram Berger')
	azeta.legend(loc='center right', numpoints=1, frameon=False)

	#plt.minorticks_on()

	plotBase.Save(fzeta,'z_phi1', settings, False)

	########################PT######################
def zpt():
	print "pT of the Z boson"
	oname = plotBase.GetNameFromSelection('z_pt')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)
	fig, ax, plt_name = plotBase.makeplot('z_pt')
	ax = plotBase.AxisLabels(ax,'pt', 'Z')
	#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

	#histo_data.x.pop()
	histo1 = ax.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='Z pt (data)')
	histo2 = ax.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='Z pt (MC)')

	#def fitfunc(x):
	#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))
	#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
	#y = fitfunc(x)

	ax = plotBase.captions(ax,settings,False)
	ax.set_ylim(top=histo_mc.ymax*1.2)
	ax = plotBase.tags(ax, 'Private work', 'Joram Berger')
	ax.legend(loc='center right', numpoints=1, frameon=False)

	#plt.minorticks_on()

	plotBase.Save(fig,'Z_pt', settings)
	ax.set_ylim(bottom=1.0)
	ax.set_yscale('log')
	plotBase.Save(fig,'Z_pt_log', settings)

def jetpt():
	print "pT of the leading jet"
	oname = plotBase.GetNameFromSelection('jet1_pt')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = plotBase.makeplot('jet1_pt')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
	ajet = plotBase.captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.2)
	ajet = plotBase.tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='center right', numpoints=1, frameon=False)
	ajet = plotBase.AxisLabels(ajet, 'pt', 'jet1')
	ajet.xmax = 400

	#plt.minorticks_on()

	plotBase.Save(fjet,'jet_pt', settings)

def jet2ptall():
	print "pT of the 2nd jet in all events"
	oname = plotBase.GetNameFromSelection('jet2_pt',{},{'incut':'qualitycuts'})
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = plotBase.makeplot('jet2_pt')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
	ajet = plotBase.captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.2)
	ajet = plotBase.tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='center right', numpoints=1, frameon=False)
	ajet = plotBase.AxisLabels(ajet, 'pt', 'jet2')
	ajet.xmax = 400

	#plt.minorticks_on()
#	ajet.set_ylim(bottom=1.0)
#	ajet.set_yscale('log')
	plotBase.Save(fjet,'jet2_pt_quality', settings)


def zphi():
	print "phi of the Z boson"
	oname = plotBase.GetNameFromSelection('z_phi')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = plotBase.makeplot('z_phi')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	ajet = plotBase.captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.4)
	ajet = plotBase.tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='lower center', numpoints=1, frameon=False)
	ajet = plotBase.AxisLabels(ajet, 'phi', 'Z')

	plotBase.Save(fjet,'z_phi', settings)

def jetphi():
	print "phi of the leading jet"
	oname = plotBase.GetNameFromSelection('jet1_phi')
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
	histname = oname[0].replace('data','mc').replace('Res','')
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
	histo_mc.scale(factor)

	fjet, ajet, jetname = plotBase.makeplot('jet1_phi')
	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fillstyle='full',fmt='-', capsize=0, label ='MC')
	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
	ajet = plotBase.captions(ajet,settings,False)
	ajet.set_ylim(top=histo_mc.ymax*1.4)
	ajet = plotBase.tags(ajet, 'Private work', 'Joram Berger')
	ajet.legend(loc='lower center', numpoints=1, frameon=False)
	ajet = plotBase.AxisLabels(ajet, 'phi', 'jet1')
	print "    >>> ", histo_data.ysum/histo_mc.ysum
	plotBase.Save(fjet,'jet_phi', settings)
	
#def zphi():
#	print "phi of the Z boson"
#	oname = plotBase.GetNameFromSelection('z_phi')
#	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats,5)
#	histname = oname[0].replace('data','mc').replace('Res','')
#	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats,5)
#	histo_mc.scale(factor)

#	fjet, ajet, jetname = plotBase.makeplot('z_phi')
#	histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color=mc11color,fmt='-', capsize=0, label ='MC')
#	histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color=data11color,fmt='o', capsize=0, label ='data')
#	ajet = plotBase.captions(ajet,settings)
#	ajet.set_ylim(top=histo_mc.ymax*1.4)
#	ajet = plotBase.tags(ajet, 'Private work', 'Joram Berger')
#	ajet.legend(loc='lower center', numpoints=1, frameon=False)
#	ajet = plotBase.AxisLabels(ajet, 'phi', 'Z')
#	Print(histo_data.ysum/histo_mc.ysum)

#	plotBase.Save(fjet,'z_phi', settings)
	


def jet2pt():
	plotBase.genericplot('jet2_pt', 'pt', 'jet2', fdata, {}, fmc, {}, factor, settings)
	
def zeta():
	plotBase.genericplot('z_eta', 'eta', 'Z', fdata, {}, fmc, {}, factor, settings, 'lower center')

def jeteta():
	plotBase.genericplot('jet1_eta', 'eta', 'jet1',fdata, {}, fmc, {}, factor, settings, 'lower center')
	
def zmass():
	plotBase.genericplot('zmass', 'mass', 'Z',fdata, {}, fmc, {}, factor, settings)

def balance():
	print "Response with the balance method"
	oname = plotBase.GetNameFromSelection('jetrespgraph')
#	histo_data10 = SafeConvert(fdata10,oname[0], settings.lumi,settings.outputformats)
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats)
	histname = oname[0]
#	histo_mc10 = SafeConvert(fmc10,histname, settings.lumi,settings.outputformats)
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)

	fresp, aresp, respname = plotBase.makeplot('jetresp')
#	histo02 = aresp.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color=mc10color,fmt='-', capsize=0, label ='MC Fall10')
	histo03 = aresp.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='Summer11 MC')
#	histo00 = aresp.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
	histo01 = aresp.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data')

	aresp = plotBase.captions(aresp,settings, False)
	aresp = plotBase.tags(aresp, 'Private work', 'Joram Berger')
	aresp.legend(loc='lower right', numpoints=1, frameon=False)
	aresp = plotBase.AxisLabels(aresp, 'jetresp', 'jet')
	#aresp.xmax = 400

	#plt.minorticks_on()

	plotBase.Save(fresp,'jetresp', settings)

def mpf():
	print "Response with the MPF method"
	oname = plotBase.GetNameFromSelection('mpfrespgraph')
#	histo_data10 = SafeConvert(fdata10,oname[0], settings.lumi,settings.outputformats)
	histo_data = SafeConvert(fdata,oname[0], settings.lumi,settings.outputformats)
	histname = oname[0]
#	histo_mc10 = SafeConvert(fmc10,histname, settings.lumi,settings.outputformats)
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)

	fmpf, ampf, mpfname = plotBase.makeplot('mpfresp')
#	histo02 = ampf.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color=mc10color,fmt='-', capsize=0, label ='MC Fall10')
	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='Summer11 MC')
#	histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
	histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data')

	ampf = plotBase.captions(ampf,settings, False)
	ampf = plotBase.tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc='lower right', numpoints=1, frameon=False)
	ampf = plotBase.AxisLabels(ampf, 'mpfresp', 'jet')
	#ampf.xmax = 400

	#plt.minorticks_on()

	plotBase.Save(fmpf,'mpfresp', settings)
	
def algocomp():
	print "Comparison of jet algorithms"
	oname = plotBase.GetNameFromSelection('jetrespgraph',{},{'algo':'ak5'})[0]
	histo_10ak5 = SafeConvert(fdata10,oname, settings.lumi,settings.outputformats)
	histo_11ak5 = SafeConvert(fdata,oname, settings.lumi,settings.outputformats)
	histname = plotBase.GetNameFromSelection('jetrespgraph',{},{'algo':'ak7'})[0]
	histo_10ak7 = SafeConvert(fdata10,histname, settings.lumi,settings.outputformats)
	histo_11ak7 = SafeConvert(fdata,histname, settings.lumi,settings.outputformats)

	fmpf, ampf, mpfname = plotBase.makeplot('jetresp')
	histo00 = ampf.errorbar(histo_10ak5.xc, histo_10ak5.y, histo_10ak5.yerr,histo_10ak5.xerr, color=data10color,fmt='^', capsize=0, label ='anti-kt 0.5 (2010)')
	histo01 = ampf.errorbar(histo_11ak5.xc, histo_11ak5.y, histo_11ak5.yerr, histo_11ak5.xerr, color=data11color,fmt='o', capsize=0, label ='anti-kt 0.5 (2011)')
	histo02 = ampf.errorbar(histo_10ak7.xc, histo_10ak7.y, histo_10ak7.yerr, histo_10ak7.xerr, color='red',fmt='v', capsize=0, label ='anti-kt 0.7 (2010)')
	histo03 = ampf.errorbar(histo_11ak7.xc, histo_11ak7.y, histo_11ak7.yerr, histo_11ak7.xerr, color='orange',fmt='o', capsize=0, label ='anti-kt 0.7 (2011)')

	ampf = plotBase.captions(ampf,settings)
	ampf = plotBase.tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc='lower right', numpoints=1, frameon=False)
	ampf = plotBase.AxisLabels(ampf, 'jetresp', 'jet')
	plotBase.Save(fmpf,'algocomp', settings)

def npv():
	print "Number of primary vertices"
	oname = plotBase.GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
	print oname
#	histo_data10 = SafeConvert(fdata10,oname[0], settings.lumi,settings.outputformats)
	histo_data = SafeConvert(fdata,oname, settings.lumi,settings.outputformats)
	histname = oname
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

	fmpf, ampf, mpfname = plotBase.makeplot('recovert')
#	histo02 = ampf.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color=mc10color,fmt='-', capsize=0, label ='MC Fall10')
	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='MC Summer11')
#	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='MC Summer11')
#	histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
	histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data 2011')

#	Print( histo_mc.ymax/histo_data.ymax)
#	Print( histo_mc.y[2]/histo_data.y[2])
#	Print( histo_mc.y[5]/histo_data.y[5])

	ampf = plotBase.captions(ampf,settings, False)
	ampf = plotBase.tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc = 'upper right', bbox_to_anchor = (0.98, 0.92), numpoints=1, frameon=False)
	ampf = plotBase.AxisLabels(ampf, 'recovert')
	#ampf.xmax = 400

	#plt.minorticks_on()

	plotBase.Save(fmpf,'recovert', settings)
	ampf.set_ylim(bottom=1.0)
	ampf.set_yscale('log')
	plotBase.Save(fmpf,'recovert_log', settings)


dapv = plotBase.GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
mcpv = dapv
histo_mcpv = SafeConvert(fmc,mcpv, settings.lumi,settings.outputformats)
histo_dapv = SafeConvert(fdata,dapv, settings.lumi,settings.outputformats)
histo_mcpv.scale(factor)
print "Reweighting factors 2011: "
print plotBase.GetReweighting(histo_dapv,histo_mcpv)

#dapv = plotBase.GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
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
	oname = plotBase.GetNameFromSelection('jetrespgraph')
	histname = oname[0]
	histo_mc = SafeConvert(fmc,histname, settings.lumi,settings.outputformats)

	fmpf, ampf, mpfname = plotBase.makeplot('jetresp')
	histo02 = ampf.errorbar(dpoints.xc, dpoints.y, color='black',fmt='o', ms = 1.0, capsize=0, label ='single events')
	histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color=mc11color,fmt='-', capsize=0, label ='MC Summer11')
#	histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color=data10color,fmt='^', capsize=0, label ='data 2010')
#	histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color=data11color,fmt='o', capsize=0, label ='data 2011')


	ampf = plotBase.captions(ampf,settings,False)
	ampf = plotBase.tags(ampf, 'Private work', 'Joram Berger')
	ampf.legend(loc='lower right', numpoints=1, frameon=False)
	ampf = plotBase.AxisLabels(ampf, 'jetresp')
	#ampf.xmax = 400
	ampf.set_xlim(20.0,300.0)
	ampf.set_ylim(0.0,2.0)
	#plt.minorticks_on()

	plotBase.Save(fmpf,'cloud', settings)

def scaleres():
	print "Data/MC comparison plot for scale and resolution"
	para1s, para1r = GetScaleResolution('sr_d2011_423_l3_bal.txt')
	para2s, para2r = GetScaleResolution('sr_d2011_423_l3_mpf.txt')
	print para1s
	print para2s
	fsr, asr, nsr = plotBase.makeplot('scaleres')
	asr.set_xlabel(r"jet energy scale $s_\mathrm{data/MC}$", ha = "right", x = 1)
	asr.set_xlim(0.95,1.03)
	asr.set_ylabel(r"jet energy resolution $r_\mathrm{data/MC}$", va = "top", y = 1)
	asr.set_ylim(0.95,1.15)

	def ellipsePlot(s=[1,0,0],r=[1,0,0], colour='gray', Label='', shift=0):
		print Label
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
	asr = plotBase.captions(asr,settings, False)
#	asr = plotBase.tags(asr, 'Private work', 'Joram Berger')

	plotBase.Save(fsr,'scaleres', settings)


def npu():
	print "Number of pile-up interactions"
	fpuApr = OpenFile("../s/data/pudist423Apr.root", (settings.verbosity>1))
	fpuMay = OpenFile("../s/data/pudist423May.root", (settings.verbosity>1))
	fpuJune = OpenFile("../s/data/pudist423June24Prompt.root", (settings.verbosity>1))
	oname = plotBase.GetNameFromSelection('pu',{},{'incut':'allevents'})[0]
	hpuApr = SafeConvert(fpuApr, 'pileup', settings.lumi,settings.outputformats).normalize()
	hpuMay = SafeConvert(fpuMay, 'pileup', settings.lumi,settings.outputformats).normalize()
	hpuJune = SafeConvert(fpuJune, 'pileup', settings.lumi,settings.outputformats).normalize()

#	histo_mc10.scale(factor10)
#	histo_data10.dropbin(0)

	fnpu, anpu, npuname = plotBase.makeplot('recovert')
	histo01 = anpu.errorbar(hpuApr.xc, hpuApr.y, hpuApr.yerr, color='brown', fmt='o', capsize=0, label ='2010 (Apr21, 36 pb${}^{-1}$)')
	histo02 = anpu.errorbar(hpuMay.xc, hpuMay.y, hpuMay.yerr, color='black', fmt='^', capsize=0, label ='2011 (May10, 206 pb${}^{-1}$)')
	histo03 = anpu.errorbar(hpuJune.xc, hpuJune.y, hpuJune.yerr, color='blue', fmt='x', capsize=0, label ='2011 (Prompt, 800 pb${}^{-1}$)')

	anpu = plotBase.captions(anpu,settings, False)
	anpu = plotBase.tags(anpu, 'Private work', 'Joram Berger')
	anpu.legend(loc = 'upper right', bbox_to_anchor = (0.98, 0.92), numpoints=1, frameon=False)
	anpu = plotBase.AxisLabels(anpu, 'recovert')
	#anpu.xmax = 400
	anpu.set_xlabel('Number of pile-up events')
	anpu.set_ylabel('fraction of events')
	#plt.minorticks_on()

	plotBase.Save(fnpu,'npu', settings,False)
	anpu.set_ylim(bottom=1.0)
	anpu.set_yscale('log')
	plotBase.Save(fnpu,'npu_log', settings, False)

def npumcflat():
	npumc('flat')

def npumcstd():
	npumc('')

def npureco():
	npumc('reco')

def npumc(typ=''):
	print "Number of pile-up interactions"
	oname = plotBase.GetNameFromSelection('pu',{},{'incut':'allevents', 'type': 'mc'})[0]
	if typ=='flat':
		datei=fmcflat
	else:
		datei=fmc
	rname = plotBase.GetNameFromSelection('recovert',{},{'incut':'allevents'})[0]
	hrv = SafeConvert(fdata, rname, settings.lumi,settings.outputformats).normalize()
	hrvmc = SafeConvert(fmc, rname, settings.lumi,settings.outputformats).normalize()
	hpuflat = SafeConvert(datei, oname, settings.lumi,settings.outputformats).normalize()
	hpuflatb = SafeConvert(datei, oname.replace('pu','pu_before'), settings.lumi,settings.outputformats).normalize()
	hpuflata = SafeConvert(datei, oname.replace('pu','pu_after'), settings.lumi,settings.outputformats).normalize()
	fpuApr = OpenFile("../s/data/pudist423Apr.root", (settings.verbosity>1))
	fpuMay = OpenFile("../s/data/pudist423May.root", (settings.verbosity>1))
	fpuJune = OpenFile("../s/data/pudist423June24Prompt.root", (settings.verbosity>1))
	oname = plotBase.GetNameFromSelection('pu',{},{'incut':'allevents'})[0]
	hpuApr = SafeConvert(fpuApr, 'pileup', settings.lumi,settings.outputformats).normalize()
	hpuMay = SafeConvert(fpuMay, 'pileup', settings.lumi,settings.outputformats).normalize()
	hpuJune = SafeConvert(fpuJune, 'pileup', settings.lumi,settings.outputformats).normalize()
#	histo_mc10.scale(factor10)
#	histo_data10.dropbin(0)

	fnpu, anpu, npuname = plotBase.makeplot('recovert')
#	histo01 = anpu.errorbar(hpurw.xc, hpurw.y, hpurw.yerr, color='brown', fmt='o', capsize=0, label ='reweighted')

	histo02 = anpu.errorbar(hpuflat.xc, hpuflat.y, hpuflat.yerr, color='FireBrick', fmt='--', capsize=0, label =r'$n_\mathrm{PU}$ MC (mixed)')
	histo06 = anpu.errorbar(hpuMay.xc, hpuMay.y, hpuMay.yerr, color='black', fmt='^', capsize=0, label =r'$n_\mathrm{PU}$ 2011 data (estimated)')
	if typ=='flat' or typ =='':
		histo03 = anpu.errorbar(hpuflatb.xc, hpuflatb.y, hpuflatb.yerr, color='blue', fmt='.', capsize=0, label =r'$n_\mathrm{PU,\, previous\, BX}$')
		histo04 = anpu.errorbar(hpuflata.xc, hpuflata.y, hpuflata.yerr, color='green', fmt='.', capsize=0, label =r'$n_\mathrm{PU,\, next\, BX}$')
	else:
		histo08 = anpu.errorbar(hrvmc.xc, hrvmc.y, hrvmc.yerr, color='orange', fmt='-', capsize=0, label =r'$n_\mathrm{reco. vertices}$ MC')
		histo09 = anpu.errorbar(hrv.xc, hrv.y, hrv.yerr, color='green', fmt='s', capsize=0, label =r'$n_\mathrm{reco. vertices}$ 2011 data')
	if typ=='flat':
		histo05 = anpu.errorbar(range(25),	
		[0.0698146584,0.0698146584,0.0698146584,0.0698146584,0.0698146584,0.0698146584,
		0.0698146584,0.0698146584,0.0698146584,0.0698146584,0.0698146584,0.0630151648,
		0.0526654164,0.0402754482,0.0292988928,0.0194384503,0.0122016783,0.007207042,
		0.004003637,0.0020278322,0.0010739954,0.0004595759,0.0002229748,0.0001028162,4.58337152809607E-05],
		color = 'FireBrick', fmt='-', label='MC "flat10"')
#	else:




	anpu = plotBase.captions(anpu,settings, False)
	anpu = plotBase.tags(anpu, 'Private work', 'Joram Berger')
	anpu.legend(loc = 'upper right', bbox_to_anchor = (0.98, 0.92), numpoints=1, frameon=False)
	anpu = plotBase.AxisLabels(anpu, 'recovert')
	anpu.set_ylim(0,0.2)
	#anpu.xmax = 400
	anpu.set_xlabel('Number of pile-up vertices')
	anpu.set_ylabel('Fraction of events')
	#plt.minorticks_on()
	plotBase.Save(fnpu,'npumc'+typ, settings,False)

# The actual plotting starts here:
#List of plots to do - leave empty for all plots
plots = []

if len(plots)==0:
	plots = [zeta, jeteta, zpt, jetpt, balance, mpf, npv, zphi,jetphi, jet2pt,zmass, jet2ptall]
for plot in plots:
	print "New plot:",
	plot()

print "%1.2f Done." % time.clock()
