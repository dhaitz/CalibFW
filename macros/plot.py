# -*- coding: utf-8 -*-

from getROOT import *
import sys, os, math #, kein pylab!
import numpy as np
import matplotlib
matplotlib.rc('text', usetex = True) 
#import matplotlib.pyplot as plt
from plotBase import *

### SETTINGS
settings = StandardSettings()
settings.outputformat = ['png', 'txt', 'dat']
settings.lumi = 36
settings.verbosity = 2

###quantity R(ptZ) pt eta phi
###set algo, cone size, #pv, L1L2L3, 
###data/mc(generator,tune)
###document: serif,LaTeX ; slides: sans serif 
#mpl.rc('text', usetex=True)#does not work on the naf! no latex there
#matplotlib.rc('font', family='serif')
#mpl.pylab.rcParams['legend.loc'] = 'best' rc lines.color linewidth font family weight size

#### INPUTFILES
file_data, filename_data = OpenFile(GetPath() + "data2011_v7_double.root", (settings.verbosity>1))
file_mc,   filename_mc   = OpenFile(GetPath() + "mc_spring11_dy_v1.root", (settings.verbosity>1))
file_data10, filename_data10 = OpenFile(GetPath() + "data2011_v6.root", (settings.verbosity>1))
file_mc10,   filename_mc10   = OpenFile(GetPath() + "mc_fall10_dy_v1.root", (settings.verbosity>1))
#print "and from MC file (Herwig): " + filename_mch

oname = GetNameFromSelection('z_pt')
histo_data = SafeConvert(file_data,oname[0], settings.lumi,settings.outputformat)
histname = oname[0].replace('data','mc').replace('Res','')
histo_mc = SafeConvert(file_mc,histname, settings.lumi,settings.outputformat)
histo_mc.scale(0.005)
###Test section
#histo_data.write('out/dat/textout.txt')
#histo_data.read('out/dat/textout.txt')
#print histo_data.x
#histo_data.dump('out/dat/zmass_ak7PFJetsL1L2L3Res_Zplusjet_data.dat')
#histo_data.load('out/dat/zmass_ak7PFJetsL1L2L3Res_Zplusjet_data.dat')
#print histo_data

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


fig, ax, plt_name = makeplot('z_pt')
ax = AxisLabels(ax,'pt', 'Z')
#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

#histo_data.x.pop()
histo1 = ax.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black',fmt='o', capsize=0, label ='Z pt (data)')
histo2 = ax.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color='black',fmt='-', capsize=0, label ='Z pt (MC)')

#def fitfunc(x):
#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))
#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
#y = fitfunc(x)

ax = captions(ax,settings)
ax = tags(ax, 'Private work', 'Joram Berger')
ax.legend(loc='center right', numpoints=1, frameon=False)

#plt.minorticks_on()

Save(fig,'Z_pt', settings)

ax.set_yscale('log')
Save(fig,'Z_pt_log', settings)

oname = GetNameFromSelection('jet1_pt')
histo_data = SafeConvert(file_data,oname[0], settings.lumi,settings.outputformat)
histname = oname[0].replace('data','mc').replace('Res','')
histo_mc = SafeConvert(file_mc,histname, settings.lumi,settings.outputformat)
histo_mc.scale(0.005)

fjet, ajet, jetname = makeplot('jet1_pt')
histo01 = ajet.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black',fmt='o', capsize=0, label ='data')
histo02 = ajet.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color='red',fmt='-', capsize=0, label ='MC')
ajet = captions(ajet,settings)
ajet = tags(ajet, 'Private work', 'Joram Berger')
ajet.legend(loc='center right', numpoints=1, frameon=False)
ajet = AxisLabels(ajet, 'pt', 'jet1')
ajet.xmax = 400

#plt.minorticks_on()

Save(fjet,'jet_pt', settings)


oname = GetNameFromSelection('jetrespgraph')
histo_data10 = SafeConvert(file_data10,oname[0], settings.lumi,settings.outputformat)
histo_data = SafeConvert(file_data,oname[0], settings.lumi,settings.outputformat)
histname = mchisto(oname[0])
histo_mc10 = SafeConvert(file_mc10,histname, settings.lumi,settings.outputformat)
histo_mc = SafeConvert(file_mc,histname, settings.lumi,settings.outputformat)

fresp, aresp, respname = makeplot('jetresp')
histo00 = aresp.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color='gray',fmt='^', capsize=0, label ='data 2010')
histo01 = aresp.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color='black',fmt='o', capsize=0, label ='data 2011')
histo02 = aresp.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color='blue',fmt='-', capsize=0, label ='MC Fall10')
histo03 = aresp.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color='red',fmt='-', capsize=0, label ='MC Spring11')

aresp = captions(aresp,settings)
aresp = tags(aresp, 'Private work', 'Joram Berger')
aresp.legend(loc='center right', numpoints=1, frameon=False)
aresp = AxisLabels(aresp, 'jetresp', 'jet')
#aresp.xmax = 400

#plt.minorticks_on()

Save(fresp,'jetresp', settings)

oname = GetNameFromSelection('mpfrespgraph')
histo_data10 = SafeConvert(file_data10,oname[0], settings.lumi,settings.outputformat)
histo_data = SafeConvert(file_data,oname[0], settings.lumi,settings.outputformat)
histname = mchisto(oname[0])
histo_mc10 = SafeConvert(file_mc10,histname, settings.lumi,settings.outputformat)
histo_mc = SafeConvert(file_mc,histname, settings.lumi,settings.outputformat)

fmpf, ampf, mpfname = makeplot('mpfresp')
histo00 = ampf.errorbar(histo_data10.xc, histo_data10.y, histo_data10.yerr, color='gray',fmt='^', capsize=0, label ='data 2010')
histo01 = ampf.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color='black',fmt='o', capsize=0, label ='data 2011')
histo02 = ampf.errorbar(histo_mc10.xc, histo_mc10.y, histo_mc10.yerr, color='blue',fmt='-', capsize=0, label ='MC Fall10')
histo03 = ampf.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, color='red',fmt='-', capsize=0, label ='MC Spring11')

ampf = captions(ampf,settings)
ampf = tags(ampf, 'Private work', 'Joram Berger')
ampf.legend(loc='center right', numpoints=1, frameon=False)
ampf = AxisLabels(ampf, 'mpfresp', 'jet')
#ampf.xmax = 400

#plt.minorticks_on()

Save(fmpf,'mpfresp', settings)


