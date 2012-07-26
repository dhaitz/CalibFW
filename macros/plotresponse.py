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

# global variables for extrapolation factors:
k_fsr = {}


extrapolation_is_done = False


def fillgraph(method, changes, file_numerator, file_denominator=None, #extrapolateRatio=True,
        var=[0.1, 0.15, 0.2, 0.3], varstr = "var_CutSecondLeadingToZPt_%s",
        median=False):
    """Fill a TGraphErrors with the cut variation values for extrapolation

       var: variation values (list)
       if a file_denominator is given, the ratio is extrapolated
       the errors are uncorrelated in this function
    """
    print "fillgraph", method, var
    graph = TGraphErrors(len(var))
    ch = copy.deepcopy(changes)
    var = copy.deepcopy(var)
    var.reverse()

    # loop over variation values (from highest to lowest value)
    for x in var:
        # get the values
        ch['var'] = varstr % str(x).replace(".", "_") + (
                    "_" + changes['var'] if 'var' in changes else "")

        oj = getroot.getobjectfromnick(method, file_numerator, ch)
        if median:
            y = getroot.histomedian(oj)
        else:
            y = oj.GetMean()
        yerr = oj.GetMeanError()
        if y < 0.000001:
            print "No valid response in %s (cut = %1.2f)!" % (ch, x)
            continue
            plotbase.fail("No valid response in %s (cut = %1.2f)!" % (ch, x) )

        # take the ratio (including error propagation):
        if file_denominator is not None:
            oj = getroot.getobjectfromnick(method, file_denominator, ch)
            yerr =  abs(y / oj.GetMean()) * math.sqrt((yerr / y)**2 + (oj.GetMeanError() / oj.GetMean())**2)
            if median:
                y /= getroot.histomedian(oj)
            else:
                y /= oj.GetMean()
            if y < 0.000001:
                print "No valid response in %s (cut = %1.2f)!" % (ch, x)
                continue
                #plotbase.fail("No valid response in %s (cut = %1.2f)!" % (ch, x) )

        print "   %1.2f | %1.4f +- %1.4f" % (x, y, yerr),

        # uncorrelate errors
        if x == var[0]: # remember last point
            y0 = y
            x0 = x
            y0err = yerr
        else:
            if y0err < yerr:
                yerr = math.sqrt(yerr ** 2 - y0err ** 2)
            else:
                yerr = yerr
                print "=> Errors are not in order!",
        print # insert newline

        # set the point
        graph.SetPoint(var.index(x), x, y)
        graph.SetPointError(var.index(x), 0.0, yerr)

    return graph


def fitextrapolation(graph, verbose=True):
    print "Fit ..."
    # get the fixed parameters
    x0 = ROOT.Double()
    y0 = ROOT.Double()
    graph.GetPoint(0, x0, y0)
    y0err = graph.GetErrorY(0)
    # prepare the root fit-function
    rootfunc = TF1("fit1", "[0]+[1]*(x-[2])", 0, 1000.0)
    rootfunc.SetParameters(y0, 0.0, x0)
    rootfunc.FixParameter(0, y0)
    rootfunc.FixParameter(2, x0)
    # fit
    fitres = graph.Fit(rootfunc, "SQN")
    # calculate results and store them in the getroot.Fitfunction
    m_fit_err = fitres.GetErrors()[1]
    m_fit = fitres.GetParams()[1]
    yex = y0 - m_fit * x0
    yex_err = y0err + m_fit_err * x0
    function = getroot.Fitfunction([0, x0], [yex, y0], [yex_err, y0err], fitres.Chi2(), fitres.Ndf())
    # show the results
    if verbose:
        print x0, y0, y0err
        print "Result (slope): m = %1.5f +- %1.5f" % (m_fit, m_fit_err)
        print "Result (resp.): R = %1.5f +- %1.5f" % (yex, yex_err)
        print function
    return function


