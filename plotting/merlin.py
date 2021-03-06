#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""This is the main plotting program"""

import sys
import copy
import glob
import inspect
import argparse
import matplotlib
import subprocess
import artus
import os
import datetime

import plotbase
import plotrc


def options(
            # standard values go here:

            files=plotbase.getDefaultFiles(),

            plots=['zmass'],

            algorithm="AK5PFJetsCHS",
            correction="L1L2L3Res",

            out="out",
            formats=['png'],

            labels=[],
            colors=['black',
                    '#7293cb',  # light blue
                    '#e1974c',  # mustard yellow
                    '#808585',  # grey
                    '#d35e60',  # light red
                    '#9067a7',  # violet
                    '#ab6857',  # brown
                    '#84ba5b',  # green
                    '#ccc210',  # dirty yellow
                    'salmon', 'mediumaquamarine'
                    ],
            markers=["o", "f", "-"],
            lumi=19.789,
            energy=8,
            status=None,
            author=None,
            date=None,
            layout='cmsstyle_JetMET',
            title="",
            eventnumberlabel=None,
            legloc='best',

            nbins=20,
            rebin=5,
            ratio=False,
            fit=None,
            filename=None,

            npv=[(0, 4), (5, 8), (9, 15), (16, 21), (22, 45)],
            cut=[0.3],
            eta=[0, 0.783, 1.305, 1.93, 2.5, 2.964, 3.139, 5.191],
            zbins=[30, 40, 50, 60, 75, 95, 125, 180, 300, 1000],
    ):
    """Set standard options and read command line arguments.

    To be turned into a class with str method and init
    """

    parser = argparse.ArgumentParser(
        description="%(prog)s does all the plotting.",
        epilog="Have fun.")

    #plots
    parser.add_argument('plots', type=str, nargs='*', default=plots,
        help="do only this plot/these plots. " +
             "The function names are required here.")

    # source options
    source = parser.add_argument_group('Source options')
    parser.add_argument('-i', '--files', type=str, nargs='*',
        default=files,
        help="data and Monte Carlo input root file(s). One data file and at " +
             "least one Monte Carlo file is assumed. Default: %(default)s")
    source.add_argument('--selection', '-S', type=str, nargs='*',
        default=None,
        help='selection (cut) expression, C++ expressions are valid')
    source.add_argument('--folder', type=str,
        default='incut',
        help="folder in rootfile: 'incut' or 'allevents'")
    source.add_argument('--allalpha', action='store_true',
        default=False,
        help='Extend the alpha range up to 0.4')
    source.add_argument('--alleta', action='store_true',
        default=False,
        help='Extend the eta range beyond 1.3')
    source.add_argument('-a', '--algorithm', type=str,
        default=algorithm, help="Jet algorithm. Default is %(default)s")
    source.add_argument('-c', '--correction', type=str, default=correction,
        help="Jet energy correction level. Default is %(default)s")

    # more general options
    general = parser.add_argument_group('General options')
    general.add_argument('-r', '--rebin', type=int, default=rebin,
        help="Rebinning value n")
    general.add_argument('-R', '--ratio', action='store_true',
        help="do a ratio plot from the first two input files")
    general.add_argument('--subtract', action='store_true',
        help="subtract the second from the first histogram")
    general.add_argument('--ratiosubplot', action='store_true',
        help="Add a ratio subplot")
    general.add_argument('-F', '--fit', type=str, default=fit,
        help="Do a fit. Options: vertical, chi2, gauss, slope, intercept")
    general.add_argument('--extrapolation', type=str, default=False,
        help='For response plots: Apply alpha-extrapolation. Possible values \
              are bin or global')
    general.add_argument('--run', type=str, default=False,
        help='Some special options for runplots. Valid options are true or diff')
    general.add_argument('--special_binning', action='store_true', default=False,
        help='special binning for npv, zpt, eta')
    general.add_argument('--nbins', type=int, default=nbins,
        help='number of bins in histogram. Default is %(default)s')
    general.add_argument('-n', '--normalize', action='store_false',  # make it float 0.0 is default
        help="normalize Monte Carlo samples to the event count in data ")
    general.add_argument('-Y', '--year', type=int, default=2012,
        help="Year of data-taking. Default is %(default)s")
    general.add_argument('--factor', type=float, default=1.26,
        help="additional external weight for each MC event %(default)s")
    general.add_argument('--efficiency', type=float, default=0.965 ** 2,
        help="trigger efficiency. Default is %(default)s")
    general.add_argument('--stacked', action='store_true',
        help="Stack the MC samples")

    # output settings
    output = parser.add_argument_group('Output options')
    output.add_argument('-o', '--out', type=str, default=out,
        help="output directory for plots")
    output.add_argument('-f', '--formats', type=str, nargs='+', default=formats,
        help="output format for the plots.  Default is %(default)s")
    output.add_argument('--filename', type=str, default=filename,
        help='specify a filename')
    output.add_argument('--root', type=str, default=False,
        help="Name of the histogramm which is then saved into a root file")
    output.add_argument('--save_individually', action='store_true',
        default=False,
        help='save each plot separately')

    # plot labelling and formatting
    formatting = parser.add_argument_group('Formatting options')
    formatting.add_argument('-l', '--lumi', type=float, default=lumi,
        help="luminosity for the given data in /fb. Default is %(default)s")
    formatting.add_argument('-e', '--energy', type=int, default=energy,
        help="centre-of-mass energy for the given samples in TeV. \
                                                       Default is %(default)s")

    formatting.add_argument('-s', '--status', type=str, default=status,
        help="status of the plot (e.g. CMS preliminary)")
    formatting.add_argument('-A', '--author', type=str, default=author,
        help="author name of the plot")
    formatting.add_argument('--date', type=str, default=date,
        help="show the date in the top left corner. 'iso' is YYYY-MM-DD, " +
             "'today' is DD Mon YYYY and 'now' is DD Mon YYYY HH:MM.")
    formatting.add_argument('-E', '--eventnumberlabel', action='store_true',
        help="add event number label")
    formatting.add_argument('-t', '--title', type=str, default=title,
         help="plot title")
    formatting.add_argument('--layout', type=str,
        default='cmsstyle_JetMET',
        help="layout for the plots. E.g. 'document': serif, LaTeX, pdf; " +
             "'slides': sans serif, big, png; 'generic': slides + pdf. " +
             "This is not implemented yet.")
    formatting.add_argument('-g', '--legloc', type=str, nargs="?", default=legloc,
        help="Location of the legend. Default is %(default)s. Possible values " +
             "are keywords like 'lower left' or coordinates like '0.5,0.1'.")
    formatting.add_argument('--subtext', type=str, default=None,
        help='Add subtext')
    formatting.add_argument('-C', '--colors', type=str, nargs='+', default=colors,
        help="colors for the plots in the order of the files. Default is: " +
             ", ".join(colors))
    formatting.add_argument('-k', '--labels', type=str, nargs='+', default=labels,
        help="labels for the plots in the order of the files. Default is: " +
             ", ".join(labels))
    formatting.add_argument('-m', '--markers', type=str, nargs='+', default=markers,
        help="style for the plot in the order of the files. 'o' for points, \
              '-' for lines, 'f' for fill. Default is: %s" % ", ".join(markers))
    formatting.add_argument('--text', type=str,
        default=None,
        help='Place a text at a certain location. Syntax is --text="abs" or \
                                                          --text="abc,0.5,0.9"')
    formatting.add_argument('-G', '--grid', action='store_true', default=False,
        help="Place an axes grid on the plot.")

    formatting.add_argument('--cutlabel', type=str,
        default=None, help="Place a cutlabel on the plot. Options are: %s"
                                     % ", ".join(plotbase.cutlabeldict.keys()))

    # AXIS
    axis = parser.add_argument_group('Axis options')
    axis.add_argument('--log', action='store_true', default=None,
         help="log plot")
    axis.add_argument('--xlog', action='store_true', default=None,
         help="xlog plot")
    axis.add_argument('-y', type=float, nargs='+', default=None,
        help="upper and lower limit for y-axis")
    axis.add_argument('-x', type=float, nargs='+', default=None,
        help="upper and lower limit for x-axis")
    axis.add_argument('-z', type=float, nargs='+', default=None,
        help="upper and lower limit for z-axis")
    axis.add_argument('--xynames', type=str, nargs='+', default=None,
        help='x-y-axis label names,')
    axis.add_argument('--xname', type=str, default=None,
        help='x-axis label names,')
    axis.add_argument('--yname', type=str, default=None,
        help='y-axis label names,')
    axis.add_argument('--xticks', type=float, nargs='+', default=None,
        help="add custom xticks")
    axis.add_argument('--xbins', type=float, nargs='+', default=None,
        help=" custom bins on x axis")

    # Other options
    group = parser.add_argument_group('Other options')
    group.add_argument('-v', '--verbose', action='store_true',
        help="verbosity")
    group.add_argument('--list', action='store_true',
        help="Show a list of the available predefined functions with docstrings")
    group.add_argument('--quantities', action='store_true',
        help="Show a list of the available quantities in the NTuple in each file")
    general.add_argument('-L', '--live', action='store_true',
        help="Live plotting: directly display the plot on your local EKP machine.")
    general.add_argument('-N', '--nologo', action='store_true',
        help="Don't show the merlin logo at startup")
    general.add_argument('-w', '--www', type=str, default="", nargs='?',
        help="Push output plots directly to your public EKP webspace")
    general.add_argument('--sync', action='store_true',
        help="After plotting, push all files in the output directory into the \
        corresponding folder on your local desktop ('plots/...')")
    general.add_argument('-u', '--uncertaintyband', action='store_true',
        help="responseratio plots: add an uncertaintyband in the ratio")

    general.add_argument('--xquantities', type=str, nargs='+', default=None,
        help="several x quantities to be iterated over.")
    general.add_argument('--yquantities', type=str, nargs='+', default=None,
        help="several y quantities to be iterated over.")

    opt, unknown_arguments = parser.parse_known_args()

    # unknown arguments are also added to opt, so one can use them
    dic = {}
    if len(unknown_arguments) > 0:
        print "\033[93m WARNING: unknown arguments:",
        for argument in unknown_arguments:
            if argument.startswith('-'):
                print "\n  %s " % argument,
                argument = argument.replace('-', '')
                try:  # make sure a negative number is parsed as value, not argument
                    argument = float(argument)
                    dic[arg].append(argument)
                except:
                    arg = argument
                    dic[argument] = []
            else:
                print argument,
                #argument = float(argument)
                dic[arg].append(argument)
        print "\033[0m"

    # edit lists with one or no entries
    for key in dic.keys():
        if len(dic[key]) == 0:
            dic[key] = None
        elif len(dic[key]) == 1:
            dic[key] = dic[key][0]
    # add values from dic to opt
    for value in dic.keys():
        setattr(opt, value, dic[value])

    opt.fit_offset = 0
    parser.set_defaults(fit_offset=0)
    opt.subplot = False
    parser.set_defaults(subplot=False)
    opt.zbins = zbins
    opt.npv = npv
    opt.cut = cut
    opt.eta = eta
    opt.twoD = False
    opt.git_commit_hash = get_git_revision_short_hash()

    if opt.www is not "":
        opt.out = "/".join([artus.getPath('SYNCDIR'), datetime.date.today().strftime('%Y_%m_%d'), (opt.www or "")])
    elif opt.live:
        opt.filename = 'plot'
        opt.formats = ['pdf']
        opt.out = 'out/'

    if opt.year == 2011:
        opt.npv = [(0, 3), (4, 5), (6, 7), (8, 10), (11, 24)]
        opt.lumi = 5.1
        opt.energy = 7

    # get a separate dictionary with only the user-set values
    user_options = {}
    default_options = {}
    for key in vars(opt):
        default_options[key] = parser.get_default(key)
        if vars(opt)[key] is not parser.get_default(key):
            user_options[key] = vars(opt)[key]
    opt.user_options = user_options
    opt.default_options = default_options

    # if labels are not explicitly give, take x/y quantities or file names without extension
    if opt.labels == []:
        if len(opt.files) == 1 and opt.xquantities is not None and len(opt.xquantities) > 1:
                opt.labels = opt.xquantities
        elif len(opt.files) == 1 and opt.yquantities is not None and len(opt.yquantities) > 1:
                opt.labels = opt.yquantities
        elif len(opt.files) == 1 and opt.selection is not None and len(opt.selection) > 1:
                opt.labels = opt.selection
        else:
            opt.labels = [r"%s" % f.split(".root")[0].split("/")[-1].title().replace('Mc', 'MC') for f in opt.files]
        opt.labels = [string.replace("_", " ") for string in opt.labels]
        opt.default_options['labels'] = opt.labels

    matplotlib.rcParams.update(plotrc.getstyle(opt.layout))
    matplotlib.rc('text.latex', preamble=r'\usepackage{helvet},\usepackage{sfmath}')

    # if there is only 1 file but n selections/x-quantities/y-quantities, automatically
    # expand opt.files to length n
    if len(opt.files) == 1:
        n = max(
         (len(opt.selection) if opt.selection is not None else 0),
         (len(opt.xquantities) if opt.xquantities is not None else 0),
         (len(opt.yquantities) if opt.yquantities is not None else 0),
        )
        opt.files *= max(1, n)

    return opt


