# -*- coding: utf-8 -*-

import socket
import getpass
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from time import localtime, strftime
import getROOT

#os.makedir('out/dat')

def Print(objekt):
    print '    > dbg:', objekt, '<'
    
def GetReweighting(datahisto, mchisto, drop=True):
    if drop:
        datahisto.dropbin(0)
        datahisto.dropbin(-1)
        mchisto.dropbin(0)
        mchisto.dropbin(-1)
    reweighting = []
    for i in range(len(mchisto)):
        reweighting.append(datahisto.y[i] / mchisto.y[i])
    return reweighting


class StandardSettings:
    outputformats = ['png', 'pdf']
    style = 'document' # web, presentation, tdr, sloppy 
    lumi = 0
    cme = 7
    author = 'Berger / Hauth'
    outputdir = 'out/'
    outputdatadir = 'out/dat/'
    def SetOutputdir(self, path, datapath=outputdatadir):
        outputdir = path
        try:
            os.mkdir(self.outputdatadir)
            os.mkdir(self.outputdir)
        except(OSError):
            pass
    verbosity = 1  # 0: no comments, only errors, 1: no comments only warnings, 2: , 3: comment everything
    

def GetPath():
    """Return datapath depending on machine and user name.
    
    Detailed
    """
    host = socket.gethostname()
    username = getpass.getuser()
    datapath = ""
    if username == 'berger':
        if host.find('naf') >= 0:
            datapath = "/scratch/hh/lustre/cms/user/berger/analysis/"
        elif host.find('ekplx46') >= 0:
            datapath = "/local/scratch/berger/data/"
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

def GetNameFromSelection(quantity='zmass', variation={}, common={}):
    #'zmass_ak7PFJetsL1L2L3Res_Zplusjet_data_hist'
    # Set standard values
    keys = ['bin', 'incut', 'var', 'sep', 'algo', 'jettype', 'jet', 'correction', 'zjet', 'type', 'object']
    standardselection = {'incut': 'incut', 'bin': 'NoBinning', 'var': '', 'sep': '/', 'algo': 'ak5', 'jettype': 'PF', 'jet': 'Jets', 'correction': 'L1L2L3NoPU', 'zjet': 'Zplusjet', 'type': 'data', 'object': 'hist', 'generator':'none'}
    hst = ''
    histolist = []
    
    # apply requested changes
    for k in common.keys():
        print "Replacing standardselection for " + k + " with " + common[k] 
        standardselection[k] = common[k]
    # consequences of changes
    # correct string formatting
    for k in keys:
        hst += standardselection[k] + '_'
    hst = hst.replace('_/_', '/').replace('hist_', 'hist').replace('graph_', 'graph').replace('_Jets_', 'Jets')
    if quantity.find('graph') >= 0:
        hst = hst[hst.find('/') + 1:]
        hst = hst.replace('hist', 'graph')
        quantity = quantity[:quantity.find('graph')]
    if hst.find('/') > 0:
        hst = hst.replace('/', '/' + quantity + '_').replace('_/', '/')
    else:
        hst = quantity + '_' + hst
    if hst.find('allevents') > 0:
        hst = hst.replace('_hist', '_nocut_hist')
    hst = hst.replace('_PF', 'PF').replace('_Calo', 'Calo')
    hst = hst.replace('L3_Zplusjet_data', 'L3_Zplusjet_data')
    while hst.find('__') >= 0:
        hst = hst.replace('__', '_')
    
    print " ‣ Histogram: ", hst
    # apply variation
#    for i in key:
#    hst.replace(variationkey standardvalue, variation i)
    histolist.append(hst)
    return histolist
    
#    wording = {'ak5': 'Anti-$k_T$ 0.5', 'ak7': 'Anti-$k_T$ 0.7',
#       'PF': Particle Flow jets', 'Calo': 'Calorimeter jets',
#        'L1': 'L1 corrected', 'L1L2': 'L2 corrected', 'L1L2L3': 'Fully corrected',
#        'data': '2011 data','jet': 'Jets', 'correction': 'L1L2L3', 'zjet': 'Zplusjet', 'type': 'data', 'object': 'hist', 'generator':'none'}

