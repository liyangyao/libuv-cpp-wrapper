__author__ = 'liyangyao'

import os
import sys



dir = os.path.split(os.path.realpath(__file__))[0]
size = len(dir)

output = open(dir + '\\output.txt', 'w')

headers = []
sources = []

for dirpath, dirnames, filenames in os.walk(dir):
    for filename in filenames:
        suffix = os.path.splitext(filename)[1]
        if suffix==".h" or suffix==".c" or suffix==".cc" or suffix==".cpp" or suffix==".hpp":
            line = dirpath + "\\" + filename;
            line =  line[size +1 :len(line)]
            line = line.replace("\\", "/")
            if suffix==".hpp" or suffix==".h":
                line = "HEADERS += $$PWD/" + line
                headers.append(line)
            else:
                line = "SOURCES += $$PWD/" + line
                sources.append(line)

for header in headers:
    print header
    output.write(header)
    output.write("\n")

for source in sources:
    print source
    output.write(source)
    output.write("\n")


print "----------------"
os.system("pause")

