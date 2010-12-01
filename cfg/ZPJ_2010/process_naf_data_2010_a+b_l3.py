import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"

p.general.tchain_file_path = _data_path + "skim_data_2010a+b/data_*.root"
p.general.output_path = _data_path + "mu_data_2010a+b_l3"
p.general.process_algos = ["ak5PF"]
#p.general.custom_binning = [0,50,120,220]

p.general.cut_zpt = 50
p.general.custom_binning = [50,70,100,140,250]

p.general.do_l3_correction_formula = 1
#p.general.l3_formula = "[0] + [1]/(x^[3]) + [2]/(x^[4])"
p.general.l3_formula = "[0] + [1]/((log(x)^[2]) + [3])"
p.general.l3_formula_params = [1.114, 1.134, 3.00, -4.31]

p.update()
p.dump_cfg()
