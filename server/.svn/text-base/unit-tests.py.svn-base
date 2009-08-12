#!/usr/bin/env python
from local_settings import cfg
import os
import random
import re
import shutil
import sys
import time
import zipfile

WINDOWS = 'win'
MACOS   = 'mac'
LINUX   = 'linux'

##
# GetOS
##
def GetOS():
    if sys.platform.startswith('win'):
        return WINDOWS
    if sys.platform.startswith('darwin'):
        return MACOS
    return LINUX

##
# wait_for_file
##
def wait_for_file(filename, timeout=60):
    interval = 1;
    time_remaining = timeout;

    #print "\nWaiting for " + filename + "\n"
    while not os.path.exists(filename) and time_remaining > 0:
        time.sleep(interval)
        time_remaining -= interval
        #print "\nStill Waiting for " + filename + "\n"

    #print "\ndone Waiting for " + filename + "\n"
    return os.path.exists(filename)

##
# unzip_to_dir
##
def unzip_to_dir(file, dir):
    os.mkdir(cfg['jam_install'], 0777)
    zfobj = zipfile.ZipFile(file)
    for name in zfobj.namelist():
        if name.endswith('/'):
            os.mkdir(os.path.join(dir, name))
        else:
            outfile = open(os.path.join(dir, name), 'wb')
            outfile.write(zfobj.read(name))
            outfile.close()

##
# main
##
print "\n===== Running JS framework unit tests."
sys.stdout.flush()

# remove any old installs
if os.path.exists(cfg['jam_install']):
    shutil.rmtree(cfg['jam_install'])

if GetOS() == WINDOWS:
    # unzip package
    unzip_to_dir(cfg['jam_package'], cfg['jam_install'])
elif GetOS() == MACOS:
    # copy DMG
    os.system('hdiutil attach "%s" -readonly' % cfg['jam_package'])
    shutil.copytree('%s/Aptana_Jaxer' % cfg['jam_mount'], cfg['jam_install'])
    os.system('hdiutil detach "%s"' % cfg['jam_mount'])
else:
    # unzip package
    os.system('unzip ' + cfg['jam_package'] + ' -d ' + cfg['unzip_location'])

# clean start servers
if os.path.exists(cfg['httpd_pid']):
    os.unlink(cfg['httpd_pid'])

# make sure nothing is running - TODO: make this work on other OSes
print('kill apache and jaxermanager if running before starting test')
os.system(cfg['kill'] + ' ' + cfg['apache-name'])
os.system(cfg['kill'] + ' ' + cfg['jaxermanager-name'])

print('Executing: "%s" start' % cfg['start_servers'])
os.system('"%s" start' % cfg['start_servers'])

if wait_for_file(cfg['httpd_pid']):
    print('    apache started')
    # clean up any old test results
    if os.path.exists(cfg['results_file']):
        os.unlink(cfg['results_file'])

    # build unit test runner URL
    host = cfg['host']
    page = cfg['page']
    query = cfg['query'] + "&rnd=" + str(random.randint(0, 65535))
    url = host + "/" + page + "?" + query

    # run unit tests
    if GetOS() == WINDOWS:
        # Sleep for 10 secs to wait for jaxer to come up
        time.sleep(10)
        os.spawnl(os.P_NOWAIT, cfg['browser'], '%s "%s"' % (cfg['browser-name'], url))
    elif GetOS() == MACOS:
        os.system('open %s "%s"' % (cfg['browser'], url))
    else:
        os.system('%s "%s" &' % (cfg['browser'], url))

    # wait for unit tests to complete or timeout
    wait_for_file(cfg['results_file'], 300)
    print('    Unit test done.  Closing browser')

    # close browser
    os.system(cfg['kill'] + ' ' + cfg['browser-name'])

    # stop servers
    print('    Stopping servers.')
    if GetOS() == WINDOWS:
        print('"' + cfg['start_servers'] + '" stop')
        os.system('"' + cfg['start_servers'] + '" stop')
    else:
        print(cfg['stop_servers'])
        os.system(cfg['stop_servers'])
        if GetOS() != MACOS:
            print(cfg['kill'] + ' ' + cfg['jaxermanager-name'])
            os.system(cfg['kill'] + ' ' + cfg['jaxermanager-name'])

print('Unit-test completed')

