#! /usr/bin/python3

# Installs the Plugin Configuration File
# Parameters:
#     Path to AudispPlugin binary
#  or
#     uninstall

from genericpath import exists
import os
import sys

fileName = "/etc/audit/plugins.d/AudispPlugin.conf"

pluginConfig = """active = yes
direction = out
path = INSERTPATH
type = always
args = test
format = string
"""

# Check Parameter and sudo Access
errorFlag = False
if len(sys.argv) != 2:
    print("""Provide one argument:
    - Path to AudispPlugin binary
    - \"uninstall\" if Plugin should be removed""")
    errorFlag = True

if os.geteuid() != 0:
    print("    Must be run as sudo")
    errorFlag = True

if errorFlag:
    exit(1)

# uninstall
if sys.argv[1] == "uninstall":
    if exists(fileName):
       os.remove(fileName)
       print("Deleted " + fileName)
    else:
        print("Uninstall failed, " + fileName + " does not exists")

# setConfigFile
else:
    if(exists(sys.argv[1])):
        # Update Plugin Conf
        output = pluginConfig.replace("INSERTPATH", os.path.abspath(sys.argv[1]))
        # Write to File
        f = open(fileName, 'w')
        f.write(output)
        f.close
        print("Finished")
    else:
        print("File not found")
