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

the_filename_data="/home/piparo/Paper/extrapolation_data.root"
the_filename_mc="/home/piparo/Paper/extrapolation_data.root"
the_filename_mc_pu="/home/piparo/Paper/extrapolation_dy_pu.root"
 
#the_filename_data="/home/piparo/Paper/extrapolation_data_less2vtx.root"
#the_filename_mc="/home/piparo/Paper/extrapolation_data_less2vtx.root"
#the_filename_mc_pu="/home/piparo/Paper/extrapolation_dy_pu_less2tvx.root" 

#the_filename_data="/home/piparo/Paper/extrapolation_data_more2vtx.root"
#the_filename_mc="/home/piparo/Paper/extrapolation_data_more2vtx.root"
#the_filename_mc_pu="/home/piparo/Paper/extrapolation_dy_pu_more2tvx.root" 

the_balance_response_graph_name_mc=   "jetresp_ak5PFJetsL1L2L3_Zplusjet_mc_graph"
the_balance_response_graph_name_data= "jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_graph"
the_mpf_response_graph_name_mc=       "mpfresp_ak5PFJetsL1L2L3_Zplusjet_mc_graph"
the_mpf_response_graph_name_data=     "mpfresp_ak5PFJetsL1L2L3Res_Zplusjet_data_graph"

the_pt_bins=["Pt0to15",
             "Pt15to30",
             "Pt30to60",
             "Pt60to100",
             "Pt100to500",
             "Pt50to500"]


noL1_noPU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{uncorrected}}{No PU simulated}}"
noL1_PU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{uncorrected}}{PU simulated}}"
L1_noPU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{jet area PU und UE subtracted}}{No PU simulated}}"
L1_PU_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{jet area PU und UE subtracted}}{PU simulated}}"

fully_corr_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{Fully Corrected}}{Z2 Tune with PU simulated}}"
fully_corr_latex_mpf="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{Fully Corrected}}{Z2 Tune with PU simulated}}"

L1_corr_latex="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{Corrected for PU}}{Z2 Tune with PU simulated}}"
L1_corr_latex_mpf="#scale[.6]{#splitline{#splitline{anti-kt 0.5 Particle Flow Jets}{Corrected for PU}}{Z2 Tune with PU simulated}}"

#---------------------------------------------
# 2nd pt jet cut 
the_2nd_pt_cut_vals = [ 0.3 , 0.2, .17 , 0.15, 0.13, 0.1, 0.05, 0.03  ]
the_balance_histo_names_data=\
["@PTBIN@_incut_var_CutSecondLeadingToZPt_0_3/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_2/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_17/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_15/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_13/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_1/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_05/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutSecondLeadingToZPt_0_03/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist"]

the_mpf_histo_names_data=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_data)
#the_mpf_histo_names_data=map(lambda name: name.replace("L1L2L3Res","L1"),the_mpf_histo_names_data)

the_balance_histo_names_mc=map(lambda name: name.replace("data","mc"),the_balance_histo_names_data)
the_balance_histo_names_mc=map(lambda name: name.replace("L1L2L3Res","L1L2L3"),the_balance_histo_names_mc)

the_mpf_histo_names_mc=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_mc)
#the_mpf_histo_names_mc=map(lambda name: name.replace("L1L2L3","L1"),the_mpf_histo_names_mc)

#---------------------------------------------
# DeltaPhi
the_deltaphi_cut_vals = [.34,.24,.14,0.04,0.02]
the_balance_histo_names_data_deltaphi=\
["@PTBIN@_incut_var_CutBack2Back_0_34/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutBack2Back_0_24/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutBack2Back_0_14/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutBack2Back_0_04/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist",
 "@PTBIN@_incut_var_CutBack2Back_0_02/jetresp_ak5PFJetsL1L2L3Res_Zplusjet_data_@PTBIN@_hist"]

the_mpf_histo_names_data_deltaphi=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_data_deltaphi)
the_balance_histo_names_mc_deltaphi=map(lambda name: name.replace("data","mc"),the_balance_histo_names_data_deltaphi)
the_balance_histo_names_mc_deltaphi=map(lambda name: name.replace("L1L2L3Res","L1L2L3"),the_balance_histo_names_mc_deltaphi)
#the_balance_histo_names_mc_deltaphi=map(lambda name: name.replace("data","data"),the_balance_histo_names_data_deltaphi)
the_mpf_histo_names_mc_deltaphi=map(lambda name: name.replace("jetresp","mpfresp"),the_balance_histo_names_mc_deltaphi)


#-------------------------------------------------------------------------------
def makeL1(name):
  string = 'L1L2L3Res'
  if string in name:
    return name.replace(string,"L1")
  string = 'L1L2L3'
  if string in name:
    return name.replace(string,"L1")
  
#-------------------------------------------------------------------------------

p.pt_extrapolation_fullycorr=ZPJConfiguration.section(\
    "pt_extrapolation_fullycorr",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment_balance = fully_corr_latex,
    comment_mpf = fully_corr_latex_mpf,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",

    balance_response_graph_name_mc=the_balance_response_graph_name_mc,
    balance_response_graph_name_data=the_balance_response_graph_name_data,
    mpf_response_graph_name_mc=the_mpf_response_graph_name_mc,
    mpf_response_graph_name_data=the_mpf_response_graph_name_data,

    balance_histo_names_data=the_balance_histo_names_data,
    mpf_histo_names_data=the_mpf_histo_names_data,
    balance_histo_names_mc=the_balance_histo_names_mc,
    mpf_histo_names_mc=the_mpf_histo_names_mc,
    pt_bins=the_pt_bins)
    
