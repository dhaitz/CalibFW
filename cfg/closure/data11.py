import sys
import ClosureConfigBase as cbase

def get_template(algo="AK5PFJets"):
    conf = cbase.BaseConfig('data', '2011')
    conf["InputFiles"] = cbase.CreateFileList("/storage/a/dhaitz/data11/*.root", sys.argv)
    conf['OutputPath'] = __file__.split('/')[-1][:-3]

    algorithms = [algo+"CHSL1L2L3", algo+"CHSL1L2L3Res"]
    base_algorithms = [algo, algo+"CHS"]

    conf = cbase.ExpandConfig(algorithms, conf, expandptbins=False, addResponse=False)
    conf = cbase.treeconfig(conf)
    return conf


if __name__ == "__main__":
    """Unit test: doing the plots standalone (not as a module)."""
    conf = get_template()
    cbase.Run(conf, sys.argv)
