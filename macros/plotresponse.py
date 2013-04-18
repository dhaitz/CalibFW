# -*- coding: utf-8 -*-
"""Extrapolate and plot the response and ratio

   Function hierachy:
   * Plotting (values from getresponse)
      plotratio
      plotresponse

   * Getting the graph:
      getresponse
        uses getroot.getgraph

"""
import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile
import ROOT

import plotbase
import plotdatamc
import getroot

from responsetools import *


def responseplot(files, opt, types=None, over=None, settings=None, 
                    figaxes=None, changes=None, 
                    markers=['o', '*', 'v', '^', '>']*4,
                    colors=['black', '#CBDBF9', 'black', 'blue']*5):

    if over=='jet1eta' or over=='jet1abseta':
        if changes is None:
            changes = {'alleta':True}
        else:
            changes['alleta'] = True

    if settings is None:
        settings = plotbase.createsettings(opt, changes, 'response_%s' % over)

    if figaxes == None: 
        figaxes = plotbase.newplot()
    fig =figaxes[0]
    ax=figaxes[1]

    labels = [labelformat(string) for string in types] 


    # iterate over types:
    for t, l, m in zip(types, labels, markers):
        # iterate over files:        
        for f, label, color in zip(files, opt.labels, 
                                            colors[len(files)*types.index(t):]):

            plot = getroot.root2histo(getresponse(t, over, opt, settings, f, 
                            None, changes, extrapol=settings['extrapolation']))
            ax.errorbar(plot.x, plot.y, plot.yerr, color=color, fmt=m, 
                                                label='%s (%s)' % (l, label))
            plotbinborders(ax, over, plot.y, opt)

    # format plot
    ax.axhline(1.0, color="black", linestyle='--')
    plotbase.labels(ax, opt, settings)
    plotbase.axislabels(ax, over, 'response', labels=opt.labels, settings=settings)
    plotbase.setaxislimits(ax, settings)

    #label with pt and eta cut
    if over in ['jet1eta', 'jet1abseta'] :
        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}$>30 GeV"
    else:
        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}$>30 GeV    $\eta^\mathrm{Jet1}$<1.3     $\alpha$ < 0.2"# or  $p_{\mathrm{T}}^{\mathrm{Jet2}}$<12 GeV"
    ax.text(0.02, 0.98, pt_eta_label, va='top', ha='left', color='black', 
                                        transform=ax.transAxes, size='small')

    # return subplot or save:
    if settings['subplot'] is True:
        return fig
    else:
        settings['filename'] = plotbase.getdefaultfilename(
                "Response__%s_over_%s" % ("_".join(types), over), opt, settings)
        plotbase.Save(fig, settings['filename'], opt)


def ratioplot(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='zpt',
                 binborders=False,
                 drawextrapolation=False,
                 figaxes = None,
                 fit=True,
                 legloc='lower right',
                 changes = None,
                 subtext = "",
                 ratiosubplot=False,
                 subplot = False,
                 extrapol=None,
                 settings=None):
    """type: bal|mpf[ratio|seperate]
    """

    if over=='jet1eta' or over=='jet1abseta':
        if changes is None:
            changes = {'alleta':True}
        else:
            changes['alleta'] = True

    if settings is None:
        settings = plotbase.createsettings(opt, changes, 'datamcratio_%s' % over)


    if figaxes == None: figaxes = plotbase.newplot()
    fig =figaxes[0]
    ax=figaxes[1]
    if labels is None:
        labels = [labelformat(t) for t in types]
    ax.axhline(1.0, color="black", linestyle='--')
    if binborders:
        for x in opt.bins:
            pass #ax.axvline(x, color='gray')

    for t, l, m, c in zip(types, labels, markers, colors):
        rgraph = getresponse(t, over, opt, settings, files[0], files[1], changes,
                                 extrapol=settings['extrapolation'], draw=False)
        if fit:
            line, err, chi2, ndf = plotbase.fitline(rgraph)
            if settings['subplot']:
                fitlabel_offset=0.65
            else:
                fitlabel_offset=0.0
            ax.text(0.97-0.55*colors.index(c), fitlabel_offset*1.2+0.10, r"R = {0:.4f} $\pm$ {1:.4f}".format(line, err),
                va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=14)
            ax.text(0.97-0.55*colors.index(c), fitlabel_offset+0.05, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f}".format(chi2, ndf),
                va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=14)
            ax.axhline(line, color=c)
            ax.axhspan(line-err, line+err, color=c, alpha=0.2)
        plot = getroot.root2histo(rgraph)
        ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)
        plotbinborders(ax, over, plot.y, opt)

    # format plot
    plotbase.labels(ax, opt, settings)
    plotbase.setaxislimits(ax, settings)

    if ratiosubplot: label = 'ratio'
    else: label = 'responseratio'

    
    if settings['subplot'] == True:
        return fig

    
    file_name = "Ratio_"+"_".join(types)+"_"+over
    plotbase.Save(fig, file_name, opt)


