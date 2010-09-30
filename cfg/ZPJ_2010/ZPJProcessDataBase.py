import ZPJConfiguration


def getDataBaseConfig(  ):
  process = ZPJConfiguration.configuration("Zplusjet_process_data")
  p=process

  _calib_data_path = "data/"
  _is_data = 1
  _json_file= _calib_data_path + "json/Cert_132440-144114_7TeV_StreamExpress_Collisions10_JSON_v3.txt"
  _process_algos=["ak5PF", "ak7PF", "kt4PF", "kt6PF", "ak5Calo", "ak7Calo", "kt4Calo", "kt6Calo"]

  
  _l2_correction_data=["ak5PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK5PF.txt",
      "ak7PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK7PF.txt",
      "kt4PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT4PF.txt",
      "kt6PFJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT6PF.txt",
      "ak5CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK5Calo.txt",
      "ak7CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_AK7Calo.txt",
      "kt4CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT4Calo.txt",
      "kt6CaloJets_Zplusjet:" + _calib_data_path + "jec_data/Spring10_L2Relative_KT6Calo.txt"]

  p.general = ZPJConfiguration.section ("general",
      is_data = _is_data,
      write_events = "incut", # none, incut, all
      fixed_weighting = 0,
      do_l2_correction = 1,
      do_l3_correction = 0,
      tchain_file_path = "not set",
      l2_correction_data = _l2_correction_data,
      output_path = "not set",
      json_file = _json_file,
      process_algos=_process_algos
  )
  
  return p


