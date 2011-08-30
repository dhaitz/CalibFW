# -*- coding: utf-8 -*-

import getROOT
import sys, os, math #, kein pylab!
import time
import numpy as np
import matplotlib 
import matplotlib.pyplot as plt 
import matplotlib.font_manager as font_man
import copy as copy

from ROOT import TGraphErrors, TCanvas, TF1, TFitResultPtr, TMath
from ROOT import kRed, kBlue


matplotlib.rc('text', usetex=True) 
matplotlib.rc('font', size=16)
#import matplotlib.pyplot as plt
import plotBase

print "%1.2f Start with settings" % time.clock()
### SETTINGS
settings = plotBase.StandardSettings()
settings.outputformats = ['png', 'pdf', 'txt', 'dat']
#settings.outputformats = ['png', 'txt', 'dat']
settings.lumi = 1000
settings.verbosity = 2
#factor = global_factor = 0.158781242981 # qualitycuts
#factor = global_factor = 0.141210916022 # incuts

bins = [ 30, 60, 100, 140, 1000 ]

factor10 = 36 * 0.001 * 0.75
mc11color = 'FireBrick'
mc10color = 'MidnightBlue'
data11color = 'black'
data10color = 'gray'
use_mpl = True
stg = plotBase.StandardSettings()
stg.lumi = 1079

from_folder = "NoBinning_incut/"

###quantity R(ptZ) pt eta phi
###set algo, cone size, #pv, L1L2L3, 
###data/mc(generator,tune)
###document: serif,LaTeX ; slides: sans serif 
#mpl.rc('text', usetex=True)#does not work on the naf! no latex there
#matplotlib.rc('font', family='serif')
#mpl.pylab.rcParams['legend.loc'] = 'best' rc lines.color linewidth font family weight size

#### INPUTFILES
print "%1.2f Open files:" % time.clock()
fdata = getROOT.OpenFile(plotBase.GetPath() + "Run2011A-combined.root", (settings.verbosity > 1))
fmc = getROOT.OpenFile(plotBase.GetPath() + "DYToMuMu_Summer11-PU.root", (settings.verbosity > 1))

def CreateBinStrings (the_bins):
	
	str_bins = []
	
	for i in range (len (the_bins) - 1):
		str_bins += ["Pt" + str(the_bins[i]) + "to" + str(the_bins[i + 1])]

	return str_bins

def IsErrorCompatible( v1, v2, err1, err2 ):



	diff = TMath.Abs(v1-v2);
	errsum = err1+err2;

	print "diff " + str( diff )
	print "errsum " + str( errsum )	
	
	if diff < errsum:
		return True
	else:
		return False

def GetErrErr( rootHisto ):
	sqrtNminus1=TMath.Sqrt( rootHisto.GetEntries() - 1 );
	center_err_err = rootHisto.GetRMSError()/sqrtNminus1;
	
	return 	center_err_err