"""def plotkfsr(files, opt, method='balresp', label=None,
                 colors=["FireBrick", 'blue', 'green', 'orange']*7,
                 markers=['o', 'D', 's']*8,
                 over='zpt',
                 binborders=False,
                 drawextrapolation=False,
                 fit=True):
    #type: bal|mpf[ratio|seperate]
    
    fig, ax = plotbase.newplot()
    label = label or labelformat(method)

    grExt = getresponse(method, over, opt, files[0], files[1], {}, extrapol='seperate')
    grStd = getresponse(method, over, opt, files[0], files[1], {}, extrapol=False)
    grK = getroot.dividegraphs(grExt, grStd)
    if fit:
            line, err, chi2, ndf = plotbase.fitline(grK)
            ax.text(0.1, 0.27+0.07*0, r"$k_\mathrm{{FSR}} = {0:.3f} \pm {1:.3f}$ ".format(line, err),
                va='bottom', ha='left', color=colors[0], transform=ax.transAxes, fontsize=16)
            ax.text(0.1, 0.2+0.07*0, r"$\mathrm{\chi^2}$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                va='bottom', ha='left', color=colors[0], transform=ax.transAxes, fontsize=16)
            ax.axhline(line, color=colors[0])
            ax.axhspan(line-err, line+err, color=colors[0], alpha=0.2)
    plot = getroot.root2histo(grK)
    ax.axhline(1.0, color="black", linestyle='--')
    ax.errorbar(plot.x, plot.y, plot.yerr, color=colors[0], fmt=markers[0], label=label)


    # format plot
    if over == 'jet1eta':
        over_name = "jet1abseta"
    else:
        over_name=over

    plotbase.labels(ax, opt, jet=True, legloc='lower left')
    plotbase.axislabels(ax, over_name, 'kfsrratio')

    file_name = "kfsr_" + "_".join([method])
    file_name += "_"+over+"_" + opt.algorithm + opt.correction

    plotbase.Save(fig, file_name, opt)
"""

def labelformat(label):
    labeldict = {
        'bal':"$p_{\mathrm{T}}$ Balance",
        'ptbalance':"$p_{\mathrm{T}}$ Balance",
        'balresp':"$p_{\mathrm{T}}$ Balance",
        'mpfresp':"MPF",
        'mpf':"MPF"}
    return labeldict.get(label, label)

def plot_all(files, opt, plottype='response'):
    """Create a number of plots over different quantities.
       plottype can either be response or ratio
       Each plot contains several subplots for the different algorithms/ correction levels."""

    over = ['zpt', 'npv', 'jet1eta', 'alpha']
    types = ['balresp', 'mpfresp']
    subtexts = ["a)", "b)", "c)", "d)", "e)", "f)", "g)", "h)", "i)", "j)"]
    
    # create list_ac with all variation combinations from the alg/corr lists
    list_ac = plotbase.getcorralgovariations()

    for o in over:

        if o == 'alpha':
            list_ac = [ac for ac in list_ac if 'L1L2L3' in ac['correction']]

        fig_axes = plotbase.newplot(subplots=len(list_ac))

        for ch, subtext, ax in zip(list_ac, subtexts, fig_axes[1]):    # iterate over subplots in figure figaxes
            if plottype == 'ratio':
                if o == 'jet1eta': fit=False
                else: fit=True
                ratioplot(files, opt, types, drawextrapolation=True, binborders=True, over=o, subplot=True, changes=ch, fit=fit,
                    figaxes=(fig_axes[0],ax), subtext = subtext)
                strings = ["Jet response "+opt.labels[0]+"/"+opt.labels[1]+" ratio", "Ratio"]

            elif plottype == 'response':
                responseplot(files, opt, types, over=o, figaxes=(fig_axes[0],ax), subtext = subtext, subplot = True, changes=ch)
                strings = ["Jet response", "Response"]

        title = strings[0]+ " over %s for different correction levels / CHS" % plotbase.nicetext(o)
        fig_axes[0].suptitle(title, size='xx-large')

        file_name = strings[1]+"_all_"+o+"_"+"_".join(types)+"_"+opt.algorithm
        plotbase.Save(fig_axes[0], file_name, opt)
