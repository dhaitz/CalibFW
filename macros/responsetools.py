import plotbase
import getroot

import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile
import ROOT

from uncertainties import ufloat



def getextrapolated(balancetype, rootfile, changes={}, quadratic=False, getfactor=False):
    quantity = balancetype + "_alpha"
    if "gen" in quantity:
        quantity = quantity.replace("alpha", "genalpha")

    #temporarily copy changes to get the intercept, ierr values from the alpha=0.35 plot
    changes2 = copy.deepcopy(changes)
    if 'var' in changes2:
        changes2['var'] = "var_CutSecondLeadingToZPt_0_35__" + changes2['var']
    else:
        changes2['var'] = "var_CutSecondLeadingToZPt_0_35"
    rootobject = getroot.getobjectfromnick(quantity, rootfile, changes2, rebin=1)
    intercept, ierr = getroot.fitline2(rootobject, quadratic)[:2]
    if getfactor:
        method_dict = {'ptbalance':'balresp', 'mpf':'mpfresp'}
        rootobject = getroot.getobjectfromnick(method_dict[balancetype], rootfile, changes, rebin=1)
        mean, merr = getroot.fitline2(rootobject, gauss=True, limits=[0.,2.])[2:4]
        print " gauss: ", round(mean,3), round(merr,3),
        print " extra: ", round(intercept,3), round(ierr,3),

        # get the extrapolation factor k
        k = intercept / mean
        kerr = k * math.sqrt(abs((ierr/intercept)**2 - (merr/mean)**2))
        return k, kerr
    return intercept, ierr


# global variables for extrapolation factors:
k_fsr = {}


extrapolation_is_done = False


def fillgraph(method, changes, file_numerator, file_denominator=None, #extrapolateRatio=True,
        var=[0.2, 0.4], varstr = "var_CutSecondLeadingToZPt_%s",
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


def extrapolatebin(method, bin, changes, opt, f1, f2=None, draw=True, source='ratio', over='zpt'):
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
    if over == 'zpt':
        ch['bin'] = bin
    elif over in ['npv', 'jet1eta']:
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


def getresponse(method, over, opt, f1, f2=None, changes=None, extrapol=None, draw=True):
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
    assert method in ['balresp', 'mpfresp', 'recogen', 'mpf-rawresp']
    assert type(changes) == dict
    #assert extrapol in [False, '', 'data', 'mc', 'ratio', 'separate']
    if method == 'mpf-rawresp': method = 'mpfresp-raw'
    changes = plotbase.getchanges(opt, changes)

    graph = getroot.getgraphratio(over, method, f1, f2, opt, changes, absmean=(over=='jet1_eta'))

    # extrapolation options: None, 'bin' -> binwise extrapolation, 'global; -> global extrapol. factor
    if extrapol is not None:
        var_dict = {    
            'zpt':{
                    'binstrings':getroot.binstrings(opt.bins),
                    'x_quantity':"zpt",
                    'changekey':'bin',
                    },
            'jet1eta':{
                    'binstrings':getroot.etastrings(opt.eta),
                    'x_quantity':"jet1abseta",
                    'changekey':'var'
                    },
            'npv':{
                    'binstrings':getroot.npvstrings(opt.npv),
                    'x_quantity':"npv",
                    'changekey':'var'
                    }
        }

        method_dict = {'balresp':'ptbalance', 'mpfresp':'mpf'}
        if extrapol == 'global':
            # get the factor
            k = ufloat((getextrapolated(method_dict[method], f1, changes=changes, getfactor = True)))
            if f2 is not None:
                k2 = ufloat((getextrapolated(method_dict[method], f2, changes=changes, getfactor = True)))
                k = k / k2

            for i in range(len(var_dict[over]['binstrings'])):
                x, y, dx, dy = getroot.getgraphpoint(graph, i)
                yu = ufloat((y, dy))
                ex = k * yu

                graph.SetPoint(i, x, ex.nominal_value)
                graph.SetPointError(i, dx, ex.std_dev())
        elif extrapol == 'bin':
            changes['var'] = "var_CutSecondLeadingToZPt_0_35"
            for string, i in zip(var_dict[over]['binstrings'], range(len(var_dict[over]['binstrings']))):
                changes[var_dict[over]['changekey']] = string
                resp, resperr =  getextrapolated(method_dict[method], f1, changes=changes)
                if f2 is not None:
                    resp2, resperr2 =  getextrapolated(method_dict[method], f2, changes=changes)
                    resp = resp / resp2
                    resperr =  abs(resp / resp2) * math.sqrt((resperr / resp)**2 + (resperr2 / resp2)**2)

                x, y, dx, dy = getroot.getgraphpoint(graph, i)
                graph.SetPoint(i, x, resp)
                graph.SetPointError(i, dx, resperr)
            del changes[var_dict[over]['changekey']]
            if 'var' in changes: del changes['var']
     

            """if over == 'zpt':
                bins = getroot.binstrings(opt.bins)
                #bins.pop(0)
            elif over == 'jet1eta':
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
                y, dy = extrapolatebin(method, bins[i], changes, opt, f1, f2, source=extrapol, draw=draw, over=over)
                graph.SetPoint(i, x, y)
                graph.SetPointError(i, dx, dy)"""
    return graph


def plotbinborders(ax, quantity, y, opt):
    if quantity == 'jet1eta':
        l_borders = opt.eta[:-1]
        r_borders = opt.eta[1:]
    elif quantity == 'npv':
        l_borders = [bin[0]-0.5 for bin in opt.npv]
        r_borders = bins1 = [bin[1]+0.5 for bin in opt.npv]
    else:
        l_borders = opt.bins[:-1]
        r_borders = opt.bins[1:]
    for l_border, r_border, y in zip(l_borders, r_borders, y): 
        ax.add_line(plotbase.matplotlib.lines.Line2D((l_border,r_border), (y,y), color='black', alpha=0.1))
