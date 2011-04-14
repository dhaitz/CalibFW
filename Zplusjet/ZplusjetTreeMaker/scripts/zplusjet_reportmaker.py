#!/usr/bin/env python

'''
The idea is to produce as many plots as possible to monitor what is done in 
the skimming procedure.
You can describe your quantities in a string and use python reflection 
properties to produce the plots.

Everything should be summarised in a web page to ease the reading.
'''

from ROOT import *
import time
import os

ROOT.gROOT.SetStyle('Plain')

#-------------------------------------------------------------------------------
ifilename="/home/ws/bd103/scartch/plain_Zplusjet_tree.root"

#-------------------------------------------------------------------------------

def draw_Zplusjet_event(particles):

    frame = TGraph2D(2)
    frame.SetName("Evt_frame")
    frame.SetTitle("Z+jet event")

    #frame.GetXaxis().SetTitle("x")
    #frame.GetYaxis().SetTitle("y")
    #frame.GetZaxis().SetTitle("z")

    # Broad enough
    l=100
    frame.SetPoint(0,l,l,l)
    frame.SetPoint(1,-l,-l,-l)

    c = TCanvas();c.cd()
    frame.Draw("AP")

    # draw the particles!
    for particle in particles:
        particle.Draw("Same")
    raw_input("continue? ")

#-------------------------------------------------------------------------------

def make_html_list(arglist):
    htmlstr='<lm>\n'
    for arg in arglist:
        htmlstr+='\t<li>%s</li>\n' %arg
    htmlstr+='</lm>\n'

    return htmlstr

#-------------------------------------------------------------------------------

def zoom_histo_x(histo):
    pass

#-------------------------------------------------------------------------------

def make_std_plots(algo,ifile):
    '''
    Make standard plots wrt the algorithms. FIXME standard??
    - Z pt spectrum
    - Muons pt Spectrum
    '''

    # name, title, obj name, method name, nbins, min, max
    plot_info=[('Z_P_spectrum_%s' %algo,'Z P_{T} spectrum %s' %algo,'Z.P()',100,0,1000),
               ('Z_pt_spectrum_%s' %algo,'Z P spectrum %s' %algo,'Z.Pt()',100,0,300),
               ('Z_E_spectrum_%s' %algo,'Z E spectrum %s' %algo,'Z.Energy()',100,0,1000),
               ('Z_eta_spectrum_%s' %algo,'Z #eta spectrum %s' %algo,'Z.Eta()',100,-10,10),
               #('jet_P_spectrum_%s' %algo,'jet P_{T} spectrum %s' %algo,'jet.P()',100,0,1000),
               #('jet_pt_spectrum_%s' %algo,'jet P spectrum %s' %algo,'jet.Pt()',100,0,300),
               ('jet_eta_spectrum_%s' %algo,'jet #eta spectrum %s' %algo,'jet.Eta()',100,-10,10),
               ('Z_jet_pt_ratio_%s' %algo,'Z/jet Pt ratio %s' %algo,'Z.Pt() / jet.Pt()',100,0,10),
               #('mus_pt_%s' %algo,'#mu s Pt %s' %algo,['mu_plus.Pt()','mu_minus.Pt()'],100,0,500),
               ('mus_eta_%s' %algo,'#mu s #eta %s' %algo,['mu_plus.Eta()','mu_minus.Eta()'],100,-10,10)]


    mytree = getattr(ifile,'%s_Zplusjet' %algo)
    for name,title,expr,nbins,minb,maxb in plot_info:
        if os.path.exists('%s.png' %name):
            continue
        h=TH1F(name,title,nbins,minb,maxb)
        for evt in mytree:
            Z=evt.Z
            jet=evt.jet
            mu_minus=evt.mu_minus
            mu_plus=evt.mu_plus
            print expr
            if type(expr)==type("str"):
                expr=[expr]
            for sub_expr in expr:
                val=eval(sub_expr)
                h.Fill(val)
            #draw_Zplusjet_event([Z,mu_plus,mu_minus,jet])
        c=TCanvas()
        c.cd()
        h.Draw()
        c.Print('%s.png' %name)
        print 'Next plot!'

    return 1

#-------------------------------------------------------------------------------

#def make_stats_table(algos, ifile):
    #htmlstr='<table border="1"><tr>'+\
            #'<th>Algorithm</th>'+\
            #'<th>Processed events</th>'+\
            #'<th>Selection Passed</th>'+\
            #'<th>Fraction</th>'+\
            #'<th>Lumi?</th>'+\
            #'</tr>\n'
    #for algo in algos:
        #mytree= getattr(ifile,'%s_Zplusjet' %algo)
        #proc_evts=int(mytree.eventsProcessed)
        #sel_pass=mytree.GetEntries()
        #frac=sel_pass/float(proc_evts)
        #htmlstr+='<tr>'+\
                 #'<td>%s</td>'%algo+\
                 #'<td>%s</td>'%proc_evts+\
                 #'<td>%s</td>'%sel_pass+\
                 #'<td>%.3s</td>'%frac+\
                 #'<td>Undefined</td></tr>\n'
    #htmlstr+='</table>'
    #return 

#-------------------------------------------------------------------------------


htmlstr='<html><body><title>Z plus jet Report</title>'\
        '<h1>Z plus jet Report %s %s</h1>\n' %(ifilename,time.asctime())

ifile = TFile(ifilename)

# The introduction

htmlstr+="Jet Algorithm processed (RECO):\n"
reco_algos=list( ifile.reco_jetalgonames)
htmlstr+=make_html_list(reco_algos)

htmlstr+='<br>'

htmlstr+="Jet Algorithm processed (GEN):\n"
gen_algos=list( ifile.gen_jetalgonames)
htmlstr+=make_html_list(gen_algos)

htmlstr+='<hr>'

# table to resume number of events!
total_events_processed=ifile.number_of_processed_events.GetMean()

htmlstr+='----Here the table----<br>'

#htmlstr+=make_stats_table(reco_algos,ifile)
#htmlstr+='<br>'
#htmlstr+=make_stats_table(gen_algos,ifile)


# Make some plots
for algo in reco_algos+gen_algos:
    make_std_plots(algo,ifile)

# Put them in the html 
htmlstr+='\n<table align="center" border="1">'

line_elements=0
for file in os.listdir('./'):
    if file[-4:]!='.png':
        continue
    if line_elements==0:
        htmlstr+='<td>\n'
    htmlstr+='<tr><image src="%s" height=240 width=320 ></image></tr>\n'%file
    line_elements+=1
    if line_elements==3:
        line_elements=0
        htmlstr+='</td>\n'

if line_elements!=0:
    for i in range(line_elements+1):
        htmlstr+='<tr>empty!</tr>\n'
    htmlstr+='</td>\n'

htmlstr+='</table>'

htmlstr+='</body></html>'

htmlfile =open ("htmlreport.html","w")
htmlfile.write(htmlstr)
htmlfile.close()