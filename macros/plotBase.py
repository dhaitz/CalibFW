# -*- coding: utf-8 -*-

import socket
import getpass
import os
import sys
import numpy as np
import matplotlib
from matplotlib.pyplot import figure as plt_figure
from time import localtime, strftime, clock
import getROOT
import argparse

matplotlib.rcParams.update({
	'text.usetex': False,
	'axes.linewidth': 0.8, # thickness of main box lines
#	'patch.linewidth': 1.5, # thickness of legend pictures and border
#	'grid.linewidth': 1.3, # thickness of grid lines
#	'lines.linewidth': 2.5, # thickness of error bars
#	'lines.markersize': 2.5, # size of markers
	'legend.fontsize': 20,
	'xtick.labelsize': 18,
	'ytick.labelsize': 18,
	'text.fontsize': 16,
	'font.size': 18,
})

# The actual plotting starts here:
#List of plots to do - leave empty for all plots

def guessBins(rootfile, fallbackBins):
    # assumes directories starting with Pt<low>to<high>_
    result = []
    try:
        for key in rootfile.GetListOfKeys():
            name = key.GetName()
            if name.find("Pt")==0 and name.find("to")>0:
                low  = int(name[2:name.find("to")])
                high = int(name[name.find("to")+2:name.find("_")])
                if low  not in result: result.append(low)
                if high not in result: result.append(high)
        assert result != []
    except:
        print result
        print "Bins could not be determined from root file, fall-back binning used:", fallbackBins
        result = fallbackBins
        assert result != []
    result.sort()
    return result

def plot(module, plots, fdata, mc, op):
    if op.verbose: print "%1.2f | Start plotting" % clock()
    if len(plots)==0:
        plots = []
    for p in plots:
        if hasattr(module, p):
            print "New plot:",
            getattr(module,p)(fdata,mc,op)
    if op.verbose: print "%1.2f | End" % clock()


def commandlineOptions(algorithm="ak5PFJets", correction = "L1L2L3CHS", lumi=0.0, energy=7, out="out", formats=['png', 'pdf'], files=["../../data/data_Oct19.root", "../../data/powheg_Oct19.root"], plots = [], bins = []):
    parser = argparse.ArgumentParser(
        description="%(prog)s does all the plotting.",
        epilog="Have fun.")
    # input files
    parser.add_argument('files', metavar='file', type=str, nargs='*', default=files,#type=argparse.FileType('r'), nargs='+',
        help="data and Monte Carlo input root file(s). One data file and at least one Monte Carlo file is assumed.")
#    parser.add_argument('-m','--mc', type=argparse.FileType('r'), nargs='+',
#        help="Monte Carlo input file(s) if one likes to use more than one data input file (not implemented yet)")
#    parser.add_argument('-d','--data', type=argparse.FileType('r'), nargs='+',
#        help="data input file(s). This is an alternative to simply specifying files if more than one data input file is required (not implemented yet)")
    parser.add_argument('-D','--data-label', type=str, nargs='+', default = "data",
        help="pile-up distributions")
    parser.add_argument('-M','--mc-label', type=str, nargs='+', default = "MC",
        help="pile-up distributions")
    parser.add_argument('-p','--pudist', type=str, nargs='+', #type=argparse.FileType('r'), nargs='+',
        help="pile-up distributions")
    # lumi and energy settings
    parser.add_argument('-a','--algorithm', type=str, default=algorithm,
        help="output directory for plots")
    parser.add_argument('-c','--correction', type=str, default=correction,
        help="output directory for plots")
    parser.add_argument('-l','--lumi', type=float, default=lumi,
        help="luminosity for the given data in /pb")
    parser.add_argument('-e','--energy', type=int, default=energy,
        help="centre-of-mass energy for the given samples in TeV")
    # output and selection settings
    parser.add_argument('-o','--out', type=str, default=out,
        help="output directory for plots")
    parser.add_argument('-f','--formats', type=str, nargs='+', default=formats,
        help="output format for the plots")
    parser.add_argument('-L','--layout', type=str, nargs='+', default='generic', ##document: serif,LaTeX,pdf ; slides: sans serif, big, png ; generic: slides + pdf
        help="output format for the plots")
    parser.add_argument('-P','--plots', type=str, nargs='+', default = plots,
        help="do only this plot/these plots")

    parser.add_argument('-n', '--normalize', action='store_true', help="normalize Monte Carlo samples to the event count in data and regardless of the given luminosity. This is only applied to data/MC comparisons")
    parser.add_argument('-v', '--verbose', action='store_true', help="verbosity")
    opt = parser.parse_args()
    #to be substituted by commandline arguments (perhaps changed, no formatting options here? but for multiple MC, colors (predefined sequence?) and labels are needed)
    if opt.verbose: print opt #schoener for o in opt.dict print...
    opt.mc_color = '#CBDBF9'
    opt.data_color = 'black'
    opt.author = 'Berger / Hauth'
    opt.factor = 1.0
    opt.data = opt.files[0]
    opt.mc = opt.files[1]
    opt.bins = bins
    return opt


