# -*- coding: utf-8 -*-
"""Plot quantities from data and MC.

   This module is dedicated to the simple comparison of plots from data and MC.
   The plots must be already present in the closure output files.
"""
import numpy
import math
import getroot
import plotbase

def datamcplot(quantity, files, opt, legloc='center right',
               changes={}, log=False, rebin=5, file_name = "", subplot=False, subtext="", fig_axes=()):
    """Template for all data/MC comparison plots for basic quantities."""
    # read the values
    if opt.verbose:
        print quantity
    change= plotbase.getchanges(opt, changes)
    datamc=[]
    events=[]

    #create list with histograms
    if change.has_key('algorithm') and 'Gen' in change['algorithm']:
        datamc = [getroot.getplotfromnick(quantity, files[1], change, rebin)]
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
        if opt.normalize and "L1" not in quantity:
            if 'cut_' not in quantity and f.ysum()!=0:
                f.scale(datamc[0].ysum() / f.ysum())
            elif 'cut_' not in quantity:
                f.scale(opt.lumi)
        if 'L1' in quantity: s='o'
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
            if 'resp' in quantity: ax.axhline(1.0, color="black", linestyle='--')

            # add a vertical line to confirm a possible drop after run 195530
            ax.axvline(195535.0, color="black", linestyle=':')

            # fit line and display slope
            intercept, ierr, slope, serr,  chi2, ndf = getroot.fitline2(getroot.getobjectfromnick(quantity, files[0], change, rebin))
            ax.plot([190000, 200000],[intercept+190000*slope, intercept+200000*slope], color = c)
            ax.plot([190000, 200000],[intercept+ierr+190000*(slope-serr), intercept+ierr+200000*(slope-serr)], alpha=0.3, color = c)
            ax.plot([190000, 200000],[intercept-ierr+190000*(slope+serr), intercept-ierr+200000*(slope+serr)], alpha=0.3, color = c)
            ax.text(0.97, 0.97, r"$Fit\/slope = %1.2f\pm%1.2f \times 10^{-4}$" % (slope*10000, serr*10000),
               va='top', ha='right', transform=ax.transAxes, color=c,
               size='x-large')

            # fit a horizontal line and display chi^2
            #intercept, ierr, chi2, ndf = getroot.fitline(getroot.getobjectfromnick(quantity, files[0], change, rebin))
            #ax.axhline(intercept, color='blue', linestyle='--')
            #ax.axhspan(intercept+ierr, intercept-ierr, color='blue', alpha=0.2)
            #ax.text(0.97, 0.17, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
            #   va='top', ha='right', transform=ax.transAxes, color='blue',
            #   size='x-large')
            

    # Jet response plots: add vertical lines for mean and mean error to see data/MC agreement
    if quantity in ['balresp', 'mpfresp'] and 'Gen' not in change['algorithm']:
        ax.axvline(datamc[0].mean, color='black', linestyle='-')
        ax.axvspan(datamc[0].mean-datamc[0].meanerr, datamc[0].mean+datamc[0].meanerr, color='black', alpha=0.1)
        ax.text(0.97, 0.97, r"$%s = %1.3f\pm%1.3f$" % (opt.labels[0], datamc[0].mean, datamc[0].meanerr),
               va='top', ha='right', transform=ax.transAxes, color='black')
        ax.axvline(datamc[1].mean, color='blue', linestyle='-')
        ax.axvspan(datamc[1].mean-datamc[1].meanerr, datamc[1].mean+datamc[1].meanerr, color='blue', alpha=0.1)
        ax.text(0.97, 0.92, r"$%s = %1.3f\pm%1.3f$" % (opt.labels[1],datamc[1].mean,datamc[1].meanerr),
               va='top', ha='right', transform=ax.transAxes, color='blue')

        if (datamc[1].mean != 0.0): R = datamc[0].mean/datamc[1].mean
        else: R =0
        if (R != 0.0):
            Rerr=abs(datamc[0].mean / datamc[1].mean)*math.sqrt((datamc[0].meanerr / datamc[0].mean)**2 + (datamc[1].meanerr / datamc[1].mean)**2)
        else: Rerr=0
        ax.text(0.97, 0.87, r"$%s/%s = %1.3f\pm%1.3f$" %(opt.labels[0], opt.labels[1], R, Rerr),
               va='top', ha='right', transform=ax.transAxes, color='maroon')


    plotbase.labels(ax, opt, legloc=legloc, frame=True, changes=change, jet=False, sub_plot=subplot)
    if opt.eventnumberlabel is True: plotbase.eventnumberlabel(ax, opt, events)

    ax.set_ylim(top=max(d.ymax() for d in datamc) * 1.2)
    if 'cut_' in quantity:
        if '_npv' in quantity: 
            ax = plotbase.axislabels(ax, 'npv', quantity)
        elif '_zpt' in quantity:
            ax = plotbase.axislabels(ax, 'z_pt', quantity)
    elif '_run' in quantity:
        ax = plotbase.axislabels(ax, 'runs', quantity[:-4])
    else:
        ax = plotbase.axislabels(ax, quantity)


    if 'var' in change and 'Cut' in change['var'] and len(change['var']) > 35:
        change['var'] = 'var'+change['var'].split('var')[2]
    if 'bin' in change:
        file_name = change['bin'] + "_" + quantity
        ranges = change['bin'][2:].split('to')
        plotbase.binlabel(ax, 'ptz', int(ranges[0]), int(ranges[1]))
    elif 'var' in change and 'Eta' in change['var']:
        ranges = change['var'][11:].replace('_','.').split('to')
        plotbase.binlabel(ax, 'eta', float(ranges[0]), float(ranges[1]))        
    elif 'var' in change and 'Npv' in change['var']:
        ranges = change['var'][8:].split('to')
        plotbase.binlabel(ax, 'Npv', int(ranges[0]), int(ranges[1]))
    elif 'var' in change and 'Cut' in change['var']:
        ranges = change['var'][27:].replace('_','.')
        plotbase.binlabel(ax, 'alpha', float(ranges))

    if not file_name:
        if 'incut' in change and change['incut'] == 'allevents':
            file_name = quantity + "_nocuts"
        else:
            file_name = quantity

    if subtext is not 'None':
        ax.text(-0.05, 1.02, subtext, va='bottom', ha='right', transform=ax.transAxes, size='xx-large', color='black')

    # save it
    if change.has_key('bin'):file_name += "_"+change['bin']
    elif change.has_key('var'):file_name += "_"+change['var']

    if 'algorithm' in change:
        file_name += "__"+change['algorithm']
    else:
        file_name += "__"+opt.algorithm
    if 'correction' in change:
        file_name += change['correction']
    else:
        file_name += opt.correction

    if subplot is not True: plotbase.Save(fig, file_name, opt)
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

