# -*- coding: utf-8 -*-

import getROOT
import math
import copy

from ROOT import TGraphErrors, TCanvas, TF1
import plotBase


#from_folder = "NoBinning_incut/"

def BinStrings(the_bins):
	return [ "Pt" + str(the_bins[i]) + "to" + str(the_bins[i+1]) for i in range(len(the_bins)-1) ]	

def IsErrorCompatible( v1, v2, err1, err2 ):
	return (abs(v1-v2) < err1+err2)

def GetErrErr( rootHisto ):
	sqrtNminus1 = math.sqrt( rootHisto.GetEntries() - 1 )
	return rootHisto.GetRMSError() / sqrtNminus1



def extrapolateRatio(fdata, fmc,
					folder, #="Pt100to140_incut_var_CutSecondLeadingToZPt_",
					histo, #="jetresp_ak5PFJetsL1L2L3CHS_hist",
	#				zpt, #="z_pt_ak5PFJetsL1L2L3_hist",
					var = [0.1, 0.15, 0.2, 0.3],
					fit_formula = "[0]+[1]*(x-[2])",
					drawPlot = True,
					opt=plotBase.commandlineOptions()):

	print "Fit", fit_formula, "to [" + ", ".join("%1.3f" % v for v in var) + "]"
	var = copy.deepcopy(var)
	var.reverse()

  	fitFunc = TF1("fit1", fit_formula, 0, 1000.0)
   	fitFunc.SetParameters(1.0, 0.001, 0.3)
	graph = TGraphErrors(len(var))

	n = 0
	x0 = y0 = y0err = y0errerr = yerr = 0.0
	
	# for every variation point there is a point to be set and errors
	for x in var: 
		# get the histograms
		escaped_var = str(x).replace(".", "_")   # 0.3 -> 0_3
		hist_respdata = getROOT.SafeGet(fdata, folder + escaped_var + "/" + histo)
		hist_respmc = getROOT.SafeGet(fmc, folder + escaped_var + "/" + histo)	
		# read the ratio and error (propagated)
		y = hist_respdata.GetMean()/hist_respmc.GetMean()
		yerr = y * (hist_respdata.GetMeanError() / hist_respdata.GetMean() +
					hist_respmc.GetMeanError() / hist_respmc.GetMean() )
		yerrerr = GetErrErr(hist_respdata) + GetErrErr(hist_respmc)
		
		print "   %1.2f | %1.4f +- (%1.4f +- %1.4f)" % (x, y, yerr, yerrerr),
		
		if n == 0: #remember values for first point var[0]
  			y0 = y
  			x0 = x
  			y0err = yerr
  			y0errerr = yerrerr
  		else:
  			if y0err < yerr:  	
				#uncorrelate errors		  			
  				yerr = math.sqrt( yerr**2 - y0err**2)
  			else: 
  			 	yerr = yerr
				print " >>>>  This is not ok: Errors are not in order!",
		print 
		# set the point
		graph.SetPoint(n, x, y)
		graph.SetPointError(n, 0.0, yerr)

		n += 1

	# Fix the parameters and do the fit
	fitFunc.FixParameter(0, y0)
	fitFunc.FixParameter(2, x0)
	fitres = graph.Fit(fitFunc, "SQN")

	print "Fit Result for extrapolation " + folder + ":"

	m_fit_err = fitres.GetErrors()[1]
	m_fit = fitres.GetParams()[1]
	
	print "Slope: m = %1.3f +- %1.3f" % (m_fit, m_fit_err)
	
	## simple geometrical calculation of the error introduced by the uncertainty of the slope
	yex = y0 - m_fit*x0	
	yex_err = y0err + m_fit_err*x0

	if drawPlot:
		drawExtrapolation(graph, fitres, m_fit, m_fit_err, y0, x0, y0err, folder, histo, opt)

	print "Extrapolated response: R = %1.4f +- %1.4f" % (yex, yex_err)

	hist_zpt = getROOT.SafeGet(fdata, folder + escaped_var + "/z_pt_ak5PFJetsL1L2L3CHS_hist")
	zpt = hist_zpt.GetMean()
	return yex, yex_err, zpt