"""
# responses
def response(files, opt):
    responseplot(files, opt, ['balresp', 'mpfresp'])#, 'recogen'])
    for key in k_fsr:
        print key, k_fsr[key]
def response_npv(files, opt):
    responseplot(files, opt, ['balresp', 'mpfresp', 'recogen'], over='npv')
def response_eta(files, opt):
    responseplot(files, opt, ['balresp', 'mpfresp', 'recogen'], over='jet1eta', legloc='lower left')

def balflavours(files, opt):
    responseplot(files, opt, ['balgluon', 'baluds', 'balc', 'balb'])

def mpfflavours(files, opt):
    responseplot(files, opt, ['mpfgluon', 'mpfuds', 'mpfc', 'mpfb'])

def balflavours_eta(files, opt):
    responseplot(files, opt, ['balgluon', 'baluds', 'balc', 'balb'], over='jet1eta')

def mpfflavours_eta(files, opt):
    responseplot(files, opt, ['mpfgluon', 'mpfuds', 'mpfc', 'mpfb'], over='jet1eta')

def bal_eta(files, opt):
    responseplot(files, opt, ['balresp'], over='jet1eta', legloc='lower left')
def bal_npv(files, opt):
    responseplot(files, opt, ['balresp'], over='npv')
def bal_zpt(files, opt):
    responseplot(files, opt, ['balresp'], over='zpt')

def mpf_eta(files, opt):
    responseplot(files, opt, ['mpfresp'], over='jet1eta', legloc='lower left')


def mpf_zpt(files, opt):
    responseplot(files, opt, ['mpfresp'], over='zpt', legloc='lower left')




# ratios
def balratio(files, opt):
    ratioplot(files, opt, ['balresp', 'balratio', 'balseparate'], drawextrapolation=True, binborders=True)

def mpfratio(files, opt):
    ratioplot(files, opt, ['mpfresp', 'mpfratio', 'mpfseperate'])


def ratio(files, opt):
    ratioplot(files, opt, ['balresp', 'mpfresp'], drawextrapolation=True, binborders=True)

def ratio_npv(files, opt):
    ratioplot(files, opt, ['balresp', 'mpfresp'], drawextrapolation=True, binborders=True, over='npv')

def ratio_eta(files, opt):
    ratioplot(files, opt, ['balresp', 'mpfresp'], drawextrapolation=True, binborders=True, over='jet1eta', fit=False)

#kfsr
def kfsr(files, opt):
    plotkfsr(files, opt)

def kfsr_eta(files, opt):
    plotkfsr(files, opt, over='jet1eta')


# subplots
def ratio_all(files, opt):
    plot_all(files, opt, plottype='ratio')

def response_all(files, opt):
    plot_all(files, opt)



# response + ratio
def bal_responseratio_eta(files, opt, extrapol=None):
    responseratio(files, opt, over='jet1eta', fit=False, types=['balresp'], extrapol=extrapol)

#def bal_responseratio_zpt(files, opt, extrapol=None):
#    responseratio(files, opt, types=['balresp'], fit=True)

def bal_responseratio_npv(files, opt, extrapol=None):
    responseratio(files, opt, over='npv', fit=True, types=['balresp'], extrapol=extrapol)

def mpf_responseratio_eta(files, opt, extrapol=None):
    responseratio(files, opt, over='jet1eta', fit=False, types=['mpfresp'], extrapol=extrapol)

def mpf_responseratio_zpt(files, opt, extrapol=None):
    responseratio(files, opt, fit=True, types=['mpfresp'], extrapol=extrapol)
def mpfraw_responseratio_zpt(files, opt, extrapol=None):
    responseratio(files, opt, fit=True, types=['mpf-rawresp'], extrapol=extrapol)

def mpf_responseratio_npv(files, opt, extrapol=None):
    responseratio(files, opt, over='npv', fit=True, types=['mpfresp'], extrapol=extrapol)
"""
def respbal(files, opt):
    for zpt in getroot.binstrings(opt.bins):
        changes = {'bin':zpt}
        responseplot(files, opt, ['balresp'], over='npv', changes=changes)