# NPV
def npv(datamc, opt):
    datamcplot('npv', datamc, opt, 'center right', rebin = 1)


def npv_nocuts(datamc, opt):
    datamcplot('npv', datamc, opt, 'center right', {'incut': 'allevents'}, rebin = 1)


def numpu(datamc, opt):
    datamcplot('numpu', datamc, opt, 'center right', rebin = 1)


def numpu_nocuts(datamc, opt):
    datamcplot('numpu', datamc, opt, 'center right', {'incut': 'allevents'}, rebin = 1)


def numputruth(datamc, opt):
    datamcplot('numputruth', datamc, opt, 'center right', rebin = 1)


def numputruth_nocuts(datamc, opt):
    datamcplot('numputruth', datamc, opt, 'center right', {'incut': 'allevents'}, rebin = 1)


# Z boson
def zpt(datamc, opt):
    datamcplot('z_pt', datamc, opt, 'center right', log=True)


def zpt_nocuts(datamc, opt):
    datamcplot('z_pt', datamc, opt, 'center right', {'incut': 'allevents'})


def zeta(datamc, opt):
    datamcplot('z_eta', datamc, opt, 'lower center')


def zeta_nocuts(datamc, opt):
    datamcplot('z_eta', datamc, opt, 'lower center', {'incut': 'allevents'})


def zphi(datamc, opt):
    datamcplot('z_phi', datamc, opt, 'lower center')


