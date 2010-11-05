import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

p.general.tchain_file_path = "/scratch/hh/lustre/cms/user/hauth/ZPJ2010/skim_data_2010a+b/data_*out.root" 
p.general.output_path = "/scratch/hh/lustre/cms/user/berger/skim/skim2010AB_5bins"
p.general.write_events = "incut"
p.general.process_algos = ["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]

p.general.custom_binning = [0,35,60,100,170,250]

p.update()
p.dump_cfg()