def respmpf(files, opt):
    for zpt in getroot.binstrings(opt.bins):
        changes = {'bin':zpt}
        responseplot(files, opt, ['mpfresp'], over='npv', changes=changes)


def responseratio(files, opt, over='zpt', types=['balresp'], fit=False):

    settings = plotbase.createsettings(opt, None, 'response_%s' % over)

    changes = {}
    changes['subplot'] = True

    fig = plotbase.plt.figure(figsize=[7, 7])
    fig.suptitle(opt.title, size='xx-large')
    ax1 = plotbase.plt.subplot2grid((3,1),(0,0), rowspan=2)
    ax2 = plotbase.plt.subplot2grid((3,1),(2,0))
    fig.add_axes(ax1)
    fig.add_axes(ax2)

    #if over== 'jet1eta' and types == ['balresp']: changes['legloc'] = 'upper right'
    #else: changes['legloc'] = 'lower right'

    responseplot(files, opt, types, over, changes=changes, figaxes=(fig,ax1))

    changes['energy']=None
    changes['lumi']=None

    ratioplot(files, opt, types, changes=changes, over=over, figaxes=(fig,ax2))

    fig.subplots_adjust(hspace=0.05)
    ax1.set_xticks([])
    ax1.set_xlabel("")

    plotbase.setaxislimits(ax1, settings, ax2)

    extrapolation_dict = {None:'_', 'bin':'_bin-extrapol_', 'global':'_global-extrapol_'}

    if settings['filename'] == None:
        settings['filename'] = "responseratio_"+"_".join(types)+"_"+over+"_"+opt.algorithm+opt.correction
    plotbase.Save(fig, settings['filename'], opt)



def responseratio_all(files, opt, types=['balresp'], changes=None):
    
    fig = plotbase.plt.figure(figsize=[21, 14])
    fig.suptitle(opt.title, size='xx-large')
    for typ, row in zip(['balresp', 'mpfresp'], [0,4]):
        for over, col, fit in zip(['zpt', 'npv', 'jet1eta'], [0,1,2], [True, True, True]):

            ax1 = plotbase.plt.subplot2grid((7,3),(row,col), rowspan=2)
            ax2 = plotbase.plt.subplot2grid((7,3),(row+2,col))
            fig.add_axes(ax1)
            fig.add_axes(ax2)

            responseplot(files, opt, [typ], over=over, figaxes=(fig,ax1), changes={'subplot':True})
            ratioplot(files, opt, [typ], binborders=True, fit=fit, over=over, subplot=True, figaxes=(fig,ax2), ratiosubplot = True)

            fig.subplots_adjust(hspace=0.05)
            ax1.set_xticks([])
            ax1.set_xlabel("")
            ax2.set_yticks([1.05, 1.00, 0.95])
            if col > 0:
                ax1.set_ylabel("")
                ax2.set_ylabel("")

            plotbase.setaxislimits(ax1, ax2, changes, opt)

    title="                               Jet Response ($p_T$ balance / MPF) vs. Z $p_T$, $N_{vtx}$ ,  Jet $\eta$   ("  +opt.algorithm+" "+opt.correction+")"
    fig.suptitle(title, size='x-large')

    file_name = "responseratio_ALL_"+opt.algorithm+opt.correction
    plotbase.Save(fig, file_name, opt)


def extrapol_raw(files, opt):
    extrapol(files, opt, use_rawMET=True)

def extrapol_zpt(files, opt):
    extrapol(files, opt, variation='zpt')
def extrapol_zpt_raw(files, opt):
    extrapol(files, opt, variation='zpt', use_rawMET=True)


