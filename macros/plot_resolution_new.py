import plotbase
import getroot


plots = ['bal_resolution_zpt', 'bal_resolution_eta', 'mpf_resolution_zpt', 'mpf_resolution_eta']

# ptbalance:
def bal_resolution_zpt(files, opt):
    resolution_new(files, opt, 'balresp', 'zpt')

def bal_resolution_eta(files, opt):
    resolution_new(files, opt, 'balresp', 'jet1eta')

def bal_resolution_npv(files, opt):
    resolution_new(files, opt, 'balresp', 'npv')


# MPF
def mpf_resolution_zpt(files, opt):
    resolution_new(files, opt, 'mpfresp', 'zpt')

def mpf_resolution_eta(files, opt):
    resolution_new(files, opt, 'mpfresp', 'jet1eta')

def mpf_resolution_npv(files, opt):
    resolution_new(files, opt, 'mpfresp', 'npv')


def resolution_new(files, opt, quantity='mpfresp', x='zpt'):

    changes = {}
    fig, ax = plotbase.newplot()

    # variation dictionary:
    var_dict = {    
        'zpt':{
                'binstrings':getroot.binstrings(opt.bins),
                'x_quantity':"zpt",
                'changekey':'bin',
                },
        'jet1eta':{
                'binstrings':getroot.etastrings(opt.eta),
                'x_quantity':"jet1abseta",
                'changekey':'var'
                },
        'npv':{
                'binstrings':getroot.npvstrings(opt.npv),
                'x_quantity':"npv",
                'changekey':'var'
                }
    }

    # create nice labels for the plot legend
    if quantity == 'mpfresp':
        labels = ["MPF (%s)" % s for s in opt.labels]
    elif quantity == 'balresp':
        labels = ["$p_\mathrm{T}$ balance (%s)" % s for s in opt.labels]
    else:
        labels = opt.labels

    for f, l, c in zip(files, labels, opt.colors):

        y_values = []
        y_errors = []
        x_values = []

        #iterate over the list of bins:
        for var_string in var_dict[x]['binstrings']:
            changes[var_dict[x]['changekey']] = var_string

            # get the values for the gaussian fit:
            rootobject = getroot.getobjectfromnick(quantity, f, changes, rebin=1)
            p0, p0err, p1, p1err, p2, p2err, chi2, ndf, conf_intervals = getroot.fitline2(rootobject, gauss=True, limits=[0, 2])
            y_values.append(p2)
            y_errors.append(p2err)

            #get the mean of the bin on the x-axis 
            rootobject = getroot.getobjectfromnick(var_dict[x]['x_quantity'], f, changes, rebin=1)
            x_values.append(rootobject.GetMean())
        del changes[var_dict[x]['changekey']]



        ax.errorbar(x_values, y_values, y_errors, drawstyle='steps-mid', color=c, fmt='o', capsize=0 ,label=l)

    # labels and stuff:
    plotbase.labels(ax, opt, legloc='upper right', frame=True, changes=changes, jet=False, sub_plot='False')
    plotbase.axislabels(ax, var_dict[x]['x_quantity'], 'resolution')

    # override default axis limits with command line arguments:
    if opt.x_limits is not None: ax.set_xlim(opt.x_limits[0], opt.x_limits[1])
    if opt.y_limits is not None: ax.set_ylim(opt.y_limits[0], opt.y_limits[1])

    # put the file name together and save
    filename = '_'.join(['resolution', quantity, var_dict[x]['x_quantity'], opt.algorithm])+opt.correction
    plotbase.Save(fig, filename, opt)
