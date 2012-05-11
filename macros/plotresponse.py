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
        var=[0.1, 0.15, 0.2, 0.3], varstr = "var_CutSecondLeadingToZPt_%s"):
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
        y = oj.GetMean()
        yerr = oj.GetMeanError()
        if y < 0.000001:
            plotbase.fail("No valid response in %s (cut = %1.2f)!" % (ch['bin'], x) )

        # take the ratio (including error propagation):
        if file_denominator is not None:
            oj = getroot.getobjectfromnick(method, file_denominator, ch)
            y /= oj.GetMean()
            yerr =  (yerr + y * oj.GetMeanError()) / oj.GetMean()
            if y < 0.000001:
                plotbase.fail("No valid response in %s (cut = %1.2f)!" % (ch['bin'], x) )

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
    ax = plotbase.labels(ax, opt, legloc='upper right')
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
       2     'seperate'=> ratio (first extrapolate, then ratio)
        TODO: for npv, eta etc.
    """
    ch = copy.deepcopy(changes)
    if over == 'z_pt':
        ch['bin'] = bin
    elif over in ['npv', 'jet1_eta']:
        ch['var'] = bin
    print "extrapolate", source, "...", ch
    if f2 is None or source == 'ratio':
        graph = fillgraph(method, ch, f1, f2)
        fit = fitextrapolation(graph)
        if draw:
            graph = getroot.root2histo(graph)
            graph.ylabel = method + "_" + source
            graph.name = "%s_%s_extrapolation_%s_%s%s" % (bin, method, source, opt.algorithm, opt.correction)
            draw_extrapolation(graph, fit, ptbin, opt)
        print "k(%s) =" % bin, fit.f(0) / fit.f(0.2)
        global k_fsr
        if str(changes)+method+source not in k_fsr:
            k_fsr[str(changes)+method+source] = getroot.Histo()
        k_fsr[str(changes)+method+source].append(10, True, fit.f(0) / fit.f(0.2), fit.ferr(0) / fit.f(0.2) + fit.ferr(0.2) * fit.f(0) / fit.f(0.2) / fit.f(0.2) )
        return fit.f(0), fit.ferr(0)
    else: # 'seperate'
        graphd = fillgraph(method, ch, f1)
        graphm = fillgraph(method, ch, f2)
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
       2     'seperate'=> ratio (first extrapolate, then ratio)

       TODO: for npv, eta etc.
    """
    assert method in ['balresp', 'mpfresp']
    assert type(changes) == dict
    assert extrapol in [False, '', 'data', 'mc', 'ratio', 'seperate']
    # override default changes from options with changes argument
    print "Get response ratio", method, over, changes, extrapol
    defaultchanges = plotbase.createchanges(opt)
    defaultchanges.update(changes)

    graph = getroot.getgraphratio(over, method, f1, f2, opt, defaultchanges)
    if extrapol:
        if over == 'z_pt':
            bins = getroot.binstrings(opt.bins)
            bins.pop(0)
        elif over == 'jet1_eta':
            bins = getroot.etastrings(opt.eta)
        elif over == 'npv':
            bins = getroot.npvstrings(opt.npv)
        else:
            print "The 'over' value", repr(over), "is not known."
            exit(0)
        for i in range(len(bins)):
            x, y, dx, dy = getroot.getgraphpoint(graph, i)
            y, dy = extrapolatebin(method, bins[i], defaultchanges, opt, f1, f2, source=extrapol, draw=draw, over=over)
            graph.SetPoint(i, x, y)
            graph.SetPointError(i, dx, dy)
    return graph


def responseplot(fdata, fmc, opt, types, labels=None,
                 colors=["FireBrick", 'red', 'green', 'blue']*7,
                 markers=['o', '*', 's']*8,
                 over='z_pt',
                 binborders=False,
                 drawextrapolation=False):
    """type: bal|mpf[:ratio,seperate,data,mc,ex]
    """
    fig, ax = plotbase.newplot()
    if labels is None:
        labels = types
    for t, l, m, c in zip(types, labels, markers, colors):
        extrapolation = False
        if len(t) > 3:
            extrapolation = t[3:]
            t = t[:3]
        if extrapolation in ['ex', 'data', 'mc', 'datamc']:
            extrapolation = 'data'
        plot = getroot.root2histo(getresponse(t+'resp', over, opt, fdata, None, {}, extrapolation))
        ax.errorbar(plot.x, plot.y, plot.yerr, color='black', fmt=m, label=l+' (data)')
        if extrapolation == 'data':
            extrapolation = 'mc'
        plot = getroot.root2histo(getresponse(t+'resp', over, opt, fmc, None, {}, extrapolation))
        ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l+' (MC)')

    # format plot
    ax.axhline(1.0, color="black", linestyle='--')
    plotbase.labels(ax, opt, jet=True)
    plotbase.axislabels(ax, over, 'response')

    file_name = "_".join(types)
    file_name += "_"+over+"_" + opt.algorithm + opt.correction

    plotbase.Save(fig, file_name, opt)


