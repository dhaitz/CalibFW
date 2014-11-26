# -*- cod
"""Matplotlib rc specs
"""
import sys
import copy
import matplotlib.font_manager


class MplStyles():
	# Raw style without any settings
	rawstyle = {}

	# Default settings that fit any usecase (may be overwritten)
	defaultstyle = {
		# font size
		'font.size': 16,
		'text.fontsize': 16,
		'legend.fontsize': 16,
		'xtick.labelsize': 16,
		'ytick.labelsize': 16,
		'axes.labelsize': 16,

		'grid.color': 'gray',
		'grid.linestyle': '-',
		'grid.linewidth': 0.5,

		# linewidth
		'axes.linewidth': 0.8,   # thickness of main box lines
		# 'patch.linewidth':  1.5:   # thickness of legend pictures and border
		# 'grid.linewidth':   1.3:   # thickness of grid lines
		# 'lines.linewidth':  2.5:   # thickness of error bars
		# 'lines.markersize': 2.5:   # size of markers
		'text.usetex': True,
		'legend.numpoints': 1,
	}

	# style for slides (usually more bold lines for reduced resolution)
	slidestyle = copy.deepcopy(defaultstyle)

	# CMS Style guidelines
	cmsstyle = copy.deepcopy(defaultstyle)
	cmsstyle.update({})

	# specific changes for JetMET publication
	cmsstyle_JetMET = copy.deepcopy(cmsstyle)
	cmsstyle_JetMET.update({
		'font.sans-serif': 'Helvetica',
	})

	# style for own documents and thesis
	documentstyle = copy.deepcopy(defaultstyle)
	documentstyle.update({
		#'figure.figsize': (6.299:  3.832),
		'figure.dpi': 600,
		'savefig.dpi': 600,

		# font
		'font.family': 'serif',
		'font.serif': 'Computer Modern Roman',
		'font.sans-serif': 'Computer Modern Sans serif',
		'font.cursive': 'Computer Modern Roman',
		'font.monospace': 'Computer Modern Typewriter',

		'figure.subplot.left': 0.125,
		'figure.subplot.right': 0.9,
		'figure.subplot.bottom': 0.1,
		'figure.subplot.top': 0.9,
		'figure.subplot.wspace': 0.2,
		'figure.subplot.hspace': 0.2,
	})


def getstyle(style='cmsstyle_JetMET'):
	s = MplStyles()
	if not hasattr(s, style):
		print "Style %r not defined!" % style
		return s.defaultstyle
	return getattr(s, style)


if __name__ == "__main__":
    styles = sys.argv[1:]
    s = MplStyles()
    availableFonts = matplotlib.font_manager.findSystemFonts(fontpaths=None, fontext='ttf')
    availableFonts = [f[f.rfind('/') + 1:-4] for f in availableFonts]
    print availableFonts
    if not styles:
        styles = [d for d in dir(s) if type(getattr(s, d)) == dict]
    for style in styles:
		print "Style: %s" % style
		for k, v in getattr(s, style).items():
			print "  {0:24}: {1}".format(k, repr(v))
			if k in ['font.serif', 'font.sans-serif', 'font.cursive', 'font.monospace'] and v not in availableFonts:
				print "       Font %s not available!" % v
