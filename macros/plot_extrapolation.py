# -*- coding: utf-8 -*-

import getROOT
import sys, os, math #, kein pylab!
import time
import numpy as np
import matplotlib
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
#settings.outputformats = ['png', 'pdf', 'txt', 'dat']
settings.outputformats = ['png', 'txt', 'dat']
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
							type ="data",					 
							varvals = [0.1, 0.15, 0.2, 0.3],
							ext_formula="[0]+[1]*(x-[2])"):

  	extr_func = TF1("interpolation1", ext_formula, 0, 1000.0)
  	extr_func.SetLineWidth(2)
  	extr_func.SetLineColor(kRed)

   	extr_func.SetParameter(0, 1)
   	extr_func.SetParameter(1, 1)
   	extr_func.SetParameter(2, 1)
	
	c = TCanvas ("extrapolation_prototype", "extrapolation_prototype", 600, 600)
	
	c.SetName(folder + quantity + "_" + type)
	c.SetTitle(folder + quantity + "_" + type)

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
	
	tge.Draw("AP")
	upper_exp_err.Draw("SAME")
	c.Print(c.GetName() + ".png")
	
	#getROOT.ConvertToArray( tge )
	# draw as nice MatPlotLibPlot
	
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
	
	
	algoname_data = algoname
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


#extrapolate_ratio("jetresp", "ak5PFJets")
#extrapolate_ratio("mpfresp", "ak5PFJets")

#tf, ta, tname = plotBase.makeplot("jetresp_mc")
#extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta)
#extrapolate_ratio("mpfresp", "ak5PFJetsL1L2L3", tf, ta)
#plotBase.Save(tf, "resp_ratio_ak5PFJetsL1L2L3_data_mc_ratio_extrapol", settings, False)

#tf, ta, tname = plotBase.makeplot("jetresp_mc")
#extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta, False)
#extrapolate_ratio("mpfresp", "ak5PFJetsL1L2L3", tf, ta, False)
#plotBase.Save(tf, "resp_ratio_ak5PFJetsL1L2L3_data_mc_ratio_noextrapol", settings, False)

#tf, ta, tname = plotBase.makeplot("jetresp_mc")
#extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta, False)
#extrapolate_ratio("mpfresp", "ak5PFJetsL1L2L3", tf, ta, False)
#plotBase.Save(tf, "jetresp_ratio_ak5PFJetsL1L2L3_data_mc_ratio_noextrapol", settings, False)


#tf, ta, tname = plotBase.makeplot("jetresp_mc")
#extrapolate_ratio("jetresp", "ak7PFJetsL1L2L3", tf, ta, False)
#extrapolate_ratio("mpfresp", "ak7PFJetsL1L2L3", tf, ta, False)
#plotBase.Save(tf, "jetresp_ratio_ak7PFJetsL1L2L3_data_mc_ratio_noextrapol", settings, False)


#extrapolate_ratio("jetresp", "ak5PFets")
#extrapolate_ratio("mpfresp", "ak5PFets")

# !! barrel

# Balance 

tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta, True, "extrapol", "Res")
AddEtaRange(ta, "$|\eta| < 1.3$")
plotBase.Save(tf, "jetresp_ratio_ak5PFJetsL1L2L3Res_data_mc_ratio_extrapol", settings, False)

tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta, True, "extrapol", "")
AddEtaRange(ta, "$|\eta| < 1.3$")
plotBase.Save(tf, "jetresp_ratio_ak5PFJetsL1L2L3_data_mc_ratio_extrapol", settings, False)

#chs
tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3CHS", tf, ta, True, "extrapol", "")
AddEtaRange(ta, "$|\eta| < 1.3$")
plotBase.Save(tf, "jetresp_ratio_ak5PFJetsL1L2L3CHS_data_mc_ratio_extrapol", settings, False)


#chs
tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3CHS", tf, ta, True, "extrapol", "Res")
AddEtaRange(ta, "$|\eta| < 1.3$")
plotBase.Save(tf, "jetresp_ratio_ak5PFJetsL1L2L3ResCHS_data_mc_ratio_extrapol", settings, False)



# MPF 
tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("mpfresp", "ak5PFJetsL1", tf, ta, False, "no_extrapol", "")
AddEtaRange(ta, "$|\eta| < 1.3$")
plotBase.Save(tf, "mpfresp_ratio_ak5PFJetsL1_data_mc_ratio", settings, False)

#chs
tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("mpfresp", "ak5PFJetsL1CHS", tf, ta, False, "no_extrapol", "")
AddEtaRange(ta, "$|\eta| < 1.3$")
plotBase.Save(tf, "mpfresp_ratio_ak5PFJetsL1CHS_data_mc_ratio", settings, False)


# !! ENDCAP

# Balance 

tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta, True, "extrapol", "Res", "endcap_")
ta.set_ylim( 0.78, 1.22 )
AddEtaRange(ta, "$1.5 < |\eta| < 2.4$")
plotBase.Save(tf, "endcap_jetresp_ratio_ak5PFJetsL1L2L3Res_data_mc_ratio_extrapol", settings, False)

tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("jetresp", "ak5PFJetsL1L2L3", tf, ta, True, "extrapol", "", "endcap_")
ta.set_ylim( 0.78, 1.22 )
AddEtaRange(ta, "$1.5 < |\eta| < 2.4$")
plotBase.Save(tf, "endcap_jetresp_ratio_ak5PFJetsL1L2L3_data_mc_ratio_extrapol", settings, False)


# MPF 
tf, ta, tname = plotBase.makeplot("jetresp_mc")
extrapolate_ratio("mpfresp", "ak5PFJetsL1", tf, ta, False, "no_extrapol", "", "endcap_")
ta.set_ylim( 0.78, 1.22 )
AddEtaRange(ta, "$1.5 < |\eta| < 2.4$")

plotBase.Save(tf, "endcap_mpfresp_ratio_ak5PFJetsL1L2L3_data_mc_ratio", settings, False)


