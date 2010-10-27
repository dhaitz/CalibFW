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

data_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10_weighted_gen.root"
mc_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10_gen.root"

the_pt_bins=[0,25,30,36,43,51,61,73,87,104,124,148,177,212,254,304,364]
the_algos=["ak5Gen"]
the_good_algos=["anti-kt 0.5"]

the_info_string= multiline_text("p_{T}^{#mu}>15 GeV , |#eta^{#mu}|<2.3 , |M-M_{Z}|<20 GeV",
                                "|#eta^{jet}|<1.3 , |#Delta #phi (Z,jet) - #pi|<0.2 , p_{T}^{jet2}/p_{T}^{Z}< 0.2",
                                "Gen __ALGO__ jets",
                                 "__CORR__")

the_img_formats=["png","pdf"]

p.general=ZPJConfiguration.section ("general",
    correction_level = 0,
    mc_input_file=mc_file,
    cut="",
    global_prefix = "MC_MC_gen_cut_",
    data_input_file=data_file,
    algos=the_algos,
    pt_bins=the_pt_bins,
    good_algos=the_good_algos,
    info_string=the_info_string,
    img_formats=the_img_formats)

p.update()
p.dump_cfg()