def zmass(datamc, opt):
    datamcplot('z_mass', datamc, opt, rebin=2)


def zmass_nocuts(datamc, opt):
    datamcplot('z_mass', datamc, opt, 'center right', {'incut': 'allevents'},
               rebin=2, log=True)


# Leading jet
def jetpt(datamc, opt):
    datamcplot('jet1_pt', datamc, opt, 'center right',
               log=True)


def jetpt_nocuts(datamc, opt):
    datamcplot('jet1_pt', datamc, opt, 'center right', {'incut': 'allevents'},
               log=True)


def jeteta(datamc, opt):
    datamcplot('jet1_eta', datamc, opt, 'lower center')


def jetphi(datamc, opt):
    datamcplot('jet1_phi', datamc, opt, 'lower center')


# Second leading jet
def jet2pt(datamc, opt):
    datamcplot('jet2_pt', datamc, opt)


def jet2pt_nocuts(datamc, opt):
    datamcplot('jet2_pt', datamc, opt, 'center right',
               {'incut': 'allevents'}, log=True)


def jet2eta(datamc, opt):
    datamcplot('jet2_eta', datamc, opt, 'lower center')


def jet2phi(datamc, opt):
    datamcplot('jet2_phi', datamc, opt, 'lower center')


def jet3pt(datamc, opt):
    datamcplot('jet3_pt', datamc, opt)


def jet3pt_nocuts(datamc, opt):
    datamcplot('jet3_pt', datamc, opt, 'center right',
               {'incut': 'allevents'}, log=True)


def jet3eta(datamc, opt):
    datamcplot('jet3_eta', datamc, opt, 'lower center')


def jet3phi(datamc, opt):
    datamcplot('jet3_phi', datamc, opt, 'lower center')


# cut efficiencies
def cut_all_npv(datamc, opt):
    datamcplot('cut_all_npv', datamc, opt, rebin=1)


def cut_backtoback_npv(datamc, opt):
    datamcplot('cut_backtoback_npv', datamc, opt)


def cut_jet2pt_npv(datamc, opt):
    datamcplot('cut_jet2pt_npv', datamc, opt)


def cut_muon_npv(datamc, opt):
    datamcplot('cut_muonpt_npv', datamc, opt)


def cut_zmass_npv(datamc, opt):
    datamcplot('cut_zmass_npv', datamc, opt)


def cut_backtoback_zpt(datamc, opt):
    datamcplot('cut_backtoback_zpt', datamc, opt)


def cut_jet2pt_zpt(datamc, opt):
    datamcplot('cut_jet2pt_zpt', datamc, opt)


def cut_muon_npv(datamc, opt):
    datamcplot('cut_muonpt_zpt', datamc, opt)


def cut_zmass_npv(datamc, opt):
    datamcplot('cut_zmass_zpt', datamc, opt)

#Response
def balresp(datamc, opt):
    datamcplot('balresp', datamc, opt)


def mpfresp(datamc, opt):
    datamcplot('mpfresp', datamc, opt)


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
    for y in ['z_mass', 'z_pt', 'jet1_pt']:
        plotany('npv', y, datamc, opt)


def basic_zpt(datamc, opt):
    for y in ['npv', 'z_mass', 'jet1_pt'
    ]:
        plotany('z_pt', y, datamc, opt)


def basic_jet1eta(datamc, opt):
    for y in ['z_pt', 'npv', 'jet1_pt'
    ]:
        plotany('jet1_eta', y, datamc, opt)




plots = [
    'npv', 'npv_nocuts',
    'zpt', 'zeta', 'zphi', 'zmass',
    'jetpt', 'jeteta', 'jetphi',
    'jet2pt',  'jet2eta', 'jet2phi', 'jet2pt_nocuts',
    'jet3pt',  'jet3eta', 'jet3phi', 'jet3pt_nocuts',
    'cut_all_npv',
    'balresp', 'mpfresp',
    'basic_npv', 'basic_zpt', 'basic_jet1eta',
    ]

genplots = [
    'jetpt', 'jeteta', 'jetphi',
    'jet2pt',  'jet2eta', 'jet2phi',
    'jet2pt_nocuts',
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
