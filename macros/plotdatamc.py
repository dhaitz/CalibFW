# -*- coding: utf-8 -*-
"""Plot quantities from data and MC.

   This module is dedicated to the simple comparison of plots from data and MC.
   The plots must be already present in the closure output files.
"""
import numpy
import math
import getroot
import plotbase
import ROOT
import sys
import multiprocessing as mp
import os

def datamcplot(quantity, files, opt, legloc='center right',
               changes={}, log=False, rebin=5, file_name = "", subplot=False, subtext="", fig_axes=(), xy_names=None, normalize=True,           
               runplot_diff=False):
    """Template for all data/MC comparison plots for basic quantities."""
    # read the values
    if opt.verbose:
        print quantity
    change= plotbase.getchanges(opt, changes)
    datamc=[]
    events=[]
    if 'npv' in quantity or 'area' in quantity: rebin=1

    #create list with histograms
    if change.has_key('algorithm') and 'Gen' in change['algorithm']:
        datamc = [getroot.getplotfromnick(quantity, files[1], change, rebin)]
    elif '_run' in quantity:
        plot = getroot.getplotfromnick(quantity, files[0], change, rebin)
        #remove empty elements:
        for x_elem, y_elem, yerr_elem in zip(plot.xc, plot.y, plot.yerr):
            if y_elem == 0.0:
                plot.xc.remove(x_elem)
                plot.y.remove(y_elem)
                plot.yerr.remove(yerr_elem)
        if len(plot.y)==0: return

        if runplot_diff:
            mc_mean = getroot.getplotfromnick(quantity[:-4], files[1], change, rebin=1).mean
            plot.y = [y_data - mc_mean for y_data in plot.y]
            datamc.append(plot)
        else:
            datamc.append(plot)
            datamc.append([ getroot.getplotfromnick(quantity[:-4], files[1], change, rebin=1).mean, 
                                getroot.getplotfromnick(quantity[:-4], files[1], change, rebin=1).meanerr ])
    else: 
        datamc = [getroot.getplotfromnick(quantity, f, change, rebin) for f in files]
    if quantity in ['numpu', 'numputruth']:
        datamc[0] = getPUindata(quantity)

    # create the plot
    if subplot==True: fig, ax = fig_axes
    else: fig, ax = plotbase.newplot()

    #loop over histograms: scale and plot 
    for f, l, c, s in reversed(zip(datamc, opt.labels, opt.colors, opt.style)):
        events.insert(0,f.ysum())
        if opt.normalize and normalize and quantity and "L1" not in quantity and "run" not in quantity and len(quantity.split("_")) < 2:
            if 'cut_' not in quantity and f.ysum()!=0:
                f.scale(datamc[0].ysum() / f.ysum())
            elif 'cut_' not in quantity and opt.lumi !=None:
                f.scale(opt.lumi)

        if 'L1' in quantity or 'L2' in quantity or 'L3' in quantity: 
            s='o'

        #remove empty elements for run plots:
        if 'run' in quantity:
            for x_elem, y_elem, yerr_elem in zip(f.xc, f.y, f.yerr):
                if y_elem == 0.0:
                    f.xc.remove(x_elem)
                    f.y.remove(y_elem)
                    f.yerr.remove(yerr_elem)
            if len(f.y)==0: return

        if change.has_key('algorithm') and 'GenJets' in change['algorithm']:
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=opt.colors[1], fmt='-', capsize=0 ,label=opt.labels[1])
            ax.bar(f.x, f.y, (f.x[2] - f.x[1]), bottom=numpy.ones(len(f.x)) * 1e-6, fill=True, facecolor=opt.colors[1], edgecolor=opt.colors[1])
        elif s=='f':
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c, fmt='-', capsize=0 ,label=l)
            ax.bar(f.x, f.y, (f.x[2] - f.x[1]), bottom=numpy.ones(len(f.x)) * 1e-6, fill=True, facecolor=c, edgecolor=c)
        else:
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c, fmt=s, capsize=0 ,label=l)


        # add fits for time dependence plots
        if 'run' in quantity:
            # add a horizontal line at unity for jet response plots
            #if 'resp' or 'twojet' in quantity: ax.axhline(1.0, color="black", linestyle='--')

            intercept, ierr, slope, serr,  chi2, ndf = getroot.fitline2(getroot.getobjectfromnick(quantity, files[datamc.index(f)], change, rebin))
            # fit line and display slope
            if runplot_diff:
                line_fit = ax.plot([190000, 206000],[intercept-mc_mean+190000*slope, intercept-mc_mean+205000*slope], color = c, linestyle='--')
                ax.plot([190000, 206000],[intercept-mc_mean+ierr+190000*(slope-serr), intercept-mc_mean+ierr+206000*(slope-serr)], alpha=0.2, color = c, linestyle='--')
                ax.plot([190000, 206000],[intercept-mc_mean-ierr+190000*(slope+serr), intercept-mc_mean-ierr+206000*(slope+serr)], alpha=0.2, color = c, linestyle='--')
                
                
            else:                
                line_fit = ax.plot([190000, 206000],[intercept+190000*slope, intercept+205000*slope], color = c, linestyle='--')
                ax.plot([190000, 206000],[intercept+ierr+190000*(slope-serr), intercept+ierr+206000*(slope-serr)], alpha=0.2, color = c, linestyle='--')
                ax.plot([190000, 206000],[intercept-ierr+190000*(slope+serr), intercept-ierr+206000*(slope+serr)], alpha=0.2, color = c, linestyle='--')
                #ax.text(0.97, 0.95-(datamc.index(f)/10.), r"$\mathrm{Fit\/slope} = (%1.2f\pm%1.2f) \times 10^{-6}$" % (slope*1000000, serr*1000000),
                #   va='top', ha='right', transform=ax.transAxes, color=c,
                #   size='x-large')

                # fit a horizontal line and display chi^2
                """intercept, ierr, chi2, ndf = getroot.fitline(getroot.getobjectfromnick(quantity, files[datamc.index(f)], change, rebin))
                ax.axhline(intercept, color=c, linestyle='--')
                ax.axhspan(intercept+ierr, intercept-ierr, color=c, alpha=0.2)
                ax.text(0.97, 0.17+(datamc.index(f)/10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                   va='top', ha='right', transform=ax.transAxes, color=c, size='x-large')"""
    if "run" in quantity and not runplot_diff:
        mc_mean = getroot.getplotfromnick(quantity[:-4], files[1], change, rebin=1).mean
        mc_meanerr = getroot.getplotfromnick(quantity[:-4], files[1], change, rebin=1).meanerr
        line_mc = ax.axhline(mc_mean, color=opt.colors[0])
        ax.axhspan(mc_mean+mc_meanerr,mc_mean-mc_meanerr, color=opt.colors[0], alpha=0.2)

    # Jet response plots: add vertical lines for mean and mean error to see data/MC agreement
    if quantity in ['balresp', 'mpfresp', 'mpfresp-notypeI', 'baltwojet', 'response', 'METpt'] and 'Gen' not in change['algorithm']:
        if quantity == 'METpt': unit = r' \/ / \/ GeV'
        else: unit = ""
        ax.axvline(datamc[0].mean, color='black', linestyle='-')
        ax.axvspan(datamc[0].mean-datamc[0].meanerr, datamc[0].mean+datamc[0].meanerr, color='black', alpha=0.1)
        ax.text(0.97, 0.97, r"$\langle \mathrm{%s} \rangle = %1.3f\pm%1.3f" % (opt.labels[0], datamc[0].mean, datamc[0].meanerr)+"\mathrm{%s}$" % unit,
               va='top', ha='right', transform=ax.transAxes, color='black')
        ax.axvline(datamc[1].mean, color='blue', linestyle='-')
        ax.axvspan(datamc[1].mean-datamc[1].meanerr, datamc[1].mean+datamc[1].meanerr, color='blue', alpha=0.1)
        ax.text(0.97, 0.92, r"$\langle \mathrm{%s} \rangle = %1.3f\pm%1.3f" % (opt.labels[1],datamc[1].mean,datamc[1].meanerr)+" \mathrm{%s}$" % unit,
               va='top', ha='right', transform=ax.transAxes, color='blue')

        if (datamc[1].mean != 0.0): R = datamc[0].mean/datamc[1].mean
        else: R =0
        if (R != 0.0):
            Rerr=abs(datamc[0].mean / datamc[1].mean)*math.sqrt((datamc[0].meanerr / datamc[0].mean)**2 + (datamc[1].meanerr / datamc[1].mean)**2)
        else: Rerr=0
        ax.text(0.97, 0.87, r"$ \langle \mathrm{%s} \rangle / \langle \mathrm{%s} \rangle = %1.3f\pm%1.3f$" %(opt.labels[0], opt.labels[1], R, Rerr),
               va='top', ha='right', transform=ax.transAxes, color='maroon')


    plotbase.labels(ax, opt, legloc=legloc, frame=True, changes=change, jet=False, sub_plot=subplot)
    if 'run' in quantity and 'fraction' in quantity and not runplot_diff:
        legend1 = ax.legend(loc='lower right', numpoints=1, frameon=True)
        legend2 = ax.legend([line_fit, line_mc], ["data fit", "MC"], loc='upper right')
        plotbase.plt.gca().add_artist(legend1)

    if opt.eventnumberlabel is True: plotbase.eventnumberlabel(ax, opt, events)

    if xy_names is not None:
        plotbase.axislabels(ax, xy_names[0], xy_names[1])
    else:
        xy = quantity.split("_")
        y = ""
        if len(xy) == 1:
            ax = plotbase.axislabels(ax, quantity)
            ax.set_ylim(top=max(d.ymax() for d in datamc) * 1.2)
            y = 'events'
        elif len(xy) == 2:
            x = xy[1]
            y = xy[0]
            ax = plotbase.axislabels(ax, x, y)
            if (y in ['balresp', 'mpfresp', 'ptbalance', 'L1', 'L2', 'L3', 'mpf', 'mpfresp']) or 'cut' in y:
                ax.axhline(1.0, color='black', linestyle='--')

    if subtext is not 'None':
        ax.text(-0.03, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

    # save it

    if not file_name:
        file_name = plotbase.getdefaultfilename(quantity, opt, changes)

    if subplot is not True: 
        if y is 'events' or y is "":
            plotbase.Save(fig, file_name, opt)
        else:
            plotbase.EnsurePathExists(opt.out+"/"+y)
            plotbase.Save(fig, y+"/"+file_name, opt)
            
    if log:
        ax.set_ylim(bottom=1.0, top=max(d.ymax() for d in datamc) * 2)
        ax.set_yscale('log')
        if subplot is not True: plotbase.Save(fig, file_name + '_log', opt)


def getPUindata(version=''):
    """Estimated pile-up distributions in data

       This function returns the pile-up distributions officially estimated in
       data, i.e. the estimated distribution of npv and of the poisson mean
       (pile-up truth distribution).
    """
    result = getroot.Histo()

    if version == 'numpu':
        result.y = [14270649.239810746, 58990748.352223635, 142150149.40766457, 245256691.15595514, 338436389.97566742, 397985695.68997073, 416213978.05408591, 399465500.10306287, 360392674.57897019, 311175771.31574732, 260361488.5309844, 212678650.06446162, 170173269.53691545, 133433048.3509787, 102403443.7904025, 76780777.957231909, 56143916.895207874, 39980742.414969571, 27700580.582106292, 18664152.486487065, 12228440.899056926, 7792471.5964409728, 4831946.5122446436, 2917355.3677396402, 1716360.6078244832, 984800.02188280306, 551568.04700530577, 301831.7033449692, 161529.35198793598, 84617.549058685574, 43429.244216440435, 21856.99816211635, 10795.351639796547, 5236.5632377837173, 4645.721078383227]
    elif version == 'numputruth':
        result.y = [0.0, 279840.46209498082, 5774196.5336854951, 50638611.730361067, 268771849.74966705, 515883259.2782445, 555096689.70183992, 469327380.55458814, 379750064.11310208, 330774376.95963043, 269479045.19981587, 223542407.91147482, 188292685.89233157, 146804692.82040361, 94443749.898273259, 46031697.288334906, 16923096.857849758, 5181606.4255731292, 1428052.4246575104, 437008.14233305631, 102694.0511659878, 6516.1959370700497]
    else:
        print "There are no PU distributions for", version, "defined in plotdatamc."
        exit(0)
    result.yerr = [math.sqrt(i) for i in result.y] 
    result.x = range(len(result))
    result.xc = result.x
    result.normalize()
    assert len(result) == len(result.yerr)
    assert len(result) > 10
    return result


#Some additional submodules ...

def plotany(x, y, datamc, opt, changes={}, save=True, fig_axes=(), sub_plot=False):
    if fig_axes == ():
        fig_axes = plotbase.newplot()
    fig = fig_axes[0]
    ax = fig_axes[1]
    plot = getroot.getgraph(x, y, datamc[0], opt, change=changes, root=False)
    ax.errorbar(plot.x, plot.y, plot.yerr,
        color=opt.colors[0], fmt='o', capsize=0, label=opt.labels[0])
    plot = getroot.getgraph(x, y, datamc[1], opt, change=changes, root=False)
    ax.errorbar(plot.x, plot.y, plot.yerr,
        color='FireBrick', fmt='s', capsize=0, label=opt.labels[1])
    plotbase.labels(ax, opt, jet=True, changes=changes, sub_plot=sub_plot)
    if x == 'jet1_eta':
        pre = "abs_"
    else:
        pre = ""
    
    plotbase.axislabels(ax, pre+x, y)
    if sub_plot==True:
        prefix="chs_"
    else:
        prefix=""
    if save: plotbase.Save(fig,prefix+"_".join(['plot', x, y, opt.algorithm]) + opt.correction, opt)

# Plot the L1 correction factors. Plot contains two subplots for correction with/without CHS
def L1(quantity, datamc, opt, rebin=5):
    fig_axes=plotbase.newplot(subplots=2)
    for alg, a in zip(plotbase.getalgorithms(opt.algorithm), fig_axes[1]):
        datamcplot(quantity, datamc, opt, 'upper center', changes={'algorithm':alg, 'correction':''}, fig_axes=(fig_axes[0],a),
                    rebin=rebin, subplot=True, subtext="")
    fig_axes[1][1].text(0.03, 0.97, r"CHS applied", va='top', ha='left', transform=fig_axes[1][1].transAxes, color='red')
    filename = quantity+"__"+opt.algorithm
    plotbase.Save(fig_axes[0], filename, opt)

# Plot GenJets
def genjets(datamc, opt):
    for quantity in ['jet1pt', 'jet1eta', 'jet1phi', 'jet2pt',  'jet2eta', 'jet2phi']:
        datamcplot(quantity, datamc, opt, changes={'algorithm': plotbase.getgenname(opt), 'correction':''})
        datamcplot(quantity, datamc, opt, changes={'algorithm': plotbase.getgenname(opt), 'correction':'', 'incut': 'allevents'})


def datamc_all(quantity, datamc, opt, rebin=5, log=False, run=False, legloc='center right'):
    """Plot subplots of one quantity in bins of different variation.
       Loop over the different variations and the different alpha cut values.

       plotbase.getvariationlist gets a list of 'changes' dictionaries, one for each variation bin: 
         ch_list = [{'var': 'var_JetEta_0to0_783'}, {'var': 'var_JetEta_0_783to1_305'}, ...]

       fig_axes = plotbase.newplot(subplots=n) creates a plot figure (fig_axes[0]) with a list fig_axes[1]
         of n 'axes' elements (=subplots), where fig_axes[1][n] is the n-th subplot
    """
    if 'run' in quantity: 
        variations = ['eta', 'z_pt']
        datamc = [d for d, name in zip(datamc, opt.files)]# if "data" in name] # Use only data file for run plots! Disable to compare several data files
        print datamc
    else:
        variations = ['npv', 'jet1eta', 'zpt', 'alpha']
        #variations = ['eta']

    subtexts = ["a)", "b)", "c)", "d)", "e)", "f)", "g)", "h)", "i)", "j)"]

    if quantity in variations: variations.remove(quantity)
    for variation in variations:
        for cut, cut_string in zip(opt.cut, getroot.cutstrings(opt.cut)):
        #for cut, cut_string in zip([0.2], ["var_CutSecondLeadingToZPt__0_1"]):
            ch_list = plotbase.getvariationlist(variation, opt)
            fig_axes = plotbase.newplot(subplots=len(ch_list), run=run)
            for ch, ax, subtext in zip(ch_list, fig_axes[1], subtexts):
                if variation == 'zpt':
                    ch['var'] = cut_string
                elif variation is not 'alpha':
                    ch['var'] = cut_string+"_"+ch['var']

                datamcplot(quantity, datamc, opt, changes=ch,fig_axes=(fig_axes[0],ax),subplot=True, log=log, subtext=subtext, rebin=rebin, legloc=legloc)

            if variation == 'alpha': text = " for different "+plotbase.nicetext(variation)+" values "
            else: text = " in "+plotbase.nicetext(variation)+" bins for "+r"$\alpha$ "+str(cut)+"  "
            title=plotbase.nicetext(quantity)+text+opt.algorithm+" "+opt.correction
            fig_axes[0].suptitle(title, size='x-large')

            if variation == 'alpha': text = "_bins__"
            else: text = "_bins__alpha_"+str(cut).replace('.','_')+"__"
            filename = quantity+"/"+quantity+"_in_"+variation+text+opt.algorithm+opt.correction
            plotbase.EnsurePathExists(opt.out+"/"+quantity)
            plotbase.Save(fig_axes[0], filename, opt)
            if variation == 'alpha': break

#Response
def balresp_bins(datamc, opt):
    for bin in getroot.binstrings(opt.bins):
        datamcplot('balresp', datamc, opt, 'upper right', {'bin': bin})

def balresp_fornpv(datamc, opt):
    for npv in getroot.npvstrings(opt.npv):
        datamcplot('balresp', datamc, opt, 'upper right', {'var': npv}, file_name=npv + "_balresp")

def mpfresp_bins(datamc, opt):
    for bin in getroot.binstrings(opt.bins):
        datamcplot('mpfresp', datamc, opt, 'upper right', {'bin': bin})


def plotany(x, y, datamc, opt):
    fig, ax = plotbase.newplot() #ratio=True?
    plot = getroot.getgraph(x, y, datamc[0], opt, root=False)
    ax.errorbar(plot.x, plot.y, plot.yerr,
        color=opt.colors[0], fmt='o', capsize=0, label=opt.labels[0])
    plot = getroot.getgraph(x, y, datamc[1], opt, root=False)
    ax.errorbar(plot.x, plot.y, plot.yerr,
        color='FireBrick', fmt='s', capsize=0, label=opt.labels[1])

    plotbase.labels(ax, opt, jet=True)
    if x == 'jet1_eta':
        pre = "abs_"
    else:
        pre = ""
    plotbase.axislabels(ax, pre+x, y)
    plotbase.Save(fig, "_".join(['plot', x, y, opt.algorithm]) + opt.correction, opt)

#plotanys
def basic_npv(datamc, opt):
    for y in ['zmass', 'zpt', 'jet1pt']:
        plotany('npv', y, datamc, opt)

def basic_zpt(datamc, opt):
    for y in ['npv', 'zmass']:
        plotany('zpt', y, datamc, opt)

def basic_jet1eta(datamc, opt):
    for y in ['zpt', 'npv', 'jet1pt']:
        plotany('jet1eta', y, datamc, opt)

def basic_alpha(datamc, opt):
    for y in ['jet1pt', 'zpt', 'npv']:
        plotany('alpha', y, datamc, opt)

def ploteverything(datamc, opt):
    # idea: get a list with the available, plots, and try to plot them (1)from the dictionary (2)from a function (3)directly
    plotlist = getroot.getplotlist(datamc, algorithm=opt.algorithm)

    #get a list of the plots which are NOT in 'allevents'
    blacklist = list(set(plotlist)-set(getroot.getplotlist(datamc, "NoBinning_allevents", algorithm=opt.algorithm)))
    print "Blacklist: ", blacklist

    for plotname in plotlist:
        if plotname in plotdictionary:                #(1) check if plot in the dictionary
           print "plot from dictionary ",  plotname
           p = mp.Process(plotfromdict(datamc, opt, plotname, blacklist))
           p.start()
           p.join()
        elif plotname in dir(sys.modules[__name__]):  #(2) if not in dictionary, call function of the same name
           print "call function", 
           p = mp.Process(eval(plotname)(datamc, opt))
           p.start()
           p.join()
        else:                                         #(3) if no function available, plot directly
           print "not in dict or function ",
           p = mp.Process(datamcplot(plotname, datamc, opt))
           p.start()
           p.join()
           if plotname not in blacklist:  
               p = mp.Process(datamcplot(plotname, datamc, opt, 'center right', {'incut':'allevents'}))
               p.start()
               p.join()

    #get allplots!
    plotlist = getroot.getplotlist(datamc, 'all', algorithm=opt.algorithm)
    for plotname in plotlist:
        p = mp.Process(datamc_all(plotname, datamc, opt))
        p.start()
        p.join()




def plotfromdict(datamc, opt, name, blacklist=[]):
    if len(plotdictionary[name]) == 0: #emptylist
        datamcplot(name, datamc, opt)

    elif len(plotdictionary[name]) == 1: #list contains only arguments
        eval("datamcplot('"+name+"', datamc, opt, "+plotdictionary[name][0]+")")

    elif len(plotdictionary[name]) == 2: #list contains arguments+function
        eval(plotdictionary[name][1]+"('"+name+"', datamc, opt, "+plotdictionary[name][0]+")")

    elif len(plotdictionary[name]) == 3: #list contains arguments+function+name
        eval(plotdictionary[name][1]+"('"+plotdictionary[name][2]+"', datamc, opt, "+plotdictionary[name][0]+")")


plotdictionary={ 
       # plot:[arguments, function, name]
        'npv':['rebin=1'],
        'npv_nocuts':['rebin=1, changes={"incut":"allevents"}', 'datamcplot', 'npv'],
        'jetpt_zeta':['rebin=5, legloc="upper left"'],
        'jetpt_zeta_nocuts':['changes={"incut":"allevents"}'],

        # eta vs. phi
        'jeteta_jetphi':['rebin=2'],
        'jet2eta_jet2phi': ['rebin=2'],
        #phi vs. eta
        'jeteta_jetphi':['rebin=2'],

        'jet2eta_jet2phi':['rebin=2'],
        'METeta':['legloc="lower center"'],
        #muons
        'mupluspt':['legloc="center right"'],
        'mupluseta':['legloc="lower center"'],
        'muplusphi':['legloc="lower center"'],
        'muminusphi':['legloc="lower center"'],
        'muonsvalid':['legloc="lower center", rebin=1'],
        'muonsinvalid':['legloc="lower center", rebin=1'],

        #correction factors
        'L1_npv':['rebin=1','L1'],
        'L1abs_npv':['rebin=1','L1'],

        'L2_npv':['rebin=1','L1'],
        'L2abs_npv':['rebin=1','L1'],

        'L1L2L3_npv':['rebin=1','L1'],
        'L1L2L3abs_npv':['rebin=1','L1'],

        'sumEt_npv':['rebin=1'],
        'mpf_npv':['rebin=1'],
        'jetpt_npv':['rebin=1'],

        'ptbalance_deltaeta-jet1-jet2':[],
        'tworesp':['legloc="lower right"', datamcplot, 'bal_twojet'],
        'bal_twojet':['legloc="lower right"'],
        'balresp_all':["", 'datamc_all', 'balresp'],
        'mpfresp_all':["", 'datamc_all', 'mpfresp'],
        'METpt_all':["", 'datamc_all', 'METpt'],
        'METphi_all':["", 'datamc_all', 'METphi'],
        'zpt_all':["", 'datamc_all', 'zpt'],
        'deltaphi-leadingjet-z_all':["", 'datamc_all', 'deltaphi-leadingjet-z'],
        'deltaphi-leadingjet-MET_all':["", 'datamc_all', 'deltaphi-leadingjet-MET'],
        'deltaphi-z-MET_all':["legloc='lower left'", 'datamc_all', 'deltaphi-z-MET'],

        'mpf_deltaphi-z-MET_all':["", 'datamc_all', 'mpf_deltaphi-z-MET'],
        'mpf_deltaphi-jet1-MET_all':["", 'datamc_all', 'mpf_deltaphi-jet1-MET'],

        #MET
        'METpt':["legloc='center right', log=True"],
        'METphi':["'lower center'"],
        'METsumEt':["'center right', rebin=10"],
        'METfraction':["'center right', rebin=2, log=True"],
        'METpt_METphi':["'lower right'"],

        'zpt':["legloc='center right', log=True"],
        'zmass':["rebin=2, log=True"],

        'jet1pt':["log=True"],

        'jet2pt':["log=True, rebin=2"],
        'jet3pt':["log=True, rebin=2"],

        'ptbalance_run':["rebin=500, legloc='lower left', run=True"],
        'ptbalance_run_all':["rebin=500, run=True ,legloc='lower left'", 'datamc_all', 'ptbalance_run' ],
        'mpfresp_run':["rebin=500, run=True"],
        'jetpt_run':["rebin=500, run=True"],
        'zpt_run':["rebin=500"],
        'sumEt_run':["rebin=500, run=True"],
        'METpt_run':["rebin=500, run=True"],
        'jetsvalid_run':["rebin=500, run=True", "datamc_all"],
        #'':[""],


        } 

plots = [
    'ploteverything',
    #'basic_npv', 'basic_zpt', 'basic_jet1eta', 'basic_alpha',
    #'genjets', 

    'METpt_all', 'balresp_all', 'METphi_all', 'mpfresp_all', 'zpt_all',
    'deltaphi-leadingjet-z_all', 'deltaphi-leadingjet-MET_all', 'deltaphi-leadingjet-MET_all',
    ]

if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    import sys
    if len(sys.argv) < 2:
        print "Usage: python macros/plotdatamc.py data_file.root mc_file.root"
        exit(0)
    datamc[0] = getroot.openfile(sys.argv[1])
    fmc = getroot.openfile(sys.argv[2])
    bins = getroot.getbins(datamc[0], [])
    zpt(datamc, opt=plotbase.options(bins=bins))
    jeteta(datamc, opt=plotbase.options(bins=bins))
    cut_all_npv(datamc, opt=plotbase.options(bins=bins))
