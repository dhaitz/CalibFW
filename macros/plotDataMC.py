# -*- coding: utf-8 -*-
import getROOT
import numpy as np
import plotBase

def initDataMC(opt, fdata, fmc):
	if opt.normalize:
		opt.factor = plotBase.getFactor(opt.lumi, fdata, fmc)
	else:
		opt.factor = 1.0


# Data MC comparison

def datamcplot(quantity, q, obj, fdata, fmc, opt, legloc='center right', change = {}, log=False, rebin=5):
	if opt.verbose: print q, "of the", obj
	hdata = plotBase.GetDataOrMC(quantity, fdata, change, rebin) # algo = opt.algo,corr
	hmc = plotBase.GetDataOrMC(quantity, fmc, change, rebin)
	if opt.normalize:
		hmc.scale(hdata.ysum/hmc.ysum)
	else:
		hmc.scale(opt.lumi)

	fig, ax = plotBase.newplot()
	ax.bar(hmc.x, hmc.y,(hmc.x[2]-hmc.x[1]),bottom=np.ones(len(hmc.x))*1e-6,
		fill=True, facecolor=opt.mc_color, edgecolor=opt.mc_color)
	ax.errorbar(hmc.xc, hmc.y, hmc.yerr, drawstyle='steps-mid',
		color=opt.mc_color, fmt='-', capsize=0, label=opt.mc_label)
	ax.errorbar(hdata.xc, hdata.y, hdata.yerr, drawstyle='steps-mid',
		color=opt.data_color, fmt='o', capsize=0, label=opt.data_label)
	ax = plotBase.captions(ax, opt)
	ax.set_ylim(top=hmc.ymax * 1.2)
	ax = plotBase.tags(ax, 'Private work', 'Joram Berger')
	ax.legend(loc=legloc, numpoints=1, frameon=False)
	ax = plotBase.AxisLabels(ax, q, obj)

	plotBase.Save(fig, quantity, opt)
	if log:
		ax.set_ylim(bottom=1.0)
		ax.set_yscale('log')
		plotBase.Save(fig, quantity+'_log', opt)


# Z boson
def zpt(fdata, fmc, opt):
	datamcplot('z_pt', 'pt', 'Z', fdata, fmc, opt, 'center right', log=True)

def zeta(fdata, fmc, opt):
	datamcplot('z_eta', 'eta', 'Z', fdata, fmc, opt, 'lower center')

def zphi(fdata, fmc, opt):
	datamcplot('z_phi', 'phi', 'Z', fdata, fmc, opt, 'lower center')
	#ajet.set_ylim(top=histo_mc.ymax*1.4)

def zmass(fdata, fmc, opt):
	datamcplot('zmass', 'mass', 'Z', fdata, fmc, opt)

def zmass_qualitycuts(fdata, fmc, opt):
	datamcplot('zmass', 'mass', 'Z', fdata, fmc, opt, 'center right', {'incut':'qualitycuts'}, log=True)


# Leading jet
def jetpt(fdata, fmc, opt):
	datamcplot('jet1_pt', 'pt', 'jet', fdata, fmc, opt, 'center right', log=True)
	#ajet.xmax = 400

def jeteta(fdata, fmc, opt):
	datamcplot('jet1_eta', 'eta', 'jet', fdata, fmc, opt, 'lower center')

def jetphi(fdata, fmc, opt):
	datamcplot('jet1_phi', 'phi', 'jet', fdata, fmc, opt, 'lower center')
	#print "    >>> ", histo_data.ysum/histo_mc.ysum


# Second leading jet
def jet2pt(fdata, fmc, opt):
	datamcplot('jet2_pt', 'pt', 'jet2', fdata, fmc, opt)

def jet2pt_qualitycuts(fdata, fmc, opt):
	datamcplot('jet2_pt', 'pt', 'jet2', fdata, fmc, opt, 'center right', {'incut':'qualitycuts'}, log=True)
	#ajet.xmax = 400

def jet2eta(fdata, fmc, opt):
	datamcplot('jet2_eta', 'eta', 'jet2', fdata, fmc, opt, 'lower center')

def jet2phi(fdata, fmc, opt):
	datamcplot('jet2_phi', 'phi', 'jet2', fdata, fmc, opt, 'lower center')


# Response plots

def responseplot(method, fdata, fmc, opt, legloc='lower right', change = {}):
	if opt.verbose: print "Response:", method
	hdata = plotBase.GetDataOrMC(method, fdata, change, opt)
	hmc = plotBase.GetDataOrMC(method, fmc, change, opt)

	fig, ax = plotBase.newplot()
	ax.errorbar(hmc.x, hmc.y, hmc.yerr,
		color=opt.mc_color, fmt='-', capsize=0, label=opt.mc_label)
	ax.errorbar(hdata.xc, hdata.y, hdata.yerr,
		color=opt.data_color, fmt='o', capsize=0, label=opt.data_label)

	ax = plotBase.captions(ax, opt)
	ax.set_ylim(top=hmc.ymax * 1.2)
	ax.legend(loc=legloc, numpoints=1, frameon=False)
	ax = plotBase.AxisLabels(ax, method[0:7], "Z")
	plotBase.Save(fig, method, opt)


def balance(fdata, fmc, opt):
	responseplot('jetrespgraph', fdata, fmc, opt)

def mpf(fdata, fmc, opt):
	responseplot('mpfrespgraph', fdata, fmc, opt)


plots = ['zpt', 'zeta', 'zphi', 'zmass', 'zmass_qualitycuts',
	'jetpt', 'jeteta', 'jetphi', 'jet2pt', 'jet2pt_qualitycuts', 'jet2eta', 'jet2phi',
	'balance', 'mpf' ]

if __name__ == "__main__":
	fdata = getROOT.openFile(plotBase.GetPath() + "data_Oct19.root")
	fmc = getROOT.openFile(plotBase.GetPath() + "pythia_Oct19.root")
	bins = plotBase.guessBins(fdata, [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]) #binning must be after file open. plots do this later: if bins[0] == 0 bins.pop(0)
	balance(fdata, fmc, opt=plotBase.commandlineOptions(bins=bins))
	mpf(fdata, fmc, opt=plotBase.commandlineOptions(bins=bins))
