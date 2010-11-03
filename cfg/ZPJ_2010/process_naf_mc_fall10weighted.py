import ZPJProcessDataBase

p = ZPJProcessDataBase.getMCBaseConfig()

_data_path = "/scratch/hh/lustre/cms/user/hauth/ZPJ2010/skim_zjet_fall10_weighted/"
_out_path = "/afs/naf.desy.de/user/h/hauth/lustre/ZPJ2010/" 

p.general.tchain_file_path = _data_path + "mc_job*.root" 
p.general.output_path = _out_path + "zplusjet_mc_fall10_weighted"
p.general.process_algos = ["ak5PF"]
p.general.plot_nocuts=1
#p.general.custom_binning = [0,25,30,36,43,51,61,73,87,104,124,148,177,212,254,304,364]
p.general.use_event_weight = 1

p.update()
p.dump_cfg()
