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
               changes={}, log=False, xlog=False, rebin=5, file_name = "", subplot=False, 
               subtext="", fig_axes=(), xy_names=None, normalize=True, runplot_diff=False, fit=None, ratio=False, fit_offset=0):
    """Template for all data/MC comparison plots for basic quantities."""
    # read the values
    if opt.verbose:
        print quantity
    change= plotbase.getchanges(opt, changes)
    if opt.rebin is not None: rebin = opt.rebin
    if opt.ratio is not False: ratio = opt.ratio

    datamc=[]
    events=[]

    #create list with histograms
    if change.has_key('algorithm') and 'Gen' in change['algorithm']:
        datamc = [getroot.getplotfromnick(quantity, files[1], change, rebin)]
    else:
        datamc = [getroot.getplotfromnick(quantity, f, change, rebin) for f in files]
    if quantity in ['numpu', 'numputruth']:
        datamc[0] = getPUindata(quantity)

    # For now this function only works with TProfiles
    if ratio and len(datamc) == 2:
        rootobjects = [getroot.getobjectfromnick(quantity, f, change, rebin) for f in files]
        #convert TProfiles into TH1Ds because root cant correctly divide TProfiles
        rootobject = ROOT.TH1D(rootobjects[0].ProjectionX())
        rootobject_2 = ROOT.TH1D(rootobjects[1].ProjectionX())
        rootobject.Divide(rootobject_2);
        datamc = [getroot.root2histo(rootobject, files[0].GetName(), rebin=1)]
    else:
        rootobject = None


    # create the plot
    if subplot==True: fig, ax = fig_axes
    else: fig, ax = plotbase.newplot()

    #loop over histograms: scale and plot 
    for f, l, c, s, rootfile in reversed(zip(datamc, opt.labels, opt.colors, opt.style, files)):
        events.insert(0,f.ysum())
        if opt.normalize and normalize and quantity and "L1" not in quantity and "run" not in quantity and len(quantity.split("_")) < 2:
            if 'cut_' not in quantity and f.ysum()!=0:
                f.scale(datamc[0].ysum() / f.ysum())
            elif 'cut_' not in quantity and opt.lumi !=None:
                f.scale(opt.lumi)

        if 'L1' in quantity or 'L2' in quantity or 'L3' in quantity: 
            s='o'

        if change.has_key('algorithm') and 'GenJets' in change['algorithm']:
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=opt.colors[1], fmt='-', capsize=0 ,label=opt.labels[1])
            ax.bar(f.x, f.y, (f.x[2] - f.x[1]), bottom=numpy.ones(len(f.x)) * 1e-6, fill=True, facecolor=opt.colors[1], edgecolor=opt.colors[1])
        elif s=='f':
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c, fmt='-', capsize=0 ,label=l)
            ax.bar(f.x, f.y, (f.x[2] - f.x[1]), bottom=numpy.ones(len(f.x)) * 1e-6, fill=True, facecolor=c, edgecolor=c)
        else:
            ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c, fmt=s, capsize=0 ,label=l)

        if fit is not None: plotbase.fit(fit, ax, quantity, rootfile, change, 1, c, datamc.index(f), rootobject=rootobject, offset=fit_offset)


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
                ax.axhline(1.0, color='black', linestyle=':')
    if opt.x_limits is not None: ax.set_xlim(opt.x_limits[0], opt.x_limits[1])
    if opt.y_limits is not None: ax.set_ylim(opt.y_limits[0], opt.y_limits[1])

    if subtext is not 'None':
        ax.text(-0.03, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

    # save it
    if not file_name:
        file_name = plotbase.getdefaultfilename(quantity, opt, changes)

    if subplot is not True: 
        if not (y is 'events' or y is ""):
            plotbase.EnsurePathExists(opt.out+"/"+y)
            file_name = y+"/"+file_name
        plotbase.Save(fig, file_name, opt)
            
    if log:
        ax.set_ylim(bottom=1.0, top=max(d.ymax() for d in datamc) * 2)
        ax.set_yscale('log')
        if subplot is not True: plotbase.Save(fig, file_name + '_log', opt)

    if xlog:
        ax.set_xscale('log')
        if subplot is not True: plotbase.Save(fig, file_name + '_xlog', opt)



def runplot(quantity, files, opt, legloc='center right',
               changes={}, log=False, rebin=500, file_name = "", subplot=False, subtext="", fig_axes=(), xy_names=None, normalize=True,           
               fractions=False, runplot_diff=False, fit='slope'):

    change= plotbase.getchanges(opt, changes)
    datamc=[]
    events=[]
    run_min = 190000
    run_max = 206000

    # create the plot
    if subplot==True: fig, ax = fig_axes
    else: fig, ax = plotbase.newplot(run=True)

    plotlist = getroot.getplotlist(None, algorithm=opt.algorithm, filenames=opt.files)

    for f, l, c, s in reversed(zip(files, opt.labels, opt.colors, opt.style)):
        if 'data' in l or fractions:
            plot = getroot.getplotfromnick(quantity, f, change, rebin)
            #remove empty elements:
            for x_elem, y_elem, yerr_elem in zip(plot.xc, plot.y, plot.yerr):
                if y_elem == 0.0:
                    plot.xc.remove(x_elem)
                    plot.y.remove(y_elem)
                    plot.yerr.remove(yerr_elem)
            if len(plot.y)==0: return

            if runplot_diff:
                mc_mean = getroot.getplotfromnick(quantity[:-4], files[1], change, rebin=1).mean
                plot.y = [y - mc_mean for y in plot.y]

            plotbase.fit(fit, ax, quantity, f, changes, rebin, c, files.index(f), runplot_diff, mc_mean, run_min, run_max)
            
            ax.errorbar(plot.xc, plot.y, plot.yerr, drawstyle='steps-mid', color=c, fmt=s, capsize=0 ,label=l)

        elif ('MC' in l  or fractions) and quantity[:-4] in plotlist:
            if runplot_diff:
                mc_mean = getroot.getplotfromnick(quantity[:-4], f, change, rebin=1).mean
                plot.y = [y_data - mc_mean for y_data in plot.y]
                datamc.append(plot)
            else:
                mc_mean = getroot.getplotfromnick(quantity[:-4], f, change, rebin=1).mean
                mc_meanerr = getroot.getplotfromnick(quantity[:-4], f, change, rebin=1).meanerr
                ax.errorbar(0.5*(run_min+ run_max), mc_mean, mc_meanerr, drawstyle='steps-mid', color=c, fmt='-', capsize=0 ,label=l)
                ax.bar(run_min, mc_mean, (run_max - run_min), bottom=0., fill=True, facecolor=c, edgecolor=c)
                ax.axhspan(mc_mean+mc_meanerr,mc_mean-mc_meanerr, color=c, alpha=0.2)
        
    run_2012B = 193834.
    run_2012C = 197770.
    ax.axvline(run_2012B, color='gray', linestyle='--', alpha=0.2)
    ax.text((run_2012B - run_min)/(run_max-run_min),  0.98, "2012B",
                     va='top', ha='left', transform=ax.transAxes, color='gray', alpha=0.5, size='medium')
    ax.axvline(run_2012C, color='gray', linestyle='--', alpha=0.2)
    ax.text((run_2012C - run_min)/(run_max-run_min),  0.98, "2012C", 
                    va='top', ha='left', transform=ax.transAxes, color='gray', alpha=0.5, size='medium')

    plotbase.labels(ax, opt, legloc=legloc, frame=True, changes=change, jet=False, sub_plot=subplot)

    if xy_names is not None:
        plotbase.axislabels(ax, xy_names[0], xy_names[1])
    else:
        y, x = quantity.split("_")
        ax = plotbase.axislabels(ax, x, y)

    if subtext is not 'None':
        ax.text(-0.03, 1.01, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

    # get file name
    if not file_name:
        file_name = plotbase.getdefaultfilename(quantity, opt, changes)
    # save
    if subplot is not True: 
        plotbase.Save(fig, file_name, opt)


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


def comparison(datamc, opt):
    """file_names = [
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root'
                ]"""
    colors=['red', 'blue', 'blue', 'red']
    markers=['*', 'o', 'o', '*']
    #labels = [['MC_52xFast', 'data_52xFast'], ['MC_52xOff', 'data_52xOff'], ['MC_53xFast', 'data_53xFast'], ['MC_53xOff', 'data_53xOff']]
    rebin = 1
    import copy
    file_names = [
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                ]
    labels = [['MC_52xFast', 'data_52xFast'], ['MC_53xFast', 'data_53xFast'], ['MC_52xOff', 'data_52xOff' ], ['MC_53xOff', 'data_53xOff']]


    files=[]
    for f in file_names:
        files += [getroot.openfile(f, opt.verbose)]
    local_opt = copy.deepcopy(opt)
    local_opt.style = markers
    local_opt.colors = colors
    quantity = 'L1abs_npv'

    # ALL
    fig, axes= plotbase.newplot(subplots=4)
    for a, f1, f2, l in zip(axes, files[::2], files[1::2], labels):
        local_opt.labels = l
        datamcplot(quantity, (f1, f2), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,a),
                    rebin=rebin, subplot=True, subtext="")

    filename = "L1_all__"+opt.algorithm
    plotbase.Save(fig, filename, opt)
    """

    #Fastjet vs Offset
    fig = plotbase.plt.figure(figsize=(14,7))
    axes = [fig.add_subplot(1,2,n) for n in [1,2]]

    local_opt.labels = labels[0]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[0], files[1]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")
    local_opt.labels = labels[1]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[2], files[3]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")
        #53
    local_opt.labels = labels[2]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[4], files[5]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")
    local_opt.labels = labels[3]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[6], files[7]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")

    filename = "L1_Fastjet_vs_Offset__"+opt.algorithm
    plotbase.Save(fig, filename, opt)


    #52X vs 53X
    fig = plotbase.plt.figure(figsize=(14,7))
    axes = [fig.add_subplot(1,2,n) for n in [1,2]]

    local_opt.labels = labels[0]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[0], files[1]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labels[2]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[4], files[5]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labels[1]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[2], files[3]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")
        #
    local_opt.labels = labels[3]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[6], files[7]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")

    filename = "L1_52X_vs_53X__"+opt.algorithm
    plotbase.Save(fig, filename, opt)

    import plotresponse


    file_names = [
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                ]
    labels = [['data_52xFast', 'MC_52xFast'], [ 'data_53xFast', 'MC_53xFast'], [ 'data_52xOff', 'MC_52xOff'], ['data_53xOff', 'MC_53xOff']]

    files=[]
    for f in file_names:
        files += [getroot.openfile(f, opt.verbose)]

    for over, fit in zip(['zpt', 'jet1eta', 'npv'], [True, False, True]):

        fig, axes= plotbase.newplot(subplots=4)
        fig2, axes2= plotbase.newplot(subplots=4)
        for a1, a2, f1, f2, l in zip(axes, axes2, files[::2], files[1::2], labels):
            local_opt.labels = l
            changes ={}# {'correction':'L1L2L3'}
            plotresponse.responseplot((f1, f2), local_opt, ['bal', 'mpf'], over=over, changes=changes, figaxes=(fig,a1),
                        subplot=True, subtext="")
            plotresponse.ratioplot((f1, f2), local_opt, ['bal', 'mpf'], over=over, changes=changes, figaxes=(fig2 ,a2), fit=fit,
                        subplot=True, subtext="")

        filename = "Response_"+over+"_all__"+opt.algorithm
        plotbase.Save(fig, filename, opt)

        filename = "Ratio_"+over+"_all__"+opt.algorithm
        plotbase.Save(fig2, filename, opt)"""




# Plot GenJets
def genjets(datamc, opt):
    for quantity in ['jet1pt', 'jet1eta', 'jet1phi', 'jet2pt',  'jet2eta', 'jet2phi']:
        datamcplot(quantity, datamc, opt, changes={'algorithm': plotbase.getgenname(opt), 'correction':''})
        datamcplot(quantity, datamc, opt, changes={'algorithm': plotbase.getgenname(opt), 'correction':'', 'incut': 'allevents'})


def datamc_all(quantity, datamc, opt, rebin=5, log=False, run=False, legloc='center right', fit=None):
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

                datamcplot(quantity, datamc, opt, changes=ch,fig_axes=(fig_axes[0],ax),subplot=True, log=log, subtext=subtext, rebin=rebin, legloc=legloc, fit=fit)

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



def plot_YvsAll(datamc, opt, y='zpt'):
    plotlist = getroot.getplotlist(datamc, algorithm=opt.algorithm, filenames=opt.files)
    plotlist = [x for x in plotlist if x.startswith(y+"_")]

    fig, axes = plotbase.newplot(subplots=len(plotlist))
    for ax, plot in zip(axes, plotlist):
        datamcplot(plot, datamc, opt, subplot=True, fig_axes=(fig, ax))

    plotbase.Save(fig, "%s_allX__%s%s" % (y, opt.algorithm, opt.correction), opt)


def plot_AllvsX(datamc, opt, x='zpt'):

    plotlist = getroot.getplotlist(datamc, algorithm=opt.algorithm, filenames=opt.files)
    plotlist = [y for y in plotlist if y.endswith("_"+x)]
    fig, axes = plotbase.newplot(subplots=len(plotlist))

    for ax, plot in zip(axes, plotlist):
        datamcplot(plot, datamc, opt, subplot=True, fig_axes=(fig, ax))

    plotbase.Save(fig, "AllY_%s__%s%s" % (x, opt.algorithm, opt.correction), opt)


plotdictionary={ 
       # plot:[arguments, function, name]'L1L2L3_npv':['rebin=1', 'L1'],
    'L1L2L3abs_npv':['rebin=1', 'L1'],
    'L1_npv':['rebin=1', 'L1'],
    'L1abs_npv':['rebin=1', 'L1'],
    'L2_npv':['rebin=1', 'L1'],
    'L2abs_npv':['rebin=1', 'L1'],
    'METeta':['legloc="lower center"'],
    'METfraction':["'center right', rebin=2, log=True"],
    'METphi':["'lower center'"],
    'METphi_all':['', 'datamc_all', 'METphi'],
    'METpt':["legloc='center right', log=True"],
    'METpt_METphi':["'lower right'"],
    'METpt_all':['', 'datamc_all', 'METpt'],
    'METsumEt':["'center right', rebin=10"],
    'bal_twojet':['legloc="lower right"'],
    'balresp_all':['', 'datamc_all', 'balresp'],
    'deltaphi-leadingjet-MET_all':['', 'datamc_all', 'deltaphi-leadingjet-MET'],
    'deltaphi-leadingjet-z_all':['', 'datamc_all', 'deltaphi-leadingjet-z'],
    'deltaphi-z-MET_all':["legloc='lower left'", 'datamc_all', 'deltaphi-z-MET'],
    'jet1pt':['log=True'],
    'jet2eta_jet2phi':['rebin=2'],
    'jet2pt':['log=True, rebin=2'],
    'jet3pt':['log=True, rebin=2'],
    'jeteta_jetphi':['rebin=2'],
    'jetpt_zeta':['rebin=5, legloc="upper left"'],
    'mpf-diff_alpha':['rebin=2, changes={"var":"var_CutSecondLeadingToZPt_0_3"}'],
    'mpf_deltaphi-jet1-MET_all':['', 'datamc_all', 'mpf_deltaphi-jet1-MET'],
    'mpf_deltaphi-z-MET_all':['', 'datamc_all', 'mpf_deltaphi-z-MET'],
    'mpfresp_all':['', 'datamc_all', 'mpfresp'],
    'mpf_alpha':['rebin=4, fit="chi2"'],
    'mpf_alpha03':['rebin=2, fit="intercept", changes={"var":"var_CutSecondLeadingToZPt_0_3"}', 'datamcplot', 'mpf_alpha'],
    'mpf_alpha_all':['rebin=4, fit="chi2"', 'datamc_all', 'mpf_alpha'],
    'muminusphi':['legloc="lower center"'],
    'muonsinvalid':['legloc="lower center", rebin=1'],
    'muonsvalid':['legloc="lower center", rebin=1'],
    'mupluseta':['legloc="lower center"'],
    'muplusphi':['legloc="lower center"'],
    'mupluspt':['legloc="center right"'],
    'npv':['rebin=1'],
    'npv_nocuts':['rebin=1, changes={"incut":"allevents"}', 'datamcplot', 'npv'],
    'ptbalance_alpha':['rebin=2, fit="intercept", legloc="lower center"'],
    'ptbalance_alpha_ratio':['rebin=2, fit="chi2",legloc="lower center", ratio=True', 'datamcplot', 'ptbalance_alpha'],
    'ptbalance_alpha_alpha04':['rebin=2, changes={"var":"var_CutSecondLeadingToZPt_0_4"}, fit="intercept", legloc="lower center"', 'datamcplot', 'ptbalance_alpha'],
    'ptbalance_alpha_alpha03':['rebin=2, changes={"var":"var_CutSecondLeadingToZPt_0_3"}, fit="intercept", legloc="lower center"', 'datamcplot', 'ptbalance_alpha'],
    'ptbalance_alpha_all':['rebin=4, fit="intercept"', 'datamc_all', 'ptbalance_alpha'],
    'ptbalance_jetsvalid':['rebin=1, legloc="lower center"'],
    'tworesp':['legloc="lower right"', 'datamcplot', 'bal_twojet'],
    'zmass':['rebin=2, log=True'],
    'zmass_zcutsonly':['rebin=2, log=True, changes={"incut":"zcutsonly"}', 'datamcplot', 'zmass'],
    'zmass_npv':["legloc='lower center', fit='chi2'"],
    'zmass_npv_zcutsonly':['legloc="lower center", fit="chi2", changes={"incut":"zcutsonly"}', 'datamcplot', 'zmass_npv'],
    'zmass_zpt':["legloc='lower center', fit='chi2'"],
    'zmass_zpt_zcutsonly':["legloc='lower center', changes={'incut':'zcutsonly'}, fit='chi2'", 'datamcplot', 'zmass_zpt'],
    'zpt':["legloc='center right', log=True"],
    'zpt_all':['', 'datamc_all', 'zpt'],
    'zpt_npv':["legloc='lower center', fit='chi2'"],
    'zpt_npv_zcutsonly':["legloc='lower center', changes={'incut':'zcutsonly'}, fit='chi2'", 'datamcplot', 'zpt_npv'],
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