def extrapol_npv(files, opt):
    extrapol(files, opt, variation='npv')
def extrapol_npv_raw(files, opt):
    extrapol(files, opt, variation='npv', use_rawMET=True)


def extrapol_eta(files, opt):
    extrapol(files, opt, variation='jet1eta')

def extrapolation_noMPF(files, opt):
    extrapol(files, opt, extrapolate_mpf = False)


def extrapol(files, opt, 
           variation='alpha',
           use_rawMET=False,       # use raw MET instead of type-I MET
           extrapolate_mpf=True): # if false, use average for MET

    settings = plotbase.createsettings(opt, None, "response_%s" % variation)
    local_opt=copy.deepcopy(opt)


    if use_rawMET==True:
        mpftype='mpf-raw'
        mpflabel='MPF(noTypeI)'
    else:
        mpftype='mpf'
        mpflabel='MPF'


    if variation=='alpha':
        variations = getroot.alphacuts(opt.cut)
        variation_label="alpha"
    else:
        pass


    l = len(variations)
    fig, axes = plotbase.newplot(subplots=2*l, subplots_Y=2)
    subtexts = plotbase.getdefaultsubtexts()

    for cut, ax1, ax2, subtext1, subtext2 in zip(variations, axes[:l], axes[l:], subtexts[:l], subtexts[l:]):
        

        files = [getroot.openfile(f, opt.verbose) for f in opt.files]

        if settings['save_individually']:
            fig, ax1 = plotbase.newplot()


        #Response
        #   balance
        local_opt.colors = ['black', 'blue']
        local_opt.style = ['o', '*']
        local_opt.labels = [r'$p_T$ balance (data)', r'$p_T$ balance (MC)']

        changes = {'legloc'     : 'lower left',
                    'xynames'   : ['alpha','response'],
                    'fit'       : True,
                    'rebin'     : 10,
                    'selection' : cut,
                    'subplot'   : True,
                    'allalpha'  : True}

        plotbase.plotdatamc.datamcplot('ptbalance_alpha', files, local_opt,
                changes=changes, fig_axes=(fig, ax1))

        #   mpf
        local_opt.colors = ['red', 'maroon']
        local_opt.labels = [mpflabel+' (data)',mpflabel+' (MC)']
        if extrapolate_mpf ==True:
            plotbase.plotdatamc.datamcplot(mpftype+'_alpha', files, local_opt,
                changes=changes, fig_axes=(fig, ax1))

        else:
            mpfmean_data = getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMean()
            mpfmean_mc = getroot.getobjectfromnick('mpfresp', files[1], changes, rebin=1).GetMean()
            mpfmeanerror_data = getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMeanError()
            mpfmeanerror_mc = getroot.getobjectfromnick('mpfresp', files[1], changes, rebin=1).GetMeanError()
            ax1.axhline(mpfmean_data, color=local_opt.colors[0])
            ax1.axhspan(mpfmean_data + mpfmeanerror_data, mpfmean_data - mpfmeanerror_data, color=local_opt.colors[0], alpha=0.2)
            ax1.axhline(mpfmean_mc, color=local_opt.colors[1])
            ax1.axhspan(mpfmean_mc + mpfmeanerror_mc, mpfmean_mc - mpfmeanerror_mc, color=local_opt.colors[1], alpha=0.2)

        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}>30$ GeV   $\eta^\mathrm{Jet1}<1.3$"
        ax1.text(0.97, 0.98, pt_eta_label, va='top', ha='right', color='black', transform=ax1.transAxes, size='large')

        if settings['save_individually']:
            file_name = plotbase.getdefaultfilename("extrapolation", opt, settings)
            file_name = file_name.replace('var_CutSecondLeadingToZPt__','')
            plotbase.Save(fig, file_name, opt)
            fig, ax2 = plotbase.newplot()

        # re-open files because we're using the same histograms again
        files = [getroot.openfile(f, opt.verbose) for f in opt.files]

        #Ratio
        #   balance
        changes['ratio'] = True
        changes['xynames'] = ['alpha','datamcratio']
        changes['fit'] ='intercept'
        changes['legloc'] ='lower left'

        local_opt.labels = [r'$p_T$ balance']
        local_opt.colors = ['blue']
        plotbase.plotdatamc.datamcplot('ptbalance_alpha', files, local_opt,
                changes=changes, fig_axes=(fig, ax2))

        changes['fitlabel_offset'] = -0.1

        #   mpf
        local_opt.labels = [mpflabel]
        local_opt.colors = ['red']
        if extrapolate_mpf ==True:
            settings['fit_offset'] = 0.1
            plotbase.plotdatamc.datamcplot(mpftype+'_alpha', files, local_opt,
                changes=changes, fig_axes=(fig, ax2))
        else:
            if (mpfmean_mc != 0.0): R = mpfmean_data/mpfmean_mc
            else: R =0
            if (R != 0.0):
                Rerr=abs(mpfmean_data / mpfmean_mc)*math.sqrt((mpfmeanerror_data / mpfmean_data)**2 + (mpfmeanerror_mc / mpfmean_mc)**2)
            else: Rerr=0
            ax2.axhline(R, color=local_opt.colors[0])
            ax2.axhspan(R+Rerr, R-Rerr, color=local_opt.colors[0], alpha=0.2)
            ax2.text(0.97, 0.67, r" Ratio $=%1.4f\pm%1.4f$" %(R, Rerr), va='top', ha='right', transform=ax2.transAxes, color='maroon')


        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}>30$ GeV   $\eta^\mathrm{Jet1}<1.3$"
        ax2.text(0.97, 0.95, pt_eta_label, va='center', ha='right', color='black', transform=ax2.transAxes, size='large')

        if settings['save_individually']:
            file_name = plotbase.getdefaultfilename("ratio_extrapolation", opt, settings)
            file_name = file_name.replace('var_CutSecondLeadingToZPt__','')
            plotbase.Save(fig, file_name, opt)

    if settings['save_individually']:
        return

    #del changes[variation] # delete changes so this isn't included in the file names
    if extrapolate_mpf:
        mpflabel = "extrapol" + mpflabel
    settings['filename'] = plotbase.getdefaultfilename("extrapolation_%s_%s" % (mpflabel, variation_label), opt, settings)

    plotbase.Save(fig, settings['filename'], opt)



