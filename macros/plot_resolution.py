# -*- coding: utf-8 -*-
"""
"""
import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile, kRed , kGreen, gROOT

import plotbase

import getroot
import matplotlib.pyplot as plt 


def binstrings(bins):
    return ["Pt{0}to{1}".format(bins[i], bins[i + 1])
            for i in range(len(bins) - 1)]


def get_errerr(roothisto):
    sqrt_nminus1 = math.sqrt(roothisto.GetEntries() - 1)
    return roothisto.GetRMSError() / sqrt_nminus1


def fit_resolution ( file, histo_name, tag,
                     fit_formula = "gaus"  ):

    c = TCanvas (histo_name, histo_name, 600, 600)

    fitFunc = TF1("fit1", fit_formula, 0, 2.0)


    hist_resp = getroot.getobject( histo_name, file )
    hist_resp.Rebin(6)
    
    # give useful start parameters 
    fitFunc.SetParameters(1.0, 1.0, 1.0)
        
    # untruncated
    fitres = hist_resp.Fit( fitFunc , "S")
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

      
    fitres = hist_resp.Fit( fitFunc_trunc , "SR")

    
    if not fitres.IsValid():
        print " A FIT FAILED !"
        exit( 0 )
    
    hist_resp.Draw("")
    
    histo_name = histo_name.replace ( "/", "_")
    print "Storing plot to " +  tag + histo_name + "_resolution_fit.png"
    c.Print ( tag + histo_name + "_resolution_fit.png")

    # extract the fit result
    m_reso_fit_err = fitres.GetErrors()[2]
    m_reso_fit = fitres.GetParams()[2]
    
    return ( m_reso_fit, m_reso_fit_err )

def extrapolate_resolution ( file, 
                             base_name, # is "Pt300to1000_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3
                             tag,
                             var=[0.1, 0.15, 0.2, 0.3] ):
    variation_result = []

    c = TCanvas (base_name, base_name, 600, 600)

    fit_formula = "[0]+[1]*(x-[2])"
    fitFunc = TF1("fit1", fit_formula, 0, 2.0)
    #fitFunc.SetParameters(1.0, 0.001, 0.3)

    
    graph = TGraphErrors(len(var))
    
    n = 0
    
    for x in var:
        # get the histograms
        folder_var = base_name.replace ( "XXX", str(x).replace(".", "_") )  # 0.3 -> 0_3

        # read the ratio and error (propagated)
        reso = fit_resolution( file, folder_var, tag )
        
        print "Variation " + str(x) + " has resolution " + str( reso )
        
        variation_result += [ x, reso ]
        
        # add to our graph
        graph.SetPoint(n, x, reso[0])
        graph.SetPointError(n, 0.0, reso[1])
        
        y_last = reso[0]
        y_last_err = reso[1]
        x_last = x
        
        n = n + 1
    
    # Fix the parameters and do the fit
    fitFunc.FixParameter(0, y_last)
    fitFunc.FixParameter(2, x_last)
    
    fitres = graph.Fit(fitFunc, "S")
    
    m_fit_err = fitres.GetErrors()[1]
    m_fit = fitres.GetParams()[1]    

    yex = fitFunc.Eval( 0.0 )  #y_last - m_fit * x_last
    yex_err = y_last_err + m_fit_err * x_last


    if not fitres.IsValid():
        print " A FIT FAILED !"
        exit( 0 )

    graph.SetMarkerColor(4);
    graph.SetMarkerSize(1.5);
    graph.SetMarkerStyle(21);
    graph.Draw( "ap" )

    print "Outermost resolution for " + base_name + " is " + str( (y_last, y_last_err) )
    print "Extrapolated resolution for " + base_name + " is " + str( (yex, yex_err) )
        
    base_name = base_name.replace ( "/", "_")
    c.Print ( tag + base_name + "_resolution_extrapolation.png")
    #c.Print ( tag + base_name + "_resolution_extrapolation.root")


    return (yex, yex_err)

