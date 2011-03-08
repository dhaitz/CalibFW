from ROOT import *

import CalibFwPlotBase

prefix ="/local/scratch/hauth/data/ZPJ2010"
filename_mc=prefix+"/extrapolation_dy_pu_03.root"
filename_data=prefix+"/extrapolation_data_03.root"

ifile_data=TFile(filename_data)
ifile_mc=TFile(filename_mc)

out_file = TFile("pt_ratio.root", "RECREATE")
out_file.cd()

gROOT.ProcessLine(".L $FWPATH/macros/tdrstyle_mod.C")
gROOT.ProcessLine("TDR_Style()")
gStyle.SetPalette(2)

gStyle.SetOptStat(1111111)
#balance_mpf_corr_ak5PFJetsL1L2L3Res_Zplusjet_data_Pt30to60_hist-

gStyle.SetOptStat(0)

#for pt_bin in pt_bins:
#dirname="%s_incut" %(pt_bin)
#name_part="%sresp_ak5PFJetsL1L2L3Res_Zplusjet_data_%s_hist" %(pt_bin)
#oname="%s/%s" %(dirname,oname_part)

#    histo_data = getattr(ifile_data,oname)
#    histo_data.SetName(pt_bin)  
#    histo_data.SetMarkerStyle(8)
#    histo_data.SetMarkerSize(1)
#    histo_data.Scale(1./histo_data.GetSumOfWeights())
#    histo_data.Rebin(4)

#    oname=oname.replace("data","mc")
#    oname=oname.replace("L3Res","L3")
#    histo_mc = getattr(ifile_mc,oname)
#    if resptype=="mpf":
#    histo_mc.SetLineColor(kRed-9)
#    histo_mc.SetFillColor(kRed-9)
#    histo_mc.SetTitle(pt_bin+";Z+jet MPF;")
#    else:
#    histo_mc.SetLineColor(kOrange)
#    histo_mc.SetFillColor(kOrange)
#    histo_mc.SetTitle(pt_bin+";Z+jet Balance;")
#    histo_mc.Scale(1./histo_mc.GetSumOfWeights())
#    histo_mc.Rebin(4)

#    histo_mc.GetYaxis().SetRangeUser(0.001,.29)

#    c=TCanvas("pippo"+pt_bin,"poppo",600,600)
#    ks = histo_mc.KolmogorovTest(histo_data)
#    histo_mc.Draw("HistE")
#    histo_data.Draw("PSame")    
#    label=pt_bindicoin
#    label=label.replace("to"," < p_{T}^{Z} < " )
#    label=label.replace("Pt","")
#    latex=TLatex(.2,.8,"#bf{#scale[.7]{#splitline{%s}{Anti-kt R=0.5 PF Fully Corrected Jets}}}" %label)
#    latex.SetNDC()
#    latex.Draw()

def plotRatio( source_plot_data, source_plot_mc, title, caption ):

    c=TCanvas(source_plot_data,source_plot_data,600,600)

    c.SetLeftMargin( 0.24 )  

    mg = TMultiGraph()

    prefix = "pt_ratio_"

    AvgAvg_mc = CalibFwPlotBase.safeGet(ifile_mc,source_plot_mc + "_mc_graph")
    AvgAvg_data = CalibFwPlotBase.safeGet(ifile_data,source_plot_data + "_data_graph")

    AvgAvg_mc.SetLineColor(kBlue)
    AvgAvg_mc.SetMarkerColor(kBlue)
    AvgAvg_mc.SetFillColor(kBlue)
    AvgAvg_mc.SetLineWidth(2)
    AvgAvg_mc.SetTitle("MC")
    AvgAvg_mc.SetMarkerStyle(20)
    mg.Add( AvgAvg_mc, "p")

    AvgAvg_data.SetLineColor(kRed)
    AvgAvg_data.SetMarkerColor(kRed)
    AvgAvg_data.SetFillColor(kRed)
    AvgAvg_data.SetLineWidth(2)
    AvgAvg_data.SetTitle("Data")
    AvgAvg_data.SetFillStyle(0)

    AvgAvg_data.Write()
    AvgAvg_mc.Write()

    AvgAvg_data.SetMarkerStyle(20)
    mg.Add( AvgAvg_data, "p" )


    #mg.SetTitle( "Calo to PF Ratio;p_{T}^{Z};#frac{<Calo Pt>}{<PF Pt>}" )
    mg.SetTitle( title )
    mg.Draw("a")
    mg.GetYaxis().SetTitleOffset(2.1)
    mg.GetYaxis().SetTitleSize(0.048)

    #latex=TLatex(.2,.8,"#bf{#scale[.7]{#splitline{%s}{Anti-kt R=0.5 PF Fully Corrected Jets}}}" %label)
    latex=TLatex(.27,.85, caption)
    latex.SetNDC()
    latex.Draw()

    cmsPrel(36)
        #gROOT.ProcessLine("cmsPrel(36)")

    leg = TLegend(0.7, 0.21, 0.9, 0.34)
    leg.SetBorderSize( 0 )
    leg.SetFillStyle( 0 )
    leg.AddEntry( AvgAvg_data, "", "p" )
    leg.AddEntry( AvgAvg_mc, "", "p" )
    leg.Draw()

        #raw_input()
    c.Print(prefix + source_plot_data + ".png" )  
    c.Print(prefix + source_plot_data + ".pdf" )  
    c.Print(prefix + source_plot_data + ".eps" )  
    c.Print(prefix + source_plot_data + ".root" )  
    #c.Draw()
    #s = raw_input('press return to go on')