def datahisto(histoname):
    return histoname.replace('_Zplusjet_mc', '_Zplusjet_data')

def mchisto(histoname):
    return histoname.replace('_Zplusjet_data', '_Zplusjet_mc')

def makeplot(quantity, variation={}, common={}):
    # fig (mit ratio?)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    name = quantity
####1. standard captions L (always), sqrt(s) (if data) tick_params
    
####2. captions (quantity) x,y label (kein titel)
####3. add plots (plot and legend) (different) errorbar contour (colormap) fill (histo) fill_between label='text' line. legend: numpoints ncol title
####   scatter subplots(2,1,True)
####4. draw extra lines and texts (common)
####axh/vline/span arrow, annotate line.set_label('') text(0.5, 0.5,'matplotlib', horizontalalignment='center', verticalalignment='center', transform = ax.transAxes)
    return fig, ax, name
    
def captions(ax, stg=StandardSettings(), twolumis=True):
    ax.text(0.99, 0.98, r"$\sqrt{s} = " + str(stg.cme) + " \,\mathrm{TeV}$",
        va='top', ha='right', transform=ax.transAxes, fontsize=15)
    if stg.lumi > 0:
        ax.text(0.01, 0.98, r"$\mathcal{L}_{2011} = " + str(int(stg.lumi)) + " \,\mathrm{pb}^{-1}$",
            va='top', ha='left', transform=ax.transAxes, fontsize=15)
        if twolumis:
            ax.text(0.01, 0.93, r"$\mathcal{L}_{2010} = " + str(36) + " \,\mathrm{pb}^{-1}$",
                va='top', ha='left', transform=ax.transAxes, fontsize=15)
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
    # according to quantity q
    if q == 'pt':
        ax.set_xlabel(r"$p_{T}^{" + obj + "} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"events", va="top", y=1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
    elif q == 'phi':
        ax.set_xlabel(r"$\phi^{" + obj + "}$", ha="right", x=1)
        ax.set_ylabel(r"events", va="top", y=1)
        ax.set_xlim(-3.5, 3.5)
        #ax.set_ticks(-3.141592654,-1.570796327,0,1.570796327,3.141592654)
        #ax.set_ticklabels(r'$-\pi',r'$-\frac{\pi}{2}$','0',r'$\frac{\pi}{2}$',r'$\pi')
        ax.set_ylim(bottom=0.0)
    elif q == 'eta':
        ax.set_xlabel(r"$\eta^{" + obj + "}$", ha="right", x=1)
        ax.set_ylabel(r"events", va="top", y=1)
        ax.set_xlim(-5.0, 5.0)
        ax.set_ylim(bottom=0.0)
    elif q == 'mass':
        ax.set_xlabel(r"$m_{" + obj + "} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"events", va="top", y=1)
        ax.set_xlim(60, 120)
        ax.set_ylim(bottom=0.0)
    elif q == 'jetresp':
        ax.set_xlabel(r"$p_{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"$p_{T}$ balance", va="top", y=1)
        ax.set_xlim(10, 200)
        ax.set_ylim(0.75, 1.0)
    elif q == 'mpfresp':
        ax.set_xlabel(r"$p_{T}^{Z} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"MPF", va="top", y=1)
        ax.set_xlim(10, 200)
        ax.set_ylim(0.75, 1.0)
    elif q == 'recovert':
        ax.set_xlabel(r"Number of reconstructed vertices $n$", ha="right", x=1)
        ax.set_ylabel(r"events", va="top", y=1)
        #ax.set_xlim(0, 350)
    else:
        print "The quantity", q, "was not found. A default formatting of the axis labels is used."
        ax.set_xlabel(r"$p_{T} / \mathrm{GeV}$", ha="right", x=1)
        ax.set_ylabel(r"arb. units", va="top", y=1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
    # more precise
    return ax


def hist_baseplot(plot_collection, caption, settings, modifierBeforeSave):
    
    tf, ta, tname = makeplot(caption) 
    ta = captions(ta, settings, False)
#    ta.set_ylim(top=histo_mc.ymax*1.2)
    ta = tags(ta, 'Private work', 'Joram Berger')
    #ta.legend(loc=legloc, numpoints=1, frameon=False)
    #sta = AxisLabels(ta, q, obj)
    
    histos = []
    
    for (quantName, inpFile, modifierFunc) in plot_collection:
        rootHisto = getROOT.SafeConvert(inpFile, quantName, settings.lumi, settings.outputformats, 5)

        mplHisto = ta.errorbar(rootHisto.xc, rootHisto.y, rootHisto.yerr, drawstyle='steps-mid', color='FireBrick', fmt='-', capsize=0, label='MC') 
        histos += [ mplHisto]

        if not (modifierFunc == None):
            modifierFunc(mplHisto, settings)
    
    if not (modifierBeforeSave == None):
        modifierBeforeSave(tf, ta, tname, plot_collection, caption, settings)        
    
    Save(tf, caption, settings)
    
 
def moregenericplot(quantity, q, obj, fdata, fmc, factor, stg, legloc='center right'):
    print q, "of the", obj
    oname = GetNameFromSelection(quantity)[0]
    histo_data = getROOT.SafeConvert(fdata, oname, stg.lumi, stg.outputformats, 5)
    histname = oname.replace('data', 'mc').replace('Res', '')
    histo_mc = getROOT.SafeConvert(fmc, histname, stg.lumi, stg.outputformats, 5)
    histo_mc.scale(factor)

    tf, ta, tname = makeplot(quantity)
    histo02 = ta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color='FireBrick', fmt='-', capsize=0, label='MC')
    histo01 = ta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black', fmt='o', capsize=0, label='data')
    ta = captions(ta, stg, False)
    ta.set_ylim(top=histo_mc.ymax * 1.2)
    ta = tags(ta, 'Private work', 'Joram Berger')
    ta.legend(loc=legloc, numpoints=1, frameon=False)
    ta = AxisLabels(ta, q, obj)

    Save(tf, quantity, stg)
    

    # try load data and mc from the input files.
    # if data was not found, mc will be tried 
def GetDataOrMC(quantity, inp_file, custom_keys,settings):
    oname = GetNameFromSelection(quantity, {}, custom_keys)[0]
    
    # try load data and mc from the input files.
    # if data was not found, mc will be tried 
    if getROOT.IsObjectExistent( inp_file, oname):
        return getROOT.SafeConvert( inp_file, oname, settings.lumi, settings.outputformats,5)
    
    oname = oname.replace('data','mc').replace('Res','')
    return getROOT.SafeConvert( inp_file, oname, settings.lumi, settings.outputformats,5)
    
def genericplot(quantity, q, obj, fdata, custom_keys_data, fmc, custom_keys_mc, factor, stg, legloc='center right'):
    #print q, "of the", obj    
    histo_data = GetDataOrMC(quantity, fdata, custom_keys_data, stg)
    histo_mc = GetDataOrMC(quantity, fmc, custom_keys_mc, stg)
    histo_mc.scale(factor)

    tf, ta, tname = makeplot(quantity)
    histo02 = ta.errorbar(histo_mc.xc, histo_mc.y, histo_mc.yerr, drawstyle='steps-mid', color='FireBrick', fmt='-', capsize=0, label='MC')
    histo01 = ta.errorbar(histo_data.xc, histo_data.y, histo_data.yerr, drawstyle='steps-mid', color='black', fmt='o', capsize=0, label='data')
    ta = captions(ta, stg, False)
    ta.set_ylim(top=histo_mc.ymax * 1.2)
    ta = tags(ta, 'Private work', 'Joram Berger')
    ta.legend(loc=legloc, numpoints=1, frameon=False)
    ta = AxisLabels(ta, q, obj)

    Save(tf, quantity, stg)


def Save(figure, name, stg):
    """Save this figure in all listed data formats.
    
    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    name = stg.outputdir + name
    print ' ‣ Saving as',
    for format in stg.outputformats:
        if format in ['pdf', 'png', 'ps', 'eps', 'svg']:
            print name + '.' + format,
            figure.savefig(name + '.' + format)
            
        elif format in ['txt', 'npz', 'dat']:
            pass    #Ignore this here, as it is respected in the SafeConvert function
        else:
            print format + "failed. Output type is unknown or not supported."
    print

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
