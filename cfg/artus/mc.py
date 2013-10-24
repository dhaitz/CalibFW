import sys
import ArtusConfigBase as base

def get_template(algo="AK5PFJets"):
    conf = base.BaseConfig('mc', '2012')
    conf["InputFiles"] = base.CreateFileList("/storage/6/berger/zpj/kappa539_MC12_madgraph_tags/*.root", 
sys.argv)
    conf['OutputPath'] = __file__.split('/')[-1][:-3]


    algorithms = [algo+"CHSL1L2L3"]
    base_algorithms = [algo, algo+"CHS"]

    conf = base.ExpandConfig(algorithms, conf, expandptbins=False, addResponse=False)
    conf = base.treeconfig(conf)
    return conf


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    conf = get_template()
    base.Run(conf, sys.argv)