#raw
plotRatio   ("calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJets_Zplusjet", 
            "calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJets_Zplusjet",
            "Calo to PF Ratio;p_{T}^{Z};#frac{< p_{T}^{Calo} >}{< p_{T}^{PF} >}",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF Raw Jets}}")

plotRatio   ("calo_pf_avg_ratio_vs_pf_pt_ak5PFJets_Zplusjet", 
            "calo_pf_avg_ratio_vs_pf_pt_ak5PFJets_Zplusjet",
            "Calo to PF Ratio;p_{T}^{PF};<#frac{p_{T}^{Calo}}{p_{T}^{PF}}>",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF Raw Jets}}")


#L1L2L3 (Res)
plotRatio   ("calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJetsL1L2L3Res_Zplusjet", 
            "calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJetsL1L2L3_Zplusjet", 
            "Calo to PF Ratio;p_{T}^{Z};#frac{< p_{T}^{Calo} >}{< p_{T}^{PF} >}",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF fully corrected Jets}}")

plotRatio   ("calo_pf_avg_ratio_vs_pf_pt_ak5PFJetsL1L2L3Res_Zplusjet", 
             "calo_pf_avg_ratio_vs_pf_pt_ak5PFJetsL1L2L3_Zplusjet",
            "Calo to PF Ratio;p_{T}^{PF};<#frac{p_{T}^{Calo}}{p_{T}^{PF}}>",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF fully corrected Jets}}")


#L1L2
plotRatio   ("calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJetsL1L2_Zplusjet", 
            "calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJetsL1L2_Zplusjet", 
            "Calo to PF Ratio;p_{T}^{Z};#frac{< p_{T}^{Calo} >}{< p_{T}^{PF} >}",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF PU and #eta corrected Jets}}")

plotRatio   ("calo_pf_avg_ratio_vs_pf_pt_ak5PFJetsL1L2_Zplusjet", 
             "calo_pf_avg_ratio_vs_pf_pt_ak5PFJetsL1L2_Zplusjet",
            "Calo to PF Ratio;p_{T}^{PF};<#frac{p_{T}^{Calo}}{p_{T}^{PF}}>",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF PU and #eta corrected Jets}}")


#L1
plotRatio   ("calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJetsL1_Zplusjet", 
            "calo_avg_pf_avg_ratio_vs_z_pt_ak5PFJetsL1_Zplusjet", 
            "Calo to PF Ratio;p_{T}^{Z};#frac{< p_{T}^{Calo} >}{< p_{T}^{PF} >}",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF PU corrected Jets}}")

plotRatio   ("calo_pf_avg_ratio_vs_pf_pt_ak5PFJetsL1_Zplusjet", 
             "calo_pf_avg_ratio_vs_pf_pt_ak5PFJetsL1_Zplusjet",
            "Calo to PF Ratio;p_{T}^{PF};<#frac{p_{T}^{Calo}}{p_{T}^{PF}}>",
            "#bf{#scale[.7]{Anti-kt R=0.5 PF PU corrected Jets}}")

ifile_mc.Close()
ifile_data.Close()

out_file.Close()

