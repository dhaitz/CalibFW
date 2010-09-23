# -*- coding: utf-8 -*-
# generates a HTML overview site of all available plots

import glob

img_ext = 'png'

sections = [
  { 'name' : 'Jet Response by pt Bin',
    'description' : 'desc',
     'img_files' : 'jetresp_',
      'img_width' : '320'},  
  { 'name' : 'L3 correction fit: Jet Response (check)',
    'description' : 'desc',
     'img_files' : 'L3_calc_*_JetResponse_l2',
      'img_width' : '320'},
  { 'name' : 'L3 correction fit: binned data',
    'description' : 'desc',
     'img_files' : 'L3_calc_*_data_binned*_l2',
      'img_width' : '320'},

  { 'name' : 'L3 correction fit: Fit !!',
    'description' : 'desc',
     'img_files' : 'L3_calc_*_JetCorrection_l2',
      'img_desc' : 
	  { 'L3_calc_-Danilo-Funcak5PF_JetCorrection_l2.png' : """<pre>
 FCN=2.01941e-10 FROM MIGRAD    STATUS=CONVERGED    1452 CALLS        1453 TOTAL
                     EDM=5.29394e-12    STRATEGY= 1      ERR MATRIX NOT POS-DEF
  EXT PARAMETER                APPROXIMATE        STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  p0          -8.69166e-01   1.77058e+00   6.87828e-05  -1.25754e-05
   2  p1           8.52280e+00   2.19446e+00   9.40859e-05  -8.67740e-06
   3  p2          -6.16648e+00   2.25969e+00   1.02056e-04  -7.80909e-06
   4  p3           6.93753e-02   5.14194e-02   2.42948e-06   4.20668e-04
   5  p4           8.75916e-02   8.18969e-02   3.65853e-06  -2.83666e-04
ChiSquare : 2.01941e-10
</pre>
""", 
"L3_calc_-AN-Funcak5PF_JetCorrection_l2.png" :"""<pre> FCN=0.00107901 FROM MIGRAD    STATUS=CONVERGED     639 CALLS         640 TOTAL
                     EDM=2.73216e-08    STRATEGY= 1      ERR MATRIX NOT POS-DEF
  EXT PARAMETER                APPROXIMATE        STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  p0          -2.19524e+00   1.90746e+00   6.88901e-05  -2.24186e-04
   2  p1           1.24091e+01   6.74837e+00   2.51374e-04  -6.42413e-05
   3  p2           2.21615e-01   4.78805e-01   3.52414e-05   9.13398e-05
   4  p3           2.25316e+00   1.88164e+00   7.48096e-05   1.36290e-04
ChiSquare : 0.00107901
</pre>"""
},
      'img_width' : '320'},
## directly include fit results here
  { 'name' : 'JetResponse L2 corrected',
    'description' : 'desc',
     'img_files' : '*JetResponse_l2',
      'img_width' : '320'},
  { 'name' : 'Contour L2 corrected',
    'description' : 'The statistic at disposal is not yet enough to build a data-driven L3 calibration. Nevertheless, we checked the comparison of Z-jet balancing in data and Monte Carlo. To do that we built a combined two-dimensional likelihood for the Jet Energy Scale and the Jet Resolution (For the moment simply estimated as the width of the response distributions) with respect to Monte Carlo. This measurement shows that there is a need for a data driven calibration method (jet energy scale ratio is 0.91+-0.04 - this statistical precision can reach the 2 percent level at the end of the year with ~30 pb-1).',      
    'img_files' : '*JetResponse_l2',
      'img_width' : '320'},
  { 'name' : 'Combined Muons',
    'description' : 'desc',
     'img_files' : 'mus_',
      'img_width' : '320'},
  { 'name' : 'Z Boson',
    'description' : 'desc',
     'img_files' : 'z',
      'img_width' : '320'},
  { 'name' : 'Number of Events',
    'description' : 'desc',
     'img_files' : 'number_of_events',
      'img_width' : '450'},
  { 'name' : 'Jet1',
    'description' : 'desc',
     'img_files' : 'jet1',
      'img_width' : '320'},
  { 'name' : 'Jet2',
    'description' : 'desc',
     'img_files' : 'jet2',
      'img_width' : '320'},


]

htfile = open( 'zpj_overview.html', 'w' );
html_head = '''<html> <head> <title> ZplusJet Energy Calibratio - WIP </title> </head><body>'''
html_tail = '''</body></html>'''

htfile.write( html_head )


print "Generation HTML overview for the following sections:" 
print sections

for sec in sections:
  files = glob.glob( sec['img_files'] + "*" + "." + img_ext )
  html_sec = "<h1>%s</h1>%s<br/><br/>" % (sec['name'], sec['description'] )

  for imgfile in files:
    html_sec += "<a href='%s'><img src='%s' width='%s'/></a>" % ( imgfile, imgfile, sec['img_width'] )
    if sec.has_key( "img_desc"):
      if sec["img_desc"].has_key( imgfile ):
	html_sec += "<br/>%s<br/>"  % ( sec["img_desc"][ imgfile ])

  htfile.write( html_sec )

htfile.write( html_tail )
htfile.close()
  

