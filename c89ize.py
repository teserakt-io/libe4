#!/usr/bin/python3

import os
import re

def process_line(line):
    r = re.compile(r'.*?//(.*)')
    if r.match(line):
        newline = line.replace("//", "/*")
        newline = newline + " */"
        return True, newline
    return False, ""

def c_read_replace(filename):

    print("Processing %s" % filename)
    with open(filename, 'rb+') as f:
        filetext = ""
        line = f.readline().decode("utf-8")

        while line:
            line = line.strip("\n")
            changed, newline = process_line(line)
            if changed:
                line = newline
            filetext = filetext + line + "\n"

            line = f.readline().decode("utf-8")

        f.seek(0)
        f.write(filetext.encode("utf-8"))
    print("Done")

def find_c_h_files():

    file_list = []
    for root, dirs, files in os.walk("."):
        for f in files:
            if f.endswith(".c"): 
                file_list.append(os.path.join(root, f))
            if f.endswith(".h"):
                file_list.append(os.path.join(root, f))
    return file_list

if __name__ == '__main__':
    c_files = find_c_h_files()
    for f in c_files:
        c_read_replace(f)
