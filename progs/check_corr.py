from ROOT import TCanvas, TH1D, TF1, TStyle, kTRUE, TFile, TTree, TParticle, TGraphErrors

# colors
from ROOT import kWhite, kRed, kBlue

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

GetDefaultStyle().cd()


#c1.Print( "out.png")

# 10 / 40 / 100

fin = TFile( "/local/scratch/hauth/data/ZPJ2010/mu_data_2010a+b.root" )
tree = fin.Get("ak5PFJets_Zplusjet_data_l2corr_events")

countEntries = tree.GetEntries()

range10 = [] # up to 10 jet.pt
range40 = []
range100 = []


for i in range( countEntries):
  tree.GetEntry( i )
  print "Z.Pt = {0} Jet1.Pt = {1} l2corrJet1 = {2}".format ( tree.Z.Pt(), tree.jet1.Pt(), tree.l2corrJet) 
  
  if (tree.jet1.Pt() < 10 ):
    range10.append( (tree.jet1.Eta(), tree.l2corrJet)  )
  elif ( tree.jet1.Pt() < 40 ):
    range40.append( (tree.jet1.Eta(), tree.l2corrJet)  )
  else:
    range100.append( (tree.jet1.Eta(), tree.l2corrJet)  )
  
  #fun1.SetPoint( i, tree.jet1.Eta(), tree.l2corrJet )

fun10 = TGraphErrors(len ( range10 ))
fun10.SetName( "correction_test_10" )

fun40 = TGraphErrors(len ( range40 ))
fun40.SetName( "correction_test_40" )
fun40.SetMarkerColor( kRed )

fun100 = TGraphErrors(len ( range100 ))
fun100.SetName( "correction_test_100" )
fun100.SetMarkerColor( kBlue )


i = 0;
for (eta, corr) in range10:
  fun10.SetPoint( i, eta, corr )
  i += 1

i = 0;
for (eta, corr) in range40:
  fun40.SetPoint( i, eta, corr )
  i += 1

i = 0;
for (eta, corr) in range100:
  fun100.SetPoint( i, eta, corr )
  i += 1

fun10.Draw("pa")
fun40.Draw("psame")
fun100.Draw("psame")
c1.Print( "out.png" )

fin.Close()