def plot_resolution ( file, 
                      base_name, # is "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3
                      tag,
                      opt,
                      algo,
                      correction,
                      ref_hist # some zpt ... 
                      ):
    str_bins = binstrings(opt.bins)
    

    

    #fit_formula = "[0]+[1]*(x-[2])"
    #fitFunc = TF1("fit1", fit_formula, 0, 2.0)
    #fitFunc.SetParameters(1.0, 0.001, 0.3)

    skip_evens = 1
    
    graph = TGraphErrors(len(str_bins) - skip_evens)
        
    
    plot_x = []
    plot_y = []
    plot_yerr = []
    
    n = 0
    
    for str_bin in str_bins:
        hist_template = base_name.replace("YYY", str_bin)
        
        extra_res = extrapolate_resolution( file, hist_template, tag )
        print hist_template + " results in extrapolation " + str(extra_res)
               
        print ref_hist.replace("YYY", str_bin)
        hist_zpt = getroot.getobject( ref_hist.replace("YYY", str_bin), file)
        
        
        # skip first bin
        if not n == 0:
            graph.SetPoint(n - skip_evens, hist_zpt.GetMean (), extra_res[0] )
            graph.SetPointError(n - skip_evens, hist_zpt.GetMeanError (), extra_res[1] )

            plot_x += [ hist_zpt.GetMean () ]
	    plot_y += [extra_res[0] ]
	    plot_yerr += [extra_res[1] ]

        n = n + 1
        
        
    plt.errorbar(plot_x, plot_y, plot_yerr, fmt='o',
                 capsize=2, label=tag )
    
    
    
    c = TCanvas (base_name, base_name, 600, 600)
    
    graph.SetMarkerColor(4)
    graph.SetMarkerSize(1.5)
    graph.SetMarkerStyle(21)
    
    graph.SetMinimum( 0.0 )
    graph.SetMaximum ( 0.3 )
    
    graph.Draw( "ap" )
    base_name = base_name.replace ( "/", "_")
    c.Print ( tag + base_name + "_resolution.png")
    
    return (plot_x, plot_y, plot_yerr )


