  _______ ___   ___  ______      ___       __       __  .______    __    __  .______
 |   ____|\  \ /  / /      |    /   \     |  |     |  | |   _  \  |  |  |  | |   _  \
 |  |__    \  V  / |  ,----'   /  ^  \    |  |     |  | |  |_)  | |  |  |  | |  |_)  |
 |   __|    >   <  |  |       /  /_\  \   |  |     |  | |   _  <  |  |  |  | |      /
 |  |____  /  .  \ |  `----. /  _____  \  |  `----.|  | |  |_)  | |  `--'  | |  |\  \----.
 |_______|/__/ \__\ \______|/__/     \__\ |_______||__| |______/   \______/  | _| `._____|
                                                                      (previously CalibFW)
                   (O)
                   <M       The mighty broadsword of cut-based jet studies
        o          <M
       /| ......  /:M\------------------------------------------------,,,,,,
     (O)[]XXXXXX[]I:K+}=====<{H}>================================------------>
       \| ^^^^^^  \:W/------------------------------------------------''''''
        o          <W
                   <W
                   (O)                 Calibrate like a king!

--------------------------------------------------------------------------------

*Excalibur* provides a complete workflow for calibration studies, namely covering
the following steps:

   1. Skimming: Creating a *Kappa* output file with CMSSW
   2. Analyzing: Performing additional event reconstruction steps and cuts
      to produce a final root NTuple
   3. Plotting: Producing the final set of plots with a collection of python
      scripts

Please also see the following pages:

- http://www-ekp.physik.uni-karlsruhe.de/twiki/bin/view/EkpCms/Zmumujetanalysis
  for an overview on the Z->mumu+jet analysis

- https://ekptrac.physik.uni-karlsruhe.de/trac/Excalibur/wiki
  for information and installation of *Excalibur*


1. Skimming: How to create Kappa files
================================================================================
Use CMSSW with one of the latest cfg files found in the `cfg/cmssw/` folder.
Config files for grid-control are also available in this directory.


2. Analyzing: How to create a ROOT NTuple from a Kappa skim
================================================================================

An NTuple can be created via a python configuration file (in `cfg/closure/`), which
is then processed by `ClosureConfigBase` to create a .json file containing all necessary
parameters.  The json file can then further be processed with `closure`, the main
executable.


## Setting the environment
CMSSW must be sourced. Set `$CLOSURE_WORK` path (e.g. `/storage/a/$USER/Excalibur`)
and execute `scripts/ClosureEnv.sh` from your main Excalibur directory.


## Basic structure of the config file
In its basic form, a config file data.py must contain a call of the BaseConfig, 
ExpandConfig and treeconfig functions (to create and modify the configuration
dictionary), a list of files and the Run command:

    conf = cbase.BaseConfig('data', '2012')
    conf["InputFiles"] = cbase.CreateFileList("/storage/.../*.root", sys.argv)
    conf = cbase.ExpandConfig("AK5PFJetsCHSL1L2L3Res", conf)
    conf = cbase.treeconfig(conf)
    cbase.Run(conf, sys.argv)

To test the configuration, use

    python $CLOSURE_BASE/cfg/closure/data.py --fast

(set the number of testfiles with `--fast n`, default is 3)

If the test was successful, you can run the jobs on the EKPcluster using the 
redo script:

    . scripts/redo.sh data.py

Your Ntuple will then be available at `$CLOSURE_WORK/work/data/out/closure.root`

## Further needed
The JEC txt files need to be places in the data/jec/ directory. For a MC file,
the pileup reweighting factors have to be stored in the puweights.json file.


3. Plotting: How to create plots from a ROOT NTuple
================================================================================

The main plotting file is `plotting/plotbase.py`. The CalibFW plotting framework
is highly customizable, you are advised to have a look at 
`plotting/plotbase.py --help` for a list and description of parameter arguments.

At its most basic level of usage, the arguments include links to a data (and MC) 
file and one (or a list of) plotname:

    python plotting/plotbase.py path-to-ntuple.root -P plot1 plot2


## Plotnames
Plotnames can be predefined functions available in the collection of plotting 
scripts in the `plotting` directory (e.g. `fractions_zpt`, `extrapol`, use the
`--list` keyword to display a complete list) or custom combinations of
quantities like:

- `x`  to produce a simple histogram
- `y_x` to produce a profile histogram (e.g. `-P zpt_npv`)
- `2D_y_x` to produce a 2D histogram
- `2D_z_y_x` to produce a 2D profile histogram
- `y1_y2_..._response/ratio/responseratio_x` to produce response plots,
  e.g (`mpf_responseratio_zpt`, `ptbalance_recogen_ratio_npv`, etc.)

The quantities can be plain variable names (that must be present in the input
NTuple) or logical expressions, e.g. `-P "(npv=1)_zpt"`, `-P "(jet1pt<zpt)_npv"`.
They can also be combined, e.g. `-P "jet1pt>100 && zpt>100"`,
 `-P "(zpt<50 || npv>15)"`.


## Cuts and selections
Apart from other cuts, the default settings are `alpha<0.2` and `jet1eta<1.3`.
However events with `alpha<0.4` and unconstrained `jet1eta` are present in the NTuple,
the cuts on these quantities are applied on-the-fly during plotting. To disable
the cuts, use the `--allalpha` and `--alleta` settings.
Additional selections can be applied via the `--selection` keyword,
e.g `--selection "(zpt<80)"`, `--selection "(npv==1)"`.
For example, to extend the alpha range from 0.2 to 0.3 you have to use
`--allalpha --selection "(alpha<0.3)"`.


## Make your own functions
To make use of the full power of the Excalibur plotting framework, you are advised
to base your custom functions on the `datamcplot()` function. Examples can be
found in the `plotting/plot_sandbox.py` file.


**Enjoy!**


*Last updated: 2013-10-14 dhaitz, berger*

