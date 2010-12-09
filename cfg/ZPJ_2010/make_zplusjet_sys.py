# -*- coding: iso-8859-1 -*-
# here the burocracy
import ZPJConfiguration

test_file="/usr/users/piparo/CalibFW/test.root"

base="/usr/users/piparo/2010_12_08_variation"

# Rootfile names (or one)
the_nominal_filename = base+'/histos_ptratio_2.root'

the_deltaphi_plus_filename = base+'/histos_deltaphi_44.root'
the_deltaphi_minus_filename = base+'/histos_deltaphi_24.root'
the_deltaphi_fit_func = "[0]"

the_pt_ratio_plus_filename = base+'/histos_ptratio_25.root'
the_pt_ratio_minus_filename = base+'/histos_ptratio_15.root'
the_pt_ratio_fit_func = "[0]"

the_z_mass_plus_filename = base+'/histos_zmass_25.root'
the_z_mass_minus_filename = base+'/histos_zmass_15.root'
the_z_mass_fit_func = "[0]"

the_muons_pt_plus_filename = base+'/histos_muonpt_20.root'
the_muons_pt_minus_filename = base+'/histos_muonpt_10.root'
the_muons_pt_fit_func = "[0]"

the_z_pt_histo_name = "zPt_ak5PFJets_Zplusjet_mc_l2corr_hist"

the_fit_lower=0.
the_fit_upper=300.

the_data_histo_name = "jetresp_graph_ak5PFJets_Zplusjet_data_l2corr_hist"
the_mc_histo_name   = "jetresp_graph_ak5PFJets_Zplusjet_mc_l2corr_hist"

# Other
the_out_filename="test_out.root"
the_image_formats=["png","pdf"]


#-------------------------------------------------------------------------------

process=ZPJConfiguration.configuration("my_cfg")
p=process

p.sys_cfg=ZPJConfiguration.section(\
    "sys_cfg",
    nominal_filename = the_nominal_filename,
    
    deltaphi_plus_filename = the_nominal_filename,
    deltaphi_minus_filename = the_deltaphi_minus_filename,
    deltaphi_fit_func = the_deltaphi_fit_func,
    
    pt_ratio_plus_filename = the_pt_ratio_plus_filename,
    pt_ratio_minus_filename = the_pt_ratio_minus_filename,
    pt_ratio_fit_func = the_pt_ratio_fit_func,
    
    z_mass_plus_filename = the_z_mass_plus_filename,
    z_mass_minus_filename = the_z_mass_minus_filename,
    z_mass_fit_func = the_z_mass_fit_func,
    
    muons_pt_plus_filename = the_muons_pt_plus_filename,
    muons_pt_minus_filename = the_muons_pt_minus_filename,
    muons_pt_fit_func = the_muons_pt_fit_func,
    
    fit_lower = the_fit_lower,
    fit_upper = the_fit_upper,
    
    data_histo_name= the_data_histo_name,
    mc_histo_name= the_mc_histo_name,
    
    z_pt_histo_name = the_z_pt_histo_name, 
    
    # Other
    out_filename = the_out_filename,
    image_formats= the_image_formats)
     
     
p.update()
p.dump_cfg()
     
     
                    