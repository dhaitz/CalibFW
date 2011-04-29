import socket
import getpass
import os
import numpy as np
import matplotlib.pyplot as plt
from time import localtime, strftime

#os.makedir('out/dat')

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
    

def GetPath():
    """Return datapath depending on machine and user name.
    
    Detailed
    """
    host = socket.gethostname()
    username = getpass.getuser()
    if username == 'berger':
        if host.find('naf') >= 0:
            datapath = "/scratch/hh/lustre/cms/user/berger/analysis/"
        elif host.find('ekplx46') > 0:
            datapath = "/local/scratch/berger/data/"
    elif username == 'hauth':
        if host.find('ekplx') >= 0:
            datapath = "/local/scratch/hauth/data/ZPJ2010/"
    elif username == 'piparo':
        if host.find('ekplx') >= 0:
            datapath = ""
    else:
        datapath = ""
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
    if hst.find('/')>0:
        hst = hst.replace('/','/'+quantity+'_').replace('_/','/')
    else:
        hst = quantity + '_' + hst
    hst = hst.replace('_PF','PF').replace('_Calo','Calo')
    hst = hst.replace('L3_Zplusjet_data','L3Res_Zplusjet_data')
    while hst.find('__')>=0:
        hst = hst.replace('__','_')
    print hst
    # apply variation
#    for i in key:
#    hst.replace(variationkey standardvalue, variation i)
    histolist.append(hst)
    return histolist
    
#    wording = {'ak5': 'Anti-$k_T$ 0.5', 'ak7': 'Anti-$k_T$ 0.7',
#       'PF': Particle Flow jets', 'Calo': 'Calorimeter jets',
#        'L1': 'L1 corrected', 'L1L2': 'L2 corrected', 'L1L2L3': 'Fully corrected',
#        'data': '2011 data','jet': 'Jets', 'correction': 'L1L2L3', 'zjet': 'Zplusjet', 'type': 'data', 'object': 'hist', 'generator':'none'}

def makeplot(quantity, variation = {}, common = {}):
    # fig (mit ratio?)
    fig =  plt.figure()
    ax = subplot()
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
        verticalalignment='top', horizontalalignment='right',
        transform=ax.transAxes, fontsize=15)
    if stg.lumi > 0:
        ax.text(0.01, 0.98, r"$\mathcal{L} = " + str(stg.lumi) + " \,\mathrm{pb}^{-1}$",
            verticalalignment='top', horizontalalignment='left',
            transform=ax.transAxes, fontsize=15)
    return ax

def tags(ax, status='', author='', date='now'):
    if status > 0:
        ax.text(0.99, 1.01, status,
            va='bottom', ha='right', transform=ax.transAxes, fontsize=15)
    if date > 0:
        if date == 'now':
            ax.text(0.5, 1.01, strftime("%d %b %Y %H:%M", localtime()),
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
    elif q.find('phi'):
        ax.set_xlabel(r"$\phi^{"+obj+"}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
    elif q.find('eta'):
        ax.set_xlabel(r"$\eta^{"+obj+"}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
    elif q == 'mass':
        ax.set_xlabel(r"$m_{"+obj+"} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"events", va = "top", y = 1)
    elif q.find('jetresp'):
        ax.set_xlabel(r"$p_{T}^{Z} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"$p_{T}$ balance", va = "top", y = 1)
    elif q.find('mpfresp'):
        ax.set_xlabel(r"$p_{T}^{Z} / \mathrm{GeV}$", ha = "right", x = 1)
        ax.set_ylabel(r"MPF", va = "top", y = 1)
    # more precise
    return ax


def Save(figure, name='plot', stg=StandardSettings()):
    """Save this figure in all listed data formats.
    
    The standard data formats are png and pdf.
    Available graphics formats are: pdf, png, ps, eps and svg
    """
    name = stg.outputdir + name
    print 'Saved as',
    for format in stg.outputformats:
        if format in ['pdf', 'png', 'ps', 'eps', 'svg']:
            figure.savefig(name + '.' + format)
            print name+'.'+format,
        elif format in ['txt', 'npz', 'dat']:
            pass    #Ignore this here, as it is respected in the SafeConvert function
        else:
            print format + " output is not possible."

