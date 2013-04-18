# This module contains the dictionaries for the plotting framework


default_settings =  {
        'algorithm'       :'AK5PFJetsCHS',# 
        'correction'      : 'L1L2L3Res',  # 
        'lumi'            : 19790,            # 
        'energy'          : 8,        # 


        'labels'          : ["data", "MC"],
        'colors'          : ['black', '#CBDBF9'],
        'markers'         : ["o","f"],

        
        'text'            : None,         # place text at locatio


        'root'            : False,        # If not False: Save the histogram 
                                          # in a root file. The value of this
                                          # variable is used as the histogram
                                          # name/title
    
        'legloc'          :'center right',# Location of the plot legend. 
                                          # Set "False" for no legend

        'subtext'         : None,         # Subtext that is added at the top 
                                          # left corner 

        'xynames'         : None,         # x-y-axis label names,
                                          # e.g. ['zpt', 'ptbalance']
        
        'rebin'           : 5,            # x-axis rebinning

        'ratio'           : False,        # if true, create a ratio plot
        'ratiosubplot'    : False,        # create an addtional ratio subplot at the bottom

        'selection'       : None,         # selection (cut) expression for 
                                          # C++ expressions are valid e.g.

        'special_binning' : False,        # special binning for npv, zpt, eta
        'extrapolation'   : False,        # alpha-extrapolation: bin, global or false

        'log'             : False,        # logarithmic y-axis
        'xlog'            : False,        # logarithmic x-axis

        'filename'        : None,         # specify filename

        'fit'             : None,         # type of fit
        'fit_offset'      : 0,

        'eventnumberlabel': None,         # add label with number of events

        'save_individually':False,        # for plot functions with several subplots:
                                          # save each plot separately

        'x'               : None,         # x-axis limits [upper, lower]
        'y'               : None,         # y-axis limits [upper, lower]

        'run'             : False,        # 'True': Some special formatting options for runplots
                                          # 'diff': additionaly, subtract MC-mean from data

        'subplot'         : False,        #

        
        'allalpha'        : False,        #
        'alleta'        : False,          #
        'folder'          : 'incut',      # folder in rootfile. 'incut', 'allevents'


    }

