#!/usr/bin/env python
from local_settings import cfg
import shutil
import sys
import os
import glob
import re

def system(cmd):
    sys.stdout.flush()
    rc = os.system(cmd)
    if not sys.platform.startswith("win"):
        rc = rc >> 8
    if rc != 0:
        print "Command failed with status %d: %s" % (rc, cmd)
        exit(rc)

# make sure supporting files for docgen are up to date
#system('svn up %s' % cfg['docgenRoot'])

# define constants
serverBrowser = "Jaxer Server Framework"
serverBrowserVersion = "0.9"
clientBrowser = "Jaxer Client Framework"
clientBrowserVersion = "0.9"
if (len(sys.argv) > 1):
    fullversion = sys.argv[1]
    matches = re.search(r'^([0-9]+\.[0-9]+)\.', fullversion)
    serverBrowserVersion = clientBrowserVersion = matches.group(1)

# define file locations
frameworkRoot = cfg['frameworkRoot']
serverFramework = frameworkRoot + "serverFramework.js"
jslib = frameworkRoot + "JSLib.js"
clientFramework = frameworkRoot + "clientFramework.js"
config = frameworkRoot + "config.js"
configApps = frameworkRoot + "configApps.js"

docRoot = cfg['docRoot']
combinedDocs = docRoot + "combined_framework.xml"

docgenRoot = cfg['docgenRoot']
docgen = docgenRoot + "AptanaDocGen.jar"
saxon = docgenRoot + "saxon8.jar"
xslt = docgenRoot + "help_documentation.xslt"

print "\n===== Creating Documentation."
sys.stdout.flush()

# remember current directory
cwd = os.getcwd()

# remove any old docs
if os.path.exists(docRoot):
    shutil.rmtree(docRoot)

# create empty directory
os.makedirs(docRoot)

# move into document directory
os.chdir(docRoot)

# build command line
output = "--output-file " + combinedDocs
browser1 = '--browser "%s" --browser-version "%s"' % (serverBrowser, serverBrowserVersion)
serverFiles = "%s %s %s %s" % (serverFramework, jslib, config, configApps)
nsImport = '--import Jaxer.=JSLib.'
browser2 = '--browser "%s" --browser-version "%s"' % (clientBrowser, clientBrowserVersion)
filter = '--filter JSLib'
filterPrivate = "--filter-private"

args = '-v %s %s %s %s %s %s %s %s' % (output, nsImport, browser1, serverFiles, browser2, clientFramework, filter, filterPrivate)
cmd = 'java -jar %s %s' % (docgen, args)

# make sure to delete XML so doc build failures don't leave old artifacts
#if os.path.exists(combinedDocs):
#   os.unlink(combinedDocs)

# generate XML
system(cmd)

# generate HTML
vars = 'ReferenceName=Jaxer ReferenceDisplayName="Jaxer Framework"'
system("java -jar %s %s %s %s" % (saxon, combinedDocs, xslt, vars))

# copy dependent resources
for name in glob.glob(docgenRoot + os.sep + "*.gif"):
    shutil.copy(name, docRoot)
for name in glob.glob(docgenRoot + os.sep + "*.png"):
    shutil.copy(name, docRoot)
for name in glob.glob(docgenRoot + os.sep + "*.js"):
    shutil.copy(name, docRoot)
for name in glob.glob(docgenRoot + os.sep + "*.css"):
    shutil.copy(name, docRoot)

# return from whence we came
os.chdir(cwd)