def ratioplot(fdata, fmc, opt, types, labels=None,
                 colors=["FireBrick", 'red', 'green', 'blue']*7,
                 markers=['o', '*', 's']*8,
                 over='z_pt',
                 binborders=False,
                 drawextrapolation=False):
    """type: bal|mpf[ratio|seperate]
    """
    fig, ax = plotbase.newplot()
    if labels is None:
        labels = [labelformat(t) for t in types]
    ax.axhline(1.0, color="black", linestyle='--')
    if binborders:
        for x in opt.bins:
            ax.axvline(x, color='gray')

    for t, l, m, c in zip(types, labels, markers, colors):
        plot = getroot.root2histo(getresponse(t[:3]+'resp', over, opt, fdata, fmc, {}, extrapol=t[3:]))
        ax.errorbar(plot.x, plot.y, plot.yerr, color=c, fmt=m, label=l)

    # format plot

    plotbase.labels(ax, opt, jet=True)
    plotbase.axislabels(ax, over, 'response')

    file_name = "_".join(types)
    file_name += "_"+over+"_" + opt.algorithm + opt.correction

    plotbase.Save(fig, file_name, opt)


def labelformat(label):
    if 'bal' in label:
        result = "Balance"
    elif 'mpf' in label:
        result = "MPF"
    if 'ratio' in label:
        result += " (ratio extrapolated)"
    elif 'seperate' in label:
        result += " (extrapolated)"
    return result


# responses
def response(fdata, fmc, opt):
    responseplot(fdata, fmc, opt, ['bal', 'balex', 'mpf'])
    for key in k_fsr:
        print key, k_fsr[key]


def response_npv(fdata, fmc, opt):
    responseplot(fdata, fmc, opt, ['bal', 'balex', 'mpf'], over='npv')


# ratios
def balratio(fdata, fmc, opt):
    ratioplot(fdata, fmc, opt, ['bal', 'balratio', 'balseperate'])


def mpfratio(fdata, fmc, opt):
    ratioplot(fdata, fmc, opt, ['mpf', 'mpfratio', 'mpfseperate'])


def respratio(fdata, fmc, opt):
    ratioplot(fdata, fmc, opt, ['bal', 'balratio', 'mpf'], drawextrapolation=True, binborders=True)


def respratio_npv(fdata, fmc, opt):
    ratioplot(fdata, fmc, opt, ['bal', 'balratio', 'balseperate', 'mpf'], over='npv')


def kfsr(fdata, fmc, opt):
    pass



plots = ['response', 'respratio']

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
#   fractions(fdata, fmc, opt=plotbase.options(bins=bins))


###############################################################################
#OLD

# is that used?
def get_errerr(roothisto):
    # to protect, if the histogram only has one entry
    if roothisto.GetEntries() > 1.0:
        sqrt_nminus1 = math.sqrt(roothisto.GetEntries() - 1)
    else:
        sqrt_nminus1 = 0.00001
    return roothisto.GetRMSError() / sqrt_nminus1


