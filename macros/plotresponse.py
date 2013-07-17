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

    settings = plotbase.getsettings(opt, changes=changes, settings=settings, 
                                                  quantity="response_%s" % over)
    
    if figaxes == None: 
        figaxes = plotbase.newplot()
    fig =figaxes[0]
    ax=figaxes[1]

    labels = [labelformat(string) for string in types] 


    # iterate over types:
    for t, l, m in zip(types, labels, markers):
        # iterate over files:        
        for f, label, color in zip(files, settings['labels'], 
                                            colors[len(files)*types.index(t):]):

            plot = getroot.root2histo(getresponse(t, over, opt, settings, f, 
                            None, changes, extrapol=settings['extrapolation']))
            ax.errorbar(plot.x, plot.y, plot.yerr, color=color, fmt=m, 
                                                label='%s (%s)' % (l, label))
            plotbinborders(ax, over, plot.y, opt)

    # format plot
    ax.axhline(1.0, color="black", linestyle='--')
    plotbase.labels(ax, opt, settings)
    plotbase.axislabels(ax, over, 'response', labels=settings['labels'], settings=settings)
    plotbase.setaxislimits(ax, settings)

    #label with pt and eta cut
    if over in ['jet1eta', 'jet1abseta'] :
        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}$>30 GeV"
    else:
        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}$>30 GeV    $\eta^\mathrm{Jet1}$<1.3"#     $\alpha$ < 0.2"# or  $p_{\mathrm{T}}^{\mathrm{Jet2}}$<12 GeV"
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

    settings = plotbase.getsettings(opt, changes=changes, settings=settings, 
                                               quantity="datamcratio_%s" % over)


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
    plotbase.axislabels(ax, over, 'datamcratio', labels=settings['labels'], settings=settings)

    if ratiosubplot: label = 'ratio'
    else: label = 'responseratio'

    
    if settings['subplot'] == True:
        return fig

    
    file_name = "Ratio_"+"_".join(types)+"_"+over
    plotbase.Save(fig, file_name, opt)



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
def respbal(files, opt):
    for zpt in getroot.binstrings(opt.bins):
        changes = {'bin':zpt}
        responseplot(files, opt, ['balresp'], over='npv', changes=changes)


def respmpf(files, opt):
    for zpt in getroot.binstrings(opt.bins):
        changes = {'bin':zpt}
        responseplot(files, opt, ['mpfresp'], over='npv', changes=changes)


