#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This script prints the metadata-dictionary in a plot which was added by merlin.
For viewing the metadata from a PDF, the PyPdDF2 package is required.
Usage: readMetadataFromPNG.py <path/to/png-or-pdf-file>
"""

from PIL import Image
from pprint import pprint
import argparse


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("file", type=str, help="Name of the png file you want to get" \
        "the meta information from")
    opt = parser.parse_args()

    if opt.file.endswith(".png"):
        image = Image.open(opt.file)
        pprint(image.info)
    elif opt.file.endswith(".pdf"):
        try:
            from PyPDF2 import PdfFileReader
            pdf_toread = PdfFileReader(open(opt.file, "rb"))
            pdf_info = pdf_toread.getDocumentInfo()
            for i in pdf_info:
                print "\n\033[92m%s\033[0m\n" % i, pdf_info[i]
        except ImportError:
            print "ERROR: you cant read the metadata from pdf files because apparently " \
                  "you dont have the PyPDF2 package installed. You can\n" \
                  "1. get the package from https://pypi.python.org/pypi/PyPDF2\n" \
                  "2. Open the pdf file in an image viewer of your choice and directly\n" \
                  "   have a look at the document properties/metadata"
