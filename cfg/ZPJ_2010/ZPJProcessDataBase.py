import ZPJConfiguration

def getDefaultCorrectionL2( data_path ):
  globalTag = "GR_R_38X_V14_"
  
  g_l2_correction_data=["ak5PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK5PF_L2Relative.txt",
      "ak7PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK7PF_L2Relative.txt",
      "kt4PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT4PF_L2Relative.txt",
      "kt6PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT6PF_L2Relative.txt",
      "ak5CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK5Calo_L2Relative.txt",
      "ak7CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK7Calo_L2Relative.txt",
      "kt4CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT4Calo_L2Relative.txt",
      "kt6CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT6Calo_L2Relative.txt",
	"iterativeCone5PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "IC5PF_L2Relative.txt",
	"iterativeCone5CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "IC5PF_L2Relative.txt"]
	
  return g_l2_correction_data

def getDefaultCorrectionL3( data_path ):
  globalTag = "GR_R_38X_V14_"
  g_l3_correction_data=["ak5PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK5PF_L3Absolute.txt",
      "ak7PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK7PF_L3Absolute.txt",
      "kt4PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT4PF_L3Absolute.txt",
      "kt6PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT6PF_L3Absolute.txt",
      "ak5CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK5Calo_L3Absolute.txt",
      "ak7CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "AK7Calo_L3Absolute.txt",
      "kt4CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT4Calo_L3Absolute.txt",
      "kt6CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "KT6Calo_L3Absolute.txt",
        "iterativeCone5PFJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "IC5PF_L3Absolute.txt",
        "iterativeCone5CaloJets_Zplusjet:" + data_path + "jec_data/" + globalTag + "IC5PF_L3Absolute.txt"]
  return g_l3_correction_data


def getDefaultBinning():
  custom_binning = [0.0, 25.0, 30.0, 36.0, 43.0, 51.0, 61.0, 73.0, 87.0, 104.0, 124.0, 148.0, 177.0, 212.0, 254.0, 304.0, 364.0]

  return custom_binning

def getBaseConfig():
  
  process = ZPJConfiguration.configuration("Zplusjet_process_data")
  
  process.general = ZPJConfiguration.section ("general")
  
  process.general.cut_zpt = 15.0
  process.general.cut_2jet = 0.2
  
  return process

def getDataBaseConfig(  ):
  process = getBaseConfig()
  p=process

  _calib_data_path = "data/"
  _is_data = 1

  _json_file= _calib_data_path + "json/Cert_132440-149442_7TeV_StreamExpress_Collisions10_JSON_v3.txt"
  _process_algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]
  
  p.general.custom_binning = getDefaultBinning()
  p.general.is_data = _is_data
  p.general.write_events = "incut" # none, incut, all
      
      # select specific plots here
  p.general.plot_nocuts = 0
  p.general.plot_cuteff = 0
      
  p.general.fixed_weighting = 0
  p.general.do_l2_correction = 1
  p.general.do_l3_correction = 0
  p.general.use_hlt = 1
  p.general.do_l3_correction_formula = 0
      
  p.general.tchain_file_path = "not set"
  p.general.l2_correction_data = getDefaultCorrectionL2(_calib_data_path)
  p.general.l3_correction_data = getDefaultCorrectionL3(_calib_data_path)
  p.general.output_path = "not set"
  p.general.json_file = _json_file
  p.general.process_algos=_process_algos
  
  return p


def getMCBaseConfig(  ):
  process = getBaseConfig()
  p=process

  _calib_data_path = "data/"
  _is_data = 0
  _process_algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]

  p.general.custom_binning = getDefaultBinning()
  p.general.is_data = _is_data

  # select specific plots here
  p.general.plot_nocuts = 0
  p.general.plot_cuteff = 0

  p.general.write_events = "none" # none, incut, all
  p.general.fixed_weighting = 0
  p.general.do_l2_correction = 1
  p.general.do_l3_correction = 0
  p.general.do_l3_correction_formula = 0
  p.general.tchain_file_path = "not set"
  p.general.l2_correction_data = getDefaultCorrectionL2(_calib_data_path)
  p.general.l3_correction_data = getDefaultCorrectionL3(_calib_data_path)
  p.general.output_path = "not set"
  p.general.json_file = "not set"
  p.general.process_algos=_process_algos

  return p


