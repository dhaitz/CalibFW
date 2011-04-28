# -*- coding: utf-8 -*-

from getROOT import *
import sys, math #, kein pylab!
import numpy as np
import matplotlib
#matplotlib.rc('text', usetex = True) 
import matplotlib.pyplot as plt
from plotBase import *


### SETTINGS
#fileformats = ['png', 'pdf'] auch dat und txt mit anweisung none(nie) used(nur convertete) [all(gehe ganzes Rootfile durch und converte alle)]
lumi = 36
cme = 7
###quantity R(ptZ) pt eta phi
###set algo, cone size, #pv, L1L2L3, 
###data/mc(generator,tune)
###document: serif,LaTeX ; slides: sans serif 
#mpl.rc('text', usetex=True)#does not work on the naf! no latex there
#matplotlib.rc('font', family='serif')
#mpl.pylab.rcParams['legend.loc'] = 'best' rc lines.color linewidth font family weight size


#### INPUTFILES
file_data, filename_data = OpenFile(GetPath() + "data2011_v5.root", True)
file_mc,   filename_mc   = OpenFile(GetPath() + "data2011_v5.root", True)

#print "and from MC file (Herwig): " + filename_mch

oname = GetNameFromSelection('z_pt')
histo_data = SafeConvert(file_data,oname[0])

###Test section
print histo_data.x
#histo_data.write('out/dat/textout.txt')
#histo_data.read('out/dat/textout.txt')
#print histo_data.x
#histo_data.dump('out/dat/zmass_ak7PFJetsL1L2L3Res_Zplusjet_data.dat')
#histo_data.load('out/dat/zmass_ak7PFJetsL1L2L3Res_Zplusjet_data.dat')
print histo_data.x
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






#mcdata = numpy.load('Zmass.npz')

fig = plt.figure()
##fig.set_size_inches(6,4.5)
ax = fig.add_subplot(111, xlim=(0,350))#, autoscale_on=False)
##fig.subplots_adjust(
##	top = 0.82,
##	bottom = 0.20,	# the bottom of the subplots of the figure
##	left = 0.15
##	)
ax = AxisLabels(ax,'pt', 'Z')
#l = plt.axhline(y=91.19, color='0.5', alpha=0.5)

histo_data.x.pop()
masshisto = ax.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, color='green', label ='Z pt')
#	numpy.ma.masked_where(histo_data.y == 0, histo_data.x),
#	numpy.ma.masked_where(histo_data.y == 0, histo_data.y))
#	drawstyle='steps-mid', color='green')


#def fitfunc(x):
#	return  a/(c*c*g*g+(x*x-c*c)*(x*x-c*c))

#x = mcdata['x']#numpy.arange(60.0, 120.0, .06)
#y = fitfunc(x)
#fitcurve = ax.plot(x, y, linewidth=1, color = 'black')


#plt.text(100, 90, r"$m_Z = \,91.01\,\mathrm{GeV}$", horizontalalignment='left', fontsize=14)
#plt.text(100, 92, r"$\Gamma_Z = \,2.89\,\mathrm{GeV}$", horizontalalignment='left', fontsize=14)
ax = captions(ax,lumi,cme)
ax = tags(ax, 'Private work', 'Joram Berger')

leg=ax.legend( loc='center right')
leg.draw_frame(False)
#plt.minorticks_on()

#fig.savefig('Zmass.pdf')
Save(fig,'Zmass')

#ax.set_yscale('log')

#fig.savefig('ZmassLog.pdf')
#fig.savefig('ZmassLog.png')

#diff = ax.errorbar(
#	numpy.ma.masked_where(mcdata['y'] == 0, mcdata['x']),
#	numpy.ma.masked_where(mcdata['y'] == 0, mcdata['y']),
#	drawstyle='steps-mid', color='red')


#fig.savefig('ZmassDiff.pdf')
#fig.savefig('ZmassDiff.png')

