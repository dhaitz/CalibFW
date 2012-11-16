import os
import getroot
import plotbase
import plot, plot_dhaitz
import plotresponse, plotfractions
import getpass
import sys

""" Usage: python macros/clusterplots.py *args   e.g. python macros/clusterplots.py -o "out/dataMC/" -a AK5PFJets
    A list of plots is extracted from the files specified in the main plotting file,
    divided into jobs and submitted to the cluster"""
def clusterplots():

    n_jobs = 50
    mods = filter(lambda x: "plot" in x, dir())

    if len(sys.argv) > 1 and ".py" in sys.argv[1]:
        plotfile = sys.argv[1].replace("macros/", "")
        arguments = " ".join(sys.argv[2:])
    else:
        plotfile = "plot.py"
        arguments = " ".join(sys.argv[1:])
    print arguments


    #preferred main plotting file: get options + filelist
    opt, datamc = plot.plot(cluster=True)

    username = getpass.getuser()
    if username == 'dhaitz':
        opt, datamc = plot_dhaitz.plot(cluster=True)
        plotfile = "plot_dhaitz.py"


    # get list of plots from the files
    plotlist =  plotresponse.plots + plotfractions.plots
    plotlist += getroot.getplotlist(datamc, algorithm=opt.algorithm, filenames=opt.files)
    plotlist += getroot.getplotlist(datamc, algorithm=opt.algorithm, filenames=opt.files, run=True)
    plotlist += ([i+"_nocuts" for i in getroot.getplotlist(datamc, folder="NoBinning_allevents", algorithm=opt.algorithm, filenames=opt.files)]) 
    plotlist += ([i+"_nocuts" for i in getroot.getplotlist(datamc, folder="NoBinning_allevents", algorithm=opt.algorithm, filenames=opt.files,
                     run=True)]) 
    plotlist += ([i+"_all" for i in getroot.getplotlist(datamc, 'all', algorithm=opt.algorithm, filenames=opt.files)])
    plotlist.sort()
    print "Number of plots: ", len(plotlist)

    # directory for STDOUT/STDERR
    jobinfo_dir = opt.out+"/jobinfo/"
    plotbase.EnsurePathExists(opt.out)
    plotbase.EnsurePathExists(jobinfo_dir)

    # divide into n jobs and submit
    for plots in [plotlist[i:i+len(plotlist)/n_jobs] for i in range(0, len(plotlist), len(plotlist)/n_jobs)]:
        plotstring = " ".join(plots)
        os.system('echo "PYTHONPATH=${PYTHONPATH} ROOTSYS=${ROOTPATH} PATH=$PATH:$ROOTSYS/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib:$ROOTSYS/lib/root PATH=$PATH:$PYTHONPATH python macros/%s %s -P %s" | qsub -q short -cwd -o %s -e %s' % (plotfile, arguments, plotstring, jobinfo_dir, jobinfo_dir))

if __name__ == "__main__":
    """Unit test: standalone (not as a module)."""
    clusterplots()
