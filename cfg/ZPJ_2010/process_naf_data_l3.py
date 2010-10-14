import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"
_out_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "skim_data/data_job*.root"
p.general.output_path = _out_path + "mu_data_l3"
p.general.process_algos = ["ak5PF"]
p.general.custom_binning = [0,50,120,220]
p.general.do_l3_correction = 1
p.general.l3_formula = "[0] + [1]/(x^[3]) + [2]/(x^[4])"
p.general.l3_params = [1.526, 1.181, -1.270, -.106, -.128]

p.update()
p.dump_cfg()
