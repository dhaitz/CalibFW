import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/"
_out_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "skim_data/data_job*.root"
p.general.output_path = _out_path + "mu_data_l3"
p.general.process_algos = ["ak5PF"]
p.general.custom_binning = [0,50,120,220]
p.general.do_l3_correction_formula = 1
p.general.do_l3_correction = 0

p.general.cut_zpt = 50
p.general.custom_binning = [50,70,100,140,250]

p.general.l3_formula = "[0] + [1]/((log(x)^[2]) + [3])"
#p.general.l3_formula = "[0] + [1]/(x^[3]) + [2]/(x^[4])"
p.general.l3_formula_params = [1.114, 1.134, 3.00, -4.31]


#  EXT PARAMETER                APPROXIMATE        STEP         FIRST   
#  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
#   1  p0           1.11363e+00   6.78135e-02  -2.15017e-05  -3.57856e-01
#   2  p1           1.13430e+00   9.65322e+00   4.42842e-02  -1.49508e-02
#   3  p2           3.00432e+00   4.31628e+00   2.02097e-02   7.38414e-02
#   4  p3          -4.31349e+01   2.20428e+02  -9.55546e-01   8.78653e-04


p.update()
p.dump_cfg()