def getFactor(lumi,f_data, f_mc, quantity='z_phi', change={}):
    name = GetHistoname(quantity,change)
    histo_data = getROOT.SafeConvert(f_data,name)
    histo_mc = getROOT.SafeConvert(f_mc, name.replace('Res',''))
    histo_mc.scale(lumi)
    print "    >>> The additional scaling factor is:", histo_data.ysum/histo_mc.ysum
    return histo_data.ysum/histo_mc.ysum
    
def GetReweighting(datahisto, mchisto, drop=True):
    if drop:
        datahisto.dropbin(0)
        datahisto.dropbin(-1)
        mchisto.dropbin(0)
        mchisto.dropbin(-1)
    reweighting = []
    for i in range(len(mchisto)):
        if i>13: break
        reweighting.append(datahisto.y[i]/mchisto.y[i])
    return reweighting

# gets obsolete
def GetPath():
    """Return datapath depending on machine and user name."""
    host = socket.gethostname()
    username = getpass.getuser()
    datapath = ""
    if username == 'berger':
        if host.find('naf') >= 0:
            datapath = "/scratch/hh/lustre/cms/user/berger/analysis/"
        elif host.find('ekplx46') >= 0:
            datapath = "/local/scratch/berger/data/"
        elif host.find('pccms') >=0:
            datapath = "/data/berger/data/"
    elif username == 'poseidon':
        # thomas local machines
        datapath = "/home/poseidon/uni/code/CalibFW/"
    elif username == 'hauth':
        if host.find('ekplx') >= 0:
            datapath = "/local/scratch/hauth/data/ZPJ2010/"
    elif username == 'piparo':
        if host.find('ekplx') >= 0:
            datapath = ""
    else:
        datapath = ""
    
    try:
        os.listdir(datapath)
    except:
        print "Input path", datapath, "does not exist."
        sys.exit(1)
    return datapath

def GetNameFromSelection(quantity='zmass', common={}, variation={}):
    GetHistoname(quantity, common)

def GetHistoname(quantity='zmass', change={}):
    #Typical name: 'NoBinning_incut/zmass_ak7PFJetsL1L2L3Res_hist'
    # Set standard values
    keys = ['bin', 'incut', 'var', 'sep', 'algo', 'jettype', 'jet', 'correction', 'plottype']
    standardselection = {'bin': 'NoBinning', 'incut': 'incut', 'var': '', 'sep': '/', 'algo': 'ak5', 'jettype': 'PF', 'jet': 'Jets', 'correction': 'L1L2L3CHS', 'plottype': 'hist'}
    hst = ''
    histolist = []
    # apply requested changes
    for k in change.keys():
        print "Replacing standardselection for " + k + " with " + change[k]
        standardselection[k] = change[k]
    # make a prototype name
    for k in keys:
        hst += standardselection[k] + '_'
    hst = hst.replace('_/_', '/').replace('hist_', 'hist').replace('graph_', 'graph').replace('_Jets_', 'Jets').replace('_PF', 'PF').replace('_Calo', 'Calo')
    # correct string formatting
    if quantity.find('graph') >= 0:
        hst = hst[hst.find('/') + 1:]
        hst = hst.replace('hist', 'graph')
        quantity = quantity[:quantity.find('graph')]
    # add the quantity
    if hst.find('/') > 0:
        hst = hst.replace('/', '/' + quantity + '_').replace('_/', '/')
    else:
        hst = quantity + '_' + hst
    while hst.find('__') >= 0:
        hst = hst.replace('__', '_')
    
    return hst
    
