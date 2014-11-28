import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import AxesGrid
import numpy as np

import getroot
import plotbase
import plot1d


def twoD(quantity, files, opt, fig_axes=(), changes=None, settings=None):
    """ Main 2D plotting function. """

    # if no settings are given, create:
    settings = plotbase.getSettings(opt, changes, settings, quantity)
    print "A %s plot is created with the following selection: %s" % (quantity,
                                                          settings['selection'])

    settings.update({
        'legloc': "None",
        'twoD': True,
        'rebin': [1, 1]
    })

    datamc, rootobjects = plot1d.getHistos(quantity, files, settings)

    # this is the code snippet to produce the diff plot for the rms methods
    # TODO remove
    if False:
        method = 'ptbalance'
        quantity = "abs((recogen-%s)/recogen)*abs((recogen-%s)/recogen)_npv_zpt" % (method, method)
        rootobjects += [getroot.getobjectfromtree(quantity, f, settings, twoD=True)]
        #rootobjects[-1].Rebin2D(settings['rebin'], settings['rebin'])
        x = getroot.root2histo(rootobjects[-1], f.GetName(), [1, 1])
        datamc[-1].BinContents -= x.BinContents

    # TODO put this into its own function?
    if settings['ratio'] and len(datamc) == 2:
        scaling_factor = datamc[0].binsum() / datamc[1].binsum()
        rootobjects[1].Scale(scaling_factor)
        rootobjects[0].Divide(rootobjects[1])
        rootobjects = [rootobjects[0]]
        datamc = [getroot.root2histo(rootobjects[0], f.GetName(), [1, 1])]

    if len(quantity.split("_")) == 2:
        # normalize to the same number of events
        if len(datamc) > 1 and settings['normalize']:
            for d in datamc[1:]:
                if d.binsum() > 0.0 and datamc[0].binsum() > 0:
                    d.scale(datamc[0].binsum() / d.binsum())
        if settings['xynames'] is not None and len(settings['xynames']) > 2:
            z_name = settings['xynames'][2]
        else:
            z_name = "Events"
        if settings['z'] is None:
            settings['z'] = [0, np.max(datamc[0].BinContents)]
    else:
        if settings['z'] is None:
            settings['z'] = plotbase.getaxislabels_list(quantity.split("_")[0])[:2]
        if settings['xynames'] is not None and len(settings['xynames']) > 2:
            z_name = settings['xynames'][2]
        else:
            z_name = quantity.split("_")[0]

    # special dictionary for z-axis scaling (TODO do we need this??)
    # 'quantity':[z_min(incut), z_max(incut), z_min(allevents), z_max(allevents)]
    z_dict = {
        'jet1pt': [0, 120, 0, 40],
        'jet2pt': [0, 40, 0, 40],
        'METpt': [15, 30, 15, 30],
        'ptbalance': [0.85, 1.1, 1, 4],
        'genzmass': [89, 93, 90.5, 92.5],
        'genzetarapidityratio': [1, 3, 0, 5]
    }

    if settings['subplot'] == True:
        fig = fig_axes[0]
        grid = [fig_axes[1]]
    else:
        # create figure  + axes
        fig = plt.figure(figsize=(3.1 * len(datamc), 3.1))
        grid = AxesGrid(fig, 111,
                        nrows_ncols=(1, len(datamc)),
                        axes_pad=0.4,
                        share_all=True,
                        aspect=False,
                        label_mode="L",
                        cbar_pad=0.05,
                        cbar_location="right",
                        cbar_mode='single',
                        )

    for plot, label, ax in zip(datamc, settings['labels'], grid):
        ax.set_title(label)

        cmap1 = matplotlib.cm.get_cmap(settings.get('colormap', 'afmhot'))
        image = ax.imshow(plot.BinContents,
            interpolation='nearest',
            cmap=cmap1,
            origin='lower',
            aspect='auto',
            extent=[plot.xborderlow, plot.xborderhigh, plot.yborderlow, plot.yborderhigh],
            vmin=settings['z'][0],
            vmax=settings['z'][1])

        # labels:
        plotbase.axislabels(ax, settings['xynames'][0], settings['xynames'][1], settings=settings)
        plotbase.labels(ax, opt, settings, settings['subplot'], mc=('MC' in label))
        plotbase.setAxisLimits(ax, settings)

    if settings['subplot']:
        return

    #add the colorbar
    cb = fig.colorbar(image, cax=grid.cbar_axes[0], ax=ax)
    cb.set_label(plotbase.unitformat(plotbase.getaxislabels_list(z_name)[2],
                                plotbase.getaxislabels_list(z_name)[3], False))

    # create filename + folder
    settings['filename'] = plotbase.getDefaultFilename(quantity, opt, settings)
    plotbase.Save(fig, settings)


def ThreeD(files, opt, changes=None):
    """
        This is the function for 3D (animated plots).
        Much of the configuration is hardcoded here.
        Make this more configurable? Currently its not regularly used.
    """
    from mpl_toolkits.mplot3d import Axes3D
    from matplotlib import cm
    import numpy as np
    import random

    quantity = "mupluspt_muminuspt"

    settings = plotbase.getSettings(opt, changes=changes, settings=None, quantity=quantity)

    rootobjects = []
    datamc = []
    for f in files:
        rootobjects += [getroot.histofromfile(quantity, f, settings, twoD=True)]
        datamc += [getroot.root2histo(rootobjects[-1], f.GetName(), [1, 1])]

    # create supporting points
    y = np.linspace(settings['y'][0], settings['y'][1], settings['nbins'])
    x = np.linspace(settings['x'][0], settings['x'][1], settings['nbins'])
    X, Y = np.meshgrid(x, y)
    # create numpy array for Z values
    Z = np.array(datamc[0].BinContents)

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Create the surface plot
    surf = ax.plot_surface(
        X, Y, Z, rstride=1, cstride=1,
        cmap=cm.afmhot,
        linewidth=0, antialiased=True, shade=False)

    # set label + limits
    #ax.set_zlim3d(0, 100)
    ax.set_xlabel(r'$\mu^{-} p_T $')
    ax.set_ylabel(r'$\mu^{+} p_T $')
    ax.set_zlabel(r'Events')

    # if animated, n plots are outputted, each with a different viewing angle.
    # Merge with sth like 'convert -delay 5 -loop 0 <outpath>/*.png 3d.gif'
    animated = True
    if not animated:
        plotbase.Save(fig, settings)
        return
    else:
        n = 720
        filename = settings['filename']
        for i in range(n):

            # rotate viewing angle
            ax.view_init(20, -120 + (360. / n) * i)

            # create filename with extension; save
            settings['filename'] = filename + str(i).zfill(3)
            plotbase.Save(fig, settings)