def responseratio(files, opt, over='zpt', types=['balresp'], fit=False, 
                                                                  changes=None):

    settings = plotbase.getsettings(opt, changes=changes, settings=None, 
                                                  quantity="response_%s" % over)

    changes = {'subplot':True,
                'legloc':'lower right'
                }

    fig = plotbase.plt.figure(figsize=[7, 7])
    fig.suptitle(opt.title, size='xx-large')
    ax1 = plotbase.plt.subplot2grid((3,1),(0,0), rowspan=2)
    #ax1.number = 1
    ax2 = plotbase.plt.subplot2grid((3,1),(2,0))
    #ax2.number = 2
    fig.add_axes(ax1)
    fig.add_axes(ax2)

    responseplot(files, opt, types, over, changes=changes, figaxes=(fig,ax1))

    changes['energy'] = None
    changes['lumi']   = None
    changes['legloc']   = "lower left"

    ratioplot(files, opt, types, changes=changes, over=over, figaxes=(fig,ax2))

    fig.subplots_adjust(hspace=0.05)
    ax1.set_xticks([])
    ax1.set_xlabel("")

    for ax in [ax1, ax2]:
        plotbase.setaxislimits(ax, settings)

    extrapolation_dict = {None:'_', 'bin':'_bin-extrapol_', 
                                                'global':'_global-extrapol_'}

    if settings['filename'] == None:
        settings['filename'] = "responseratio_%s_%s_%s%s" % ("_".join(types),
                                            over, opt.algorithm, opt.correction)
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

    settings = plotbase.getsettings(opt, None, None, "response_%s" % variation)   


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

    for cut, ax1, ax2, subtext1, subtext2 in zip(variations, axes[:l], axes[l:],
                                                    subtexts[:l], subtexts[l:]):
        

        files = [getroot.openfile(f, opt.verbose) for f in opt.files]

        if settings['save_individually']:
            fig, ax1 = plotbase.newplot()


        #Response
        #   balance

        changes = {'legloc'     : 'lower left',
                    'xynames'   : ['alpha','response'],
                    'fit'       : True,
                    'rebin'     : 10,
                    'selection' : cut + "&& alpha>0",
                    'subplot'   : True,
                    'allalpha'  : True,
                    'colors'    : ['black', 'blue'],
                    'markers'   : ['o', '*'],
                    'labels'    : [r'$p_T$ balance (data)', 
                                                       r'$p_T$ balance (MC)'],
                  }

        plotbase.plotdatamc.datamcplot('ptbalance_alpha', files, opt,
                changes=changes, fig_axes=(fig, ax1))

        #   mpf
        changes['colors'] = ['red', 'maroon']
        changes['labels'] = [mpflabel+' (data)',mpflabel+' (MC)']
        if extrapolate_mpf ==True:
            plotbase.plotdatamc.datamcplot(mpftype+'_alpha', files, opt,
                changes=changes, fig_axes=(fig, ax1))

        else:
            mpfmean_data = getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMean()
            mpfmean_mc = getroot.getobjectfromnick('mpfresp', files[1], changes, rebin=1).GetMean()
            mpfmeanerror_data = getroot.getobjectfromnick('mpfresp', files[0], changes, rebin=1).GetMeanError()
            mpfmeanerror_mc = getroot.getobjectfromnick('mpfresp', files[1], changes, rebin=1).GetMeanError()
            ax1.axhline(mpfmean_data, color=changes['colors'][0])
            ax1.axhspan(mpfmean_data + mpfmeanerror_data, mpfmean_data - mpfmeanerror_data, color=local_opt.colors[0], alpha=0.2)
            ax1.axhline(mpfmean_mc, color=changes['colors'][1])
            ax1.axhspan(mpfmean_mc + mpfmeanerror_mc, mpfmean_mc - mpfmeanerror_mc, color=local_opt.colors[1], alpha=0.2)

        pt_eta_label = r"$p_\mathrm{T}^\mathrm{Z}>30\ \mathrm{GeV}  \quad  |\eta^\mathrm{Jet1}|<1.3  \quad  \alpha<0.3$"
        ax1.text(0.97, 0.97, pt_eta_label, va='top', ha='right', color='black', transform=ax1.transAxes, size='large')

        if settings['save_individually']:
            file_name = plotbase.getdefaultfilename("extrapolation", opt, settings)
            file_name = file_name.replace('var_CutSecondLeadingToZPt__','')
            plotbase.Save(fig, file_name, opt)
            fig, ax2 = plotbase.newplot()

        # re-open files because we're using the same histograms again
        files = [getroot.openfile(f, opt.verbose) for f in opt.files]

        #Ratio
        #   balance
        changes['ratio']   = True
        changes['xynames'] = ['alpha','datamcratio']
        changes['fit']     = 'intercept'
        changes['legloc']  = 'lower left'
        changes['labels']  = [r'$p_T$ balance']
        changes['colors']  = ['blue']
        changes['fitlabel_offset'] = -0.0
        plotbase.plotdatamc.datamcplot('ptbalance_alpha', files, opt,
                changes=changes, fig_axes=(fig, ax2))

        changes['fitlabel_offset'] = -0.1

        #   mpf
        changes['labels'] = [mpflabel]
        changes['colors'] = ['red']
        if extrapolate_mpf ==True:
            settings['fit_offset'] = 0.1
            plotbase.plotdatamc.datamcplot(mpftype+'_alpha', files, opt,
                changes=changes, fig_axes=(fig, ax2))
        else:
            if (mpfmean_mc != 0.0): R = mpfmean_data/mpfmean_mc
            else: R =0
            if (R != 0.0):
                Rerr=abs(mpfmean_data / mpfmean_mc)*math.sqrt((mpfmeanerror_data / mpfmean_data)**2 + (mpfmeanerror_mc / mpfmean_mc)**2)
            else: Rerr=0
            ax2.axhline(R, color=changes['colors'][0])
            ax2.axhspan(R+Rerr, R-Rerr, color=changes['colors'][0], alpha=0.2)
            ax2.text(0.97, 0.67, r" Ratio $=%1.4f\pm%1.4f$" %(R, Rerr), va='top', ha='right', transform=ax2.transAxes, color='maroon')


        ax2.text(0.97, 0.97, pt_eta_label, va='top', ha='right', color='black', transform=ax2.transAxes, size='large')

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

    settings = plotbase.getsettings(opt, changes, settings, "response_run")

    fig, ax = plotbase.newplot(run=True)
 
    for label, color, quantity in zip(['PtBalance (data)', 'MPF (data)'], 
                                ['blue', 'red'], ['ptbalance_run', 'mpf_run']):
        changes['labels'] = [label]
        changes['colors'] = [color]
        settings['fitlabel_offset']=-0.08*['blue', 'red'].index(color)
        plotdatamc.datamcplot(quantity, files, opt, fig_axes=(fig, ax), 
                                            settings=settings)
  
    settings['filename'] = plotbase.getdefaultfilename("response_run", opt, settings)

    plotbase.Save(fig, settings['filename'], opt)


def response_algoflavour(files, opt, changes=None, settings=None):
    response_physflavour(files, opt, changes=None, settings=None, definition='algo')

