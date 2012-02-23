# -*- coding: utf-8 -*-
"""
"""
import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile, kRed , kGreen, gROOT

import plotbase

import getroot
import matplotlib.pyplot as plt
import matplotlib.ticker as tck





def get_errerr(roothisto):
    sqrt_nminus1 = math.sqrt(roothisto.GetEntries() - 1)
    return roothisto.GetRMSError() / sqrt_nminus1

# fits the resolution of an distribution with a truncated gaussian
# and returns a tuple with sigma and the stat. error on sigma
def fit_resolution ( file, histo_name, tag,
                     out_path,
                     fit_formula = "gaus",
                     strong_rebin = False  ):

    c = TCanvas (histo_name, histo_name, 600, 600)

    fitFunc = TF1("fit1", fit_formula, 0, 2.0)
    hist_resp = getroot.getobject( histo_name, file )

    if strong_rebin:
        hist_resp.Rebin(10)
    else:
        hist_resp.Rebin(8)

    # give useful start parameters
    fitFunc.SetParameters(1.0, 1.0, 1.0)

    # untruncated, to get an idea of the gauss mean
    fitres = hist_resp.Fit( fitFunc , "SQ")
    if not fitres.IsValid():
        print " A FIT FAILED !"
        exit( 0 )

    gaus_center = fitres.GetParams()[1]
    gaus_omega = fitres.GetParams()[2]

    print "Using 2 sigma to truncate " + str( 2 * gaus_omega )
    trunc_parameter = 2 * gaus_omega

    # fit again, truncated
    fitFunc_trunc = TF1("fit1_trunc", fit_formula, gaus_center - trunc_parameter, gaus_center + trunc_parameter )
    fitFunc_trunc.SetLineColor ( kRed )
    fitFunc_trunc.SetParameters(1.0, 1.0, 1.0)

    fitres = hist_resp.Fit( fitFunc_trunc , "SRQ")

    # check that the fit converged nicely
    if not fitres.IsValid():
        print " A FIT FAILED !"
        exit(0)

    hist_resp.Draw("")

    histo_name = histo_name.replace ( "/", "_")
    print "Storing plot to " +  tag + histo_name + "_resolution_fit.png"
    c.Print(out_path + tag + histo_name + "_resolution_fit.png")

    # extract the fit result
    m_reso_fit_err = fitres.GetErrors()[2]
    m_reso_fit = fitres.GetParams()[2]

    return ( m_reso_fit, m_reso_fit_err )

# extrapolates the resolution for a set of histograms which can be defined
# by the parameter 'base_name' and all belong to one Pt-Bin
# various types of extrapolation are available and can be selected via
# the parameter 'extr_method'
def extrapolate_resolution ( file,
                             base_name, # is "Pt300to1000_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3
                             tag,
                             out_path,
                             var=[0.1, 0.15, 0.2, 0.3],
                             gen_imbalance = 0.0,
                             extr_method = "plain" ):
    variation_result = []

    c = TCanvas (base_name, base_name, 600, 600)
    local_var = copy.deepcopy(var)
    local_var.reverse()
    graph = TGraphErrors(len(var))

    n = 0

    # iterate through all variations of the cut and fit the resolutions
    for x in local_var:
        # get the histograms
        folder_var = base_name.replace ( "XXX", str(x).replace(".", "_") )  # 0.3 -> 0_3

        # read the ratio and error (propagated)
        srebin = ( n == 0 )
        reso = fit_resolution( file, folder_var, tag, out_path, strong_rebin = srebin )

        print "Variation " + str(x) + " has resolution " + str( reso )
        variation_result += [ x, reso ]

        # add to our graph
        graph.SetPoint(n, x, reso[0])

        yerr = reso[1]

        if n == 0:  # remember values for first point var[0]
            y_first = reso[0]
            y_first_err = reso[1]
            x_first = x

            yerr = 0.0 # this error will be added, after the extrapolation is done
            #y0errerr = yerrerr
        else:
            if y_first_err < yerr:
                # uncorrelate errors
                yerr = math.sqrt(yerr ** 2 - y_first_err ** 2)
            else:
                print "Error not uncorrelated"

        # old code
        #graph.SetPointError(n, 0.0, reso[1])
        if extr_method == "plain":
            graph.SetPointError(n, 0.0, yerr )
        else:
            graph.SetPointError(n, 0.0, reso[1] )
        n = n + 1

    if extr_method == "plain":
        fit_formula = "[0]+[1]*(x-[2])"
        fitFunc = TF1("fit1", fit_formula, 0, 2.0)

        # Fix the parameters and do the fit
        fitFunc.FixParameter(0, y_first)
        fitFunc.FixParameter(2, x_first)

        fitres = graph.Fit(fitFunc, "S")

        m_fit_err = fitres.GetErrors()[1]
        m_fit = fitres.GetParams()[1]

        yex = fitFunc.Eval(0.0)  #y_last - m_fit * x_last
        # add the first error and extrapolate the rest
        yex_err = math.sqrt(y_first_err**2  +  ( m_fit_err * x_first ) ** 2 )

    # implements the Extrapolation method described in ( JME-11-011 )
    elif extr_method == "complex":
        fit_formula = "sqrt( [0]^2+[1]^2 + 2 * [1] * [2] * x  + [2]^2 * x^2 )"
        fitFunc = TF1("fit1", fit_formula, 0, 0.4)

        # fix the gen imbalance
        fitFunc.FixParameter(1, gen_imbalance )

        # 'resonable' start parameters
        fitFunc.SetParameter(0, gen_imbalance)
        fitFunc.SetParameter(2, 0.5)

        fitres = graph.Fit(fitFunc, "S")
        reco_res = fitres.GetParams()[0]

        print "  ---- "
        print "Complex Resolution fit. GenIntr: " + str( gen_imbalance ) + " RecoRes: " + str( reco_res )
        yex = reco_res
        yex_err = fitres.GetErrors()[0]
    else:
        plotbase.fail( "Method " + extr_method + " not supported" )


    if not fitres.IsValid():
        print " A FIT FAILED !"
        exit( 0 )

    graph.SetMarkerColor(4);
    graph.SetMarkerSize(1.5);
    graph.SetMarkerStyle(21);
    graph.Draw( "ap" )

    print "Outermost resolution for " + base_name + " is " + str( (y_first, y_first_err) )
    print "Extrapolated resolution for " + base_name + " is " + str( (yex, yex_err) )

    base_name = base_name.replace ( "/", "_")
    c.Print ( out_path + tag + base_name + "_resolution_extrapolation.png")

    return (yex, yex_err)