def extrapolateRatio(fdata, fmc,
                    folder,  # ="Pt100to140_incut_var_CutSecondLeadingToZPt_XXX", where XXX is replaced by the cut variation
                    histo,  # ="jetresp_ak5PFJetsL1L2L3CHS_hist",
                    # zpt,  # ="z_pt_ak5PFJetsL1L2L3_hist",
                    var=[0.1, 0.15, 0.2, 0.3],
                    fit_formula="[0]+[1]*(x-[2])",
                    drawPlot=True,
                    opt=plotbase.options(),
                    doExtrapolation = True):


    if doExtrapolation == False:

        folder_var = folder.replace( "XXX", "0_2")

        hist_zpt = getroot.getobject( folder_var +
                                 "/z_pt_AK5PFJetsL1L2L3", fdata)
        zpt = hist_zpt.GetMean()
        hist_respdata = getroot.getobject( folder_var + "/" + histo, fdata)
        hist_respmc = getroot.getobject( folder_var + "/" + histo, fmc)
        # read the ratio and error (propagated)
        print "Skipping extrapolation"
        y = hist_respdata.GetMean() / hist_respmc.GetMean()
        yerr = y * (hist_respdata.GetMeanError() / hist_respdata.GetMean() +
                    hist_respmc.GetMeanError() / hist_respmc.GetMean())
        return y, yerr, zpt

    # for every variation point there is a point to be set and errors

    graph = fillgraph(var)
    # Fix the parameters and do the fit
    func = fitextrapolation(graph)
    if drawPlot:
        ptbin = (int(folder[folder.find('Pt') + 2:folder.find('to')]),
                 int(folder[folder.find('to') + 2:folder.find('_incut')]))
        draw_extrapolation(getroot.root2histo(graph), func, ptbin, histo, opt)

    print "Extrapolated response: R = %1.4f +- %1.4f" % (func.f(0), func.ferr(0))

    hist_zpt = getroot.getobject( folder_var + "/z_pt_AK5PFJetsL1L2L3", fdata)
    zpt = hist_zpt.GetMean()
    return func, zpt




def extrapolate_ratio( fig, method, fdata, fmc, opt, tag,
                      folder_prefix="", folder_postfix = "_incut_var_CutSecondLeadingToZPt_XXX" ,
                      file_name_prefix = "",
                      plot_result = True,
                      extrapolation = True,
                      store_tmp_files = False):
    bins = copy.deepcopy(opt.bins)
    if bins[0] == 0:
        bins.pop(0)
    str_bins = plotbase.binstrings(bins)

    ext_res_data = []
    ratioEx = getroot.Histo()

    file_name = method + "_ratio_" + opt.algorithm + opt.correction

    c = TCanvas (file_name, file_name, 600, 600)

    # loop over bins
    for s in str_bins:
        # extrapolate ratio
        yex, yex_err, x = extrapolateRatio(fdata, fmc,
            folder_prefix + s  + folder_postfix,
            method + "_" + opt.algorithm + opt.correction , doExtrapolation = extrapolation,
            drawPlot = store_tmp_files)
        ratioEx.y += [yex]
        ratioEx.yerr += [yex_err]
        ratioEx.x += [x]

    if extrapolation:
        ext_str = " extr"
    else:
        ext_str = ""

    if method == "balresp":
        if extrapolation:
            the_color = 'blue'
            the_label = 'Balance' + ext_str
        else:
            the_color = 'grey'
            the_label = 'Balance'

    elif method == "mpfresp":
        the_color = 'blue'
        the_label = 'MPF' + ext_str
    else:
        plotbase.fail ( "Unsupported response method : " + method )

    tfit = TGraphErrors(len(ratioEx) )
    tfit.SetMarkerColor(4);
    tfit.SetMarkerStyle(21);

    tfit.SetName ( file_name + "_graph")

    for i in range(0, len(ratioEx)):
        print "   {0:2d}:{1:7.2f} | {2:7.4f} +- {3:7.4f}".format(
                i, ratioEx.x[i], ratioEx.y[i], ratioEx.yerr[i])
        tfit.SetPoint(i, ratioEx.x[i], ratioEx.y[i])
        tfit.SetPointError(i, 0, ratioEx.yerr[i])

    extr_func = TF1("fit12", "[0]", 0.0, 1000.0)
    extr_func.SetParameter(0, 1.0)

    fitres = tfit.Fit(extr_func, "SQN")
    const_fit_res = fitres.GetParams()[0]
    const_fit_res_err = fitres.GetErrors()[0]

    tfit.Draw ("AP")
    extr_func.Draw("same")

    print "Fit result: overall ratio = {0:.4f} +- {1:.4f}".format(
            const_fit_res, const_fit_res_err)

    if plot_result:
        ax = fig.get_axes()[0]
        ax.errorbar(ratioEx.x, ratioEx.y, ratioEx.yerr, drawstyle='steps-mid',
                    color=the_color, fmt='o', label=the_label)
        ax.axhline(const_fit_res, color=the_color)
        ax.axhspan(const_fit_res - const_fit_res_err,
                const_fit_res + const_fit_res_err, color=the_color, alpha=0.23)
        ax.axhline(1.0, color="black", linestyle='--')

        str_fit_quality = r"$ \chi^2$ / NDF = {0:.2f} / {1:.0f} ".format(fitres.Chi2(),fitres.Ndf() )
        plotbase.labels(ax, opt, result=("Overall Ratio (fit) = "
                "${0:.3f} \pm {1:.3f}$".format(const_fit_res, const_fit_res_err)) + "\n" + str_fit_quality,
                        legloc = 'upper right')
        plotbase.axislabels(ax, 'z_pt', 'datamc_ratio')
        plotbase.jetlabel( ax, opt.algorithm, opt.correction )
        ax.set_ylim(0.88, 1.12)

    if store_tmp_files:
        c.Print ( opt.out + "/" + file_name + "_root.png" )
        # store root objects
        f = TFile(opt.out + "/" + file_name + "_root.root" , "RECREATE" )
        #c.Store( f )
        c.SetTitle( file_name )
        c.Write(file_name)

        tfit.SetTitle( file_name )
        tfit.Write( file_name + "_graph")
        extr_func.Write( file_name + "_fit" )
        fitres.Write( file_name + "_fit_res" )
        ##c.Print ( opt.out + "/" + file_name + "_root.root" )
        f.Close()

    return ( const_fit_res , const_fit_res_err )

