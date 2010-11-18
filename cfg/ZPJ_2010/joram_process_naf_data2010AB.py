import ZPJProcessDataBase

p = ZPJProcessDataBase.getDataBaseConfig()

p.general.tchain_file_path = "/scratch/hh/lustre/cms/user/hauth/ZPJ2010/skim_data_2010a+b/data_*out.root" 
p.general.output_path = "/scratch/hh/lustre/cms/user/berger/skim/skim2010AB"
p.general.write_events = "incut"
p.general.process_algos = ["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]

p.general.custom_binning = [0.0, 25.0, 30.0, 36.0, 43.0, 51.0, 61.0, 73.0, 87.0, 104.0, 124.0, 148.0, 177.0, 212.0, 254.0, 304.0, 364.0]

p.update()
p.dump_cfg()
