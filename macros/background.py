import getroot
import sys

datasets = {
	'DYJets': {
		'name': "DYJets",
		'file': "madgraphSummer12",
		'xsec': 2950.0,
		'feff': 1.0,
		'events': 30459503,
		'weightf': 1.0,
	},
	'QCD': {
		'name': "QCD",
		'file': 'qcd',
		'xsec': 3.64e8,
		'feff': 3.70e-4,
		'events': 21484602,
		'weightf': 1.0,
	},
	'TTbar': {
		'name': "TTJets",
		'file': 'ttbar',
		'xsec': 126.0,
		'feff': 1.0,
		'events': 5186494,
		'weightf': 1.0,
	},
	'WJets': {
		'name': "WJets",
		'file': 'wjets',
		'xsec': 30400.0,
		'feff': 1.0,
		'events': 57709905,
		'weightf': 1.0,
	},
	'data': {
		'name': "Data",
		'file': "data",
		'type': "data",
	
	}
}

# conversion from /fb to /pb.
fb2pb = 1000

def main():
	files, lumi, cuts, verb, weights, nmin, style = getopt(sys.argv)
	values = []

	for f in files:
		# search for parameters in datasets:
		item = {}
		for d in datasets.values():
			if d['file'].lower() in f.lower():
				item = d
				item['file'] = f
				break
		if not item:
			print "No parameters found for", f
			exit(1)

		# calculate expectations
		if 'type' in item and item['type'] == 'data':
			item['f'] = 1.0/lumi
		else:
			item['f'] = item['xsec']*item['feff']/item['events']/item['weightf']*fb2pb

		# read numbers from file
		rootfile = getroot.openfile(f)
		for c in cuts:
			item[c] = {}
			obj = getroot.getobjectfromnick('npv', rootfile, {'incut': c})
			if weights:
				n = obj.Integral()
			else:
				n = obj.GetEntries()
			item[c]['n'] = max(nmin, n)
			item[c]['exp'] = item[c]['n']*item['f']*lumi
			if files.index(f) > 0:
				item[c]['pro'] = item[c]['n']*item['f']*lumi*100/values[0][c]['n']
			else:
				item[c]['pro'] = item['f']*lumi*100

		values.append(item)

	if style == 'dict':
		printdicts(values)
	else:
		prints[style](transpose(maketable(values, cuts, lumi)))


def maketable(content, cuts, lumi):
	result = [["Dataset", "xsec", "events", "feff"]]
	for c in cuts:
		result[-1] += [c, "events", "exp.", r" % "]
	for i in content:
		if 'ata' in i['name']:
			result += [[i['name'], "", "L =%6.2f" % lumi, ""]]
		else:
			result += [[i['name'], i['xsec'], i['events'], i['feff']]]
		for c in cuts:
			if 'ata' in i['name']:
				result[-1] += ["", i[c]['n'], "", i[c]['pro']]
			else:
				result[-1] += ["", i[c]['n'], i[c]['exp'], i[c]['pro']]
	return result

def printdicts(content):
	for i in content:
		printdict(i)

def printtxt(content):
	print "Results:"
	for i in content:
		for j in i:
			if type(j) == str:
				print "%12s" %j,
			elif type(j) == int:
				print "%12d" % j,
			elif type(j) == float:
				print "%12.2f" % j,
		print

def printlatex(content):
	print "\\begin{table}[htb]"
	print "\t\\centering"
	print "\t\\begin{tabular}{l" + "r"*(len(content[0])-1) + "}"
	for i in content:
		#print "\t\t",
		for j in i:
			if type(j) == str:
				if i == content[0]:
					print "%16s &" % ("\\textbf{%s}" %j),
				else:
					print "%16s &" %j,
			elif type(j) == int:
				print "%16d &" % j,
			elif type(j) == float:
				print "%16.2f &" % j,
		print "\b\b\\\\"
	print "\t\\end{tabular}"
	print "\t\\caption{Background estimation}"
	print "\t\\label{tab:background}"
	print "\\end{table}"


def printlist(content):
	for i in content:
		print i

def printdict(dic):
	print "%s: {" % dic['name']
	for k, v in dic.items():
		print "  %10s: %s" % (k, v)
	print "}"

def transpose(table):
	result = []
	for l in range(len(table)):
		for e in range(len(table[l])):
			if l == 0:
				result.append([])
			result[e].append(table[l][e])
	return result

prints = {
	'dict': printdicts,
	'txt': printtxt,
	'latex': printlatex,
	'list': printlist,
}

def getopt(args):
	lumi = 1.0
	nmin = 3
	files = []
	cutfolders = ['allevents', 'zcutsonly', 'alleta', 'incut']
	verbose = False
	weights = True
	style = 'txt'
	program = args.pop(0)
	while len(args) > 0:
		try:
			if args[0] == "-l":
				lumi = float(args.pop(1))
			elif args[0] == "-m":
				nmin = float(args.pop(1))
			elif args[0] == "-s":
				style = args.pop(1)
			elif args[0] == "-c":
				cutfolders = []
				while len(args) > 1 and args[1][0] != '-':
					cutfolders.append(args.pop(1))
			elif args[0][0] != "-":
				files.append(args[0])
			elif args[0] == '--':
				pass
			elif args[0] == '-v':
				verbose = True
			elif args[0] == "-w":
				weights = False
			elif args[0] == "-h":
				pass
			else:
				print args[0], "not found!"
		except:
			print "Bad arguments!"
			args.append("-h")

		if args[0] == '-h':
			print "Usage:", program, "[options] data.root signalmc.root bkg1.root bkg2.root"
			print "Options:"
			print "  -c  specify cut folders, default:", ", ".join(cutfolders)
			print "  -h  Show this help"
			print "  -l  luminosity of data sample in /fb (default: %1.2f)" % lumi
			print "  -m  upper limit for number of events (default: %d)" % nmin
			print "  -s  table style [txt, latex, list, dict] (default: %s)" % style
			print "  -v  verbosity"
			print "  -w  do not use event weights"
			exit(0)

		del args[0]
	if verbose:
		print "Files:  ", "\n         ".join(files)
		print "Lumi:   ", lumi, "/fb"
		print "Cuts:   ", ", ".join(cutfolders)
		if weights:
			print "Weights: Use event weights."
	return files, lumi, cutfolders, verbose, weights, nmin, style

if __name__ == "__main__":
	main()
