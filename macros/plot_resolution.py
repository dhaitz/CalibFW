# -*- coding: utf-8 -*-
"""
"""
import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile, kRed , kGreen, gROOT, gErrorIgnoreLevel, kError

import plotbase
import getroot

gROOT.SetBatch(True)
gErrorIgnoreLevel = kError

n_fail = [0,0,0,0,0,0]

def get_errerr(roothisto):
    sqrt_nminus1 = math.sqrt(roothisto.GetEntries() - 1)
    return roothisto.GetRMSError() / sqrt_nminus1


def fit_resolution(file, histo_name, tag, out_path, fit_formula = "gaus", rebin = 8, truncInSigma = 2.0, verb=False):
    """fit response with truncated gaussian

    fits the resolution of an distribution with a truncated gaussian
    and returns a tuple with sigma and the stat. error on sigma
    """
    c = TCanvas (histo_name, histo_name, 600, 600)
    fitFunc = TF1("fit1", fit_formula, 0, 2.0)
    hist_resp = getroot.getobject(histo_name, file)
    hist_resp.Rebin(rebin)
    n = hist_resp.GetEntries()
    message = ""

    # first untruncated fit
    fitFunc.SetParameters(1.0, 1.0, 0.25)
    fitres = hist_resp.Fit(fitFunc, "SQ")
    if fitres.IsValid():
        gaus_norm = fitres.GetParams()[0]
        gaus_center = fitres.GetParams()[1]
        gaus_sigma = fitres.GetParams()[2]
        gaus_sigmaerr = fitres.GetErrors()[2]
    else:
        print "First fit FAILED!", histo_name, "(n =", n, ")"
        n_fail[0] += 1
        message = "[No Gauß, n=%d]" % n
        gaus_norm = 1.0
        gaus_center = hist_resp.GetMean()
        gaus_sigma = hist_resp.GetRMS()
        gaus_sigmaerr = hist_resp.GetRMSError()

    if verb:
        print "Resolution: %1.4f ± %1.4f" % (gaus_sigma, gaus_sigmaerr),

    # second truncated fit
    if gaus_sigma < 0.04 or gaus_sigmaerr > 0.8*gaus_sigma:
        n_fail[2] += 1
        print "Strange sigma:", gaus_sigma, gaus_sigmaerr, "using RMS"
        message += "strange sigma"
        return (hist_resp.GetRMS(), 2*hist_resp.GetRMSError(), message) # 2 is arbitrary
    fitFunc_trunc = TF1("fit1_trunc", fit_formula,
            max(0.0, gaus_center - truncInSigma * gaus_sigma),
            min(2.0, gaus_center + truncInSigma * gaus_sigma))
    fitFunc_trunc.SetLineColor (kRed)
    fitFunc_trunc.SetParameters(1.0, 1.0, 0.25) #gaus_norm, gaus_center, gaus_sigma)
    fitres = hist_resp.Fit(fitFunc_trunc, "SRQ")

    # Draw and save the fit histogram
    hist_resp.Draw("")
    histo_name = tag + histo_name.replace("/", "_") + "_resolution_fit.png"
    #c.Print(out_path + histo_name)
    del c

    # extract the fit result and return it
    if fitres.IsValid():
        m_reso_fit = fitres.GetParams()[2]
        m_reso_fit_err = fitres.GetErrors()[2]
    else:
        print "Second fit FAILED!", histo_name
        n_fail[1] += 1
        message += "\b and no truncated Gauß]"
        m_reso_fit = hist_resp.GetRMS()
        m_reso_fit_err = hist_resp.GetRMSError()
    if verb:
        print "trunc> %1.4f ± %1.4f" % (m_reso_fit, m_reso_fit_err)
    return (m_reso_fit, m_reso_fit_err, message)


