import ZPJConfiguration


# -*- coding: utf-8 -*-
# Here the parameters for all the plots are stated

#-----------------------------------
def multiline_text(line1,line2,line3="",line4=""):
    string = "#scale[.8]{#splitline{#splitline{%s}{%s}}{#splitline{%s}{%s}}}" %(line1,line2,line3,line4)
    return string
#-----------------------------------


process = ZPJConfiguration.configuration("Plots_AN_PAS")
p=process

data_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10_weighted.root"
mc_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10.root"

the_pt_bins=[0,25,30,36,43,51,61,73,87,104,124,148,177,212,254,304,364]
the_algos=["ak5PF"]
the_good_algos=["anti-kt 0.5"]

the_info_string= multiline_text("p_{T}^{#mu} > 5 GeV , |#eta^{#mu}|<8 , 10 GeV < M_{Z} < 190 GeV",
                                "__ALGO__ Particle Flow jets",
                                 "__CORR__")

the_img_formats=["png","pdf"]

p.general=ZPJConfiguration.section ("general",
    correction_level = 0,
    mc_input_file=mc_file,
    cut="_nocut",
    global_prefix = "MC_MC_pf_nocut_",
    data_input_file=data_file,
    algos=the_algos,
    pt_bins=the_pt_bins,
    good_algos=the_good_algos,
    info_string=the_info_string,
    img_formats=the_img_formats)

p.update()
p.dump_cfg()