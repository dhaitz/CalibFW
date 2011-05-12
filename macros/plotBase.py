# -*- coding: utf-8 -*-

import socket
import getpass
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from time import localtime, strftime

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
        reweighting.append(datahisto.y[i]/mchisto.y[i])
    return reweighting


class StandardSettings:
    outputformats = ['png', 'pdf']
    style = 'document' # web, presentation, tdr, sloppy 
    lumi = 0
    cme = 7
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
    elif username == 'hauth':
        if host.find('ekplx') >= 0:
            datapath = "/local/scratch/hauth/data/ZPJ2010/"
    elif username == 'piparo':
        if host.find('ekplx') >= 0:
            datapath = ""
    else:
        datapath = ""
	print username
	print host
	print datapath
    try:
        os.listdir(datapath)
    except:
        print "Input path", datapath, "does not exist."
        sys.exit(1)
    return datapath

def GetNameFromSelection(quantity='zmass', variation={}, common={}):
    #'zmass_ak7PFJetsL1L2L3Res_Zplusjet_data_hist'
    # Set standard values
    keys = ['bin','incut','var','sep','algo','jettype','jet','correction', 'zjet', 'type', 'object']
    standardselection = {'incut': 'incut', 'bin': 'NoBinning', 'var': '', 'sep': '/', 'algo': 'ak5', 'jettype': 'PF','jet': 'Jets', 'correction': 'L1L2L3', 'zjet': 'Zplusjet', 'type': 'data', 'object': 'hist', 'generator':'none'}
    hst = ''
    histolist = []
    # apply requested changes
    for k in common.keys():
        standardselection[k] = common[k]
    # consequences of changes
    # correct string formatting
    for k in keys:
        hst += standardselection[k] + '_'
    hst = hst.replace('_/_', '/').replace('hist_','hist').replace('graph_','graph').replace('_Jets_','Jets')
    if quantity.find('graph') >= 0:
        hst = hst[hst.find('/')+1:]
        hst = hst.replace('hist','graph')
        quantity = quantity[:quantity.find('graph')]
    if hst.find('/')>0:
        hst = hst.replace('/','/'+quantity+'_').replace('_/','/')
    else:
        hst = quantity + '_' + hst
    if hst.find('allevents')>0:
        hst = hst.replace('_hist','_nocut_hist')
    hst = hst.replace('_PF','PF').replace('_Calo','Calo')
    hst = hst.replace('L3_Zplusjet_data','L3Res_Zplusjet_data')
    while hst.find('__')>=0:
        hst = hst.replace('__','_')
    
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
    return histoname.replace('_Zplusjet_mc','Res_Zplusjet_data')

def mchisto(histoname):
    return histoname.replace('Res_Zplusjet_data','_Zplusjet_mc')

def makeplot(quantity, variation = {}, common = {}):
    # fig (mit ratio?)
    fig =  plt.figure()
    ax = fig.add_subplot(111)
    name = quantity
####1. standard captions L (always), sqrt(s) (if data) tick_params
    
####2. captions (quantity) x,y label (kein titel)
####3. add plots (plot and legend) (different) errorbar contour (colormap) fill (histo) fill_between label='text' line. legend: numpoints ncol title
####   scatter subplots(2,1,True)
####4. draw extra lines and texts (common)
####axh/vline/span arrow, annotate line.set_label('') text(0.5, 0.5,'matplotlib', horizontalalignment='center', verticalalignment='center', transform = ax.transAxes)
    return fig, ax, name
    
def captions(ax,stg=StandardSettings()):
    ax.text(0.99, 0.98, r"$\sqrt{s} = " + str(stg.cme) + " \,\mathrm{TeV}$",
        va='top', ha='right', transform=ax.transAxes, fontsize=15)
    if stg.lumi > 0:
        ax.text(0.01, 0.98, r"$\mathcal{L} = " + str(stg.lumi) + " \,\mathrm{pb}^{-1}$",
            va='top', ha='left', transform=ax.transAxes, fontsize=15)
    return ax

def tags(ax, status='', author='', date='today'):
    status='' #for now do not show 'private work'
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
        ax.set_xlabel(r"$p_{T}^{"+obj+"} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
    elif q == 'phi':
        ax.set_xlabel(r"$\phi^{"+obj+"}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
        ax.set_xlim(-3.1415, 6.2830)
        ax.set_ylim(bottom=0.0)
    elif q == 'eta':
        ax.set_xlabel(r"$\eta^{"+obj+"}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
        ax.set_xlim(-5.0, 5.0)
        ax.set_ylim(bottom=0.0)
    elif q == 'mass':
        ax.set_xlabel(r"$m_{"+obj+"} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
    elif q == 'jetresp':
        ax.set_xlabel(r"$p_{T}^{Z} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"$p_{T}$ balance", va = "top", y = 1)
        ax.set_xlim = (10, 200)
    elif q == 'mpfresp':
        ax.set_xlabel(r"$p_{T}^{Z} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"MPF", va = "top", y = 1)
        ax.set_xlim(10, 200)
    elif q == 'recovert':
        ax.set_xlabel(r"Number of reconstructed vertices $n$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
        #ax.set_xlim(0, 350)
    else:
        print "The quantity", q, "was not found. A default formatting of the axis labels is used."
        ax.set_xlabel(r"$p_{T} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"arb. units", va = "top", y = 1)
        ax.set_xlim(0, 350)
        ax.set_ylim(bottom=0.0)
    # more precise
    return ax


def Save(figure, name, stg):
    """Save this figure in all listed data formats.
    
    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    name = stg.outputdir + name
    print ' ‣ Saving as',
    for format in stg.outputformats:
        if format in ['pdf', 'png', 'ps', 'eps', 'svg']:
            figure.savefig(name + '.' + format)
            print name+'.'+format,
        elif format in ['txt', 'npz', 'dat']:
            pass    #Ignore this here, as it is respected in the SafeConvert function
        else:
            print format + "failed. Output type is unknown or not supported."
    print

