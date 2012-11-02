
# function for comparing old and new corrections
def comparison(datamc, opt):
    """file_names = [
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root'
                ]"""
    colors=['red', 'blue', 'blue', 'red']
    markers=['*', 'o', 'o', '*']
    #labels = [['MC_52xFast', 'data_52xFast'], ['MC_52xOff', 'data_52xOff'], ['MC_53xFast', 'data_53xFast'], ['MC_53xOff', 'data_53xOff']]
    rebin = 1
    import copy
    file_names = [
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                ]
    labels = [['MC_52xFast', 'data_52xFast'], ['MC_53xFast', 'data_53xFast'], ['MC_52xOff', 'data_52xOff' ], ['MC_53xOff', 'data_53xOff']]


    files=[]
    for f in file_names:
        files += [getroot.openfile(f, opt.verbose)]
    local_opt = copy.deepcopy(opt)
    local_opt.style = markers
    local_opt.colors = colors
    quantity = 'L1abs_npv'

    # ALL
    fig, axes= plotbase.newplot(subplots=4)
    for a, f1, f2, l in zip(axes, files[::2], files[1::2], labels):
        local_opt.labels = l
        datamcplot(quantity, (f1, f2), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,a),
                    rebin=rebin, subplot=True, subtext="")

    filename = "L1_all__"+opt.algorithm
    plotbase.Save(fig, filename, opt)
    """

    #Fastjet vs Offset
    fig = plotbase.plt.figure(figsize=(14,7))
    axes = [fig.add_subplot(1,2,n) for n in [1,2]]

    local_opt.labels = labels[0]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[0], files[1]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")
    local_opt.labels = labels[1]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[2], files[3]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")
        #53
    local_opt.labels = labels[2]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[4], files[5]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")
    local_opt.labels = labels[3]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[6], files[7]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")

    filename = "L1_Fastjet_vs_Offset__"+opt.algorithm
    plotbase.Save(fig, filename, opt)


    #52X vs 53X
    fig = plotbase.plt.figure(figsize=(14,7))
    axes = [fig.add_subplot(1,2,n) for n in [1,2]]

    local_opt.labels = labels[0]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[0], files[1]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labels[2]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[4], files[5]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[0]),
                        rebin=rebin, subplot=True, subtext="")

    local_opt.labels = labels[1]
    local_opt.colors = ['blue', 'blue']
    datamcplot(quantity, (files[2], files[3]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")
        #
    local_opt.labels = labels[3]
    local_opt.colors = ['red', 'red']
    datamcplot(quantity, (files[6], files[7]), local_opt, 'upper center', changes={'correction':''}, fig_axes=(fig,axes[1]),
                        rebin=rebin, subplot=True, subtext="")

    filename = "L1_52X_vs_53X__"+opt.algorithm
    plotbase.Save(fig, filename, opt)

    import plotresponse


    file_names = [
                '/storage/8/dhaitz/CalibFW/work/data_2012/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_L1Offset/out/closure.root', 
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/data_2012_Fall12JEC_L1Offset/out/closure.root',
                 '/storage/8/dhaitz/CalibFW/work/mc_madgraphSummer12_Fall12JEC_L1Offset/out/closure.root',
                ]
    labels = [['data_52xFast', 'MC_52xFast'], [ 'data_53xFast', 'MC_53xFast'], [ 'data_52xOff', 'MC_52xOff'], ['data_53xOff', 'MC_53xOff']]

    files=[]
    for f in file_names:
        files += [getroot.openfile(f, opt.verbose)]

    for over, fit in zip(['zpt', 'jet1eta', 'npv'], [True, False, True]):

        fig, axes= plotbase.newplot(subplots=4)
        fig2, axes2= plotbase.newplot(subplots=4)
        for a1, a2, f1, f2, l in zip(axes, axes2, files[::2], files[1::2], labels):
            local_opt.labels = l
            changes ={}# {'correction':'L1L2L3'}
            plotresponse.responseplot((f1, f2), local_opt, ['bal', 'mpf'], over=over, changes=changes, figaxes=(fig,a1),
                        subplot=True, subtext="")
            plotresponse.ratioplot((f1, f2), local_opt, ['bal', 'mpf'], over=over, changes=changes, figaxes=(fig2 ,a2), fit=fit,
                        subplot=True, subtext="")

        filename = "Response_"+over+"_all__"+opt.algorithm
        plotbase.Save(fig, filename, opt)

        filename = "Ratio_"+over+"_all__"+opt.algorithm
        plotbase.Save(fig2, filename, opt)"""





# function for 2d grid plots
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