# plots the truth resolution in MC from GenJets
def plot_resolution_truth(file,
                      base_name, # is "YYY_incut/balresp_AK5PFJetsCHSL1L2L3
                      tag,
                      opt,
                      algo,
                      correction,
                      ref_hist,
                      fit_method = "plain",
                      subtract_gen = None,
                      draw_ax = None,
                      drop_first_bin = 0, drop_last_bin = 0 ):

    str_bins = getroot.binstrings(opt.bins)

    for i in range(drop_first_bin):
        str_bins.pop(0)
    for i in range(drop_last_bin):
        str_bins.pop(len(str_bins) - 1)


    tmp_out_path = opt.out + "/tmp_resolution/"
    plotbase.EnsurePathExists(tmp_out_path)

    plot = getroot.Histo()
    n_access = 0

    for str_bin in str_bins:
        hist_template = base_name.replace("YYY", str_bin)

        # directly take the resolution, do NO extrapolation
        reso = fit_resolution(file, hist_template, tag, tmp_out_path)

        print "Reco To Gen has resolution", reso[0]

        hist_zpt = getroot.getobject( ref_hist.replace("YYY", str_bin), file)

        plot.append(hist_zpt.GetMean(), True, reso[0], reso[1])
        n_access += 1

    draw_ax.errorbar(plot.x, plot.y, plot.yerr, fmt='o', capsize=2, label=tag)

    return plot


# plots the resolution for one algorithm, over all available PtBins
def plot_resolution ( file,
                      base_name, # is "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3
                      tag,
                      opt,
                      algo,
                      correction,
                      ref_hist,
                      fit_method = "plain",
                      subtract_gen = None,
                      draw_ax = None,
                      drop_first_bin = 0, drop_last_bin = 0,
                      draw_plot = True ):

    str_bins = getroot.binstrings(opt.bins)

    for i in range(drop_first_bin):
        str_bins.pop(0)
    for i in range(drop_last_bin):
        str_bins.pop( len(str_bins) - 1 )


    tmp_out_path = opt.out + "/tmp_resolution/"
    plotbase.EnsurePathExists( tmp_out_path )

    plot = getroot.Histo()

    n_access = 0

    for str_bin in str_bins:
        hist_template = base_name.replace("YYY", str_bin)

        if not subtract_gen == None:
            gen_imb = subtract_gen.y[n_access]
        else:
            gen_imb = 0.0

        extra_res = extrapolate_resolution( file, hist_template, tag, tmp_out_path,
                                            extr_method = fit_method,
                                            gen_imbalance = gen_imb )
        print hist_template, "results in extrapolation", extra_res

        print ref_hist.replace("YYY", str_bin)
        hist_zpt = getroot.getobject( ref_hist.replace("YYY", str_bin), file)

        plot.append(hist_zpt.GetMean(), True, extra_res[0], extra_res[1])
        n_access += 1

    if draw_plot:
        draw_ax.errorbar(plot.x, plot.y, plot.yerr, fmt='o', capsize=2, label=tag)

    return plot

