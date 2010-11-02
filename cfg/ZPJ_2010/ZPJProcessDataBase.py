import ZPJConfiguration

def getDefaultCorrection( data_path ):
  g_l2_correction_data=["ak5PFJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_AK5PF.txt",
      "ak7PFJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_AK7PF.txt",
      "kt4PFJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_KT4PF.txt",
      "kt6PFJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_KT6PF.txt",
      "ak5CaloJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_AK5Calo.txt",
      "ak7CaloJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_AK7Calo.txt",
      "kt4CaloJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_KT4Calo.txt",
      "kt6CaloJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_KT6Calo.txt",
	"iterativeCone5PFJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_IC5PF.txt",
	"iterativeCone5CaloJets_Zplusjet:" + data_path + "jec_data/Spring10_L2Relative_IC5Calo.txt"
]
  return g_l2_correction_data

def getDefaultBinning():
  custom_binning = [0.0, 25.0, 30.0, 36.0, 43.0, 51.0, 61.0, 73.0, 87.0, 104.0, 124.0, 148.0, 177.0, 212.0, 254.0, 304.0, 364.0]

  return custom_binning

def getDataBaseConfig(  ):
  process = ZPJConfiguration.configuration("Zplusjet_process_data")
  p=process

  _calib_data_path = "data/"
  _is_data = 1

  _json_file= _calib_data_path + "json/Cert_132440-148058_7TeV_StreamExpress_Collisions10_JSON.txt"
  _process_algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]
  
  p.general = ZPJConfiguration.section ("general",
      custom_binning = getDefaultBinning(),
      is_data = _is_data,
      write_events = "incut", # none, incut, all
      plot_nocuts = 0,
      fixed_weighting = 0,
      do_l2_correction = 1,
      do_l3_correction = 0,
      tchain_file_path = "not set",
      l2_correction_data = getDefaultCorrection(_calib_data_path),
      output_path = "not set",
      json_file = _json_file,
      process_algos=_process_algos
  )
  
  return p


def getMCBaseConfig(  ):
  process = ZPJConfiguration.configuration("Zplusjet_process_data")
  p=process

  _calib_data_path = "data/"
  _is_data = 0
  _process_algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo", "iterativeCone5PF", "iterativeCone5Calo"]

  p.general = ZPJConfiguration.section ("general",
      custom_binning = getDefaultBinning(),
      is_data = _is_data,
      plot_nocuts = 0,
      write_events = "none", # none, incut, all
      fixed_weighting = 0,
      do_l2_correction = 1,
      do_l3_correction = 0,
      tchain_file_path = "not set",
      l2_correction_data = getDefaultCorrection(_calib_data_path),
      output_path = "not set",
      json_file = "not set",
      process_algos=_process_algos
  )

  return p