def draw_extrapolation(graph, fit, ptbin, opt):
    """
    """
    fig, ax = plotbase.newplot()
    ax.fill_between(fit.plotx(), fit.plotyl(), fit.plotyh(), facecolor='CornflowerBlue',
                    edgecolor='white', interpolate=True, alpha=0.3)
    ax.plot(fit.plotx(), fit.ploty(), '-', label='extrapolation')
    # Cut variation data points with uncorrelated and correlated errors
    print graph.yerr[0], fit.yerr[1]
    assert graph.yerr[0] == fit.yerr[1]
    ax.errorbar(graph.xc, graph.y, graph.yerr, color='Black', fmt='o',
                capsize=2, label='uncorrelated')
    for i in range(1, len(graph)):
        if graph.yerr[i] < graph.yerr[0]:
            graph.yerr[i] = math.sqrt(graph.yerr[i] ** 2 + graph.yerr[0] ** 2)
    ax.errorbar(graph.xc, graph.y, graph.yerr, color='FireBrick', fmt='o',
                capsize=2, label='correlated')
    # Legend and labels
    ax = plotbase.labels(ax, op, legloc='upper right')
    ax = plotbase.axislabels(ax, "jet2ratio", graph.ylabel)
    ax.text(0.04, 0.17, r"$k_\mathrm{{FSR}} = {0:.3f} \pm {1:.3f}$".format(fit.k(), fit.kerr()),
            va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
    ax.text(0.04, 0.11, r"$\chi^2 / n_\mathrm{{dof}} = {0:.3f} / {1}$".format(fit.chi2, fit.ndf),
            va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
    ax.text(0.04, 0.05, r"$R_\mathrm{{corr}} = {0:.3f} \pm {1:.3f} $".format(
                        fit.f(0), fit.ferr(0)),
            va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
    ax.text(0.96, 0.05, (r"${0} < p_\mathrm{{T}}^\mathrm{{Z}} / \mathrm{{GeV}}"
                     r" < {1}$").format(ptbin[0], ptbin[1]),
            va='bottom', ha='right', transform=ax.transAxes, fontsize=18)
    plotbase.Save(fig, graph.name, opt, False)


def extrapolatebin(method, bin, changes, opt, f1, f2=None, draw=True, source='ratio', over='z_pt'):
    """The extrapolation

       This is using fillgraph, fitextrapolation and draw_extra
       4 use cases:
       files source    => returns
       1     'data'    => response (extrapolated from data file)
       1     'mc'      => response (extrapolated from mc file)
       2     'ratio'   => ratio (first ratio, then extrapolate)
       2     'separate'=> ratio (first extrapolate, then ratio)
        TODO: for npv, eta etc.
    """
    ch = copy.deepcopy(changes)
    if over == 'z_pt':
        ch['bin'] = bin
    elif over in ['npv', 'jet1_eta']:
        ch['var'] = bin
    print "extrapolate", source, "...", ch
    if f2 is None or source == 'ratio':
        graph = fillgraph(method, ch, f1, f2, median=False)
        fit = fitextrapolation(graph)
        if draw:
            graph = getroot.root2histo(graph)
            graph.ylabel = method + "_" + source
            graph.name = "%s_%s_extrapolation_%s_%s%s" % (bin, method, source, opt.algorithm, opt.correction)
            draw_extrapolation(graph, fit, bin, opt)
        print "k(%s) =" % bin, fit.f(0) / fit.f(0.2)
        global k_fsr
        if str(changes)+method+source not in k_fsr:
            k_fsr[str(changes)+method+source] = getroot.Histo()
        k_fsr[str(changes)+method+source].append(10, True, fit.f(0) / fit.f(0.2), fit.ferr(0) / fit.f(0.2) + fit.ferr(0.2) * fit.f(0) / fit.f(0.2) / fit.f(0.2) )
        return fit.f(0), fit.ferr(0)
    else: # 'seperate'
        graphd = fillgraph(method, ch, f1, median=False)
        graphm = fillgraph(method, ch, f2, median=False)
        fitd = fitextrapolation(graphd)
        fitm = fitextrapolation(graphm)
        if draw:
            graphd = getroot.root2histo(graphd)
            graphm = getroot.root2histo(graphm)
            graphd.ylabel = method + "_data"
            graphm.ylabel = method + "_mc"
            graphd.name = "%s_%s_extrapolation_%s_%s%s" % (bin, method, "data", opt.algorithm, opt.correction)
            graphm.name = "%s_%s_extrapolation_%s_%s%s" % (bin, method, "mc", opt.algorithm, opt.correction)
            draw_extrapolation(graphd, fitd, bin, opt)
            draw_extrapolation(graphm, fitm, bin, opt)
        global ktest
        if str(changes)+method+source not in k_fsr:
            k_fsr[str(changes)+method+source] = getroot.Histo()
        k_fsr[str(changes)+method+source].append(10, True, fitd.f(0) / fitm.f(0) / (fitd.f(0.2) / fitm.f(0.2)))
        print "k(%s) =" % bin, fitd.f(0) / fitm.f(0) / (fitd.f(0.2) / fitm.f(0.2))
        return fitd.f(0) / fitm.f(0), fitd.ferr(0) / fitm.f(0) + fitm.ferr(0) * fitd.f(0) / fitm.f(0) / fitm.f(0)


def getresponse(method, over, opt, f1, f2=None, changes={}, extrapol=False, draw=False):
    """
       If 2 files are given the response ratio is returned!
       This is using fillgraph, fitextrapolation and draw_extra
       6 use cases:
       files extrapol  => returns
       1     False     => response (without extrapolation)
       1     'data'    => response (extrapolated from data file)
       1     'mc'      => response (extrapolated from mc file)
       2     False     => ratio (without extrapolation)
       2     'ratio'   => ratio (first ratio, then extrapolate)
       2     'separate'=> ratio (first extrapolate, then ratio)

       TODO: for npv, eta etc.
    """
    assert method in ['balresp', 'mpfresp', 'RecoToGen_balresp']
    assert type(changes) == dict
    assert extrapol in [False, '', 'data', 'mc', 'ratio', 'separate']
    #print "getresp", changes
    
    graph = getroot.getgraphratio(over, method, f1, f2, opt, changes, absmean=(over=='jet1_eta'))
    if extrapol:
        if over == 'z_pt':
            bins = getroot.binstrings(opt.bins)
            #bins.pop(0)
        elif over == 'jet1_eta':
            bins = getroot.etastrings(opt.eta)
        elif over == 'npv':
            bins = getroot.npvstrings(opt.npv)
        elif over == 'alpha':
            bins = getroot.cutstrings(opt.cut)
        else:
            print "The 'over' value", repr(over), "is not known."
            exit(0)
        for i in range(len(bins)):
            x, y, dx, dy = getroot.getgraphpoint(graph, i)
            print x
            y, dy = extrapolatebin(method, bins[i], changes, opt, f1, f2, source=extrapol, draw=draw, over=over)
            graph.SetPoint(i, x, y)
            graph.SetPointError(i, dx, dy)
    return graph


def responseplot(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='z_pt',
                 binborders=False,
                 figaxes = plotbase.newplot(),
                 drawextrapolation=False,
                 changes = {},
                 subtext = "",
                 subplot = False):
    """type: bal|mpf[:ratio,seperate,data,mc,ex]
    """
    fig =figaxes[0]
    ax=figaxes[1]
    if labels is None:
        labels = types
    for t, l, m, c in zip(types, labels, markers, colors):
        extrapolation = False
        if t == 'RecoGen':
            t = 'RecoToGen_bal'
        elif len(t) > 3:
            extrapolation = t[3:]
            t = t[:3]
        if extrapolation in ['ex', 'data', 'mc', 'datamc']:
            extrapolation = 'data'
        if 'Gen' not in t:
            plot = getroot.root2histo(getresponse(t+'resp', over, opt, files[0], None, changes, extrapolation))
            ax.errorbar(plot.x, plot.y, plot.yerr, color='black', fmt=m, label=l+' (190645-191859)')
        if extrapolation == 'data':
            extrapolation = 'mc'
        plot = getroot.root2histo(getresponse(t+'resp', over, opt, files[1], None, changes, extrapolation))
        if l == 'RecoGen': l = "Reco/Gen"
        #else: l = l+" (193093-193621)"
        ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)
        #plot = getroot.root2histo(getresponse(t+'resp', over, opt, files[2], None, changes, extrapolation))
        #ax.errorbar(plot.x, plot.y, plot.yerr, color="green", fmt=m, label=l+" (193834-195530)")
        #plot = getroot.root2histo(getresponse(t+'resp', over, opt, files[3], None, changes, extrapolation))
        #ax.errorbar(plot.x, plot.y, plot.yerr, color="red", fmt=m, label=l+" (195540-198272)")

    # format plot
    ax.axhline(1.0, color="black", linestyle='--')
    plotbase.labels(ax, opt, jet=True, sub_plot=subplot, changes=changes)
    plotbase.axislabels(ax, over, 'response')
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
        file_name = prefix+"Response_"+"_".join(types)+"_"+over
        if subplot is not True: file_name += "_" + a + c
        plotbase.Save(fig, file_name, opt)


def ratioplot(files, opt, types, labels=None,
                 colors=["FireBrick", 'blue', 'green', 'red']*7,
                 markers=['o', '*', 's']*8,
                 over='z_pt',
                 binborders=False,
                 drawextrapolation=False,
                 figaxes = plotbase.newplot(),
                 fit=True,
                 changes = {},
                 subtext = "",
                 subplot = False):
    """type: bal|mpf[ratio|seperate]
    """
    fig =figaxes[0]
    ax=figaxes[1]
    if labels is None:
        labels = [labelformat(t) for t in types]
    ax.axhline(1.0, color="black", linestyle='--')
    if binborders:
        for x in opt.bins:
            pass #ax.axvline(x, color='gray')

    for t, l, m, c in zip(types, labels, markers, colors):
        rgraph = getresponse(t[:3]+'resp', over, opt, files[0], files[1], changes, extrapol=t[3:], draw=False)
        if fit:
            line, err, chi2, ndf = getroot.fitline(rgraph)
            ax.text(0.95, 0.65+0.07*colors.index(c), r"$R = {0:.3f} \pm {1:.3f}$ ".format(line, err),
                va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=16)
            ax.text(0.95, 0.2+0.07*colors.index(c), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                va='bottom', ha='right', color=c, transform=ax.transAxes, fontsize=16)
            ax.axhline(line, color=c)
            ax.axhspan(line-err, line+err, color=c, alpha=0.2)
        plot = getroot.root2histo(rgraph)
        ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)

        f = ROOT.TFile("/home/dhaitz/git/CalibFW/rootfiles/"+t+".root", "RECREATE")
        rgraph.Write()
        f.Close()

    # format plot
    if over == 'jet1_eta':
        plotbase.labels(ax, opt, jet=True, legloc='lower left', sub_plot=subplot, changes=changes)
        plotbase.axislabels(ax, "abs_"+over, 'responseratio')
    else:
        plotbase.labels(ax, opt, jet=True, legloc='lower right', sub_plot=subplot, changes=changes)
        plotbase.axislabels(ax, over, 'responseratio')

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
                 over='z_pt',
                 binborders=False,
                 drawextrapolation=False,
                 fit=True):
    """"""type: bal|mpf[ratio|seperate]
    """"""
    fig, ax = plotbase.newplot()
    label = label or labelformat(method)

    grExt = getresponse(method, over, opt, files[0], files[1], {}, extrapol='seperate')
    grStd = getresponse(method, over, opt, files[0], files[1], {}, extrapol=False)
    grK = getroot.dividegraphs(grExt, grStd)
    if fit:
            line, err, chi2, ndf = getroot.fitline(grK)
            ax.text(0.1, 0.27+0.07*0, r"$k_\mathrm{{FSR}} = {0:.3f} \pm {1:.3f}$ ".format(line, err),
                va='bottom', ha='left', color=colors[0], transform=ax.transAxes, fontsize=16)
            ax.text(0.1, 0.2+0.07*0, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
                va='bottom', ha='left', color=colors[0], transform=ax.transAxes, fontsize=16)
            ax.axhline(line, color=colors[0])
            ax.axhspan(line-err, line+err, color=colors[0], alpha=0.2)
    plot = getroot.root2histo(grK)
    ax.axhline(1.0, color="black", linestyle='--')
    ax.errorbar(plot.x, plot.y, plot.yerr, color=colors[0], fmt=markers[0], label=label)


    # format plot
    if over == 'jet1_eta':
        pre = "abs_"
    else:
        pre = ""
    plotbase.labels(ax, opt, jet=True, legloc='lower left')
    plotbase.axislabels(ax, pre+over, 'kfsrratio')

    file_name = "kfsr_" + "_".join([method])
    file_name += "_"+over+"_" + opt.algorithm + opt.correction

    plotbase.Save(fig, file_name, opt)


