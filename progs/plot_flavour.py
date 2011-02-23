from ROOT import TCanvas, TH1D, TF1, TStyle, kTRUE, TFile, TTree, TParticle, TGraphErrors, TMultiGraph
from ROOT import TLatex


# colors
from ROOT import kWhite, kRed, kBlue, kOrange, kGreen, kBlack

def safeGet( RootDict, ObjectName ):
    oj = RootDict.Get( ObjectName )
    if not oj:
        print "Cant load " + ObjectName + " from "  + RootDict.GetName()
        assert False

    return oj

def iterBins( bins):
    
    binList = []
    
    for i in range( len( bins) -1):
        binList.append( ( bins[i], bins[i + 1]  ))
    
    return binList

def BinToString( (low, high) ):
    return "Pt" + str(low) + "to" + str(high)

def GetDefaultStyle():
  theStyle = TStyle("cholder","cholder")

  # For the canvas:
  theStyle.SetCanvasBorderMode(0)
  theStyle.SetCanvasColor(kWhite)
  theStyle.SetCanvasDefH(600)
  theStyle.SetCanvasDefW(600)
  theStyle.SetCanvasDefX(0)
  theStyle.SetCanvasDefY(0)

  # For the Pad:
  theStyle.SetPadBorderMode(0)  
  theStyle.SetPadColor(kWhite)

  #theStyle.SetPadGridX(m_grid_x)
  #theStyle.SetPadGridY(m_grid_y)
  theStyle.SetGridColor(0)
  theStyle.SetGridStyle(3)
  theStyle.SetGridWidth(1)
  theStyle.SetPadTopMargin(0.05)
  theStyle.SetPadBottomMargin(0.13)
  theStyle.SetPadLeftMargin(0.14)
  theStyle.SetPadRightMargin(0.08)
  
  # For the frame:
  theStyle.SetFrameBorderMode(0)
  theStyle.SetFrameBorderSize(1)
  theStyle.SetFrameFillColor(0)
  theStyle.SetFrameFillStyle(0)
  theStyle.SetFrameLineColor(1)
  theStyle.SetFrameLineStyle(1)
  theStyle.SetFrameLineWidth(1)

  #For the histo:
  theStyle.SetHistLineColor(1)
  theStyle.SetHistLineStyle(0)
  theStyle.SetHistLineWidth(2)
  theStyle.SetEndErrorSize(2)
  theStyle.SetErrorX(0.)
  theStyle.SetMarkerStyle(20)

  #For the fit/function:
  theStyle.SetOptFit(0)
  theStyle.SetFitFormat("5.4g")
  theStyle.SetFuncColor(1)
  theStyle.SetFuncStyle(1)
  theStyle.SetFuncWidth(1)

  # For the date:
  theStyle.SetOptDate(0);

  # For the statistics box:
  theStyle.SetOptFile(0)
  theStyle.SetOptStat(0)
  theStyle.SetStatColor(kWhite)
  theStyle.SetStatFont(42)
  theStyle.SetStatFontSize(0.025)
  theStyle.SetStatTextColor(1)
  theStyle.SetStatFormat("6.4g")
  theStyle.SetStatBorderSize(1)
  theStyle.SetStatH(0.1)
  theStyle.SetStatW(0.15)

  # Margins:
  theStyle.SetPadTopMargin(0.05)
  theStyle.SetPadBottomMargin(0.23)
  theStyle.SetPadLeftMargin(0.17)
  theStyle.SetPadRightMargin(0.05)

  # For the Global title:

  theStyle.SetOptTitle(0)
  theStyle.SetTitleFont(42)
  theStyle.SetTitleColor(1)
  theStyle.SetTitleTextColor(1)
  theStyle.SetTitleFillColor(10)
  theStyle.SetTitleFontSize(0.05)

  # For the axis titles:
  theStyle.SetTitleColor(1, "XYZ")
  theStyle.SetTitleFont(42, "XYZ")
  theStyle.SetTitleSize(0.05, "XYZ")
  theStyle.SetTitleXOffset(0.9)
  theStyle.SetTitleYOffset(1.1)

  # For the axis labels:
  theStyle.SetLabelColor(1, "XYZ")
  theStyle.SetLabelFont(42, "XYZ")
  theStyle.SetLabelOffset(0.007, "XYZ")
  theStyle.SetLabelSize(0.045, "XYZ")

  # For the axis:
  theStyle.SetAxisColor(1, "XYZ")
  theStyle.SetStripDecimals(kTRUE)
  theStyle.SetTickLength(0.03, "XYZ")
  theStyle.SetNdivisions(510, "XYZ")
  theStyle.SetPadTickX(1)
  theStyle.SetPadTickY(1)

  # Change for log plots:
  theStyle.SetOptLogx(0)
  theStyle.SetOptLogy(0)
  theStyle.SetOptLogz(0)

  # Postscript options:
  theStyle.SetPaperSize(15.,15.)
  return theStyle


c1 = TCanvas('c1', 'Example with Formula', 200, 10, 700, 500 )

mgraph = TMultiGraph()

GetDefaultStyle().cd()

