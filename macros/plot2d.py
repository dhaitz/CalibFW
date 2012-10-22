import getroot
import plotbase

import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import AxesGrid 

plots = []

#plotting function for variations
def twoD_all(quantity, datamc, opt):
    for variation_quantity in ['npv', 'jet1eta','zpt']:
        for change in plotbase.getvariationlist(variation_quantity, opt):
            twoD(quantity, datamc, opt, changes=change, folder=variation_quantity)


# Main 2d Plotting function:
def twoD(quantity, files, opt, legloc='center right', changes={}, log=False, rebin=[1,1],
           file_name = "", subplot=False, subtext="", fig_axes=(), xy_names=None, normalize=True, folder=None, axtitle=None):

    change= plotbase.getchanges(opt, changes)

    datamc = [getroot.getplotfromnick(quantity, f, change, rebin) for f in files]
    
    # special dictionary for z-axis scaling (do we need this??)
    # 'quantity':[z_min(incut), z_max(incut), z_min(allevents), z_max(allevents)]
    z_dict = {
        'jet1pt':[0, 120, 0, 40],
        'jet2pt':[0, 40, 0, 40],
        'METpt':[15, 30, 15, 30],
        'ptbalance':[0.85, 1.1, 1, 4]
    }

    #determine plot type: 2D Histogram or 2D Profile, and get the axis properties
    names = quantity[3:].split('_')
    if len(names) == 3:
        xy_names = names[1:3]
        xy_names.reverse()
        z_name = names[0]
        if z_name in z_dict:
            if 'incut' in changes and changes['incut']=='allevents':
                z_min = z_dict[z_name][2]
                z_max = z_dict[z_name][3]
            else:
                z_min = z_dict[z_name][0]
                z_max = z_dict[z_name][1]
        else:
            labels_list = plotbase.getaxislabels_list(z_name)
            z_min = labels_list[0]
            z_max = labels_list[1]
            z_name = labels_list[2]
    elif len(names) == 2:
        # normalize to the same number of events
        for d in datamc[1:]:
            if d.binsum() > 0.0: d.scale(datamc[0].binsum() / d.binsum() )
        xy_names = names
        z_name = 'Events'
        z_min = 0.0
        z_max = max([plot.maxBin() for plot in datamc])


    if subplot==True:
        fig = fig_axes[0]
        grid = [fig_axes[1]]
    else: 
        # create figure  + axes
        fig = plt.figure(figsize=(10.*len(datamc), 7.))
        grid = AxesGrid(fig, 111,
                        nrows_ncols = (1, len(datamc)),
                        axes_pad = 0.4,
                        share_all=True,
                        label_mode = "L",
                        cbar_pad = 0.2,
                        cbar_location = "right",
                        cbar_mode='single',
                        )



    for plot, label, ax in zip(datamc, opt.labels, grid):
        cmap1 = matplotlib.cm.get_cmap('jet')
        image = ax.imshow(plot.BinContents, 
            interpolation='nearest',
            cmap=cmap1,
            origin='lower',
            aspect = 'auto',
            extent = [plot.yborderlow, plot.yborderhigh, plot.xborderlow, plot.xborderhigh],
            vmin=z_min,
            vmax=z_max)

        # labels:
        if 'MC' in label: mc = True
        else: mc = False
        if not subplot: plotbase.labels(ax, opt, legloc=False, frame=True, changes=change, jet=False,
                                        sub_plot=subplot, mc=mc, color='white', energy_label=(not subplot))
        if axtitle is not None:
            ax.set_title(axtitle)
        else:
            ax.set_title(label)
        plotbase.axislabels(ax, xy_names[0], xy_names[1])

    if subplot: return

    #add the colorbar
    cb = fig.colorbar(image, cax = grid.cbar_axes[0], ax=ax)
    cb.set_label(z_name)


    # create filename + folder
    file_name = plotbase.getdefaultfilename(quantity, opt, change)
    if folder is not None:
        file_name = quantity+"/"+folder+"/"+file_name
        plotbase.EnsurePathExists(opt.out+"/"+quantity)
        plotbase.EnsurePathExists(opt.out+"/"+quantity+"/"+folder)

    plotbase.Save(fig, file_name, opt)