def extrapolation_prototype(fdatasource,
							folder, #="Pt100to140_incut_var_CutSecondLeadingToZPt_",
							quantity, #="jetresp_ak5PFJetsL1L2L3_Zplusjet_<bin>_data_hist",
							zpt, #="z_pt_ak5PFJetsL1L2L3_Zplusjet_data_hist",
							datatype ="data",
							varvals = [0.1, 0.15, 0.2, 0.3],
							ext_formula="[0]+[1]*(x-[2])"):

  	extr_func = TF1("interpolation1", ext_formula, 0, 1000.0)
  	extr_func.SetLineWidth(2)
  	extr_func.SetLineColor(kRed)

   	extr_func.SetParameter(0, 1)
   	extr_func.SetParameter(1, 1)
   	extr_func.SetParameter(2, 1)
	
	if not use_mpl:
		c = TCanvas ("extrapolation_prototype", "extrapolation_prototype", 600, 600)
		c.SetName(folder + quantity + "_" + datatype)
		c.SetTitle(folder + quantity + "_" + datatype)

	varvals = copy.deepcopy( varvals )	
	varvals.reverse()
	print varvals

	tge = TGraphErrors(len(varvals))
	
	
	tge.SetName(folder)
	tge.SetTitle(folder)
	tge.SetMarkerStyle(24)
	#histname = plotBase.GetNameFromSelection('jetresp', {},  {  'correction': "L1L2", 'bin':"Pt100to140"})
	
	str_bins = CreateBinStrings(bins)

	n = 0

	last_error = 0.0
	first_resp = 0.0
	first_err = 0.0
	first_err_err = 0.0
	this_err = 0.0

	

	for var_val in varvals: 
		
		escaped_varval = str(var_val).replace(".", "_")
		quantity = quantity.replace("<bin>", folder.split("_")[0])
	
		hist_jetresp = getROOT.SafeGet(fdatasource, folder + escaped_varval + "/" + quantity )
		
		tge.SetPoint(n, var_val, hist_jetresp.GetMean())
		
		
		
		print "x " + str(var_val) + " y " + str(hist_jetresp.GetMean())
		print "Err " + str(hist_jetresp.GetMeanError())
		print "ErrErr " + str(GetErrErr(hist_jetresp))
		
		if n == 0:
			extr_func.FixParameter(0, hist_jetresp.GetMean());
  			extr_func.FixParameter(2, var_val);
  			
  			first_resp = hist_jetresp.GetMean()
  			first_var = var_val
  			first_err = hist_jetresp.GetMeanError()
  			first_err_err = GetErrErr( hist_jetresp )
  			this_err= hist_jetresp.GetMeanError()
  		else:
  			this_err = hist_jetresp.GetMeanError()
  			
  			if first_err <  this_err:  			  			
  				this_err=TMath.Sqrt(  hist_jetresp.GetMeanError() * hist_jetresp.GetMeanError() -  first_err * first_err);
  				
  			else: # fix this hack !
	  			#elif IsErrorCompatible(first_err, hist_jetresp.GetMeanError(), 
				#						first_err_err, GetErrErr(hist_jetresp )):
  			 	this_err = 0.5*(first_err_err+GetErrErr(hist_jetresp ));
	  			#else:
	  			#	print "FAIL: errors not in order"
	  			#	exit()
  			 

		tge.SetPointError(n, 0.0, this_err)


		n += 1

	# somehow, this was needed
	#extr_func.ReleaseParameter(0)
		
	fitres = tge.Fit(extr_func, "S");
		
		
	print "Values for extrapolation " + folder
	
	tge.Print()
	
	print "Fit Result"

	# calc the error of the extrapolated value
	#( first_resp + first_err )
	
	m_fit_err =  fitres.GetErrors()[1]
	m_fit = fitres.GetParams()[1]
	b_fit = fitres.GetParams()[0]
	xx_fit = fitres.GetParams()[2]
	
	print str(m_fit) + "  " + str(m_fit_err)
	
	## simple geometrical calculation of the error introduced by tho uncertainty of the slope
	exp_err = extr_func.Eval(0) -  ( (first_resp + first_err) + ( m_fit - m_fit_err ) * ( - first_var ) ) 
	
	upper_exp_err = TF1("upper_exp_err", "[0]+[1]*(x-[2])" )
	upper_exp_err.SetParameter(0, (first_resp + first_err) )
	upper_exp_err.SetParameter(1, ( m_fit - m_fit_err ) )
	upper_exp_err.SetParameter(2, first_var )
	
	upper_exp_err.SetLineColor( kBlue )
	
	
	print exp_err
	
	fitres.Print()
	print "Extrapolated response " + str(extr_func.Eval(0.0))

	if use_mpl:
		# Draw the extrapolation
		tf, ta, tname = plotBase.makeplot("extrapol")
		# Fit function with errorband
		func_x = [x/100.0 for x in range(34)]
		func_y = [m_fit*(x-xx_fit) + b_fit for x in func_x]
		func_yl = [(m_fit+m_fit_err)*(x-xx_fit) + b_fit-first_err for x in func_x]
		func_yh = [(m_fit-m_fit_err)*(x-xx_fit) + b_fit+first_err for x in func_x]
		ta.fill_between(func_x, func_yl, func_yh, facecolor='CornflowerBlue', edgecolor='white', interpolate=True, alpha=0.3)
		fitfct = ta.plot(func_x, func_y, '-', label='extrapolation')
		# Variation data points with uncorrelated and correlated errors
		pygraph = getROOT.ConvertToArray(tge)
		gr_extr1 = ta.errorbar(pygraph.xc, pygraph.y, pygraph.yerr, color='Black', fmt='o', capsize=2, label='uncorrelated')
		for i in range(1, len(pygraph)):
			pygraph.yerr[i] = TMath.Sqrt(  pygraph.yerr[i]*pygraph.yerr[i] + first_err * first_err)
		gr_extr = ta.errorbar(pygraph.xc, pygraph.y, pygraph.yerr, color='FireBrick', fmt='o', capsize=2, label='correlated')
		# Labels and the rest
		ta = plotBase.captions(ta, stg, False)
		ta.legend(loc="upper right", numpoints=1, frameon=False)
		ta = plotBase.AxisLabels(ta, "extrapol", "jet2")
		ta.text(0.04, 0.10, r"$\chi^2 / n_\mathrm{dof} = %1.3f / %d $" % (fitres.Chi2(), fitres.Ndf()),
				va='bottom', ha='left', transform=ta.transAxes, fontsize=16)
		ta.text(0.04, 0.05, r"$R_\mathrm{corr} = %1.3f \pm %1.3f $" % (func_y[0], (func_y[0]-func_yl[0])),
				va='bottom', ha='left', transform=ta.transAxes, fontsize=16)
		plotBase.Save(tf, folder + quantity + "_" + datatype, stg, False)
	else:
		tge.Draw("AP")
		upper_exp_err.Draw("SAME")
		c.Print(c.GetName() + ".png")

	zpt = zpt.replace("<bin>", folder.split("_")[0])
	hist_zpt = getROOT.SafeGet(fdatasource, folder + escaped_varval + "/" + zpt)
	
	return (tge, extr_func, fitres, exp_err, hist_jetresp, hist_zpt) #error 



