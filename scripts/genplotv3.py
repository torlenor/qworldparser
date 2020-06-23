#!/usr/bin/python2
import os
import subprocess
import sys
import fileinput
import string
import re

plotheight=10.0
plotwidth=5.0

def process(arg):
        print '\tGetting plot size from gnuplot file...'
        gnuplotfile = open(arg, "r")
        for line in gnuplotfile:
                m = re.match("[ ]*plotheight[ ]*=[ ]*([.?\d]+)", line)
                if m:
                        print("\t\tplothheight = " + m.group(1));
                        plotheight=float(m.group(1))
                m = re.match("[ ]*plotwidth[ ]*=[ ]*([.?\d]+)", line)
                if m:
                        print("\t\tplothwidth = " + m.group(1));
                        plotwidth=float(m.group(1))
	print '\tRunning gnuplot...'
	filename = arg[0:-4]
        gpoutfile = filename + '.tex'
        gparg = " -e \"outfilename='" + gpoutfile + "'\" " + arg
	try:
		sts = subprocess.check_output(["gnuplot " + gparg],shell=True,stderr=subprocess.STDOUT)
	except subprocess.CalledProcessError as e:
		print e.returncode
		print e.output
	
	
	print '\tBuilding latex file...'
	ftex = open(filename + '_tmp.tex', "w")

	filenameinlatex = string.replace(filename, "_", "_")

	latexcontent="\
\\nonstopmode \
\\documentclass{article} \n\
\\usepackage{graphicx} \n\
\\usepackage{nopageno} \n\
\\usepackage{txfonts} \n\
\\usepackage[usenames]{color} \n\
\\usepackage{epstopdf} \n\
\\usepackage[paperwidth=" + str(plotwidth) + "cm, paperheight=" + str(plotheight) + "cm,margin=0.0in]{geometry} \n\
\\begin{document} \n\
\\begin{center} \n\
\\input{" + filenameinlatex + ".tex} \n\
\\end{center} \n\
\\end{document}"

	ftex.write(latexcontent)
	ftex.close()

	print '\tRunning ps2pdf...'
	try:
		sts = subprocess.check_output(["ps2pdf -dEPSCrop " + filename + ".eps " + filename + ".pdf"],shell=True,stderr=subprocess.STDOUT)
	except subprocess.CalledProcessError as e:
		print e.returncode
		print e.output

	print '\tRunning pdflatex...'
	try:
		sts = subprocess.check_output(["pdflatex " + filename + "_tmp.tex"],shell=True,stderr=subprocess.STDOUT)
	except subprocess.CalledProcessError as e:
		print e.returncode
		print e.output
	
	print '\tRunning pdfcrop...'
	try:
		sts = subprocess.check_output(["mv " + filename + "_tmp.pdf " + filename + ".pdf"],shell=True,stderr=subprocess.STDOUT)
	except subprocess.CalledProcessError as e:
		print e.returncode
		print e.output
	# os.rename(filename + "_tmp.pdf", filename + ".pdf")

	print '\tCleaning up...'
	os.remove(filename + "_tmp.tex")
	os.remove(filename + "_tmp.log")
	os.remove(filename + "_tmp.aux")
	# os.remove(filename + "_tmp.pdf")
	os.remove(filename + ".eps")
	os.remove(filename + ".tex")

# Starting main part...
for arg in sys.argv[1:]:
	print 'Processing ' + arg + ' ...'
	process(arg)
	print ''

print 'Everything finished!'