#    wording = {'ak5': 'Anti-$k_T$ 0.5', 'ak7': 'Anti-$k_T$ 0.7',
#       'PF': Particle Flow jets', 'Calo': 'Calorimeter jets',
#        'L1': 'L1 corrected', 'L1L2': 'L2 corrected', 'L1L2L3': 'Fully corrected',
#        'data': '2011 data','jet': 'Jets', 'correction': 'L1L2L3', 'zjet': 'Zplusjet', 'type': 'data', 'object': 'hist', 'generator':'none'}

def newplot(ratio=False):
    fig = plt_figure(figsize=[7,7])
    ax = fig.add_subplot(111)
    if ratio:
        pass
    return fig, ax

# obsolete
def makeplot(quantity, variation={}, common={}):
    # fig (mit ratio?)
    fig = matplotlib.pyplot.figure(figsize=[7,7])
    ax = fig.add_subplot(111)
    name = quantity
####1. standard captions L (always), sqrt(s) (if data) tick_params
    
####2. captions (quantity) x,y label (kein titel)
####3. add plots (plot and legend) (different) errorbar contour (colormap) fill (histo) fill_between label='text' line. legend: numpoints ncol title
####   scatter subplots(2,1,True)
####4. draw extra lines and texts (common)
####axh/vline/span arrow, annotate line.set_label('') text(0.5, 0.5,'matplotlib', horizontalalignment='center', verticalalignment='center', transform = ax.transAxes)
    return fig, ax, name

# collect all captions here
def captions(ax, opt=commandlineOptions(), algo=""):
    # Energy
    ax.text(0.99, 1.06, r"$\sqrt{s} = %u\,\mathrm{TeV}$" % (opt.energy),
        va='top', ha='right', transform=ax.transAxes, fontsize=15)
    # Luminosity
    if opt.lumi >= 1000:
        ax.text(0.01, 1.06, r"$\mathcal{L} = %1.1f\,\mathrm{fb}^{-1}$" % (opt.lumi / 1000.0),
            va='top', ha='left', transform=ax.transAxes, fontsize=15)
    elif opt.lumi > 0:
        ax.text(0.01, 1.06, r"$\mathcal{L} = %1.1f\,\mathrm{pb}^{-1}$" % (opt.lumi),
            va='top', ha='left', transform=ax.transAxes, fontsize=15)
    AddAlgoAndCorrectionCaption(ax,algo)
    return ax

def AddAlgoAndCorrectionCaption(ax, algo = "ak5PFJets"):
    posx = 0.05
    posy = 0.95
    if algo.find("L1L2L3Res")>=0:
        corr = r"L1L2L3 Res corrected"
    elif algo.find("L1L2L3")>=0:
        corr = r"L1L2L3 corrected"
    elif algo.find("L1L2")>=0:
        corr = r"L1L2 corrected"
    elif algo.find("L1")>=0:
        corr = r"L1 corrected"
    else:
        corr = r"uncorrected"
    if algo.find("ak5")>=0:
        jet = r"anti-$k_{T}$ 0.5 PF jets"
    else:
        jet = ""
        corr = ""
    ax.text(posx, posy, jet, va='top', ha='left', transform=ax.transAxes)
    ax.text(posx, posy-0.07, corr, va='top', ha='left', transform=ax.transAxes)
    if algo.find("CHS")>=0:
        ax.text(posx, posy - 0.14, r"CHS applied",
            va='top', ha='left', transform=ax.transAxes)
    return ax

