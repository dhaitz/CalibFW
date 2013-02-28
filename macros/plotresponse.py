# -*- coding: utf-8 -*-
"""Extrapolate and plot the response and ratio

   Function hierachy:
   * Plotting (values from getresponse)
      plotratio
      plotresponse

   * Getting the graph:
      getresponse
        uses getroot.getgraph
        and  extrapolatebin
                uses fillgraph
                and  fitextrapolation
                and  draw_extrapolation


"""
import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile
import ROOT

import plotbase
import getroot

from responsetools import *


def responseplot(files, opt, types, labels=None,
                 colors=['blue', 'maroon', 'green', 'red']*7,
                 markers=['o', 'x', 'x']*8,
                 over='zpt',
                 binborders=False,
                 figaxes = None,
                 drawextrapolation=False,
                 legloc='lower left',
                 changes = None,
                 subtext = "",
                 subplot = False,
                 extrapol=None,
                 residual_trick = None):
    """type: bal|mpf[:ratio,seperate,data,mc,ex]
    """
    if figaxes == None: figaxes = plotbase.newplot()
    fig =figaxes[0]
    ax=figaxes[1]
    if labels is None:
        labels = types

    if changes is None: changes = plotbase.createchanges(opt)
    else: changes = plotbase.getchanges(opt, changes)

    labels = [string.replace("mpfresp", "MPF").replace("balresp","$p_{\mathrm{T}}$ balance") for string in labels] 
    for t, l, m, c in zip(types, labels, markers, colors):
        extrapolation = False
        #if t == 'recogen':
        #    pass#t = 'RecoToGen_bal'
        #elif len(t) > 3:
        #    extrapolation = t[3:]
        #    t = t[:3]
        if extrapolation in ['ex', 'data', 'mc', 'datamc']:
            extrapolation = 'data'
        if 'gen' not in t:
            plot = getroot.root2histo(getresponse(t, over, opt, files[0], None, changes, extrapol))
            if residual_trick:
                ax.errorbar(plot.x, [0.98 * y for y in plot.y], [0.98 * yerr for yerr in plot.yerr], color='black', fmt=m, label=l+" ("+opt.labels[0]+")")
                plotbinborders(ax, over, [0.98 * y for y in plot.y], opt)
            else:
                ax.errorbar(plot.x, plot.y, plot.yerr, color='black', fmt=m, label=l+" ("+opt.labels[0]+")")
                plotbinborders(ax, over, plot.y, opt)
        if extrapolation == 'data':
            extrapolation = 'mc'

        if (len(files)>1 and len(types)<2):
            for f, label, color, mark in zip(files[1:], opt.labels[1:], colors, markers[1:]):
                plot = getroot.root2histo(getresponse(t, over, opt, f, None, changes, extrapol))
                ax.errorbar(plot.x, plot.y, plot.yerr, color=color, fmt=mark, label=l+" ("+label+")")
        else:
            plot = getroot.root2histo(getresponse(t, over, opt, files[1], None, changes, extrapol))
            if l == 'recogen': l = "Reco/Gen"
            else: l = l+" ("+opt.labels[1]+")"
            ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)
            plotbinborders(ax, over, plot.y, opt)

    # format plot
    ax.axhline(1.0, color="black", linestyle='--')
    plotbase.labels(ax, opt, legloc=legloc, jet=True, sub_plot=subplot, changes=changes)
    if over == 'jet1eta':
        plotbase.axislabels(ax, 'jet1abseta', 'response', labels=opt.labels)
    else:
        plotbase.axislabels(ax, over, 'response', labels=opt.labels)
    if subtext is not 'None':
        ax.text(-0.04, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')
    
    """#label with pt and eta cut
    if over == 'jet1eta':
        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}$>30 GeV"
    else:
        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}$>30 GeV    $\eta^\mathrm{Jet1}$<1.3"
    ax.text(0.02, 0.98, pt_eta_label, va='top', ha='left', color='black', transform=ax.transAxes, size='small')"""

    if subplot == True:
        return fig

    if 'algorithm' in changes:
        a = changes['algorithm']
    else:
        a = opt.algorithm
    if 'correction' in changes:
        c = changes['correction']
    else:
        c = opt.correction

    if subplot == True: prefix ="All_" 
    else: prefix=""

    if c == opt.correction and a == opt.algorithm:
        file_name = prefix+"Response_"+"_".join(types)+"_"+over
        if subplot is not True: file_name += "_" + a + c
        plotbase.Save(fig, file_name, opt)


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
                 tworatios=False,
                 legendlabel=None,
                 residual_trick=False):
    """type: bal|mpf[ratio|seperate]
    """
    if changes is None: changes = plotbase.createchanges(opt)
    else: changes = plotbase.getchanges(opt, changes)

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
        rgraph = getresponse(t, over, opt, files[0], files[1], changes, extrapol=extrapol, draw=False)
        if fit:
            line, err, chi2, ndf = getroot.fitline(rgraph)
            if ratiosubplot:
                fitlabel_offset=0.65
            else:
                fitlabel_offset=0.0
            if tworatios:
                x_offset = 0.55
            else:
                x_offset = 0.

            ax.text(0.97-x_offset, fitlabel_offset+0.20+0.07*colors.index(c), r"R = {0:.4f} $\pm$ {1:.4f}".format(line, err),
                va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=14)
            ax.text(0.97-x_offset, fitlabel_offset+0.05+0.07*colors.index(c), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f}".format(chi2, ndf),
                va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=14)
            ax.axhline(line, color=c)
            ax.axhspan(line-err, line+err, color=c, alpha=0.2)
        if residual_trick:
            for i in range(rgraph.GetN()):
                x, y, dx, dy = getroot.getgraphpoint(rgraph, i)
                rgraph.SetPoint(i, x, y*0.98)
                rgraph.SetPointError(i, dx, dy*0.98)

        if legendlabel is not None:
            l = legendlabel
        plot = getroot.root2histo(rgraph)
        ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)
        plotbinborders(ax, over, plot.y, opt)

    # format plot
    if tworatios:
        ncol=2
    else:
        ncol=1
    plotbase.labels(ax, opt, jet=True, legloc=legloc, sub_plot=subplot, changes=changes, ratiosubplot=ratiosubplot, ncol=ncol)
    if ratiosubplot and not tworatios: label = 'ratio'
    else: label = 'responseratio'

    if over == 'jet1eta':
        plotbase.axislabels(ax, 'jet1abseta', label, labels=opt.labels)
    else:
        plotbase.axislabels(ax, over, label, labels=opt.labels)

    if subtext is not 'None':
        ax.text(-0.04, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

    
    if subplot == True:
        return fig

    if 'algorithm' in changes:
        a = changes['algorithm']
    else:
        a = opt.algorithm
    if 'correction' in changes:
        c = changes['correction']
    else:
        c = opt.correction

    if subplot == True: prefix ="All_" 
    else: prefix=""
    
    if c == opt.correction and a == opt.algorithm:
        file_name = prefix+"Ratio_"+"_".join(types)+"_"+over
        if subplot is not True: file_name += "_" + a + c
        plotbase.Save(fig, file_name, opt)


def plotkfsr(files, opt, method='balresp', label=None,
                 colors=["FireBrick", 'blue', 'green', 'orange']*7,
                 markers=['o', 'D', 's']*8,
                 over='zpt',
                 binborders=False,
                 drawextrapolation=False,
                 fit=True):
    """type: bal|mpf[ratio|seperate]
    """
    fig, ax = plotbase.newplot()
    label = label or labelformat(method)

    grExt = getresponse(method, over, opt, files[0], files[1], {}, extrapol='seperate')
    grStd = getresponse(method, over, opt, files[0], files[1], {}, extrapol=False)
    grK = getroot.dividegraphs(grExt, grStd)
    if fit:
            line, err, chi2, ndf = getroot.fitline(grK)
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


def labelformat(label):
    if 'balresp' in label:
        result = "$p_{\mathrm{T}}$ balance"
    elif 'mpfresp' in label:
        result = "MPF"
    elif 'mpf-rawresp' in label:
        result = "MPF-raw"
    if 'ratio' in label:
        result += " (ratio extrapol.)"
    elif 'seperate' in label:
        result += " (extrapolated)"
    return result

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

# responses
def response(files, opt):
    responseplot(files, opt, ['balresp', 'mpfresp', 'recogen'])
    for key in k_fsr:
        print key, k_fsr[key]
def response_npv(files, opt):
    responseplot(files, opt, ['balresp', 'mpfresp', 'recogen'], over='npv')
def response_eta(files, opt):
    responseplot(files, opt, ['balresp', 'mpfresp', 'recogen'], over='jet1eta', legloc='lower left')

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
def bal_responseratio_eta(files, opt, extrapol='bin'):
    responseratio(files, opt, over='jet1eta', fit=False, types=['balresp'], extrapol=extrapol)

def bal_responseratio_zpt(files, opt, extrapol=None):
    responseratio(files, opt, fit=True, types=['balresp'], extrapol=extrapol)

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



def responseratio(files, opt, over='zpt', fit=False, types=['balresp'],
        extrapol=None, changes=None, residual_trick=False):

    fig = plotbase.plt.figure(figsize=[7, 7])
    fig.suptitle(opt.title, size='xx-large')
    ax1 = plotbase.plt.subplot2grid((3,1),(0,0), rowspan=2)
    ax2 = plotbase.plt.subplot2grid((3,1),(2,0))
    fig.add_axes(ax1)
    fig.add_axes(ax2)

    if over== 'jet1eta' and types == ['balresp']: legloc = 'upper right'
    else: legloc = 'lower right'

    responseplot(files, opt, types, over=over, figaxes=(fig,ax1), legloc=legloc, subplot = True, extrapol=extrapol, residual_trick=residual_trick)

    if len(files)> 2:
        ratioplot(files, opt, types, drawextrapolation=True, binborders=True, fit=fit, over=over, subplot=True, figaxes=(fig,ax2), ratiosubplot = True, legloc='lower right', extrapol=extrapol, colors = ['blue'], legendlabel=opt.labels[1])
        ratioplot([files[0],files[2]], opt, types, drawextrapolation=True, binborders=True, fit=fit, over=over, subplot=True, figaxes=(fig,ax2), ratiosubplot = True, legloc='lower right', extrapol=extrapol, tworatios=True, colors = [opt.colors[2]],  markers = ['*'], legendlabel=opt.labels[2])
    else:
        ratioplot(files, opt, types, drawextrapolation=True, binborders=True, fit=fit, over=over, subplot=True, figaxes=(fig,ax2), ratiosubplot = True, legloc='lower right', extrapol=extrapol, residual_trick=residual_trick)

    fig.subplots_adjust(hspace=0.05)

    ax1.set_xticks([])
    ax1.set_xlabel("")
    #ax2.set_yticks([1.02,1.01, 1.00, 0.99, 0.98])

    plotbase.setaxislimits(ax1, changes, opt, ax2)

    extrapolation_dict = {None:'_', 'bin':'_bin-extrapol_', 'global':'_global-extrapol_'}

    file_name = "responseratio_"+"_".join(types)+"_"+over+"_"+extrapolation_dict[extrapol]+opt.algorithm+opt.correction
    plotbase.Save(fig, file_name, opt)



def responseratio_all(files, opt, types=['balresp'], changes=None):
    
    fig = plotbase.plt.figure(figsize=[21, 14])
    fig.suptitle(opt.title, size='xx-large')
    for typ, row in zip(['balresp', 'mpfresp'], [0,4]):
        for over, col, fit in zip(['zpt', 'npv', 'jet1eta'], [0,1,2], [True, True, True]):

            ax1 = plotbase.plt.subplot2grid((7,3),(row,col), rowspan=2)
            ax2 = plotbase.plt.subplot2grid((7,3),(row+2,col))
            fig.add_axes(ax1)
            fig.add_axes(ax2)

            if over == 'jet1eta' and typ == 'balresp': legloc = 'upper right'
            else: legloc = 'lower left'

            responseplot(files, opt, [typ], over=over, figaxes=(fig,ax1), legloc=legloc, subplot = True)
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
           use_rawMET=False, # use raw MET instead of type-I MET
           extrapolate_mpf = True, # if false, use average for MET
           save_individually = False):  # save each plot indivually, not as a subplot

    rebin = 10
    if opt.rebin is not None: rebin = opt.rebin
    changes = {}
    local_opt = copy.deepcopy(opt)


    if use_rawMET==True:
        mpftype='mpf-raw'
        mpflabel='MPF(noTypeI)'
    else:
        mpftype='mpf'
        mpflabel='MPF'


    if variation=='zpt':
        variations = getroot.binstrings(opt.bins)
        variation_label="zpt"
        variation='bin'
        changes['var']="_var_CutSecondLeadingToZPt_0_35"
    elif variation=='alpha':
        variations = getroot.cutstrings(opt.cut)
        variation_label="alpha"
        variation='var'
    elif variation=='jet1eta':
        variations = ["var_CutSecondLeadingToZPt_0_3__"+var for var in getroot.etastrings(opt.eta)]
        variation_label="eta"
        variation='var'
    elif variation=='npv':
        variations = ["var_CutSecondLeadingToZPt_0_35__"+var for var in getroot.npvstrings(opt.npv)]
        variation_label="npv"
        variation='var'


    l = len(variations)
    fig, axes = plotbase.newplot(subplots=2*l, subplots_Y=2)
    subtexts = plotbase.getdefaultsubtexts()

    for cut, ax1, ax2, subtext1, subtext2 in zip(variations, axes[:l], axes[l:], subtexts[:l], subtexts[l:]):
        changes[variation]= cut

        files = [getroot.openfile(f, opt.verbose) for f in opt.files]

        if save_individually:
            fig, ax1 = plotbase.newplot()
            subtext1 = ""


        #Response
        #   balance
        local_opt.colors = ['black', 'blue']
        local_opt.style = ['o', '*']
        local_opt.labels = [r'$p_T$ balance (data)', r'$p_T$ balance (MC)']
        plotbase.plotdatamc.datamcplot('ptbalance_alpha', files, local_opt, legloc='upper center',
                   changes=changes, rebin=rebin, file_name = "", subplot=True,  xy_names=['alpha','response'],
                   subtext="", fig_axes=(fig, ax1),fit=True, ratio=False)

        #   mpf
        local_opt.colors = ['red', 'maroon']
        local_opt.labels = [mpflabel+' (data)',mpflabel+' (MC)']
        if extrapolate_mpf ==True:
            plotbase.plotdatamc.datamcplot(mpftype+'_alpha', files, local_opt, legloc='lower left',
                           changes=changes, rebin=rebin, file_name = "", subplot=True,  xy_names=['alpha','response'],
                           subtext=subtext1, fig_axes=(fig, ax1),fit=True, ratio=False)
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

        if save_individually:
            file_name = plotbase.getdefaultfilename("extrapolation_%s" % (variation_label), opt, changes)
            file_name = file_name.replace('var_CutSecondLeadingToZPt__','')
            plotbase.Save(fig, file_name, opt)
            fig, ax2 = plotbase.newplot()
            subtext2 = ""

        # re-open files because we're using the same histograms again
        files = [getroot.openfile(f, opt.verbose) for f in opt.files]

        #Ratio
        #   balance
        local_opt.labels = [r'$p_T$ balance']
        local_opt.colors = ['blue']
        plotbase.plotdatamc.datamcplot('ptbalance_alpha', files, local_opt, legloc='lower left',
                   changes=changes, rebin=rebin, file_name = "", subplot=True, xy_names=['alpha','datamcratio'],
                   subtext="", fig_axes=(fig, ax2),fit='intercept', ratio=True)

        #   mpf
        local_opt.labels = [mpflabel]
        local_opt.colors = ['red']
        if extrapolate_mpf ==True:
            plotbase.plotdatamc.datamcplot(mpftype+'_alpha', files, local_opt, legloc='lower left',
                           changes=changes, rebin=rebin, file_name = "", subplot=True, xy_names=['alpha','datamcratio'],
                           subtext=subtext2, fig_axes=(fig, ax2),fit='intercept', ratio=True, fit_offset=-0.1)
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

        if save_individually:
            file_name = plotbase.getdefaultfilename("ratio_extrapolation_%s" % (variation_label), opt, changes)
            file_name = file_name.replace('var_CutSecondLeadingToZPt__','')
            plotbase.Save(fig, file_name, opt)

    if save_individually:
        return

    del changes[variation] # delete changes so this isn't included in the file names
    if extrapolate_mpf:
        mpflabel = "extrapol" + mpflabel
    file_name = plotbase.getdefaultfilename("extrapolation_%s_%s" % (mpflabel, variation_label), opt, changes)
    plotbase.Save(fig, file_name, opt)