def drawExtrapolation(graph, fitres, m, merr, b, x0, y0err, folder, quantity, opt):
	graph = getROOT.ConvertToArray(graph)
	fig, ax = plotBase.newplot()
	# Fit function with errorband
	func_x = [x/100.0 for x in range(34)]
	func_y = [m*(x-x0) + b for x in func_x]
	func_yl = [(m+merr)*(x-x0) + b - y0err for x in func_x]
	func_yh = [(m-merr)*(x-x0) + b + y0err for x in func_x]
	ax.fill_between(func_x, func_yl, func_yh, facecolor='CornflowerBlue', edgecolor='white', interpolate=True, alpha=0.3)
	fitfct = ax.plot(func_x, func_y, '-', label='extrapolation')
	# Cut variation data points with uncorrelated and correlated errors
	if graph.yerr[0] != y0err: print "\n\n!!!!!!!!!", graph.yerr[0], "!=", y0err, "\n\n"
	gr_extr1 = ax.errorbar(graph.xc, graph.y, graph.yerr, color='Black', fmt='o', capsize=2, label='uncorrelated')
	for i in range(1, len(graph)):
		if graph.yerr[i] < y0err:
			graph.yerr[i] = math.sqrt(  graph.yerr[i]**2 + y0err**2 )
	gr_extr = ax.errorbar(graph.xc, graph.y, graph.yerr, color='FireBrick', fmt='o', capsize=2, label='correlated')
	# Legend and labels
	ax = plotBase.captions(ax, opt)
	ax.legend(loc="upper right", numpoints=1, frameon=False)
	ax = plotBase.AxisLabels(ax, "extrapol", "jet2")
	ax.text(0.04, 0.11, r"$\chi^2 / n_\mathrm{dof} = %1.3f / %d $" % (fitres.Chi2(), fitres.Ndf()),
			va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
	ax.text(0.04, 0.05, r"$R_\mathrm{corr} = %1.3f \pm %1.3f $" % (func_y[0], (func_y[0]-func_yl[0])),
			va='bottom', ha='left', transform=ax.transAxes, fontsize=18)
	ax.text(0.96, 0.05, r"$%s < p_\mathrm{T}^\mathrm{Z} / \mathrm{GeV} < %s$" % (folder[folder.find('Pt')+2:folder.find('to')],folder[folder.find('to')+2:folder.find('_incut')]),
			va='bottom', ha='right', transform=ax.transAxes, fontsize=18)

	plotBase.Save(fig, folder + quantity + "_RATIOTEST", opt, False)




def extrapolate_ratio( response_measure, fdata, fmc, opt, do_extrapolation = True, tag = "extrapol", add_to_data = "", folder_prefix = "" ):
	tf_ratio, ta_ratio = plotBase.newplot()
	bins=opt.bins
	if bins[0]==0: bins.pop(0)
	str_bins = BinStrings (bins) 
	
	ext_res_data = []
	ratioEx = getROOT.nHisto()
	algoname = opt.algorithm + opt.correction
	
	algoname_data = algoname + add_to_data
	if "CHS" in algoname_data:
		 algoname_data = algoname_data[: len (algoname_data) - 3] + add_to_data + "CHS"
		 
	# loop over bins
	for s in str_bins:
		# extrapolate ratio
		yex, yex_err, x = extrapolateRatio(fdata, fmc,  
				folder_prefix +  s + "_incut_var_CutSecondLeadingToZPt_",
				response_measure + "_" + algoname_data + "_hist")
		ratioEx.y    += [yex]
		ratioEx.yerr += [yex_err]
		ratioEx.x    += [x]

	ext_str = ""
	if do_extrapolation:
		ext_str = " extr"
		
	if response_measure == "jetresp":
		if tag == "extrapol":
			the_color = 'blue'
			the_label = 'Balance' + ext_str
		else:			
			the_color = 'grey' 	
			the_label = 'Balance'

			
	if response_measure == "mpfresp": 
		the_color = 'blue'
		the_label = 'MPF' + ext_str
	
	
	tfit = TGraphErrors(len(ratioEx.x))
	
	for i in range(0, len(ratioEx.x)):
		print "   %2d:% 7.2f | %1.4f +- %1.4f" % (i, ratioEx.x[i], ratioEx.y[i], ratioEx.yerr[i] )
		tfit.SetPoint( i, ratioEx.x[i], ratioEx.y[i] )
		tfit.SetPointError( i, 0, ratioEx.yerr[i] )
		
	extr_func = TF1("fit12", "[0]", 0.0, 1000.0)
	extr_func.SetParameter(0, 1.0)
	
	fitres = tfit.Fit( extr_func, "SQN")
	
		
	hist = ta_ratio.errorbar(ratioEx.x, ratioEx.y,ratioEx.yerr, drawstyle='steps-mid',
							color=the_color, fmt='o', label=the_label)
	
	const_fit_res = fitres.GetParams()[0]
	const_fit_res_err = fitres.GetErrors()[0]
	

	print "Fit result: overall ratio = %1.4f +- %1.4f" % (const_fit_res, const_fit_res_err)
	
	ta_ratio.axhline( const_fit_res, color=the_color )
	ta_ratio.axhspan( const_fit_res - const_fit_res_err,  const_fit_res + const_fit_res_err, color=the_color, alpha = 0.23 )
	
	ta_ratio.axhline( 1.0, color="black", linestyle = '--' )
	 
	plotBase.captions(ta_ratio, opt)
	
	#font = font_man.Fo FontProperties( size = 10)
	
	ta_ratio.legend(numpoints=1, frameon=False)
	plotBase.AxisLabels(ta_ratio, 'datamc_ratio', 'jet')
	ta_ratio.set_ylim( 0.88, 1.12 )
#	ta_ratio.text(0.9, .22, "Overall Ratio (fit) = $" + str( round(const_fit_res,3)) + " \pm " + str( round(const_fit_res_err, 3)) + "$", color = the_color,
#				va='center', ha='right', fontsize=15, transform=ta_ratio.transAxes )
	ta_ratio.text(0.95, .05, "Overall Ratio (fit) = $%1.3f \pm %1.3f$" % (const_fit_res, const_fit_res_err),
			 color = the_color,	va='bottom', ha='right', fontsize=15, transform=ta_ratio.transAxes )
	ta_ratio.minorticks_on()
	plotBase.Save(tf_ratio, response_measure+"_ratio_"+opt.algorithm+opt.correction+"_ratio_beforextrapol", opt, False)
	
def AddEtaRange( ta, eta_range):
	ta.text(0.05, .05, eta_range, 
			va='bottom', ha='left', fontsize=15, transform=ta.transAxes )
	return ta



# Balance 

def balanceex(fdata, fmc, opt):
	extrapolate_ratio("jetresp", fdata, fmc, opt, True, "extrapol")


# MPF 

def mpfex(fdata, fmc, opt):
	extrapolate_ratio("mpfresp", fdata, fmc, opt, False, "no_extrapol")
	#AddEtaRange(ta, r"$|\eta| < 1.3$")



plots = ['balanceex', 'mpfex']

if __name__ == "__main__":
	fdata = getROOT.openFile(plotBase.GetPath() + "data_Oct19.root")
	fmc = getROOT.openFile(plotBase.GetPath() + "pythia_Oct19.root")
	bins = plotBase.guessBins(fdata, [0, 30, 40, 50, 60, 75, 95, 125, 180, 300, 1000]) #binning must be after file open. plots do this later: if bins[0] == 0 bins.pop(0)
	balanceex(fdata, fmc, opt=plotBase.commandlineOptions(bins=bins))
	mpfex(fdata, fmc, opt=plotBase.commandlineOptions(bins=bins))