#   Quantity:[ax_min, ax_max, Name, unit, z_min, z_max]
d_axes={
        'abseta':[0.0, 5.5, r"$|\eta^\mathrm{%s}|$", ""],
        'absphi':[0, 3.141593, r"$|\phi^\mathrm{%s}|$", ""],
        'alpha':[0, 0.4, r"$p_\mathrm{T}^\mathrm{Jet 2}/p_\mathrm{T}^{Z}$", ""],
        'bal':[0.0, 1.8, r"$p_\mathrm{T}$ balance", ""],
        'balparton':[0.0, 1.8, r"Matching parton$p_\mathrm{T}$ / GenZ $p_\mathrm{T}$ ", ""],
        'balrespjet2':[0, 1, r"$p_\mathrm{T}^\mathrm{Jet 2}/p_\mathrm{T}^{Z}$", ""],
        'balresp':[0.0, 1.8, r"$p_\mathrm{T}$ balance", ""],
        'balresp_ratio':[0.9, 1.1, r"$p_\mathrm{T}$ balance", ""],
        'baltwojet':[0.0, 1.8, r"$p_\mathrm{T}$ balance for 2 jets", ""],
        'chargedem':[0,1, r"%s charged em fraction", ""],
        'chargedhad':[0,1, r"%s charged hadron fraction", ""],
        'components_diff':[-0.05, 0.05, r"Data-MC of Leading Jet Components", ""],
        'components_diff_response':[-0.1, 0.1, r"Data-MC of jet fraction * response", ""],
        'components':[0, 1, r"Leading Jet Component Fraction", ""],
        'components_response':[0, 1.2, r"Leading Jet Component Fraction * Response", ""],
        'constituents':[0, 60, r"Number of Jet Constituents", ""],
        'correction':[0.85, 1.02, "Correction factor", ""],
        'cut':[0, 1.1, r"Cut Inefficiency (%s)", ""],
        'datamcratio':[0.98, 1.01, r"data/MC ratio", ""],
        'deltaeta':[0, 15, r"$\Delta \eta(\mathrm{%s})$", ""],
        'deltaeta':[0, 5, r"$\Delta \eta(\mathrm{%s,\/ %s})$", ""],
        'deltaphi':[0, 3.141593, r"$\Delta \phi(\mathrm{%s,\/%s})$", ""],
        'deltar':[0, 20, r"$\Delta \/R(\mathrm{%s,\/ %s})$", ""],
        'electron':[0,1, r"%s electron fraction", ""],
        'eta':[-5, 5, r"$\eta^\mathrm{%s}$", ""],
        'eventcount':[0, 1.1, r"Eventcount", ""],
        'events':[0, 1, r"Events", ""],
        'extrapol':[0.86, 1.04, r"Response", ""],
        'filters': [-0.5, 12.5, r"Filters", ""],
        'flavourfrc': [0.0, 1.0, r"Flavour fraction", ""],
        'genalpha':[0, 0.4, r"$p_\mathrm{T}^\mathrm{GenJet 2}/p_\mathrm{T}^\mathrm{GenZ}$", ""],
        'genbal-tobalparton':[0.501, 1.501, r"Parton Response $p_\mathrm{T}^\mathrm{GenJet}/p_\mathrm{T}^\mathrm{Parton}$", ""],
        'genbal-toparton':[0.501, 1.501, r"Parton Response $p_\mathrm{T}^\mathrm{GenJet}/p_\mathrm{T}^\mathrm{Parton_1}$", ""],
        'genbalance':[0.501, 1.501, r"$p_\mathrm{T}$ balance (Gen Jet / Gen Z)", ""],
        'genbal':[0.501, 1.501, r"$p_\mathrm{T}$ balance (Gen Jet / Gen Z)", ""],
        'genjet2pt':[ 0, 100, r"$p_\mathrm{T}^\mathrm{GenJet2}$", 'GeV'],
        'genmpf':[0.501, 1.501, r"Gen MPF Response", ""],
        'genzep':[0.0, 3.0, r"Zeppenfeld variable (Gen level)", ""],
        'genzetarapidityratio':[-4, 4, r"$\eta^\mathrm{GenZ} / y^\mathrm{GenZ}$", ""],
        'genzmass':[70, 110, r"$m^\mathrm{GenZ}$", "GeV"],
        'genzrapidity':[-4, 4, r"$y^\mathrm{GenZ}$", ""],
        'genzpt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{GenZ}$", 'GeV'],
        'HFem':[0,1, r"%s HF em fraction", ""],
        'HFhad':[0,1, r"%s HF hadron fraction", ""],
        'jet1area':[0.6, 1, r"Leading Jet area", ""],
        'jet1charged':[0,30, r"Charged constituents of the leading jet", ""],
        'jet1const':[0,30, r"%Leading jet constituents", ""],
        'jet1pt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{Leading Jet}$", 'GeV'],
        'jet2area':[0.6, 1, r"Second Jet area", ""],
        'jet2pt':[ 0, 100, r"$p_\mathrm{T}^\mathrm{Jet2}$", 'GeV'],
        'jet2ratio':[0, 0.4, r"$p_\mathrm{T}^\mathrm{Jet_2}/p_\mathrm{T}^{Z}$", ""],
        'jet3pt':[ 0, 100, r"$p_\mathrm{T}^\mathrm{Jet3}$", 'GeV'],
        'jetpt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{Z}$", 'GeV'],        
        'jetptabsdiff':[0, 100, r"$p_\mathrm{T}^\mathrm{Jet 1} - p_\mathrm{T}^{Jet 2}$", "GeV"],
        'jetptratio':[0, 10, r"$p_\mathrm{T}^\mathrm{Jet 1} / p_\mathrm{T}^{Jet 2}$", ""],
        'jetsvalid':[0, 100, r"Number of valid jets $n$",""],
        'L1':[0, 1.2, r"L1 correction factor",""],
        'L1abs':[0, 30, r"L1 absolute correction","GeV"],
        'L1L2L3':[0, 1.2, r"L1L2L3 correction factor",""],
        'L1L2L3abs':[-25, 25, r"L1L2L3 absolute correction","GeV"],
        'L2':[0, 1.2, r"L2 correction factor",""],
        'L2abs':[-25, 25, r"L2 absolute correction","GeV"],
        'L3':[0, 1.2, r"L3 correction factor",""],
        'L3abs':[-25, 25, r"L3 absolute correction","GeV"],
        'METeta':[-0.1, 0.1, r"$\eta^\mathrm{MET}$", ""],
        'METfraction':[0, 0.2, r"MET / $E^T_Total$", ""],
        'METpt':[ 0, 80, r"$E_\mathrm{T}^\mathrm{miss}$", 'GeV'],
        'METpt-diff':[ 0, 10, r"$\Delta E_\mathrm{T}^\mathrm{miss}$ (TypeI-corrected - raw)", 'GeV'],
        'METsumEt':[0, 2500, r"$\sum E^\mathrm{T}$", "GeV"],
        'mpf':[0.75, 1.2, r"$MPF$ Response", ""],
        'mpf-raw':[0.75, 1.2, r"$MPF$ Response (no type I)", ""],
        'mpf-diff':[0., 0.1, r"$\Delta MPF$ Response (TypeI-corrected - raw)", ""],
        'mpfresp-raw':[0.0, 1.8, r"$MPF$ Response (raw MET)", ""],
        'mpfresp':[0.0, 1.8, r"$MPF$ Response", ""],
        'muminuspt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{\mu-}$", 'GeV'],
        'muon':[0,1, r"%s muon fraction", ""],
        'muonsinvalid':[0, 5, "Number of invalid muons", ""],
        'muonsvalid':[0, 5, "Number of valid muons", ""],
        'mupluspt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{\mu+}$", 'GeV'],
        'muresp': [0.501, 1.501, r"Muon Response $p_\mathrm{T}^\mathrm{\mu,reco}/p_\mathrm{T}^\mathrm{\mu,gen}$", ""],
        'neutralem':[0,1, r"%s neutral em fraction", ""],
        'neutralhad':[0,1., r"%s neutral hadron fraction", ""],
        'npv':[0, 35, r"Number of Reconstructed Vertices $n$",""],
        'numpu':[0, 35, r"Number of Primary Vertices", ""],
        'numputruth':[0, 35, r"Pile-up Truth (Poisson mean)", ""],
        'parton':[0.0, 1.8, r"Leading parton$p_\mathrm{T}$ / $p_\mathrm{T}^\mathrm{GenZ}$ ", ""],
        'phi':[-3.141592653, 3.141592653, r"$\phi^\mathrm{%s}$", ""],
        'photon':[0,1., r"%s photon fraction", ""],
        'photonresponse':[0,1., r"%s photon fraction * response", ""],
        'pt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{%s}$", 'GeV'],        
        'ptbalance':[0.74, 1.04, r"$p_\mathrm{T}$ balance", ""],
        'ratio':[0.95, 1.05, r"%s / %s ratio", ""],
        'rawMETpt':[ 0, 80, r"$E_\mathrm{T}^\mathrm{miss}$ (RAW)", 'GeV'],
        'reco':[0, 35, r"Number of Reconstructed Vertices $n$",""],
        'recogen':[0.501, 1.501, r"Jet Response $p_\mathrm{T}^\mathrm{RecoJet}/p_\mathrm{T}^\mathrm{GenJet}$", ""],
        'response':[0.86, 1.06, r"Jet Response", ""],
        'responseratio':[0.88, 1.03, r"data/MC ratio", ""],
        'resolution':[0, 0.3, r"Jet response resolution", ""],
        'resolutionratio':[.5, 1.5, r"Jet response resolution data/MC ratio", ""],
        'rho':[0, 50, r"$\rho$", ""],
        'run':[190000, 210000, r"Run", ""],
        'sumEt':[0, 2500, r"$\sum E^\mathrm{T}$", "GeV"],
        'summedf':[0.8,1.2, r"$%s$ fraction sum", ""],
        'summedfr':[0.8,1.2, r"$%s$ fraction sum2", ""],
        'xsec':[0, 20, r"$n_\mathrm{Events} / \mathcal{L}$", "pb$^{-1}$"],
        'zeppenfeld':[0.0, 3, r"Zeppenfeld variable", ""],
        'zmass':[70, 110, r"$m^\mathrm{Z}$", "GeV"],
        'zpt':[ 0, 250, r"$p_\mathrm{T}^\mathrm{Z}$", 'GeV'],
        'oldbalresp':[0.0, 1.8, r"$p_\mathrm{T}$ balance (old version)", ""],
        'genbal_tobalparton':[0.0, 1.8, r"", ""],
        'genbal_toparton':[0.0, 1.8, r"", ""],
        'genquality':[0.0, 1.8, r"Distance of Parton/Z matching", ""],
        'genmpf':[0.75, 1.2, r"$MPF$ Response (Gen level)", ""],
        'zresp':[0.501, 1.501, r"Z Response $p_\mathrm{T}^\mathrm{Z, reco}/p_\mathrm{T}^\mathrm{Z,gen}$", ""],
        'zY':[-2.2, 2.2, r"$y^Z$", ""],
        }