def mytest(fdata, fmc, opt):
    #fit_resolution( fmc, "Pt300to1000_incut_var_CutSecondLeadingToZPt_0_15/balresp_AK5PFJetsCHSL1L2L3", "test")
    #extrapolate_resolution( fmc, "Pt300to1000_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3", 
    #                        "test")
    #pass
    #plotbase.newplot()
    
    # dont display any graphics
    gROOT.SetBatch( True )
    
    f = plt.figure()

    algo = "AK5PFJets"
    corr = "L1L2L3"
    
    # akp5pf 
    mc_res = plot_resolution( fmc, "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsL1L2L3", 
                    "MC",
                     opt,
                     algo,
                     corr,
                    "YYY_incut/z_pt_AK5PFJetsL1L2L3" )
    
    data_res = plot_resolution( fdata, "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsL1L2L3", 
                    "Run 2011",
                     opt,
                     algo,
                     corr,
                    "YYY_incut/z_pt_AK5PFJetsL1L2L3" )
                    
    ax = f.get_axes()[0]
    ax.set_ylim( 0.0, 0.351 )
    ax.set_xlim( 30.0, 501 )
    ax.set_xscale ("log")

    ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{Z" + r"} / \mathrm{GeV}$",
		  ha="right", x=1)
    ax.set_ylabel(r"Jet Resolution", va="top", y=1)
    ax.legend( frameon=True,  numpoints=1 )
    plotbase.lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    plotbase.statuslabel(ax, opt.status)
    plotbase.energylabel(ax, opt.energy)    
    

    plotbase.jetlabel( ax, "AK5PFJets", "L1L2L3" )
                 
    f.savefig( "resolution_balresp_AK5PFJetsL1L2L3" )

    # plot ratio
    ratio_x = []
    ratio_y = []
    ratio_err = []
    
    for i in range( len ( data_res[0] ) - 1):
	ratio_x += [data_res[0][i]]
	ratio_y += [data_res[1][i] / mc_res[1][i]]
	#ratio_err += [data_res[0][i]]
	
	ratio_err += [ abs( data_res[2][i] * ( 1.0 / mc_res[1][i] ) ) + abs ( mc_res[2][i] * ( data_res[1][i] / ( mc_res[1][i] * mc_res[1][i] ) )) ]

    f = plt.figure()
    plt.errorbar(ratio_x, ratio_y, ratio_err, fmt='o',
	      capsize=2 )
    
    ax = f.get_axes()[0]
    ax.set_ylim( 0.0, 2.5 )
    ax.set_xlim( 30.0, 501 )
    ax.set_xscale ("log")
    ax.axhline(1.0, color="black", linestyle='--')
   
    ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{Z" + r"} / \mathrm{GeV}$",
		  ha="right", x=1)
    ax.set_ylabel(r"( Data/MC ) Jet Resolution", va="top", y=1)
    plotbase.lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    plotbase.statuslabel(ax, opt.status)
    plotbase.energylabel(ax, opt.energy)  
    plotbase.jetlabel( ax, "AK5PFJets", "L1L2L3" )   
    f.savefig( "resolution_balresp_AK5PFJetsL1L2L3_ratio" )
   
   
    # akp5pf CHS
    f = plt.figure()
    algo = "AK5PFJetsCHS"
    corr = "L1L2L3"
    
    mc_res = plot_resolution( fmc, "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3", 
                    "MC",
                     opt,
                     algo,
                     corr,
                    "YYY_incut/z_pt_AK5PFJetsCHSL1L2L3" )
    
    data_res = plot_resolution( fdata, "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3", 
                    "Run 2011",
                     opt,
                     algo,
                     corr,
                    "YYY_incut/z_pt_AK5PFJetsCHSL1L2L3" )
                    
    ax = f.get_axes()[0]
    ax.set_ylim( 0.0, 0.351 )
    ax.set_xlim( 30.0, 501 )
    ax.set_xscale ("log")
    
    ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{Z" + r"} / \mathrm{GeV}$",
		  ha="right", x=1)
    ax.set_ylabel(r"Jet Resolution", va="top", y=1)
    ax.legend( frameon=True,  numpoints=1 )
    plotbase.lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    plotbase.statuslabel(ax, opt.status)
    plotbase.energylabel(ax, opt.energy)  
    plotbase.jetlabel( ax, "AK5PFJets", "CHSL1L2L3" )
                 
    f.savefig( "resolution_balresp_AK5PFJetsCHSL1L2L3" )
    
        # plot ratio
    ratio_x = []
    ratio_y = []
    ratio_err = []
    
    for i in range( len ( data_res[0] ) - 1):
	ratio_x += [data_res[0][i]]
	ratio_y += [data_res[1][i] / mc_res[1][i]]
	#ratio_err += [data_res[0][i]]
	
	ratio_err += [ abs( data_res[2][i] * ( 1.0 / mc_res[1][i] ) ) + abs ( mc_res[2][i] * ( data_res[1][i] / ( mc_res[1][i] * mc_res[1][i] ) )) ]

    f = plt.figure()
    plt.errorbar(ratio_x, ratio_y, ratio_err, fmt='o',
	      capsize=2 )
    
    ax = f.get_axes()[0]
    ax.set_ylim( 0.0, 2.5 )
    ax.axhline(1.0, color="black", linestyle='--')
    ax.set_xlim( 30.0, 501 )
    ax.set_xscale ("log")
   
    ax.set_xlabel(r"$p_\mathrm{T}^\mathrm{Z" + r"} / \mathrm{GeV}$",
		  ha="right", x=1)
    ax.set_ylabel(r"( Data/MC ) Jet Resolution", va="top", y=1)
    plotbase.lumilabel(ax, opt.lumi)    # always (if given) pure MC plots?
    plotbase.statuslabel(ax, opt.status)
    plotbase.energylabel(ax, opt.energy)  
    plotbase.jetlabel( ax, "AK5PFJets", "CHSL1L2L3" )   
    f.savefig( "resolution_balresp_AK5PFJetsCHSL1L2L3_ratio" )
#    plot_resolution( fmc, "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3", 
#                    "mc_",
#                    opt,
#                    "YYY_incut/z_pt_AK5PFJetsCHSL1L2L3" )
#    plot_resolution( fdata, "YYY_incut_var_CutSecondLeadingToZPt_XXX/balresp_AK5PFJetsCHSL1L2L3", 
#                    "data_",
#                    opt,
#                    "YYY_incut/z_pt_AK5PFJetsCHSL1L2L3" )
    #plotbase.Save(fig, "resolution_balresp_AK5PFJetsCHSL1L2L3" , opt, False)


plots = ['mytest']
#plots = ['balanceex', 'mpfex']