def extrapolate_resolution(file, base_name, # is "Pt300to1000_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3
        tag, out_path, var=[0.1, 0.2, 0.3, 0.35], gen_imbalance = 0.0, extr_method = "linear", uncorrelate=True):
    """extrapolate resolution over alpha
    
    # extrapolates the resolution for a set of histograms which can be defined
    # by the parameter 'base_name' and all belong to one Pt-Bin
    # various types of extrapolation are available and can be selected via
    # the parameter 'extr_method'
    """
    c = TCanvas(base_name+"2", base_name+"2", 600, 600)
    local_var = copy.deepcopy(var)
    local_var.reverse()
    graph = TGraphErrors(len(var))

    # iterate through all variations of the cut and fit the resolutions
    for x in local_var:
        # get the fit value
        folder_var = base_name.replace("XXX", str(x).replace(".", "_"))  # 0.3 -> 0_3
        reso = fit_resolution(file, folder_var, tag, out_path, rebin = 5)        #srebin = (n == 0) + 4 # why?
        print "Variation %1.2f has resolution %1.4f ± %1.4f %s" % (x, reso[0], reso[1], reso[2])

        # uncorrelate errors for all but largest point (index 0)
        yerr = reso[1]
        if local_var.index(x) == 0:
            x0 = x
            y0 = reso[0]
            y0err = reso[1]
        if uncorrelate and (extr_method == "linear"):
            if y0err <= yerr:
                yerr = math.sqrt(yerr ** 2 - y0err ** 2)
                n_fail[4] +=1
            else:
                #print "Error not uncorrelated"
                n_fail[3] +=1

        # add point and error to our graph
        graph.SetPoint(local_var.index(x), x, reso[0])
        graph.SetPointError(local_var.index(x), 0.0, yerr)

    print "  ---- ", extr_method

    if extr_method == "linear":
        fitFunc = TF1("fit1", "[0] + [1]*(x-[2])", 0, 2.0)
        fitFunc.FixParameter(0, y0)
        fitFunc.FixParameter(2, x0)
        fitres = graph.Fit(fitFunc, "SQ")

        m_fit_err = fitres.GetErrors()[1]
        m_fit = fitres.GetParams()[1]

        yex = fitFunc.Eval(0.0)
        # add the first error and extrapolate the rest
        yex_err = math.sqrt(y0err ** 2 + (m_fit_err * x0) ** 2)

    # implements the Extrapolation method described in ( JME-11-011 )
    elif extr_method == "quadratic":
        if gen_imbalance < 0.001:
            print "ERROR: gen_imbalance =", gen_imbalance, base_name
        fit_formula = "sqrt([0]^2 + [1]^2 + 2*[1]*[2]*x + [2]^2*x^2)"  # V¯ a² + (b + cx)²
        fitFunc = TF1("fit1", fit_formula, 0, 0.4)

        # fix the gen imbalance
        fitFunc.FixParameter(1, gen_imbalance)

        # start parameters
        fitFunc.SetParameter(0, gen_imbalance)
        fitFunc.SetParameter(1, 0.0)
        fitFunc.SetParameter(2, 0.5)

        fitres = graph.Fit(fitFunc, "SQ")
        reco_res = fitres.GetParams()[0]

        print "Complex Resolution fit. GenIntr:", gen_imbalance, " RecoRes:", reco_res
        yex = reco_res
        yex_err = fitres.GetErrors()[0]
    else:
        plotbase.fail("Method " + extr_method + " not supported")


    if not fitres.IsValid():
        plotbase.fail("AN ALPHA FIT FAILED!")

    graph.SetMarkerColor(4);
    graph.SetMarkerSize(1.5);
    graph.SetMarkerStyle(21);
    graph.Draw("ap")

    print "Extrapolated resolution for", base_name, "is %1.4f ± %1.4f, reference: %1.4f ± %1.4f" % (yex, yex_err, y0, y0err)

    base_name = base_name.replace ( "/", "_")
    c.Print (out_path + tag + base_name + "_resolution_extrapolation.png")
    if yex < 0.01:
        print "\n\nWARNING", base_name, "\n\n"
        n_fail[5]+=1

    return (yex, yex_err)


# plots the resolution for one algorithm, over all available PtBins
def plot_resolution ( file,
                      base_name, # is "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3
                      tag,
                      opt,
                      algo,
                      correction,
                      ref_hist,
                      fit_method = "linear",
                      subtract_gen = None,
                      draw_ax = None,
                      drop_first_bin = 0, drop_last_bin = 0,
                      draw_plot = True ):

    str_bins = getroot.binstrings(opt.bins)
    str_bins = str_bins[drop_first_bin:len(str_bins) if drop_last_bin==0 else -drop_last_bin]

    tmp_out_path = opt.out + "/tmp_resolution/"
    plotbase.EnsurePathExists( tmp_out_path )

    plot = getroot.Histo()
    n_access = 0

    for str_bin in str_bins:
        print "BIN:", str_bin
        hist_template = base_name.replace("YYY", str_bin)

        if not subtract_gen == None:
            gen_imb = subtract_gen.y[n_access]
        else:
            gen_imb = 0.0

        if fit_method == "none":
            reso = fit_resolution(file, hist_template, tag, tmp_out_path)
        else:
            reso = extrapolate_resolution( file, hist_template, tag, tmp_out_path,
                                            extr_method = fit_method,
                                            gen_imbalance = gen_imb )
        #print hist_template, "results in extrapolation", extra_res

        #print ref_hist.replace("YYY", str_bin)
        hist_zpt = getroot.getobject(ref_hist.replace("YYY", str_bin), file)

        plot.append(hist_zpt.GetMean(), True, reso[0], reso[1])
        n_access += 1

    if draw_plot:
        draw_ax.errorbar(plot.x, plot.y, plot.yerr, fmt='o', capsize=2, label=tag)

    return plot

