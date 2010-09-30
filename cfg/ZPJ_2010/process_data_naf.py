

##_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"
##_calib_data_path = "/afs/naf.desy.de/user/h/hauth/Ana/CalibFW/data/"

_is_data = 1
_tchain_file_path= _data_path + "skim_data/data_job*.root"
_output_path= _data_path + "mu_data"
_json_file= _calib_data_path + "json/Cert_132440-144114_7TeV_StreamExpress_Collisions10_JSON_v3.txt"
_process_algos=["ak5PF"] #, "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo"]

_l2_correction_data=["ak5PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK5PF.txt",
"ak7PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK7PF.txt",
"kt4PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT4PF.txt",
"kt6PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT6PF.txt",
"ak5CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK5Calo.txt",
"ak7CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK7Calo.txt",
"kt4CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT4Calo.txt",
"kt6CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT6Calo.txt"]

p.general=section ("general",
    custom_binning = [0.0, 25.0, 30.0, 36.0, 43.0, 51.0, 61.0, 73.0, 87.0, 104.0, 124.0, 148.0, 177.0, 212.0, 254.0, 304.0, 364.0],
    is_data = _is_data,
    fixed_weighting = 0,
    do_l2_correction = 1,
    do_l3_correction = 0,
    tchain_file_path = _tchain_file_path,
    l2_correction_data = _l2_correction_data,
    output_path = _output_path,
    json_file = _json_file,
    process_algos=_process_algos
)