def extrapolate_ratio( response_measure, algoname, tf_ratio, ta_ratio, do_extrapolation = True, tag = "extrapol", add_to_data = "", folder_prefix = ""  ):	
	
	
	str_bins = CreateBinStrings (bins) 
	
	ext_res_data = []
	
	data_x = []
	data_y = []
	data_yerr = []
	
	data_y_orig = []
	data_y_orig_err = []
	
	mc_x = []
	mc_y = []
	mc_yerr = []
	
	mc_y_orig = []
	mc_y_orig_err = []
	
	
	algoname_data = algoname + add_to_data
	if "CHS" in algoname_data:
		 algoname_data = algoname_data[: len (algoname_data) - 3]
		 algoname_data = algoname_data + add_to_data + "CHS"
		 
	
	for s in str_bins:
		(tge, extr_func, fitres, exp_err,
		   hist_jetresp_orig, hist_zpt) = extrapolation_prototype(fdata,  folder_prefix +  s + "_incut_var_CutSecondLeadingToZPt_",
																	response_measure + "_" + algoname_data,
																	"z_pt_" + algoname_data ,
																	"data")
		data_y += [extr_func.Eval(0.0)]
		data_yerr += [exp_err]
		data_y_orig += [hist_jetresp_orig.GetMean()]
		data_y_orig_err	 += [hist_jetresp_orig.GetMeanError()]
		data_x += [hist_zpt.GetMean()]
		
		#mc_y += [extr_func.Eval(0.0) * 0.98]
		#mc_yerr += [0.0]
		#mc_y_orig += [hist_jetresp_orig.GetMean() * 0.98]
		#mc_y_orig_err	 += [hist_jetresp_orig.GetMeanError()]
		#mc_x += [hist_zpt.GetMean()]
		
		( tge, extr_func, fitres, exp_err, 
		   hist_jetresp_orig, hist_zpt) = extrapolation_prototype( fmc,  
								folder_prefix + s + "_incut_var_CutSecondLeadingToZPt_",
								response_measure + "_" + algoname,
								"z_pt_" + algoname ,
								"mc")
		mc_y += [extr_func.Eval(0.0)]
		mc_yerr += [exp_err]
		mc_y_orig += [hist_jetresp_orig.GetMean()]
		mc_y_orig_err	+= [hist_jetresp_orig.GetMeanError()]
		mc_x += [hist_zpt.GetMean()]
		#extrapolation_prototype( s + "_incut_var_CutBack2Back_", [0.24, 0.34,0.44] )	
	
	# plot data solo
	tf, ta, tname = plotBase.makeplot(response_measure + "_data")
	
	hist = ta.errorbar(np.array(data_x), np.array(data_y),
						yerr=np.array(data_yerr), drawstyle='steps-mid',
							color='black', fmt='o',
							capsize=0, label='data extrapolated')
	
	
	hist_orig = ta.errorbar(np.array(data_x), np.array(data_y_orig),
						yerr=np.array(data_y_orig_err), drawstyle='steps-mid',
							color='red', fmt='o',
							capsize=0, label='data')
	ta = plotBase.captions(ta, settings, False)
	plotBase.AddAlgoAndCorrectionCaption( ta, algoname_data, settings )
	ta.set_ylim( 0.5, 1.1 )
	#ta.set_ylim(top=histo_mc.ymax * 1.2)
	#ta = plotBase.tags(ta, 'Private work', 'Joram Berger')
	ta.legend(numpoints=1, frameon=False)
	ta = plotBase.AxisLabels(ta, "jetpt", "jetpt")
	
	plotBase.Save(tf, response_measure + algoname + "_data", settings, False)
	
	tf, ta, tname = plotBase.makeplot(response_measure + "_mc")
	
	hist = ta.errorbar(np.array(mc_x), np.array(mc_y),
						yerr=np.array(mc_yerr), drawstyle='steps-mid',
							color='black', fmt='o',
							capsize=0, label='mc extrapolated')
	
	hist_orig = ta.errorbar(np.array(mc_x), np.array(mc_y_orig),
						yerr=np.array(mc_y_orig_err), drawstyle='steps-mid',
							color='red', fmt='o',
							capsize=0, label='mc')
	
	ta = plotBase.captions(ta, settings, False)
	plotBase.AddAlgoAndCorrectionCaption( ta, algoname_data, settings )
	#ta.set_ylim(top=histo_mc.ymax * 1.2)
	#ta = plotBase.tags(ta, 'Private work', 'Joram Berger')
	ta.legend(numpoints=1, frameon=False, )
	ta.set_ylim( 0.5, 1.1 )
	ta = plotBase.AxisLabels(ta, response_measure, 'jet')
	
	plotBase.Save(tf, response_measure + algoname + "_mc", settings, False)
	
	# Data / MC agreement
	
	
	
	data_mc_ratio = []
	data_mc_ratio_err =[]
	

	if do_extrapolation:
		# use the extrapolated values
		for both_x, the_mc_y, the_mc_yerr, the_data_y, the_data_yerr in zip(mc_x, mc_y, mc_yerr, data_y, data_yerr):
			data_mc_ratio += [the_data_y / the_mc_y]
			
			# error propagation
			data_mc_ratio_err += [  the_data_yerr / the_mc_y + the_data_y / ( the_mc_y * the_mc_y ) * the_mc_yerr   ]
	else:
		# use the not-extrapolated values
		for both_x, the_mc_y, the_mc_yerr, the_data_y, the_data_yerr in zip(mc_x, mc_y_orig, mc_y_orig_err, data_y_orig, data_y_orig_err):
			data_mc_ratio += [the_data_y / the_mc_y]
			
			# error propagation
			data_mc_ratio_err += [  the_data_yerr / the_mc_y + the_data_y / ( the_mc_y * the_mc_y ) * the_mc_yerr   ]


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
	
	
	c = TCanvas ("extrapolation_prototype", "extrapolation_prototype", 600, 600)
	
	c.SetName("fit_intermediate")
	c.SetTitle("fit_intermediate")
	
	tfit = TGraphErrors(len( mc_x) )
	
	for i in range(0, len(mc_x)):
		print str (  mc_x[i] ) + " : " + str( data_mc_ratio[i] )
		print str (  0 ) + " : " + str( data_mc_ratio_err[i] )
		tfit.SetPoint( i, mc_x[i], data_mc_ratio[i] )
		tfit.SetPointError( i, 0, data_mc_ratio_err[i] )
		
	extr_func = TF1("fit12", "[0]", 0, 1000.0)
	extr_func.SetParameter(0, 1.0)
	
	fitres = tfit.Fit( extr_func, "S")
	
		
	hist = ta_ratio.errorbar(np.array(mc_x), np.array(data_mc_ratio),
						yerr=np.array(data_mc_ratio_err), drawstyle='steps-mid',
							color=the_color, fmt='o',
							 label=the_label)
	
	const_fit_res = fitres.GetParams()[0]
	const_fit_res_err = fitres.GetErrors()[0]
	
	tfit.Draw ("APE")
	c.Print("fit_intermediate.png")
	c.Print("fit_intermediate.root")
	
	print "Fit result of Data/MC ratio = " + str( const_fit_res)
	
	ta_ratio.axhline( const_fit_res, color=the_color )
	ta_ratio.axhspan( const_fit_res - const_fit_res_err,  const_fit_res + const_fit_res_err, color=the_color, alpha = 0.23 )
	
	ta_ratio.axhline( 1.0, color="black", linestyle = '--' )
	 
	plotBase.captions(ta_ratio, settings, False)
	plotBase.AddAlgoAndCorrectionCaption( ta_ratio, algoname_data, settings )
	#ta = plotBase.tags(ta, 'Private work', 'Joram Berger')
	
	#font = font_man.Fo FontProperties( size = 10)
	
	ta_ratio.legend(numpoints=1, frameon=False)#, prop = font)
	plotBase.AxisLabels(ta_ratio, 'datamc_ratio', 'jet')
	ta_ratio.set_ylim( 0.88, 1.12 )
