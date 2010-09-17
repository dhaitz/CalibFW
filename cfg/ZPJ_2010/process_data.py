
process = configuration("Zplusjet_process_data")
p=process

_is_data = 1
_tchain_file_path="/local/scratch/hauth/data/Zplusjet_2010_08_16/data_job*.root"
_output_path="test"
_json_file="Cert_132440-144114_7TeV_StreamExpress_Collisions10_JSON_v2.txt"
_process_algos=["ak5PF"]

_l2_correction_data=["ak5PF:/local/scratch/hauth/Ana/Zplusjet/jec_data/Spring10_L2Relative_AK5PF.txt"]

p.general=section ("general",
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


