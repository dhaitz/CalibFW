import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()

_data_path = "/scratch/hh/lustre/cms/user/piparo/ZPJ_2010/Zplusjet_mc/"
_out_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "mc_job_*.root" 
p.general.output_path = _out_path + "zplusjet_mc_l3"
p.general.process_algos = ["ak5PF"]
p.general.do_l3_correction = 1
p.general.l3_formula = "[0] + [1]/(x^[3]) + [2]/(x^[4])"
p.general.l3_params = [1.89, -10.46, 13.07, .298, .425]

p.update()
p.dump_cfg()