def response_physflavour(files, opt, changes=None, settings=None, definition='phys'):
    """Get response vs. flavour. This function only works with MC."""
    settings = plotbase.getsettings(opt, changes, settings, "response_flavour")

    x = '%sflavour' % definition
    
    markers = ['o', 's', 'd', '*']
    colors = ['red', 'black', 'yellowgreen', 'lightskyblue', ]
    quantities = ['ptbalance', 'mpf', 'recogen', 'genbalance', ]

    plotbase.debug("neutrino-inclusive quantities are used")
    ################################################################################
    quantities = ['jet1npt/zpt', 'mpfn', 'jet1npt/matchedgenjet1pt', 'genbalance', ]
    ################################################################################

    labels = ['PtBalance', 'MPF', 'RecoJet/GenJet', 'GenJet/RecoZ']
    
    changes = {
        'legloc':'upper center',
        'xynames':[x, 'response'],
        'subplot':True,
        'lumi':0,
        'rebin':4,
            }

    fig, ax = plotbase.newplot()

    for m, c, q, l in zip(markers, colors, quantities, labels):
        changes['markers'] = [m]
        changes['colors'] = [c]
        changes['labels'] = [l]
        plotdatamc.datamcplot("%s_%s" % (q, x), files, opt, fig_axes=(fig, ax), 
                                            changes=changes, settings=settings)

    settings['filename'] = plotbase.getdefaultfilename("response_%s" % x, opt,
                                                                 settings)
    ax.set_xticks([0,1,2,3,4,5,21])
    plotbase.Save(fig, settings['filename'], opt)


def physflavour_extrapol_all(files, opt, changes=None, settings=None):
    for quantity in ['jet1npt/zpt', 'mpfn', 'jet1npt/matchedgenjet1pt', 'genbalance']:
        physflavour_extrapol(files, opt, definition='phys', responsetype = quantity)


def physflavour_extrapol(files, opt, changes=None, settings=None,
        definition='phys', responsetype = 'jet1npt/zpt'):

    flavour = "%sflavour" % definition

    changes = {'legloc':'lower center'}
    settings = plotbase.getsettings(opt, changes, settings, "response_alpha")

    markers = ['o', 's', 'd', '*']
    colors = ['red', 'black', 'yellowgreen', 'lightskyblue', ]
    qlabels = ['PtBalance', 'MPF', 'RecoJet/GenJet', 'GenJet/RecoZ']

    labels = ['u & d', 's', 'c & b', 'gluon']
    x = [1,2,3,4]
    selections = ["%s>0 && %s<3" % (flavour, flavour),
                    "%s==3" % flavour,
                    "%s>3 && %s<6" % (flavour, flavour),
                    "%s==21" % flavour,
                 ]

    axisdict = {'jet1npt/zpt': "$p_T$ balance (incl. neutrinos)",
                'mpfn': "MPF response (incl. neutrinos)",
                'jet1npt/matchedgenjet1pt' :"$p_\mathrm{T}^\mathrm{RecoJet}/p_\mathrm{T}^\mathrm{GenJet}$ (incl. neutrinos)"
                }

    namedict= {'jet1npt/zpt': "ptbalance",
                'mpfn': "mpf",
                'jet1npt/matchedgenjet1pt' :"recogen"
                }

    changes = {
        'legloc':'lower center',
        'xynames':['alpha', 'response'],
        'lumi':0,
        'y':[0.84, 1.05],
        'rebin':5,
        'fit':'intercept',
            }

    fig, ax = plotbase.newplot()

    #add neutrinos t genjets
    genneutrinos = False
    if genneutrinos:
        responses = ['jet1npt/zpt', 'mpfn', 'jet1npt/(matchedgenjet1pt+neutrinopt)', '(matchedgenjet1pt+neutrinopt)/zpt']
    else:
        responses = ['jet1npt/zpt', 'mpfn', 'jet1npt/matchedgenjet1pt', 'genbalance']

    for m, c, responsetype, l in zip(markers, colors, responses, qlabels):
        changes['markers'] = [m]
        changes['colors'] = [c]

        y = []
        yerr = []

        for s in  selections:
            if settings['extrapolation']:
                changes['selection'] = "%s && %s" % ("alpha<0.3", s)
                changes['allalpha'] = True
            
                quantity = "%s_alpha" % responsetype

                rsettings = {}
                rsettings = plotbase.getsettings(opt, changes, settings, quantity)

                rootobject = getroot.getobjectfromtree(quantity, files[0], rsettings)
                rootobject.Rebin(rsettings['rebin'])

                intercept, ierr = plotbase.fitline2(rootobject)[:2]
                y += [intercept]
                yerr += [ierr]
            else:
                changes['selection'] = "%s && %s" % ("alpha<0.3", s)
                quantity = responsetype

                rsettings = {}
                rsettings = plotbase.getsettings(opt, changes, settings, quantity)
                rsettings['x'] = [0, 2]

                rootobject = getroot.getobjectfromtree(quantity, files[0], rsettings)
                rootobject.Rebin(rsettings['rebin'])

                y += [rootobject.GetMean()]
                yerr += [rootobject.GetMeanError()]


        ax.errorbar(x, y, yerr, drawstyle='steps-mid', color=c, fmt=m, 
                          capsize=0 ,label=l)


    # set the axis labels and limits
    plotbase.labels(ax, opt, settings, settings['subplot'])
    plotbase.axislabels(ax, flavour, settings['xynames'][1], 
                                                            settings=settings)
    plotbase.setaxislimits(ax, settings)
    ax.set_xlim(0, 5)
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.axhline(1.0, color='black', linestyle=':')

    

    settings['filename'] = plotbase.getdefaultfilename("%s_alpha" % 
                    namedict.get(responsetype, responsetype), opt, settings)
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