def ThreeD(files, opt, changes={}, rebin=[2,2]):
    from mpl_toolkits.mplot3d import Axes3D
    from matplotlib import cm
    import numpy as np
    import random

    change= plotbase.getchanges(opt, changes)
    change['incut']='allevents'
    datamc = [getroot.getplotfromnick("2D_jet1eta_jet1phi", f,change, rebin) for f in files[1:]]

    # create supporting points
    x = np.linspace(-5,5,100/rebin[0])
    y = np.linspace(-3.2,3.2,100/rebin[1])
    X,Y = np.meshgrid(x,y)

    # create numpy array
    Z = np.array(datamc[0].BinContents)


    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # 
    ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=cm.jet, linewidth=0)

    # set label + limits
    ax.set_zlim3d(0, datamc[0].maxBin())
    ax.set_xlabel(r'$\eta$')
    ax.set_ylabel(r'$\phi$')

    n = 360
    for i in range(n):

        # rotate viewing angle
        ax.view_init(20,-120+(360/n)*i)

        """if (i % 2 == 0):
            ax.text(0, 0, 11000, "WARNING!!!", va='top', ha='left', color='red', size='xx-large')
        ax.text(0, 0, 9800, "critical spike detected!", va='top', ha='left', color='black')
        ax.text(0, 0, 9100, str(random.random())+str(random.random())+str(random.random()), va='top', ha='center', color='black')"""

        # create filename + save
        plotbase.Save(fig, str(i).zfill(3), opt)

# for now, disable this
"""def twoD_all_grid(quantity, datamc, opt):
    pt_thresholds = [12, 16, 20, 24, 28, 32, 36]
    var_list = ['var_JetPt_%1.fto%1.f' % (s1, s2) for (s1, s2) in zip(pt_thresholds, [1000, 1000, 1000, 1000, 1000, 1000, 1000])]
    var_list_2 = getroot.npvstrings(opt.npv)

    fig = plt.figure(figsize=(10.*len(var_list), 7.*len(var_list_2)))
    grid = AxesGrid(fig, 111,
                        nrows_ncols = (len(var_list), len(var_list_2)),
                        axes_pad = 0.4,
                        share_all=True,
                        label_mode = "L",
                        #aspect = True,
                        #cbar_pad = 0,
                        #cbar_location = "right",
                        #cbar_mode='single',
                        )

    for n1, var1 in enumerate(var_list):
            for n2, var2 in enumerate(var_list_2):
                    change = {'var':var1+"_"+var2}
                    index = len(var_list_2)*n1 + n2
                    change['incut']='allevents'
                    twoD(quantity, datamc, opt, changes=change, fig_axes = [fig, grid[index]], subplot = True, axtitle = change['var'].replace('var_', ''))

    for grid_element, var_strings in zip(grid, opt.npv):
        text = r"$%s\leq\mathrm{NPV}\leq%s$" % var_strings
        grid_element.text(0.5, 5.5, text, ha='center', va='center', size ='40')

    for grid_element, pt_threshold in zip(grid[::len(var_list_2)], pt_thresholds):
        text = r"$p_\mathrm{T}^\mathrm{Jet1}$"+"\n"+r"$\geq%s\mathrm{GeV}$" % pt_threshold
        grid_element.text(-8.7, 0, text, ha='left', va='center', size ='30')

    #fig.suptitle("%s leading jet $\eta-\phi$ distribution ($before$ cuts) for %s %s" % (opt.labels[0], opt.algorithm, opt.correction), size='50')
    fig.suptitle("%s %s $\eta-\phi$ distribution ($before$ cuts) for %s %s" % (opt.labels[0], quantity[7:-16], opt.algorithm, opt.correction), size='30')

    file_name = "grid_"+opt.labels[0]+"_"+quantity +"_"+opt.algorithm + opt.correction
    fig.set_figwidth(fig.get_figwidth() * 1.2)
    plotbase.Save(fig, file_name, opt, crop=False, pad=1.5)"""


