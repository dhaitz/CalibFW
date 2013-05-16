import sys
import ClosureConfigBase as cbase

def get_template(algo="AK5PFJets"):
    conf = cbase.BaseConfig('mc', '2012')
    conf["InputFiles"] = cbase.CreateFileList("/storage/6/berger/zpj/kappa539_MC12_madgraph/*.root", sys.argv)

    algorithms = [algo+"CHSL1L2L3"]
    base_algorithms = [algo, algo+"CHS"]

    conf = cbase.ExpandConfig(algorithms, conf, expandptbins=False, addResponse=False)
    conf = cbase.treeconfig(conf)
    return conf


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    conf = get_template()
    cbase.Run(conf, sys.argv)