# plots all resolutions ( MC Truth, MC Reco and Data Reco ) in one plot
def combined_resolution( files, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJets",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "",
                        subtract_gen = True,
                        drop_first = 0,
                        drop_last = 0):
    f,ax = plotbase.newplot()

    plotbase.labels(ax, opt)
    #plotbase.jetlabel(ax, algo, corr)
    plotbase.axislabels(ax, 'zpt', 'resolution')


    # construct names
    plot_filename = "resolution_" + method_name + "_" + algo + corr + filename_postfix
    hist_name = folder_template + "/" + method_name + "_" + algo + corr
    hist_truth_name = "YYY_incut" + "/" + method_name + "_" + algo + corr

    hist_z = "YYY_incut/zpt_" + algo + corr

    s = "recogen".join(hist_truth_name.split(method_name))
    print "Truth:", s
    plot_resolution(files[1], s,
                    "MC_Truth",
                     opt,
                     algo,
                     corr,
                    hist_z,
                    "none",
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last )
    print "GEN"
    # get the gen addition
    gen_res = plot_resolution(files[1], "genbal".join(hist_truth_name.split(method_name)),
                    "MC_Instrinsic",
                     opt,
                     algo,
                     corr,
                    hist_z,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last, draw_plot = False )
    print gen_res, "\n\n\n"

    if subtract_gen:
        title_postfix = "Reco"
    else:
        gen_res = None
        title_postfix = "Total"

    print  "MC"
    mc_res = plot_resolution( files[1], hist_name,
                    opt.labels[1] + " " + title_postfix,
                     opt,
                     algo,
                     corr,
                    hist_z,
                    fit_method = "quadratic",
                    subtract_gen = gen_res ,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last )

    print "DATA"
    data_res = plot_resolution( files[0], hist_name,
                    opt.labels[0] + " " + title_postfix,
                     opt,
                     algo,
                     corr,
                    hist_z,
                    fit_method = "quadratic",
                    subtract_gen = gen_res,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last )

    ax.legend( frameon=True,  numpoints=1 )
    plotbase.Save(f, plot_filename, opt)

    # plot ratio
    ratio = getroot.Histo()
    if 0 in mc_res.y:
        print "HIER:", mc_res.y

    for i in range(len(data_res) - 1):
        ratio.append(data_res.x[i], True,
                     data_res.y[i] / mc_res.y[i],
                     abs(data_res.yerr[i] * (1.0 / mc_res.y[i])) +
                     abs(mc_res.yerr[i] * (data_res.y[i] / (mc_res.y[i] * mc_res.y[i])))
                    )

    f, ax = plotbase.newplot()
    ax.errorbar(ratio.x, ratio.y, ratio.yerr, fmt='o', capsize=2 )
    ax.axhline(1.0, color="black", linestyle='--')

    plotbase.labels(ax, opt)
    #plotbase.jetlabel(ax, algo, corr)
    plotbase.axislabels(ax, 'zpt', 'resolutionratio')

    plotbase.Save(f,plot_filename + "_ratio", opt)
    print "1st: %d, 2nd %d, sigma: %d, uncorr: %d, corr: %d, extrapol: %d" % tuple(n_fail)


def resolution(files, opt):

    # balance
    #combined_resolution(files, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "_plus_gen",
    #                    subtract_gen = False )"""
    #print ">>>>>>>>>>>>>>>>>>>combined1"
    combined_resolution(files, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJetsCHS",
                        corr = "L1L2L3Res",
                        method_name = "balresp",
                        filename_postfix = "_plus_gen",
                        subtract_gen = False )

    """combined_resolution(files, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJets",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "",
                        subtract_gen = True,
                        drop_first = 2, drop_last = 1)"""
    print ">>>>>>>>>>>>>>>>>>>combined2"
    combined_resolution(files, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJetsCHS",
                        corr = "L1L2L3Res",
                        method_name = "balresp",
                        filename_postfix = "",
                        subtract_gen = True,
                        drop_first = 0, drop_last = 0)

    # mpf >> not in gen right now
    #    combined_resolution( files, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "mpfresp",
    #                    filename_postfix = "",
    #                    subtract_gen = False )

    # PU related -- problematic due to low statistics
    #combined_resolution( files, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX_var_Npv_0to2",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "npv_0_2" )

    #combined_resolution( files, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX_var_Npv_3to5",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "npv_3_5" )

    #combined_resolution( files, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX_var_Npv_6to11",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "npv_6_11" )



plots = ['resolution']