d_plots={ 
       # plot:[arguments, function, name]
    'L1L2L3_npv':["changes={'rebin':1}"],
    'L1L2L3abs_npv':["changes={'rebin':1}"],
    'L1_npv':["changes={'rebin':1}"],
    'L2_npv':["changes={'rebin':1}"],
    'bal_twojet':['legloc="lower right"'],
    'balresp':['changes={"fit":"vertical"}'],
    'balresp_all':['changes={"fit":"vertical"}', 'datamc_all', 'balresp'],
    'filters': ['rebin=1, legloc="upper left"'],
    'filters_all': ['rebin=1, legloc="upper left"', 'datamc_all', 'filters'],
    'genzmass_genzpt':['fit="chi2", legloc="lower center"'],
    'genzmass_genzpt_zcutsonly':['legloc="lower center", fit="chi2", changes={"incut":"zcutsonly"}', 'datamcplot', 'genzmass_genzpt'],
    'genzmass_genzpt_nocuts':['legloc="lower center", fit="chi2", changes={"incut":"allevents"}', 'datamcplot', 'genzmass_genzpt'],
    'genbal':['fit="vertical"'],
    'genbalance_genalpha':['fit="intercept", rebin=5'],
    'genbalance_genalpha_alpha03':['fit="intercept", fit_offset=0.3, rebin=5, changes={"var":"var_CutSecondLeadingToZPt_0_3"}', 'datamcplot', 'genbalance_genalpha'],
    'jet1pt':['changes={"log":True}'],
    'jet2eta_jet2phi':['rebin=2'],
    'jet3pt':['log=True, rebin=2'],
    'jeteta_jetphi':['rebin=2'],
    'jetpt_zeta':['rebin=5, legloc="upper left"'],
    'mpf-diff_alpha':['rebin=2, changes={"var":"var_CutSecondLeadingToZPt_0_3"}'],
    'mpf_deltaphi-jet1-MET_all':['', 'datamc_all', 'mpf_deltaphi-jet1-MET'],
    'mpf_deltaphi-z-MET_all':['', 'datamc_all', 'mpf_deltaphi-z-MET'],
    'mpfresp':['changes={"fit":"vertical"}'],
    'mpfresp-raw':['fit="vertical"'],
    'mpfresp_all':['', 'datamc_all', 'mpfresp'],
    'muminusphi':['legloc="lower center"'],
    'muonsinvalid':['legloc="lower center", rebin=1'],
    'muonsvalid':['legloc="lower center", rebin=1'],
    'mupluseta':['legloc="lower center"'],
    'muplusphi':['legloc="lower center"'],
    'mupluspt':['legloc="center right"'],
    'npv':["changes={'rebin':1}"],
    'npv_nocuts':["changes={'rebin':1, 'folder':'allevents'}", 'datamcplot', 'npv'],
    'tworesp':['legloc="lower right"', 'datamcplot', 'bal_twojet'],
    'zpt':["changes={'legloc':'center right', 'rebin':1}"],
    'zpt_all':['', 'datamc_all', 'zpt'],
    'zresp':['rebin=1, fit="vertical"'],
    'parton':['fit="vertical"'],
    'balparton':['fit="vertical"'],
    'genbal-tobalparton':['fit="vertical"'],
    'genbal-tobalparton_alpha03':['fit="vertical", changes={"var":"var_CutSecondLeadingToZPt_0_3"}', 'datamcplot', 'genbal-tobalparton'],
    'recogen':['fit="vertical"'],
    'recogen_alpha03':['fit="vertical", changes={"var":"var_CutSecondLeadingToZPt_0_3"}', 'datamcplot', 'recogen'],
    } 