def labelformat(label):
    if 'bal' in label:
        result = "Balance"
    elif 'mpf' in label:
        result = "MPF"
    if 'ratio' in label:
        result += " (ratio extrapol.)"
    elif 'seperate' in label:
        result += " (extrapolated)"
    return result

def plot_all(files, opt, plottype='response'):
    """Create a number of plots over different quantities.
       plottype can either be response or ratio
       Each plot contains several subplots for the different algorithms/ correction levels."""

    over = ['z_pt', 'npv', 'jet1_eta', 'alpha']
    types = ['bal', 'mpf']
    subtexts = ["a)", "b)", "c)", "d)", "e)", "f)", "g)", "h)", "i)", "j)"]
    
    # create list_ac with all variation combinations from the alg/corr lists
    list_ac = plotbase.getcorralgovariations()

    for o in over:
        fig_axes = plotbase.newplot(subplots=len(list_ac))
        for ch, subtext, ax in zip(list_ac, subtexts, fig_axes[1]):    # iterate over subplots in figure figaxes
            if plottype == 'ratio':
                if o == 'jet1_eta': fit=False
                else: fit=True
                ratioplot(files, opt, types, drawextrapolation=True, binborders=True, over=o, subplot=True, changes=ch, fit=fit,
                    figaxes=(fig_axes[0],ax), subtext = subtext)
                strings = ["Jet response data/MC ratio", "Ratio"]

            elif plottype == 'response':
                responseplot(files, opt, types, over=o, figaxes=(fig_axes[0],ax), subtext = subtext, subplot = True, changes=ch)
                strings = ["Jet response", "Response"]

        title = strings[0]+ "over %s for different correction levels / CHS" % plotbase.nicetext(o)
        fig_axes[0].suptitle(title, size='xx-large')

        file_name = strings[1]+"_all_"+o+"_"+"_".join(types)+"_"+opt.algorithm
        plotbase.Save(fig_axes[0], file_name, opt)