def getResponseRatioOverNPV(fdata, fmc,  opt, method, do_extrapolation):
    hst = getroot.Histo()

    for n, m in opt.npv:
        npvname = "Npv_%dto%d" % (n, m)

        # litlte hack, the new version of the code writes out the number of primary vetrices
        meannpv = ( n + min(m, 15) ) / 2.0

        the_folder_postfix = "_incut_var_CutSecondLeadingToZPt_XXX_var_" + npvname

        extr_res = extrapolate_ratio(None, method, fdata, fmc, opt, "extrapol",
                          folder_postfix = the_folder_postfix,
                          file_name_prefix = npvname,
                          extrapolation = do_extrapolation,
                          plot_result = False)

        hst.x += [ meannpv ]
        hst.y += [ extr_res[0] ]
        hst.yerr += [ extr_res[1] ]

    return hst

def plolResponseRatioOverNPV( fdata, fmc,  opt, method, use_extrapolation, the_label ):
    hst = getResponseRatioOverNPV( fdata, fmc, opt, method, do_extrapolation = use_extrapolation )

    # in relation to NPV for the constant factor
    fig, ax = plotbase.newplot()

    ax.errorbar( hst.x, hst.y, hst.yerr, color='FireBrick', fmt='o',
                capsize=2, label= the_label)

    for n, m in opt.npv[1:]:
            ax.axvline( n, color="blue", alpha=0.5, linestyle='-')

    # format plot
    ax.axhline(1.0, color="black", linestyle='--')
    plotbase.labels(ax, opt )
    plotbase.axislabels(ax, 'npv', 'datamc_ratio')
    plotbase.jetlabel( ax, opt.algorithm, opt.correction )

    file_name = method

    if not use_extrapolation:
        file_name += "_no-extr"

    file_name += "_" + opt.algorithm + opt.correction + "_over_npv"

    plotbase.Save( fig, file_name, opt )



# Balance with various dependencies
def balanceex(fdata, fmc, opt):
    print "Balex"
    # regular
    fig, ax = plotbase.newplot()
    extrapolate_ratio( fig , "balresp", fdata, fmc, opt, "extrapol", store_tmp_files = True)
    file_name = "balresp_ratio_" + opt.algorithm + opt.correction
    plotbase.Save( fig, file_name, opt)

    fig, ax = plotbase.newplot()
    extrapolate_ratio( fig , "balresp", fdata, fmc, opt, "no extrapol", extrapolation = False)
    file_name = "balresp_ratio_noextr_" + opt.algorithm + opt.correction
    plotbase.Save( fig, file_name, opt )

    plolResponseRatioOverNPV( fdata, fmc, opt, "balresp", use_extrapolation = True, the_label = "Balance extr." )
    plolResponseRatioOverNPV( fdata, fmc, opt, "balresp", use_extrapolation = False, the_label = "Balance")





# MPF
def mpfex(fdata, fmc, opt):
    print "MPF ex"
    fig, ax = plotbase.newplot()
    extrapolate_ratio(fig, "mpfresp", fdata, fmc, opt, "no_extrapol", extrapolation = False,  store_tmp_files = True)
    file_name = "mpfresp_ratio_" + opt.algorithm + opt.correction

    plolResponseRatioOverNPV( fdata, fmc, opt, "mpfresp", use_extrapolation = False, the_label = "MPF" )


    plotbase.Save( fig, file_name, opt )