def get_git_revision_short_hash():
    p = subprocess.Popen(['git', 'rev-parse', '--short', 'HEAD'], stdout=subprocess.PIPE)
    out, err = p.communicate()
    return out[:-1]


htmlTemplate = """<!DOCTYPE html>
<html>
<head>
<style type="text/css">
div { float:left; }
pre { display: inline; padding: 3px 7px; font-size: 16px; background-color: #F5F5F5; border: 1px solid rgba(0, 0, 0, 0.15); border-radius: 4px; }
h3 { color: #888; font-size: 16px; }
</style>
</head>
<body>
<h1>Plot overview</h1>
<p>A <a href=".">file list</a> is also available and all plots can be downloaded using</p>
<p><code>wget -r -l 1 %s</code></p>
%s
</body>
</html>
"""
htmlTemplatePlot = """<div><h3>%s</h3><a href="%s" title="%s"><img src="%s" height="400"></a></div>\n"""


if __name__ == "__main__":
    """ This is the starting point for plotting.
        1. Get options  from command line
        2. Call the plotbase plotting function
        3. some additional commands for live and web plotting are used
    """
    print "Merlin - the plot wizard"

    opt = options()

    if not opt.nologo:
        print """\n
              *    Hocus pocus ploticus!                
             / \                                        
            /___\                        ______________ 
           ( o o )            * *       |         |data|
           )  L  (           /   * *    |       s |MC  |
   ________()(-)()________  /     * *  *|       d '----|
 E\| _____ )()()() ______ |/3     * * * |       d:     |
   |/      ()()()(       \|        * * *|      yNh     |
           | )() |                    * |      yMh     |
           /     \                     *|     :mMm/    |
          / *  *  \                     |__.-omMMMm+-__|
         /   *  *  \                                    
        / *_  *  _  \                                   
"""

    plotbase.plot(opt)

    user = artus.getPath('USER')
    userpc = "%s@%s" % (user, artus.getPath('USERPC'))

    if opt.www is not "":
        if not os.path.exists(opt.out):
            print "No syncdir %r found!" % opt.out
            exit(1)
        print "Copying plots to webspace..."
        # make gallery
        date = datetime.date.today().strftime('%Y_%m_%d')
        url = "http://www-ekp.physik.uni-karlsruhe.de/~%s/plots_archive/%s/%s/overview.html" % (user, date, (opt.www or ""))
        plots = sorted(os.listdir(opt.out))
        content = ""
        for plot in [p for p in plots if '.png' in p]:
            href = plot.replace('.png', '.pdf')
            if href not in plots:
                href = plot
            title = plot.split('/')[-1][:-4].replace('_', ' ')
            content += htmlTemplatePlot % (title, href, title, plot)
        with open(opt.out + '/overview.html', 'w') as f:
            f.write(htmlTemplate % (url, content))
        if os.path.basename(url) not in plots:
            plots.append(os.path.basename(url))
        # sync
        remote_dir = '/disks/ekpwww/web/%s/public_html/plots_archive/%s/%s/' % (user, date, (opt.www or ""))
        create_dir_command = ['ssh', userpc, 'mkdir -p', remote_dir]
        subprocess.call(create_dir_command)
        command = ['rsync', '-u'] + [os.path.join(opt.out, p) for p in plots] + ["%s:%s" % (userpc, remote_dir)]
        subprocess.call(command)
        print "Copied %d plots to %s" % (len(plots) - 1, url)
        exit(0)

    if opt.live:
        imageviewer = 'evince'
        image = '%s.%s' % (opt.filename, opt.formats[0])
        image_with_path = '%s/%s' % (opt.out, image)
        if 'ekplx' in userpc:

            subprocess.call(['rsync', image_with_path,
                                      '%s:/usr/users/%s/plot.pdf' % (userpc, user)])

            # check if the imageviewer is running on the users local machine:
            p = subprocess.Popen(['ssh', userpc, 'ps', 'aux', '|', 'grep',
                            '"%s /usr/users/%s/%s"' % (imageviewer, user, image), '|', 'grep',
                             '-v', 'grep', '|', 'wc', '-l'], stdout=subprocess.PIPE)
            out, err = p.communicate()

            # if its not already running, start!
            if out[:1] == '0':
                print "\nOpening %s..." % imageviewer
                subprocess.Popen(['ssh', userpc,
                    'DISPLAY=:0 %s /usr/users/%s/%s &' % (imageviewer, user, image)])
        else:
            #if logged in from laptop, start image viewer directly on portal machine
            subprocess.Popen([imageviewer, image_with_path], stdout=subprocess.PIPE)
    elif opt.sync:
        remote = "%s:plots" % userpc
        subprocess.call(['rsync', opt.out, remote, "-r"])
        print "\033[92mFolder %s has been copied to %s\033[0m" % (opt.out, remote)


