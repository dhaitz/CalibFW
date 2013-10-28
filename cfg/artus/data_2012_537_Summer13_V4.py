import ArtusConfigBase as base
import data_template


def get_template():
    conf = data_template.get_template()
    conf["InputFiles"] = "/storage/8/dhaitz/Winter13ReReco_2/*.root"
    conf["JsonFile"] = base.GetBasePath() + "data/json/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
    conf['Jec'] = base.GetBasePath() + "data/jec/Summer13_V4_DATA"
    return conf
