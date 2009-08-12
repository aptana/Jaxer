#!/usr/bin/env python
import os
import time
import sys
import re

# A script that should be run on the Mac to remove the old failed Jaxer builds

def rm_files_older_than_n_days(dir, pattern, days):
    now = int(time.time())
    cutoff = now - (days * 24 * 3600)
    #contents = os.listdir(dir)
    for root, dirs, files in os.walk(dir):
        for file in files:
            name = os.path.join(root, file)
            if (os.path.isfile(name) and 
                    os.path.getmtime(name) <= cutoff
                    and re.search(pattern, file)):
                print name
                os.remove(name)


RootDir="/Shared Items/Public/Builds"
rm_files_older_than_n_days(RootDir, r'failed', 14)

