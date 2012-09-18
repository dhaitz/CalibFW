import os
import getroot
import plot_dhaitz
import plot
import plotresponse, plotfractions
import getpass
import sys

""" Usage: python macros/clusterplots.py *args   e.g. python macros/clusterplots.py -o "out/dataMC/" -a AK5PFJets
    A list of plots is extracted from the files specified in the main plotting file,
    divided into jobs and submitted to the cluster"""
def clusterplots():

    n_jobs = 50
    arguments = " ".join(sys.argv[1:])
    print arguments

    #preferred main plotting file: get options + filelist
    username = getpass.getuser()
    if username == 'dhaitz':
        opt, datamc = plot_dhaitz.plot(cluster=True)
        plotfile = "plot_dhaitz.py"
    else:
        opt, datamc = plot.plot(cluster=True)
        plotfile = "plot.py"

    # get list of plots from the files
    plotlist = plotfractions.plots + plotresponse.plots
    plotlist += getroot.getplotlist(datamc, algorithm=opt.algorithm)
    plotlist += ([i+"_nocuts" for i in getroot.getplotlist(datamc, folder="NoBinning_allevents", algorithm=opt.algorithm)]) 
    plotlist += ([i+"_all" for i in getroot.getplotlist(datamc, 'all')])  
    print "Number of plots: ", len(plotlist)

    # directory for STDOUT/STDERR
    jobinfo_dir = opt.out+"/jobinfo/"
    if not os.path.exists(opt.out): os.mkdir(opt.out)
    if not os.path.exists(jobinfo_dir): os.mkdir(jobinfo_dir)

    # divide into n jobs and submit
    for plots in [plotlist[i:i+len(plotlist)/n_jobs] for i in range(0, len(plotlist), len(plotlist)/n_jobs)]:
        plotstring = " ".join(plots)
        os.system('echo "PYTHONPATH=${PYTHONPATH} ROOTSYS=${ROOTPATH} PATH=$PATH:$ROOTSYS/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib:$ROOTSYS/lib/root PATH=$PATH:$PYTHONPATH python macros/'+plotfile+' -P '+ plotstring +' '+arguments+'"'+' | qsub -q short -cwd -o '+jobinfo_dir+' -e '+jobinfo_dir)
    

if __name__ == "__main__":
    """Unit test: standalone (not as a module)."""
    clusterplots()
