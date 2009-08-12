#!/usr/bin/env python
import os
import time
import sys
import re
import shutil

# A script that should be run on the Mac to move the old builds to archive

def mv_files_older_than_n_days(fromDir, toDir, days):
    now = int(time.time())
    cutoff = now - (days * 24 * 3600)
    for root, dirs, files in os.walk(fromDir):
        destroot = root.replace(fromDir, toDir, 1)
        for file in files:
            name = os.path.join(root, file)
            if (os.path.isfile(name) and 
                    os.path.getmtime(name) <= cutoff):
                print ("%s->%s" % (name, destroot))
                shutil.copy2(name, os.path.join(destroot,file))
                os.remove(name)


FromRootDir="/Shared Items/Public/Builds"
ToRootDir="/Volumes/Archive HD/Builds"
mv_files_older_than_n_days(FromRootDir, ToRootDir, 14)

