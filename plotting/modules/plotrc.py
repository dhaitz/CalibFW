# -*- cod
"""Matplotlib rc specs

   here everything is set, columns can be added
"""
import sys


rctable = [  # entry 0 is number of styles, None for standard value
3,						'generic',	'slide',	'document',
# figure
'figure.figsize',		None,		(3, 3),		(6.299, 3.832),
# (4.774, 2.950) twocolumn (3.404, 2.104) (6.299,)  (3.100,1.916)
# determined with \showthe\textwidth in LaTeX
'figure.dpi',			None,		100,		600,
'savefig.dpi',			None,		100,		600,

# font
'font.family',			None,		None,		'serif',
'font.serif',			None,		None,		'Computer Modern Roman',
'font.sans-serif',		None,		None,		'Computer Modern Sans serif',
'font.cursive',			None,		None,		'Zapf Chancery',
'font.monospace',		None,		None,		'Computer Modern Typewriter',

# font size
'font.size',			16,			16,			11,  # axis labels and text
'text.fontsize',		18,			18,			11,
'legend.fontsize',		16,			16,			11,
'xtick.labelsize',		16,			16,			11,
'ytick.labelsize',		16,			16,			11,
'axes.labelsize',		16,			16,			11,
#'axes',				None, 		[0.150, 0.175, 0.95-0.15, 0.95-0.25],
#												[0.150, 0.175, 0.95-0.15, 0.95-0.25],

'grid.color',			'gray', 	'gray',		'gray',
'grid.linestyle', 		':',		'-',		'-',		# :, -
'grid.linewidth',		0.5,		0.5,		0.5,

'figure.subplot.left',	None,		0.125,		0.125,
'figure.subplot.right',	None,		0.9,		0.9,
'figure.subplot.bottom',None,		0.1,		0.1,
'figure.subplot.top',	None,		0.9,		0.9,
'figure.subplot.wspace',None,		0.2,		0.2,
'figure.subplot.hspace',None,		0.2,		0.2,

# linewidth
'axes.linewidth', 		0.8,		0.8,		0.5,  # thickness of main box lines
# 'patch.linewidth',1.5,  # thickness of legend pictures and border
# 'grid.linewidth',	1.3,  # thickness of grid lines
# 'lines.linewidth',2.5,  # thickness of error bars
# 'lines.markersize',2.5,  # size of markers
'text.usetex',			False,		False,		True,
'legend.numpoints',		1,			1,			1,
]


def getstyle(style='generic', tab=rctable):
    maxcol = tab[0] + 1
    result = {}
    try:
        col = tab.index(style)
    except ValueError:
        print repr(style), "is no valid style from", tab[1:maxcol]
        print repr(tab[1]), "is used instead."
        col = 1
    # check table
    assert len(tab) % maxcol == 0, "Wrong table format."
    assert col < maxcol and col > 0
    # fill dictionary result
    for i in range(maxcol, len(tab)):
        if i % maxcol == 0:
            key = tab[i]
            assert type(key) is str, "Did you change the number?"
        if i % maxcol == col and tab[i] is not None:
            result[key] = tab[i]
    return result


if __name__ == "__main__":
    if len(sys.argv) > 1:
        d = getstyle(sys.argv[1])
    else:
        d = get_style()
    for k, v in d.items():
        print k, ":", v