# responses
def response(files, opt):
    responseplot(files, opt, ['bal', 'mpf'])
    for key in k_fsr:
        print key, k_fsr[key]

def response_npv(files, opt):
    responseplot(files, opt, ['bal', 'mpf'], over='npv')

def response_eta(files, opt):
    responseplot(files, opt, ['bal', 'mpf'], over='jet1_eta')


# ratios
def balratio(files, opt):
    ratioplot(files, opt, ['bal', 'balratio', 'balseperate'])

def mpfratio(files, opt):
    ratioplot(files, opt, ['mpf', 'mpfratio', 'mpfseperate'])


def ratio(files, opt):
    ratioplot(files, opt, ['bal', 'mpf'], drawextrapolation=True, binborders=True)

def ratio_npv(files, opt):
    ratioplot(files, opt, ['bal', 'mpf'], drawextrapolation=True, binborders=True, over='npv')

def ratio_eta(files, opt):
    ratioplot(files, opt, ['bal', 'mpf'], drawextrapolation=True, binborders=True, over='jet1_eta', fit=False)

#kfsr
def kfsr(files, opt):
    plotkfsr(files, opt)

def kfsr_eta(files, opt):
    plotkfsr(files, opt, over='jet1_eta')


# subplots
def ratio_all(files, opt):
    plot_all(files, opt, plottype='ratio')

def response_all(files, opt):
    plot_all(files, opt)


plots = [
'response', 'response_npv', 'response_eta',
'response_all',
'ratio',  'ratio_npv', 'ratio_eta', 
'ratio_all'
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