#-------------------------------------------------------------------------------

p.deltaphi_extrapolation_fullycorr=ZPJConfiguration.section(\
    "deltaphi_extrapolation_fullycorr",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment_balance = fully_corr_latex,
    comment_mpf = fully_corr_latex_mpf,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",

    balance_response_graph_name_mc=the_balance_response_graph_name_mc,
    balance_response_graph_name_data=the_balance_response_graph_name_data,
    mpf_response_graph_name_mc=the_mpf_response_graph_name_mc,
    mpf_response_graph_name_data=the_mpf_response_graph_name_data,

    balance_histo_names_data=the_balance_histo_names_data_deltaphi,
    mpf_histo_names_data=the_mpf_histo_names_data_deltaphi,
    balance_histo_names_mc=the_balance_histo_names_mc_deltaphi,
    mpf_histo_names_mc=the_mpf_histo_names_mc_deltaphi,
    pt_bins=the_pt_bins)

    
#-------------------------------------------------------------------------------

p.pt_extrapolation_L1=ZPJConfiguration.section(\
    "pt_extrapolation_L1",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment_balance = L1_corr_latex,
    comment_mpf = L1_corr_latex_mpf,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",

    balance_response_graph_name_mc=makeL1(the_balance_response_graph_name_mc),
    balance_response_graph_name_data=makeL1(the_balance_response_graph_name_data),
    mpf_response_graph_name_mc=makeL1(the_mpf_response_graph_name_mc),
    mpf_response_graph_name_data=makeL1(the_mpf_response_graph_name_data),

    balance_histo_names_data=map(makeL1,the_balance_histo_names_data),
    mpf_histo_names_data=map(makeL1,the_mpf_histo_names_data),
    balance_histo_names_mc=map(makeL1,the_balance_histo_names_mc),
    mpf_histo_names_mc=map(makeL1,the_mpf_histo_names_mc),
    pt_bins=the_pt_bins)
    
#-------------------------------------------------------------------------------

p.deltaphi_extrapolation_fullycorr=ZPJConfiguration.section(\
    "deltaphi_extrapolation_fullycorr",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment_balance = fully_corr_latex,
    comment_mpf = fully_corr_latex_mpf,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",

    balance_response_graph_name_mc=the_balance_response_graph_name_mc,
    balance_response_graph_name_data=the_balance_response_graph_name_data,
    mpf_response_graph_name_mc=the_mpf_response_graph_name_mc,
    mpf_response_graph_name_data=the_mpf_response_graph_name_data,

    balance_histo_names_data=the_balance_histo_names_data_deltaphi,
    mpf_histo_names_data=the_mpf_histo_names_data_deltaphi,
    balance_histo_names_mc=the_balance_histo_names_mc_deltaphi,
    mpf_histo_names_mc=the_mpf_histo_names_mc_deltaphi,
    pt_bins=the_pt_bins)

#-------------------------------------------------------------------------------

p.deltaphi_extrapolation_L1=ZPJConfiguration.section(\
    "deltaphi_extrapolation_L1",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment_balance = L1_corr_latex,
    comment_mpf = L1_corr_latex_mpf,
    cut_vals=the_deltaphi_cut_vals,
    cut_name="#Delta #Phi(Z,jet1)",

    balance_response_graph_name_mc=makeL1(the_balance_response_graph_name_mc),
    balance_response_graph_name_data=makeL1(the_balance_response_graph_name_data),
    mpf_response_graph_name_mc=makeL1(the_mpf_response_graph_name_mc),
    mpf_response_graph_name_data=makeL1(the_mpf_response_graph_name_data),

    balance_histo_names_data=map(makeL1,the_balance_histo_names_data_deltaphi),
    mpf_histo_names_data=map(makeL1,the_mpf_histo_names_data_deltaphi),
    balance_histo_names_mc=map(makeL1,the_balance_histo_names_mc_deltaphi),
    mpf_histo_names_mc=map(makeL1,the_mpf_histo_names_mc_deltaphi),
    pt_bins=the_pt_bins)




#-------------------------------------------------------------------------------

p.pt_extrapolation_fullycorr_ak7=ZPJConfiguration.section(\
    "pt_extrapolation_fullycorr_ak7",
    filename_data = the_filename_data,
    filename_mc = the_filename_mc_pu,
    comment_balance = fully_corr_latex,
    comment_mpf = fully_corr_latex_mpf,
    cut_vals=the_2nd_pt_cut_vals,
    cut_name="p_{T}^{jet 2}/p_{T}^{Z}",

    balance_response_graph_name_mc=the_balance_response_graph_name_mc.replace("ak5","ak7"),
    balance_response_graph_name_data=the_balance_response_graph_name_data.replace("ak5","ak7"),
    mpf_response_graph_name_mc=the_mpf_response_graph_name_mc.replace("ak5","ak7"),
    mpf_response_graph_name_data=the_mpf_response_graph_name_data.replace("ak5","ak7"),

    balance_histo_names_data=map(lambda name: name.replace("ak5","ak7"),the_balance_histo_names_data),
    mpf_histo_names_data=map(lambda name: name.replace("ak5","ak7"),the_mpf_histo_names_data),
    balance_histo_names_mc=map(lambda name: name.replace("ak5","ak7"),the_balance_histo_names_mc),
    mpf_histo_names_mc=map(lambda name: name.replace("ak5","ak7"),the_mpf_histo_names_mc),
    pt_bins=the_pt_bins)


#-------------------------------------------------------------------------------

p.update()
p.dump_cfg()     
     
     
                    
                    