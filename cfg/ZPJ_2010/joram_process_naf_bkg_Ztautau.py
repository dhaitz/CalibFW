import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

_data_path = "/scratch/hh/lustre/cms/user/piparo/ZPJ_2010/bkg_mc"

p.general.tchain_file_path = _data_path + "mc_job_Z*.root"
p.general.output_path = "/scratch/hh/lustre/cms/user/berger/skim/bkg_Ztautau"
p.general.write_events = "incut"
#p.general.process_algos = ["ak5PF"]

p.general.custom_binning = [0,50,120,220]

p.update()
p.dump_cfg()
