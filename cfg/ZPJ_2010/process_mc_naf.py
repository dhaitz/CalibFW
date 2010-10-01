
process = configuration("Zplusjet_process_data")
p=process

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"
_calib_data_path = "/afs/naf.desy.de/user/h/hauth/Ana/CalibFW/data/"

_is_data = 0
_tchain_file_path= "/scratch/hh/lustre/cms/user/piparo/ZPJ_2010/Zplusjet_mc/mc_job_*.root"
_output_path= _data_path + "zplusjet_mc"
_process_algos=["ak5PF"]

_l2_correction_data=["ak5PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK5PF.txt"]
#_l2_correction_data=["ak5PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L3Absolute_AK5PF.txt"]

p.general=section ("general",
    custom_binning = [0.0, 25.0, 30.0, 36.0, 43.0, 51.0, 61.0, 73.0, 87.0, 104.0, 124.0, 148.0, 177.0, 212.0, 254.0, 304.0, 364.0],
    is_data = _is_data,
    fixed_weighting = 0,
    do_l2_correction = 1,
    do_l3_correction = 0,
    tchain_file_path = _tchain_file_path,
    l2_correction_data = _l2_correction_data,
    output_path = _output_path,
    json_file = "no json for mc",
    process_algos=_process_algos
)