def response_run(files, opt, changes=None, settings=None):
    changes = {'runplot': True,
                'legloc':'upper right',
                'subplot':True,
                'legloc':'upper left',
                'fit':'slope',
                'xynames':['run', 'response']}

    # if no settings are given, create:
    if settings==None:
        settings = plotbase.createsettings(opt, changes, 'response_run')

    fig, ax = plotbase.newplot(run=True)

    local_opt = copy.deepcopy(opt)
    
 
    for label, color, quantity in zip(['PtBalance (data)', 'MPF (data)'], 
                                ['blue', 'red'], ['ptbalance_run', 'mpf_run']):
        local_opt.labels = [label]
        local_opt.colors = [color]
        settings['fitlabel_offset']=-0.08*['blue', 'red'].index(color)
        plotdatamc.datamcplot(quantity, files, local_opt, fig_axes=(fig, ax), 
                                            settings=settings)
  
    settings['filename'] = plotbase.getdefaultfilename("response_run", opt, settings)

    plotbase.Save(fig, settings['filename'], opt)


plots = [
'response', 'response_npv', 'response_eta', 'bal_eta', 'mpf_eta', 'response_all',

'ratio',  'ratio_npv', 'ratio_eta', 'ratio_all',

'responseratio_all',
'bal_responseratio_eta', 'bal_responseratio_zpt', 'bal_responseratio_npv', 
'mpf_responseratio_eta', 'mpf_responseratio_zpt', 'mpf_responseratio_npv', 
'extrapol', 'extrapol_zpt', 'extrapolation_noMPF', 'extrapol_raw',
'response_run'
]

if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    import sys
    if len(sys.argv) < 3:
        print "Usage: python macros/plotfractions.py data_file.root mc_file.root"
        exit(0)
    fdata = getroot.openfile(sys.argv[1])
    fmc = getroot.openfile(sys.argv[2])
    bins = getroot.getbins(fdata, [])
    test_getvalues(bins, fdata)
#   fractions(files, opt=plotbase.options(bins=bins))
