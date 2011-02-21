# -*- coding: iso-8859-1 -*-
# here the burocracy
#
# Sample execution
# make_extrapolation.exe $CFGPATH/ZPJ_2010/make_extrapolation.py pt_extrapolation pt_extrapolationL1 pt_extrapolation_pu pt_extrapolation_puL1  
# deltaphi_extrapolation deltaphi_extrapolationL1 deltaphi_extrapolation_pu deltaphi_extrapolation_puL1
#

import ZPJConfiguration

process=ZPJConfiguration.configuration("my_cfg")
p=process

basedir="/home/piparo/Paper/"

the_filename_data=basedir+"extrapolation_data.root"
the_filename_mc=basedir+"extrapolation_dy_pu.root"
the_filename_mc_pu=basedir+"extrapolation_dy_pu.root"
             
the_pt_bins=["Pt10to500"]


#the_filename_data="/home/piparo/DiplomarbeitThomas/extrapolation_1_2_data.root"
#the_filename_mc="/home/piparo/DiplomarbeitThomas/extrapolation_dy_pu_1_2.root"
#the_filename_mc_pu="/home/piparo/DiplomarbeitThomas/extrapolation_dy_pu_1_2.root"


#the_pt_bins=["Pt0to10",
             #"Pt10to30",
             #"Pt30to60",
             #"Pt60to100",
             #"Pt10to500"]

the_pt_bins=["Pt30to60","Pt60to100","Pt100to140","Pt140to500","Pt10to500"]


noL1_noPU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{uncorrected}}{No PU simulated}}"
noL1_PU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{uncorrected}}{PU simulated}}"
L1_noPU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{jet area PU und UE subtracted}}{No PU simulated}}"
L1_PU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{jet area PU und UE subtracted}}{PU simulated}}"


#---------------------------------------------
# 2nd pt jet cut 
the_2nd_pt_cut_vals = [ 0.2, 0.15, .1,.05]
the_balance_histo_names_data=\
["@PTBIN@_incut_var_CutSecondLeadingToZPt_0_2/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutSecondLeadingToZPt_0_15/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutSecondLeadingToZPt_0_1/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutSecondLeadingToZPt_0_05/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist"]

the_mpf_histo_names_data=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_data)
the_balance_histo_names_mc=map(lambda name: name.replace("data","mc"),the_balance_histo_names_data)
the_mpf_histo_names_mc=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_mc)

#---------------------------------------------
# DeltaPhi
the_deltaphi_cut_vals = [.34,.24,.14,0.04,0.02]
the_balance_histo_names_data_deltaphi=\
["@PTBIN@_incut_var_CutBack2Back_0_34/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutBack2Back_0_24/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutBack2Back_0_14/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutBack2Back_0_04/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist",
"@PTBIN@_incut_var_CutBack2Back_0_02/jetresp_ak5PFJets_Zplusjet_data_@PTBIN@_hist"]

the_mpf_histo_names_data_deltaphi=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_data_deltaphi)
the_balance_histo_names_mc_deltaphi=map(lambda name: name.replace("data","mc"),the_balance_histo_names_data_deltaphi)
the_mpf_histo_names_mc_deltaphi=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_mc_deltaphi)


#-------------------------------------------------------------------------------
makeL1 = lambda name: name.replace("ak5PFJets","ak5PFJetsL1")
#-------------------------------------------------------------------------------

p.pt_extrapolation=ZPJConfiguration.section(\
    "pt_extrapolation",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc,
    comment = noL1_noPU_latex,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",
    balance_histo_names_data=the_balance_histo_names_data,
    mpf_histo_names_data=the_mpf_histo_names_data,
    balance_histo_names_mc=the_balance_histo_names_mc,
    mpf_histo_names_mc=the_mpf_histo_names_mc,
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------

p.pt_extrapolationL1=ZPJConfiguration.section(\
    "pt_extrapolationL1",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc,
    comment = L1_noPU_latex,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",
    balance_histo_names_data=map(makeL1,the_balance_histo_names_data),
    mpf_histo_names_data=map(makeL1,the_mpf_histo_names_data),
    balance_histo_names_mc=map(makeL1,the_balance_histo_names_mc),
    mpf_histo_names_mc=map(makeL1,the_mpf_histo_names_mc),
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------

p.pt_extrapolation_pu=ZPJConfiguration.section(\
    "pt_extrapolation_pu",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment = noL1_PU_latex,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",
    balance_histo_names_data=the_balance_histo_names_data,
    mpf_histo_names_data=the_mpf_histo_names_data,
    balance_histo_names_mc=the_balance_histo_names_mc,
    mpf_histo_names_mc=the_mpf_histo_names_mc,
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------

p.pt_extrapolation_puL1=ZPJConfiguration.section(\
    "pt_extrapolation_puL1",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment = L1_PU_latex,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",
    balance_histo_names_data=map(makeL1,the_balance_histo_names_data),
    mpf_histo_names_data=map(makeL1,the_mpf_histo_names_data),
    balance_histo_names_mc=map(makeL1,the_balance_histo_names_mc),
    mpf_histo_names_mc=map(makeL1,the_mpf_histo_names_mc),
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------
# Deltaphi
p.deltaphi_extrapolation=ZPJConfiguration.section(\
    "deltaphi_extrapolation",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc,
    comment =  noL1_noPU_latex,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",
    balance_histo_names_data=the_balance_histo_names_data_deltaphi,
    mpf_histo_names_data=the_mpf_histo_names_data_deltaphi,
    balance_histo_names_mc=the_balance_histo_names_mc_deltaphi,
    mpf_histo_names_mc=the_mpf_histo_names_mc_deltaphi,
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------

p.deltaphi_extrapolationL1=ZPJConfiguration.section(\
    "deltaphi_extrapolationL1",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc,
    comment = L1_noPU_latex,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",
    balance_histo_names_data=map(makeL1,the_balance_histo_names_data_deltaphi),
    mpf_histo_names_data=map(makeL1,the_mpf_histo_names_data_deltaphi),
    balance_histo_names_mc=map(makeL1,the_balance_histo_names_mc_deltaphi),
    mpf_histo_names_mc=map(makeL1,the_mpf_histo_names_mc_deltaphi),
    pt_bins=the_pt_bins)


#-------------------------------------------------------------------------------
p.deltaphi_extrapolation_pu=ZPJConfiguration.section(\
    "deltaphi_extrapolation_pu",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment =  noL1_PU_latex,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",
    balance_histo_names_data=the_balance_histo_names_data_deltaphi,
    mpf_histo_names_data=the_mpf_histo_names_data_deltaphi,
    balance_histo_names_mc=the_balance_histo_names_mc_deltaphi,
    mpf_histo_names_mc=the_mpf_histo_names_mc_deltaphi,
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------
p.deltaphi_extrapolation_puL1=ZPJConfiguration.section(\
    "deltaphi_extrapolation_puL1",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment = L1_PU_latex,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",
    balance_histo_names_data=map(makeL1,the_balance_histo_names_data_deltaphi),
    mpf_histo_names_data=map(makeL1,the_mpf_histo_names_data_deltaphi),
    balance_histo_names_mc=map(makeL1,the_balance_histo_names_mc_deltaphi),
    mpf_histo_names_mc=map(makeL1,the_mpf_histo_names_mc_deltaphi),
    pt_bins=the_pt_bins)


#-------------------------------------------------------------------------------

p.update()
p.dump_cfg()     
     
     
                    
                    