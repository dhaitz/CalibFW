# -*- coding: utf-8 -*-
"""
"""
import math
import copy
from ROOT import TGraphErrors, TCanvas, TF1, TFile

import plotbase
import getroot


def binstrings(bins):
    return ["Pt{0}to{1}".format(bins[i], bins[i + 1])
            for i in range(len(bins) - 1)]


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

    print "Fit", fit_formula, "[" + ", ".join("%1.3f" % v for v in var) + "]"
    var = copy.deepcopy(var)
    var.reverse()

    fitFunc = TF1("fit1", fit_formula, 0, 1000.0)
    fitFunc.SetParameters(1.0, 0.001, 0.3)
    graph = TGraphErrors(len(var))

    n = 0
    x0 = y0 = y0err = y0errerr = yerr = 0.0


    if doExtrapolation == False:
  
        folder_var = folder.replace( "XXX", str(0.2).replace(".", "_"))   # 0.3 -> 0_3
        
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
    for x in var:
        # get the histograms
        folder_var = folder.replace( "XXX", str(x).replace(".", "_"))   # 0.3 -> 0_3
        hist_respdata = getroot.getobject( folder_var + "/" + histo, fdata)
        hist_respmc = getroot.getobject( folder_var + "/" + histo, fmc)
        # read the ratio and error (propagated)
        
        if hist_respmc.GetMean() < 0.0000001 or hist_respdata.GetMean() < 0.0000001:
	    plotbase.fail( "No valid response available for the region " + folder_var ) 
	    
        
        y = hist_respdata.GetMean() / hist_respmc.GetMean()
        yerr = y * (hist_respdata.GetMeanError() / hist_respdata.GetMean() +
                    hist_respmc.GetMeanError() / hist_respmc.GetMean())
        yerrerr = get_errerr(hist_respdata) + get_errerr(hist_respmc)

        print "   %1.2f | %1.4f +- (%1.4f +- %1.4f)" % (x, y, yerr, yerrerr),

        if n == 0:  # remember values for first point var[0]
            y0 = y
            x0 = x
            y0err = yerr
            y0errerr = yerrerr
        else:
            if y0err < yerr:
            # uncorrelate errors
                yerr = math.sqrt(yerr ** 2 - y0err ** 2)
            else:
                yerr = yerr
		print " >>>>  This is not ok: Errors are not in order!",

	# set the point
        graph.SetPoint(n, x, y)
        graph.SetPointError(n, 0.0, yerr)

        n += 1

    # Fix the parameters and do the fit
    fitFunc.FixParameter(0, y0)
    fitFunc.FixParameter(2, x0)
    fitres = graph.Fit(fitFunc, "SQN")
    m_fit_err = fitres.GetErrors()[1]
    m_fit = fitres.GetParams()[1]

    print "Fit Result for extrapolation " + folder + ":"
    print "Slope: m = %1.3f +- %1.3f" % (m_fit, m_fit_err)

    # simple geometrical calculation of the error introduced by the
    # uncertainty of the slope
    yex = y0 - m_fit * x0
    yex_err = y0err + m_fit_err * x0

    if drawPlot:
        draw_extrapolation(graph, fitres, m_fit, m_fit_err, y0, x0, y0err,
                           folder, histo, opt)

    print "Extrapolated response: R = %1.4f +- %1.4f" % (yex, yex_err)

    hist_zpt = getroot.getobject( folder_var +
                                 "/z_pt_AK5PFJetsL1L2L3", fdata)
    zpt = hist_zpt.GetMean()    
    return yex, yex_err, zpt