# plots all resolutions ( MC Truth, MC Reco and Data Reco ) in one plot
def combined_resolution( fdata, fmc, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJets",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "",
                        subtract_gen = True,
                        drop_first = 0,
                        drop_last = 0):
    f,ax = plotbase.newplot()
    # move the plot a bit
    #ax = f.add_subplot(111, position=[0.125,0.1,0.825,0.8])
    #ax.xaxis.set_major_locator( tck.FixedLocator([60,100,400]) )
    ax.set_ylim( 0.0, 0.301 )
    ax.set_xlim( 30.0, 501 )

    # this modifies the tick formaters on the x-axis !
    ax.set_xscale ("log")

    ax.xaxis.set_major_locator( tck.LogLocator( base = 10 , subs = [1,2,4,5,7]) )
    ax.xaxis.set_major_formatter( tck.ScalarFormatter() )


    ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{Z" + r"} / \mathrm{GeV}$",
          ha="right", x=1)
    ax.set_ylabel(r"Jet Resolution", va="top", y=1)
    plotbase.lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    plotbase.statuslabel(ax, opt.status)
    plotbase.energylabel(ax, opt.energy)


    plotbase.jetlabel( ax, algo, corr )


    # construct names
    plot_filename = "resolution_" + method_name + "_" + algo + corr + filename_postfix
    hist_name = folder_template + "/" + method_name + "_" + algo + corr
    hist_truth_name = "YYY_incut" + "/" + method_name + "_" + algo + corr

    hist_z = "YYY_incut/z_pt_" + algo + corr


    plot_resolution_truth( fmc, hist_truth_name + "RecoToGen",
                    "MC Truth",
                     opt,
                     algo,
                     corr,
                    hist_z,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last )

    # get the gen addition
    gen_res = plot_resolution( fmc, hist_name + "Gen",
                    "MC Instrinsic",
                     opt,
                     algo,
                     corr,
                    hist_z,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last, draw_plot = False )

    if subtract_gen:
        title_postfix = "Reco"
    else:
        gen_res = None
        title_postfix = "Total"

    mc_res = plot_resolution( fmc, hist_name,
                    "MC " + title_postfix,
                     opt,
                     algo,
                     corr,
                    hist_z,
                    fit_method = "complex",
                    subtract_gen = gen_res ,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last )

    data_res = plot_resolution( fdata, hist_name,
                    "Run 2011 " + title_postfix,
                     opt,
                     algo,
                     corr,
                    hist_z,
                    fit_method = "complex",
                    subtract_gen = gen_res,
                    draw_ax = ax,
                    drop_first_bin = drop_first, drop_last_bin = drop_last )

    ax.legend( frameon=True,  numpoints=1 )
    plotbase.Save(f, plot_filename, opt)

    # plot ratio
    ratio = getroot.Histo()

    for i in range(len(data_res) - 1):
        ratio.append(data_res.x[i], True,
                     data_res.y[i] / mc_res.y[i],
                     abs(data_res.yerr[i] * (1.0 / mc_res.y[i])) +
                     abs(mc_res.yerr[i] * (data_res.y[i] / (mc_res.y[i] * mc_res.y[i])))
                    )

    f, ax = plotbase.newplot()
    ax.set_xscale("log")
    ax.xaxis.set_major_locator( tck.LogLocator( base = 10 , subs = [1,2,4,5,7]) )
    ax.xaxis.set_major_formatter( tck.ScalarFormatter() )
    ax.set_ylim( -0.5, 2.5 )
    ax.set_xlim( 30.0, 501 )
    plotbase.lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    plotbase.statuslabel(ax, opt.status)
    plotbase.energylabel(ax, opt.energy)
    plotbase.jetlabel( ax, algo, corr )

    ax.errorbar(ratio.x, ratio.y, ratio.yerr, fmt='o', capsize=2 )

    ax.axhline(1.0, color="black", linestyle='--')

    ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{Z" + r"} / \mathrm{GeV}$",
          ha="right", x=1)
    ax.set_ylabel(r"( Data/MC ) Jet Resolution", va="top", y=1)

    plotbase.Save(f,plot_filename + "_ratio", opt)

def resolution(fdata, fmc, opt):

    # balance
    combined_resolution(fdata, fmc, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJets",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "_plus_gen",
                        subtract_gen = False )

    combined_resolution(fdata, fmc, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJetsCHS",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "_plus_gen",
                        subtract_gen = False )

    combined_resolution(fdata, fmc, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJets",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "",
                        subtract_gen = True,
                        drop_first = 2, drop_last = 1)

    combined_resolution(fdata, fmc, opt,
                        folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
                        algo = "AK5PFJetsCHS",
                        corr = "L1L2L3",
                        method_name = "balresp",
                        filename_postfix = "",
                        subtract_gen = True,
                        drop_first = 1, drop_last = 1)

    # mpf >> not in gen right now
    #    combined_resolution( fdata, fmc, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "mpfresp",
    #                    filename_postfix = "",
    #                    subtract_gen = False )

    # PU related -- problematic due to low statistics
    #combined_resolution( fdata, fmc, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX_var_Npv_0to2",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "npv_0_2" )

    #combined_resolution( fdata, fmc, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX_var_Npv_3to5",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "npv_3_5" )

    #combined_resolution( fdata, fmc, opt,
    #                    folder_template = "YYY_incut_var_CutSecondLeadingToZPt_XXX_var_Npv_6to11",
    #                    algo = "AK5PFJets",
    #                    corr = "L1L2L3",
    #                    method_name = "balresp",
    #                    filename_postfix = "npv_6_11" )



plots = ['resolution']