#	ta_ratio.text(0.9, .22, "Overall Ratio (fit) = $" + str( round(const_fit_res,3)) + " \pm " + str( round(const_fit_res_err, 3)) + "$", color = the_color,
#				va='center', ha='right', fontsize=15, transform=ta_ratio.transAxes )
	ta_ratio.text(0.95, .05, "Overall Ratio (fit) = $" + str( round(const_fit_res,3)) + " \pm " + str( round(const_fit_res_err, 3)) + "$", color = the_color,
				va='bottom', ha='right', fontsize=15, transform=ta_ratio.transAxes )
	ta_ratio.minorticks_on()
	
	
def AddEtaRange( ta, eta_range):
	ta.text(0.05, .05, eta_range, 
			va='bottom', ha='left', fontsize=15, transform=ta.transAxes )

def DrawComposition(algoname, bins):
	
	tf, ta, tname = plotBase.makeplot("jetresp_mc")
	# use a finer binning
	ta.set_xlim(18.0,500.0)
	ta.semilogx()
	
	str_bins = CreateBinStrings (bins) 
	name_f = ["jet1_neutralhadronenergy_fraction_", "jet1_chargedhadronenergy_fraction_", 
			  "jet1_neutralemenergy_fraction_", "jet1_electronenergy_fraction_"]
	
	all_x = []
	bin_center_x = []
	y_data = {}
	y_data_err = {}
	y_mc = {}
	bar_width = []
	
	for n in name_f:
		y_data[n] = [ ]
		y_data_err[n] = []
		y_mc[n] = [ ]
				
	i = 0
	for sbin in str_bins:
		zplot = "z_pt_" + algoname

		hist_mc_z = getROOT.SafeGet(fdata, sbin + "_incut/" + zplot )
		#all_x += [ bins[i + 1 ] / 2 ]	
		all_x += [ bins[i] ]	
		bin_center_x += [ hist_mc_z.GetMean() ]
		bar_width += [bins[i + 1 ] - bins[i]]  
		
		
		for n in name_f:
			quantity = n + algoname

			hist_data = getROOT.SafeGet(fdata, sbin + "_incut/" + quantity )
			hist_mc = getROOT.SafeGet(fmc, sbin + "_incut/" + quantity )
		
		
			print n
			print hist_data.GetMean()
		
			y_data[n] += [ hist_data.GetMean() ]
			y_data_err[n] += [ hist_data.GetMeanError()]
			y_mc[n] += [ hist_mc.GetMean() ]
			
		i = i + 1
		
		
	# use mikkos order
	
	#NHF
	#NEF
	#CHF
		
	plt.bar( all_x,  y_mc[ name_f[1] ], width= bar_width, color="#ff6666")
	plt.bar( all_x,  y_mc[ name_f[2] ], bottom= y_mc[ name_f[1] ],
			 width= bar_width, color="#6666ff")
	
	temp_bottom = []
	for i in range ( len ( y_mc[ name_f[1] ])):
		temp_bottom += [  y_mc[ name_f[1]][i] +  y_mc[ name_f[2]][i]  ]
	
	plt.bar( all_x,  y_mc[ name_f[0] ], bottom= temp_bottom,
			 width= bar_width, color="#66ff66")
	
	temp_bottom = []
	for i in range ( len ( y_mc[ name_f[1] ])):
		temp_bottom += [  y_mc[ name_f[1]][i] +  y_mc[ name_f[2]][i]  +  y_mc[ name_f[0]][i] ]
	
	plt.bar( all_x,  y_mc[ name_f[3] ], bottom= temp_bottom,
			 width= bar_width, color="#66ffff")
	
	#data
	print y_data[ name_f[1] ]
	print y_data_err[ name_f[1] ]
	
	plt.errorbar( bin_center_x,  y_data[ name_f[1] ],  y_data_err[name_f[1]], 
				elinewidth=2, color="black", marker = "o", lw = 0, ecolor=None)
	

	temp_bottom = []
	for i in range ( len ( y_data[ name_f[1] ])):
		temp_bottom += [  y_data[ name_f[1]][i] +  y_data[ name_f[2]][i]  ]
		
			
	plt.errorbar( bin_center_x, temp_bottom,  y_data_err[name_f[2]], 
				elinewidth=2, marker = "x", color="black", lw = 0, ecolor=None)

	temp_bottom = []
	for i in range ( len ( y_data[ name_f[1] ])):
		temp_bottom += [  y_data[ name_f[1]][i] +  y_data[ name_f[2]][i] +  y_data[ name_f[0]][i]  ]
		
	plt.errorbar( bin_center_x, temp_bottom,  y_data_err[name_f[0]], 
				elinewidth=2, marker = "*", color="black", lw = 0, ms=6, ecolor=None)

	temp_bottom = []
	for i in range ( len ( y_data[ name_f[1] ])):
		temp_bottom += [  y_data[ name_f[1]][i] +  y_data[ name_f[2]][i] +  y_data[ name_f[0]][i] +  y_data[ name_f[3]][i]  ]
		
	plt.errorbar( bin_center_x, temp_bottom,  y_data_err[name_f[0]], 
				elinewidth=2, marker = "*", color="black", lw = 0, ms=6, ecolor=None)


	plotBase.AxisLabels(ta, 'datamc_diff', 'z')
	ta.set_ylabel("Component Fraction Stack")
	plotBase.Save(tf, algoname + "comp", settings, False)


	## now plot the data - mc
	
	ratio = {}
	ratio_err = {}
	for n in name_f:
		ratio[n] = []
		ratio_err[n] = []
		for i in range ( len ( y_data[ n ])):
			ratio[n] +=  [y_data[ n ][i] -  y_mc[ n ][i]]
			ratio_err[n] += [y_data_err[ n ][i]] # this is error propagation with the MC error neglected
 	

	tf, ta, tname = plotBase.makeplot("jetresp_mc")
	
	ta.set_ylim(-.05, 0.05)
	
	plt.errorbar( bin_center_x, ratio[name_f[1]],  ratio_err[name_f[1]], 
				label = "CHF", fmt = "o", capsize = 2, color = "red")
	plt.errorbar( bin_center_x, ratio[name_f[2]],  ratio_err[name_f[2]], 
				label = "NEF", fmt = "o", capsize = 2, color = "blue")
	plt.errorbar( bin_center_x, ratio[name_f[0]],  ratio_err[name_f[0]], 
				label = "NHF", fmt = "o", capsize = 2, color = "green")	
	plt.errorbar( bin_center_x, ratio[name_f[3]],  ratio_err[name_f[0]], 
				label = "NHF", fmt = "o", capsize = 2, color = "#66ffff")
	
	
	plt.axhline(0.0, color='black', lw=1)
	plotBase.AxisLabels(ta, 'datamc_diff', 'z')
	ta.set_ylabel("Data - MC of Component Fraction") 
	plt.legend()
	
	plotBase.Save(tf, algoname + "comp_ratio", settings, False)
	

#	plt.bar( all_x,  y_data[ name_f[2] ], bottom= y_data[ name_f[1] ],
#			 width= bar_width, color="#6666ff", hatch ="x")
#	
#	temp_bottom = []
#	for i in range ( len ( y_data[ name_f[1] ])):
#		temp_bottom += [  y_data[ name_f[1]][i] +  y_data[ name_f[2]][i]  ]
#	
#	plt.bar( all_x,  y_data[ name_f[0] ], bottom= temp_bottom,
#			 width= bar_width, color="#6666ff", hatch = "+")
#	matplotlib.bar( all_x, nhf_y_data)
		
	




DrawComposition( "ak5PFJetsL1L2L3", bins)

