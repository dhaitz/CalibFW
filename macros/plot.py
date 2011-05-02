# -*- coding: utf-8 -*-

from getROOT import *
import sys, os, math #, kein pylab!
import numpy as np
#matplotlib.rc('text', usetex = True) 
import matplotlib.pyplot as plt
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
file_data, filename_data = OpenFile(GetPath() + "data2011_v6.root", (settings.verbosity>1))
file_mc,   filename_mc   = OpenFile(GetPath() + "data2011_v5.root", (settings.verbosity>1))

#print "and from MC file (Herwig): " + filename_mch

oname = GetNameFromSelection('z_pt')
histo_data = SafeConvert(file_data,oname[0], settings.lumi,settings.outputformat)

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

histo_data.x.pop()
masshisto = ax.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black',fmt='o', capsize=0, label ='Z pt')


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