def tags(ax, status='', author='', date='today'):
    status = '' #for now do not show 'private work'
    author = ''
    date = ''
    if status > 0:
        ax.text(0.99, 1.01, status,
            va='bottom', ha='right', transform=ax.transAxes, fontsize=15)
    if date > 0:
        if date == 'now':
            ax.text(0.5, 1.01, strftime("%d %b %Y %H:%M", localtime()),
            va='bottom', ha='center', transform=ax.transAxes, fontsize=15)
        elif date == 'today':
            ax.text(0.5, 1.01, strftime("%d %b %Y", localtime()),
            va='bottom', ha='center', transform=ax.transAxes, fontsize=15)
        else:
            ax.text(0.5, 1.01, date,
            va='bottom', ha='center', transform=ax.transAxes, fontsize=15)
    if author > 0:
        ax.text(0.01, 1.01, author,
        va='bottom', ha='left', transform=ax.transAxes, fontsize=15)
    return ax


def AxisLabels(ax, q='resp', obj='jet'):
    """label the axes according to the plotted quantity"""
    # ax.legend(loc='best', numpoints=1, frameon=True)
    # according to quantity q
    def gev():
        return unit("GeV")
    def unit(s="", brackets=False):
        if s !="":
            if brackets: return " [\mathrm{%s}]" % s
            else: return " / \mathrm{%s}" % s
        else: return s
    # all labels va top ha right x=1 y =1,
    if q == 'pt':
        ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{" + obj + "} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(0, 200)
        ax.set_ylim(bottom=0.0)
    elif q == 'phi':
        ax.set_xlabel(r"$\phi^\mathrm{" + obj + "}$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(-3.5, 3.5)
        ax.set_xticks([-3.141592654,-1.570796327,0,1.570796327,3.141592654])
        ax.set_xticklabels([r"$-\pi$",r"$-\frac{\pi}{2}$",r"$0$",r"$\frac{\pi}{2}$",r"$\pi$"])
#        ax.set_xticklabels([r"$20$",r"$30$",r"$40$",r"$50$",r"$60$",r"",r"$80$",r"",r"$200$",r"$300$",r"$400$"],minor=True)
        ax.set_ylim(bottom=0.0)
        ax.legend(loc='lower center', numpoints=1, frameon=True)
    elif q == 'eta':
        ax.set_xlabel(r"$\eta^\mathrm{" + obj + "}$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(-5.0, 5.0)
        ax.set_ylim(bottom=0.0)
        if obj == 'Z':
            ax.legend(loc='lower center', numpoints=1, frameon=True)
    elif q == 'mass':
        ax.set_xlabel(r"$m_\mathrm{" + obj + "} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(70, 110)
        ax.set_ylim(bottom=0.0)
    elif q == 'jetresp':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} %s$" % unit("GeV"), ha="right", x=1)
        ax.set_ylabel(r"$p_\mathrm{T}$ balance", va="top", y=1)
        ax.set_xlim(10, 240)
        ax.set_ylim(0.75, 1.0)
    elif q == 'mpfresp':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"MPF", va="top", y=1)
        ax.set_xlim(10, 240)
        ax.set_ylim(0.75, 1.0)
    elif q == 'datamc_ratio':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Data/MC", va="top", y=1)
        ax.set_xlim(10, 240)
        ax.set_ylim(0.8, 1.1)
        ax.legend(bbox_to_anchor=(0.65, 0.6, 0.3, 0.2), loc='upper right', numpoints=1, frameon=True)
    elif q == 'cutineff':
        ax.set_ylabel(r"Cut Infficiency", y=1, va="top" )
        ax.set_xlabel(r"NRV",x=1)
        ax.set_xlim(0, 25)
        ax.set_ylim(0.0, 1.0)
    elif q == 'recovert':
        ax.set_xlabel(r"Number of reconstructed vertices $n$", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(0,25)
        ax.set_ylim(bottom=0.0)
    elif q == 'jetconstituents':
        ax.set_xlabel(r"Jet Constituents", ha="right", x=1)
        ax.set_ylabel(r"Events", va="top", y=1)
        ax.set_xlim(1, 60)
        #ax.set_xlim(0, 350)
    elif q == 'components':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Leading Jet Component Fraction", va="top", y=1)
        ax.set_xlim(25,500)
        ax.set_ylim(0.0,1.0)
        ax.semilogx()
        ax.set_xticklabels([r"$10$",r"$100$",r"$1000$"])
        ax.set_xticklabels([r"$20$",r"$30$",r"$40$",r"$50$",r"$60$",r"",r"$80$",r"",r"$200$",r"$300$",r"$400$"],minor=True)
    elif q == 'components_diff':
        ax.set_xlabel(r"$p_\mathrm{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"Data$-$MC of Leading Jet Components", va="top", y=1)
        ax.set_xlim(25,500)
        ax.set_ylim(-0.05,0.05)
        ax.semilogx()
        ax.set_xticklabels([r"$10$",r"$100$",r"$1000$"])
        ax.set_xticklabels([r"$20$",r"$30$",r"$40$",r"$50$",r"$60$",r"",r"$80$",r"",r"$200$",r"$300$",r"$400$"],minor=True)
    elif q == 'extrapol':
        if obj == 'jet2':
            ax.set_xlabel(r"$p_\mathrm{T}^{\mathrm{"+obj+"}}/p_\mathrm{T}^{Z}$", ha="right", x=1)
        elif obj == 'deltaphi':
            ax.set_xlabel(r"$\Delta\phi$", ha="right", x=1)
        ax.set_ylabel(r"Response", va="top", y=1)
        ax.set_xlim(0,0.4)
        ax.set_ylim(0.86,1.04)
    elif q == 'runs':
        ax.set_xlabel(r"run", ha="right", x=1)
        ax.set_ylabel(r"n_\mathrm{Events} / $\mathcal{L} \; [\mathrm{pb}]$", va="top", y=1)
        ax.set_xlim(160404,173692)
        ax.set_ylim(0.0,20.0)
    elif q == 'runlist':
        ax.set_xlabel(r"run range", ha="right", x=1)
        ax.set_ylabel(r"$n_\mathrm{Events} / \mathcal{L} \; [\mathrm{pb}]$", va="top", y=1)
        ax.set_xlim(0,300)
        ax.set_ylim(0.0,20.0)
    else:
        print "The quantity", q, "was not found. A default formatting of the axis labels is used."
        ax.set_xlabel(r"$p_\mathrm{T} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"arb. units", va="top", y=1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
    # more precise
    return ax


def hist_baseplot(plot_collection, caption, settings, modifierBeforeSave, alsoInLogScale = True):
    
    tf, ta, tname = makeplot(caption) 
    ta = captions(ta, settings, False)
#    ta.set_ylim(top=histo_mc.ymax*1.2)
    ta = tags(ta, 'Private work', 'Joram Berger')
    #ta.legend(loc=legloc, numpoints=1, frameon=False)
    #sta = AxisLabels(ta, q, obj)
    #ta.autoscale()
    
    for (quantName, inpFile, drawParameters, modifierFunc, modifierDataFunc) in plot_collection:
        rootHisto = getROOT.SafeConvert(inpFile, quantName, settings.lumi, settings.outputformats, 5)

        if not (modifierDataFunc == None):
            modifierDataFunc(rootHisto, settings)

        my_fmt = "-"
        my_color = "Red"
        my_drawstyle = "steps-mid"
        my_label = ""

        for drawKey, drawValue in drawParameters.items():
            if drawKey == "fmt":
                my_fmt = drawValue
            if drawKey == "color":
                my_color = drawValue
            if drawKey == "drawstyle":
                my_drawstyle = drawValue
            if drawKey == "label":
                my_label = drawValue                
            if drawKey == "log":
                my_label = drawValue 

        #print rootHisto.xc
        #print rootHisto.y
        mplHisto = ta.errorbar(rootHisto.xc, rootHisto.y, rootHisto.yerr, drawstyle=my_drawstyle,  
                               color=my_color, fmt=my_fmt, capsize=0, label=my_label) 
        
        if not (modifierFunc == None):
            modifierFunc(mplHisto, settings)
    
    if not (modifierBeforeSave == None):
        modifierBeforeSave(tf, ta, tname, plot_collection, caption, settings)        
    
    Save(tf, caption, settings, alsoInLogScale)
   
 
def moregenericplot(quantity, q, obj, fdata, fmc, factor, stg, legloc='center right'):
    print q, "of the", obj
    oname = GetNameFromSelection(quantity)[0]
    histo_data = getROOT.SafeConvert(fdata, oname, stg.lumi, stg.outputformats, 5)
    histname = oname.replace('data', 'mc').replace('Res', '')
    histo_mc = getROOT.SafeConvert(fmc, histname, stg.lumi, stg.outputformats, 5)
    histo_mc.scale(factor)

    tf, ta, tname = makeplot(quantity)
    histo02 = ta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color='FireBrick', fmt='-', capsize=0, label='MC')
    histo01 = ta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black', fmt='o', capsize=0, label='data', fillcolor=stg.mcColor)
    ta = captions(ta, stg, False)
    ta.set_ylim(top=histo_mc.ymax * 1.2)
    ta = tags(ta, 'Private work', 'Joram Berger')
    ta.legend(loc=legloc, numpoints=1, frameon=False)
    ta = AxisLabels(ta, q, obj)

    Save(tf, quantity, stg)
    

# use this!
def GetDataOrMC(quantity, rootfile, changes={}, rebin=1):
    histo = GetHistoname(quantity, changes)
    if getROOT.IsObjectExistent(rootfile, histo):
        return getROOT.SafeConvert(rootfile, histo, rebin=rebin)
    else:
        return getROOT.SafeConvert(rootfile, histo.replace("Res",""), rebin=rebin)

    
    
def genericplot(quantity, q, obj, fdata, custom_keys_data, fmc, custom_keys_mc, factor, stg, legloc='center right'):
    #print q, "of the", obj    
    histo_data = GetDataOrMC(quantity, fdata, custom_keys_data, stg)
    histo_mc = GetDataOrMC(quantity, fmc, custom_keys_mc, stg)
    histo_mc.scale(factor)

    tf, ta, tname = makeplot(quantity)
    histo00 = ta.bar(histo_mc.x, histo_mc.y,(histo_mc.x[2]-histo_mc.x[1]),bottom =np.ones(len(histo_mc.x))*1e-6, fill=True, facecolor=stg.mcColor, edgecolor=stg.mcColor)
    histo02 = ta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color='#CBDBF9', fmt='-', capsize=0, label='MC')
    histo01 = ta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black', fmt='o', capsize=0, label='data')
    ta = captions(ta, stg, False)
    ta.set_ylim(top=histo_mc.ymax * 1.2)
    ta = tags(ta, 'Private work', 'Joram Berger')
    ta.legend(loc=legloc, numpoints=1, frameon=False)
    ta = AxisLabels(ta, q, obj)

    Save(tf, quantity, stg)


def Save(figure, name, opt, alsoInLogScale = False):
    _internal_Save ( figure, name, opt)
    
    if alsoInLogScale:
        figure.get_axes()[0].set_yscale( 'log' )
        _internal_Save(figure, name + "_log_scale", opt)

def _internal_Save(figure, name, opt):
    """Save this figure in all listed data formats.
    
    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    if opt.out not in os.listdir("."):
        os.mkdir(opt.out)
    name = opt.out + '/' + name
    print ' -> Saving as',
    first = True
    for format in opt.formats:
        if format in ['pdf', 'png', 'ps', 'eps', 'svg']:
            if not first:
                print ",",
            else:
                first = False
            print name + '.' + format,
            figure.savefig(name + '.' + format, dpi = 100)
            
        elif format in ['txt', 'npz', 'dat']:
            pass    #Ignore this here, as it is respected in the SafeConvert function
        else:
            print format, "failed. Output type is unknown or not supported."
    print
    

# obsolete to be replaced
def GetScaleResolution(filename='scale_and_resolution.txt'):
    """Read the values for the jet energy scale and the jet energy resolution from a file
    
    The file should look like:
    #Jet energy scale, scale error-, scale error+:
    #Jet energy resolution, resolution error-, resolution error+:
    1.00 0.01 0.01
    1.00 0.01 0.01
    """
    f = file(filename, 'r')
    scale = []
    resol = []
    while True:
        line = f.readline()
        if len(line) == 0: break
        if line.find('#') < 0:
            if len(scale) == 0:
                for v in line.split():
                    scale.append(float(v))
            else:
                for v in line.split():
                    resol.append(float(v))
    print scale
    print resol
    return scale, resol
