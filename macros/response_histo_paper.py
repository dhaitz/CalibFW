from ROOT import *

prefix ="/home/piparo/Paper"
filename_mc=prefix+"/extrapolation_dy_pu_03_ALLALGOSRENAMEME.root"
filename_data=prefix+"/extrapolation_data_03.root"

pt_bins=["Pt0to15",
         "Pt15to30",
         "Pt30to60",
         "Pt60to100",
         "Pt100to500",
         "Pt30to500"][2:3]


ifile_data=TFile(filename_data)
ifile_mc=TFile(filename_mc)

gROOT.ProcessLine(".L $FWPATH/macros/tdrstyle_mod.C")
gROOT.ProcessLine("setTDRStyle()")
gStyle.SetPalette(2)

gStyle.SetOptStat(1111111)
#balance_mpf_corr_ak5PFJetsL1L2L3Res_Zplusjet_data_Pt30to60_hist-

gStyle.SetOptStat(0)
  
for resptype in ["mpf","jet"]:
  for pt_bin in pt_bins:
    dirname="%s_incut" %(pt_bin)
    oname_part="%sresp_ak5PFJetsL1L2L3Res_Zplusjet_data_%s_hist" %(resptype,pt_bin)
    oname="%s/%s" %(dirname,oname_part)
    
    histo_data = getattr(ifile_data,oname)
    histo_data.SetName(pt_bin)  
    histo_data.SetMarkerStyle(8)
    histo_data.SetMarkerSize(1)
    histo_data.Scale(1./histo_data.GetSumOfWeights())
    histo_data.Rebin(4)
    
    oname=oname.replace("data","mc")
    oname=oname.replace("L3Res","L3")
    histo_mc = getattr(ifile_mc,oname)
    if resptype=="mpf":
      histo_mc.SetLineColor(kRed-9)
      histo_mc.SetFillColor(kRed-9)
      histo_mc.SetTitle(pt_bin+";Z+jet MPF;")
    else:
      histo_mc.SetLineColor(kOrange)
      histo_mc.SetFillColor(kOrange)
      histo_mc.SetTitle(pt_bin+";Z+jet Balance;")
    histo_mc.Scale(1./histo_mc.GetSumOfWeights())
    histo_mc.Rebin(4)

    histo_mc.GetYaxis().SetRangeUser(0.001,.29)

    c=TCanvas("pippo"+pt_bin,"poppo",600,600)
    ks = histo_mc.KolmogorovTest(histo_data)
    histo_mc.Draw("HistE")
    histo_data.Draw("PSame")    
    label=pt_bin
    label=label.replace("to"," < p_{T}^{Z} < " )
    label=label.replace("Pt","")
    latex=TLatex(.2,.8,"#bf{#scale[.7]{#splitline{%s}{Anti-kt R=0.5 PF Fully Corrected Jets}}}" %label)
    latex.SetNDC()
    latex.Draw()

    #c.SetLeftMargin(c.GetLeftMargin()+0.02)
    #c.SetTopMargin(c.GetTopMargin()+0.05)
    #c.SetBottomMargin(c.GetBottomMargin()+0.02)
    
    #lumi_latex= TLatex (.18, .93 , "#scale[.7]{CMS}")
    #lumi_latex.SetNDC()
    #lumi_latex.Draw()
    
    #cem_latex =TLatex(.65, .93 , "#bf{#scale[.7]{36 pb^{-1}      #sqrt{s}= 7 TeV}}")
    #cem_latex.SetNDC()  
    #cem_latex.Draw()  
    
    cmsPrel(36)
    #gROOT.ProcessLine("cmsPrel(36)")

    #raw_input()
    c.Print("%sresp_%s.png" %(resptype,pt_bin))  
    c.Print("%sresp_%s.pdf" %(resptype,pt_bin))  
    c.Print("%sresp_%s.eps" %(resptype,pt_bin))  