def draw_extrapolation(graph, fitres, m, merr, b, x0, y0err, folder, quantity,
                       opt):
    graph = getroot.root2histo(graph)
    # Fit function with errorband
    func_x = [x / 100.0 for x in range(34)]
    func_y = [m * (x - x0) + b for x in func_x]
    func_yl = [(m + merr) * (x - x0) + b - y0err for x in func_x]
    func_yh = [(m - merr) * (x - x0) + b + y0err for x in func_x]

    fig, ax = plotbase.newplot()
    ax.fill_between(func_x, func_yl, func_yh, facecolor='CornflowerBlue',
                    edgecolor='white', interpolate=True, alpha=0.3)
    ax.plot(func_x, func_y, '-', label='extrapolation')
    # Cut variation data points with uncorrelated and correlated errors
    if graph.yerr[0] != y0err:
        print "\n\n!!!!!!!!!", graph.yerr[0], "!=", y0err, "\n\n"
    ax.errorbar(graph.xc, graph.y, graph.yerr, color='Black', fmt='o',
                capsize=2, label='uncorrelated')
    for i in range(1, len(graph)):
        if graph.yerr[i] < y0err:
            graph.yerr[i] = math.sqrt(graph.yerr[i] ** 2 + y0err ** 2)
    ax.errorbar(graph.xc, graph.y, graph.yerr, color='FireBrick', fmt='o',
                capsize=2, label='correlated')
    # Legend and labels
    ax = plotbase.labels(ax, opt, legloc='upper right')
    ax = plotbase.axislabels(ax, "jet2ratio", "extrapol")
    ax.text(0.04, 0.11, r"$\chi^2 / n_\mathrm{{dof}} = {0:.3f} / {1}$".format(
                        fitres.Chi2(), fitres.Ndf()),
            va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
    ax.text(0.04, 0.05, r"$R_\mathrm{{corr}} = {0:.3f} \pm {1:.3f} $".format(
                        func_y[0], (func_y[0] - func_yl[0])),
            va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
    ax.text(0.96, 0.05, (r"${0} < p_\mathrm{{T}}^\mathrm{{Z}} / \mathrm{{GeV}}"
                     r" < {1}$").format(
                         folder[folder.find('Pt') + 2:folder.find('to')],
                         folder[folder.find('to') + 2:folder.find('_incut')]),
            va='bottom', ha='right', transform=ax.transAxes, fontsize=18)

    plotbase.Save(fig, folder + quantity + "_RATIOTEST", opt, False)


def extrapolate_ratio( fig, method, fdata, fmc, opt, tag,
                      folder_prefix="", folder_postfix = "_incut_var_CutSecondLeadingToZPt_XXX" ,
                      file_name_prefix = "",
                      plot_result = True,
                      extrapolation = True,
                      store_tmp_files = False):
    bins = copy.deepcopy(opt.bins)
    if bins[0] == 0:
        bins.pop(0)
    str_bins = binstrings(bins)

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

def response_ratio(fdata, fmc, method='balresp', extrapol=None,
        algorithm=plotbase.options().algorithm,
        correction=plotbase.options().correction, fit=None):
    return getroot.Histo()


def getResponseRatioOverNPV( fdata, fmc,  opt, method, do_extrapolation ):
    hst = getroot.Histo()
    
    for n in opt.npv:
	npvname = "Npv_" + str(n[0]) + "to" + str(n[1])
	
	# litlte hack, the new version of the code writes out the number of primary vetrices
	if n[1] == 100:
	    meannpv = ( n[0] + 14.0 ) / 2.0
	else:
	    meannpv = ( n[0] + n[1] ) / 2.0
      
	the_folder_postfix = "_incut_var_CutSecondLeadingToZPt_XXX_var_" + npvname

	extr_res = extrapolate_ratio(None, method, fdata, fmc, opt, "extrapol", 
			  folder_postfix = the_folder_postfix,
			  file_name_prefix = npvname,
			  extrapolation = do_extrapolation,
			  plot_result = False)
		
	hst.x += [ meannpv ]
	hst.y += [ extr_res [ 0 ] ]
	hst.yerr += [ extr_res [ 1 ] ]
	
    return hst

def plolResponseRatioOverNPV( fdata, fmc,  opt, method, use_extrapolation, the_label ):
    hst = getResponseRatioOverNPV( fdata, fmc, opt, method, do_extrapolation = use_extrapolation )
  
    # in relation to NPV for the constant factor
    fig, ax = plotbase.newplot()

    ax.errorbar( hst.x, hst.y, hst.yerr, color='FireBrick', fmt='o',
                capsize=2, label= the_label)    

    i = 0        
    for n in opt.npv:
        if i > 0:
            ax.axvline( n[0], color="blue", alpha=0.5, linestyle='-')
	#if i < ( len(opt.npv) - 1):
	#    ax.axvline( n[1], color="black", linestyle='-')
	
	i += 1
                
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

plots = ['balanceex', 'mpfex']