bins = [0,20,30,50,80,120, 170, 230, 270, 300, 350]
binTuples = iterBins( bins )

print binTuples
#c1.Print( "out.png")

# 10 / 40 / 100

fin = TFile( "/local/scratch/hauth/data/ZPJ2010/mc_fall10_dy_pu_reco1-2.root" )

#Pt0to30_incut
#partonflavour_ak5PFJets_Zplusjet_mc_Pt0to30_hist

flavour_gluon = TGraphErrors(len ( binTuples ))
flavour_gluon.SetName( "Gluon" )
flavour_gluon.SetTitle( "gluons" )
flavour_gluon.SetLineColor( 6 )
flavour_gluon.SetMarkerColor( 6 )
flavour_gluon.SetFillStyle(0)

flavour_uds = TGraphErrors(len ( binTuples ))
flavour_uds.SetName( "UDS_quark" )
flavour_uds.SetTitle( "uds" )
flavour_uds.SetLineColor( kRed )
flavour_uds.SetMarkerColor( kRed )
flavour_uds.SetFillStyle(0)

flavour_c = TGraphErrors(len ( binTuples ))
flavour_c.SetName( "c_quark" )
flavour_c.SetTitle( "c" )
flavour_c.SetLineColor( kOrange )
flavour_c.SetMarkerColor( kOrange )
flavour_c.SetFillStyle(0)

flavour_b = TGraphErrors(len ( binTuples ))
flavour_b.SetName( "b_quark" )
flavour_b.SetTitle( "b" )
flavour_b.SetLineColor( kGreen )
flavour_b.SetMarkerColor( kGreen )
flavour_b.SetFillStyle(0)

flavour_add = TGraphErrors(len ( binTuples ))
flavour_add.SetName( "all" )
flavour_add.SetTitle( "all" )
flavour_add.SetLineColor( kBlack )
flavour_add.SetMarkerColor( kBlack )
flavour_add.SetFillStyle(0)

i = 0

for (blow,bhigh) in binTuples:
    
    folderName = BinToString( (blow,bhigh) ) + "_incut/"
    histBaseName = "_ak5PFJets_Zplusjet_mc_" + BinToString( (blow,bhigh) )  + "_hist"
    
    histflav = safeGet( fin, folderName + "partonflavour" + histBaseName)
    histzpt = safeGet( fin, folderName + "z_pt" + histBaseName)
     
    integral = histflav.Integral()

    # particle / anti-particle, bin 51 is zero !
    print str(histflav.GetBinContent( 51 + 1 )) + "  anti: " + str(histflav.GetBinContent( 51 - 1 ))
    gluon =  histflav.GetBinContent( 51 + 21 )
    up = histflav.GetBinContent( 51 + 1 ) + histflav.GetBinContent( 51 - 1 )
    down= histflav.GetBinContent( 51 + 2 ) + histflav.GetBinContent( 51 - 2 )
    strange  = histflav.GetBinContent( 51 + 3 ) + histflav.GetBinContent( 51 - 3 )

    charm  = histflav.GetBinContent( 51 + 4 ) + histflav.GetBinContent( 51 - 4 )
    beauty  = histflav.GetBinContent( 51 + 5 ) + histflav.GetBinContent( 51 - 5 )
         
    #if overall > 0:
    flavour_gluon.SetPoint(i, histzpt.GetMean(), gluon/integral )
    flavour_uds.SetPoint(i, histzpt.GetMean(), (strange + up + down)/integral )
    flavour_c.SetPoint(i, histzpt.GetMean(), (charm)/integral )
    flavour_b.SetPoint(i, histzpt.GetMean(), (beauty )/integral )
    
    flavour_add.SetPoint( i, histzpt.GetMean(), 
                          (beauty + charm + strange + up + down + gluon )/integral )
    
    #flavour.SetPoint(i, i + 2 , 1.2  )
    print "bam"
    
    i = i + 1

c1.SetLogy()
mgraph.Add(flavour_gluon,"lp")
mgraph.Add(flavour_uds,"lp")
mgraph.Add(flavour_c,"lp")
mgraph.Add(flavour_b,"lp")
mgraph.Add(flavour_add,"lp")





mgraph.SetTitle( "Flavour Fraction;p_{T}^{Z};Flavour Fraction" )
mgraph.Draw("a")
mgraph.GetYaxis().SetRangeUser(0.01, 1.1)

#mgraph.GetYaxis().SetTitle( "Flavour Fraction" )

leg = c1.BuildLegend()
leg.SetBorderSize( 1 )
leg.SetX1( 0.7 )
leg.SetX2( 0.85 )
leg.SetY1( 0.14 )
leg.SetY2( 0.29 )



lumi = TLatex()
lumi.SetTextSize(0.04)
lumi.SetNDC(True)
lumi.DrawLatex( 0.1, 0.93, "#scale[.8]{#int} L = 36 pb^{-1}")
lumi.DrawLatex( 0.81, 0.93, "#sqrt{s}= 7 TeV")


c1.Draw()
c1.Print( "flavour_fract.png" )

s = raw_input('press return to go on')

fin.Close()