def response_run(files, opt, changes=None):
    fig, ax = plotbase.newplot(run=True)

    local_opt = copy.deepcopy(opt)
    if changes is None: changes = {}

    for label, color, quantity in zip(['PtBalance (data)', 'MPF (data)'], ['blue', 'red'], ['ptbalance_run', 'mpf_run']):
        local_opt.labels = [label]
        local_opt.colors = [color]
        plotbase.plotdatamc.runplot(quantity, files, local_opt, legloc='upper right',
                       subplot=True, fit=False, changes=changes, xy_names=['run','response'],
                       fig_axes=(fig, ax))
    ax.axhline(1.0, color='black', linestyle=':')
    if 'var' in changes:
        filename = "response_run" + "_" + changes['var'] +"_"+opt.algorithm+opt.correction
    else:
        filename = "response_run" +"_"+opt.algorithm+opt.correction
    plotbase.Save(fig, filename, opt)


plots = [
'response', 'response_npv', 'response_eta', 'bal_eta', 'mpf_eta', 'response_all',

'ratio',  'ratio_npv', 'ratio_eta', 'ratio_all',

'responseratio_all',
'bal_responseratio_eta', 'bal_responseratio_zpt', 'bal_responseratio_npv', 
'mpf_responseratio_eta', 'mpf_responseratio_zpt', 'mpf_responseratio_npv', 
'extrapol', 'extrapol_zpt', 'extrapolation_noMPF', 'extrapol_raw',
'response_run'
#,'balratio', 'mpfratio', 'kfsr'
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
