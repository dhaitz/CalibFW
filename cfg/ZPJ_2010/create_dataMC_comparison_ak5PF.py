import ZPJConfiguration
import ZPJLocalConfig
import ZPJBase

# -*- coding: utf-8 -*-
# Here the parameters for all the plots are stated

#-----------------------------------
def multiline_text(line1,line2,line3="",line4=""):
    string = "#scale[.8]{#splitline{#splitline{%s}{%s}}{#splitline{%s}{%s}}}" %(line1,line2,line3,line4)
    return string
#-----------------------------------


process = ZPJConfiguration.configuration("Plots_AN_PAS")
p=process

data_file=ZPJLocalConfig.GetLocalRootFilePath() + "mu_data_2010a+b.root"
#mc_file="/local/scratch/hauth/data/ZPJ2010/zplusjet_mc_fall10_weighted.root"
mc_file=ZPJLocalConfig.GetLocalRootFilePath() + "zplusjet_mc_fall10.root"
the_eta_rebin_factor=5
the_phi_rebin_factor=5
the_pt_rebin_factor=2
the_mass_rebin_factor=2
#the_lumi=2.96;
the_lumi=21.88;
the_pt_bins=[0,25,30,36,43,51,61,73,87,104,124,148,177,212,254,304,364]
the_algos=["ak5PF"]
the_good_algos=["anti-kt 0.5"]
#the_algos=["kt4PF"]
#the_good_algos=["kt 0.4"]


# Constraints for minimum location
the_min_jes=0.75;
the_max_jes=1.05;
the_min_jer=.7;
the_max_jer=1.4;

the_info_string= multiline_text(ZPJBase.GetCutsText1(),
				      ZPJBase.GetCutsText2(),
                                "__ALGO__ Particle Flow jets",
                                 "__CORR__")

the_img_formats=["png"]

p.general=ZPJConfiguration.section ("general",
    correction_level = 2,
    mc_input_file=mc_file,
    data_input_file=data_file,
    eta_rebin_factor=the_eta_rebin_factor,
    phi_rebin_factor=the_phi_rebin_factor,
    pt_rebin_factor=the_pt_rebin_factor,
    mass_rebin_factor=the_mass_rebin_factor,
    lumi=the_lumi,
    algos=the_algos,
    pt_bins=the_pt_bins,
    good_algos=the_good_algos,
    info_string=the_info_string,
    img_formats=the_img_formats,
    min_jes=the_min_jes,
    max_jes=the_max_jes,
    min_jer=the_min_jer,
    max_jer=the_max_jer)

p.update()
p.dump_cfg()
