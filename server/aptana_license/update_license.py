#! /usr/local/bin/python

################################################################################
# Add license text to each aptana file.  If license has already been added, then
# remove it before adding.
#
################################################################################

################################################################################
# TODO
# Cannot tell if a .h file is C or CPP.  Treated as CPP for now.  The only place
# it gets wrong is mod_jaxer.  Just manually change the C++ to C in the first
# line.
#
################################################################################

import re
import os
import shutil


################################################################################
#  Modify this section to get things correct

#  The directory where this file sits.  All other files/dirs are relative to
#  this dir.
gMY_DIR = os.getcwd()

#  The server dir relative to gMY_DIR
gSERVER_DIR = os.path.join(gMY_DIR, '..')

#  If we encountered any of the following dirs, we shouldnot proceed into them
gSKIP_DIRS = ('.svn', 'lib_linux', 'lib_linux64', 'lib_mac', 'lib_solaris', 'lib_win',
 'include_linux', 'include_linux64', 'include_mac', 'include_solaris', 'include_win')

# Max number of lines of our (last) license block
gMAX_LICENSE_LINES = 500

# The last line of our license block (for .cpp, .c, .h, .idl, .js)
# must look like the following line (without the 1st 2 chars)
# * ***** END APTANA LICENSE BLOCK ***** */

# And must look like this for makefile (without the 1st 2 chars)
# # ***** END APTANA LICENSE BLOCK *****

greCPP_LICENSE_LAST_LINE = re.compile(r'^[ \t\*]*END[ \t]+LICENSE[ \t]+BLOCK[ \t]*\*+[ \t]+\*+/')
greSH_LICENSE_LAST_LINE = re.compile(r'^#[ \t\*]*END[ \t]+LICENSE[ \t]+BLOCK[ \t\*]*')

# How to determine the file type
greCPP_FILE = re.compile(r'\.(cpp|h|java|idl)$', re.I)
greC_FILE = re.compile(r'\.c$', re.I)
greJS_FILE = re.compile(r'\.js$', re.I)
greMK_FILE = re.compile(r'^Makefile|\.mk$', re.I)
grePY_FILE = re.compile(r'\.py$', re.I)
greSH_FILE = re.compile(r'\.sh$', re.I)

gAPTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'JaxerLicense.txt')
gAPTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'JaxerMozLicense.txt')

gCPP_FIRST_LINE = ''
gCPP_APTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'cppJaxerLicense.txt')
gCPP_APTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'cppJaxerMozLicense.txt')
gCPP_VIM_APTANA_LINE = '/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-\n * vim: set sw=4 ts=4 et: */'
gCPP_VIM_MOZ_LINE = '/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */'
gCPP_APTANA_LICENSE_FIRST_LINE = '/* ***** BEGIN LICENSE BLOCK *****'
gCPP_APTANA_LICENSE_LAST_LINE = ' * ***** END LICENSE BLOCK ***** */'
gCPP_LEFT = ' * '

gC_FIRST_LINE = ''
gC_APTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'cJaxerLicense.txt')
gC_APTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'cJaxerMozLicense.txt')
gC_VIM_APTANA_LINE = '/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-\n * vim: set sw=4 ts=4 et: */'
gC_VIM_MOZ_LINE = '/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */'
gC_APTANA_LICENSE_FIRST_LINE = '/* ***** BEGIN LICENSE BLOCK *****'
gC_APTANA_LICENSE_LAST_LINE = ' * ***** END LICENSE BLOCK ***** */'
gC_LEFT = ' * '


gJS_FIRST_LINE = ''
gJS_APTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'jsJaxerLicense.txt')
gJS_APTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'jsJaxerMozLicense.txt')
gJS_VIM_APTANA_LINE = ''
gJS_VIM_MOZ_LINE = '/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */'
gJS_APTANA_LICENSE_FIRST_LINE = '/* ***** BEGIN LICENSE BLOCK *****'
gJS_APTANA_LICENSE_LAST_LINE = ' * ***** END LICENSE BLOCK ***** */'
gJS_LEFT = ' * '


gSH_FIRST_LINE = '#! /bin/sh'
gSH_APTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'shJaxerLicense.txt')
gSH_APTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'shJaxerMozLicense.txt')
gSH_VIM_APTANA_LINE = ''
gSH_VIM_MOZ_LINE = ''
gSH_APTANA_LICENSE_FIRST_LINE = '# ***** BEGIN LICENSE BLOCK *****'
gSH_APTANA_LICENSE_LAST_LINE = '# ***** END LICENSE BLOCK *****'
gSH_LEFT = '# '

gPY_FIRST_LINE = '#! /usr/local/bin/python'
gMK_FIRST_LINE = ''
gPY_APTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'pyJaxerLicense.txt')
gMK_APTANA_NEW_LICENSE = os.path.join(gMY_DIR, 'mkJaxerLicense.txt')
gPY_APTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'pyJaxerMozLicense.txt')
gMK_APTANA_MOZ_LICENSE = os.path.join(gMY_DIR, 'mkJaxerMozLicense.txt')

# The file that contains the list of mozilla files that we modified
gMODEFIED_MOZ_FILES = os.path.join(gMY_DIR, 'modified_file_list.txt')

#  The top most dirs that we need to add license to all files under them.
aptana_dir = os.path.join(gSERVER_DIR, 'src', 'mozilla', 'aptana')
mod_jaxer_dir = os.path.join(gSERVER_DIR, 'src', 'connectors', 'apache', 'apache-modules', 'mod_jaxer')
isapi_dir = os.path.join(gSERVER_DIR, 'src', 'connectors', 'iis')
manager_dir = os.path.join(gSERVER_DIR, 'src', 'manager')
utils_dir = os.path.join(gSERVER_DIR, 'src', 'Utils')
gAPTANA_DIRS = (aptana_dir, mod_jaxer_dir, isapi_dir, manager_dir, utils_dir)

#  Do not modify below this line.
################################################################################

# The parent dir of the mozilla folder
gMOZILLA_PARENT = os.path.join(gSERVER_DIR, 'src')

# A single tmp file used for adding the license block
gTMP_FILE = os.path.join(gMY_DIR, 'tmp_work_area')


# construct the various files based on type of file
def generate_tmp_license_file(orig_license_file, new_license_file, first_line,
  vim_line, license_1st_line, license_last_line, left_fill):
  # add first line if not empty (shell header)
  # add vim line (self commented)
  # add license_1st_line (including comment-open)
  # for each license text line, prefix with left_fill
  # add license_last_line (including closing-comment)
  f = open(new_license_file, 'wb')
  if (len(first_line)>0):
    f.write(first_line)
    f.write("\n")
  if (len(vim_line)>0):
    f.write (vim_line)
    f.write("\n")
  
  f.write (license_1st_line)
  f.write("\n")
  
  f2 = open(orig_license_file, 'r')
  for line in f2:
    f.write(left_fill)
    f.write(line)
    #f.write(line.rstrip('\n\r'))
    #f.write('\x0a')
  f2.close()
  
  f.write(license_last_line)
  f.write("\n")
  f.close()
  
  
# return the offset of the existing license block.
# return 0 if no license block
def license_offset(filename, re_license_last_line):
	n = 0
	f = open (filename, 'rb')
	while n < gMAX_LICENSE_LINES:
		line = f.readline()
		if line:
			if re_license_last_line.match(line):
				pos = f.tell()
				f.close()
				return pos
			n += 1
		else:
			f.close()
			return 0
	f.close()
	return 0

# Add license to afile
def add_license (license_file, re_license_last_line, afile):
	shutil.copy(license_file, gTMP_FILE)
	fp = open (gTMP_FILE, 'ab')
	offset = license_offset(afile, re_license_last_line)
	fp2 =open (afile, 'rb')
	fp2.seek(offset, 0)
	line = fp2.read(8000)
	while len(line)>0:
		fp.write(line)
		line = fp2.read(8000)
	fp2.close()
	fp.close()

	#abackup = '%s.bk' % afile 
	#if os.path.exists(abackup):
	#	os.remove(abackup)
	#os.rename (afile, abackup)
	os.remove(afile)
	os.rename (gTMP_FILE, afile)

def add_license_recursive(root_dir):
  def handle_dir(junk, dirpath, namelist):
    # print 'Updating License in folder: ' + dirpath
    for name in namelist:
      # print ' ' + name
      filepath = os.path.join(dirpath, name)
      if os.path.isfile(filepath):
        if greCPP_FILE.search(name):
          add_license(gCPP_APTANA_NEW_LICENSE, greCPP_LICENSE_LAST_LINE, filepath)
        elif greC_FILE.search(name):
          add_license(gC_APTANA_NEW_LICENSE, greCPP_LICENSE_LAST_LINE, filepath)
        elif greJS_FILE.search(name):
          add_license(gJS_APTANA_NEW_LICENSE, greCPP_LICENSE_LAST_LINE, filepath)
        elif grePY_FILE.search(name):
          add_license(gPY_APTANA_NEW_LICENSE, greSH_LICENSE_LAST_LINE, filepath)
        elif greSH_FILE.search(name):
          add_license(gSH_APTANA_NEW_LICENSE, greSH_LICENSE_LAST_LINE, filepath)
        elif greMK_FILE.search(name):
          add_license(gMK_APTANA_NEW_LICENSE, greSH_LICENSE_LAST_LINE, filepath)
        
    
    for dir in gSKIP_DIRS:
      testdir = os.path.join(dirpath, dir)
      if os.path.exists(testdir) and os.path.isdir(testdir) and dir in namelist:
        # print 'Skipping Folder: ' + dir  
        namelist.remove(dir)
      
  os.path.walk(root_dir, handle_dir, None)


# Generate the various license files needed
# C-like files
generate_tmp_license_file(gAPTANA_NEW_LICENSE, gCPP_APTANA_NEW_LICENSE, '',
  gCPP_VIM_APTANA_LINE, gCPP_APTANA_LICENSE_FIRST_LINE, gCPP_APTANA_LICENSE_LAST_LINE, gCPP_LEFT)
generate_tmp_license_file(gAPTANA_NEW_LICENSE, gC_APTANA_NEW_LICENSE, '',
  gC_VIM_APTANA_LINE, gC_APTANA_LICENSE_FIRST_LINE, gC_APTANA_LICENSE_LAST_LINE, gC_LEFT)
generate_tmp_license_file(gAPTANA_NEW_LICENSE, gJS_APTANA_NEW_LICENSE, '',
  gJS_VIM_APTANA_LINE, gJS_APTANA_LICENSE_FIRST_LINE, gJS_APTANA_LICENSE_LAST_LINE, gJS_LEFT)

#Shell script files
generate_tmp_license_file(gAPTANA_NEW_LICENSE, gSH_APTANA_NEW_LICENSE, gSH_FIRST_LINE,
  gSH_VIM_APTANA_LINE, gSH_APTANA_LICENSE_FIRST_LINE, gSH_APTANA_LICENSE_LAST_LINE, gSH_LEFT)
  
#Python files
generate_tmp_license_file(gAPTANA_NEW_LICENSE, gPY_APTANA_NEW_LICENSE, gPY_FIRST_LINE,
  gSH_VIM_APTANA_LINE, gSH_APTANA_LICENSE_FIRST_LINE, gSH_APTANA_LICENSE_LAST_LINE, gSH_LEFT)
  
#Make files
generate_tmp_license_file(gAPTANA_NEW_LICENSE, gMK_APTANA_NEW_LICENSE, gMK_FIRST_LINE,
  gSH_VIM_APTANA_LINE, gSH_APTANA_LICENSE_FIRST_LINE, gSH_APTANA_LICENSE_LAST_LINE, gSH_LEFT)

#Mozilla files
generate_tmp_license_file(gAPTANA_MOZ_LICENSE, gCPP_APTANA_MOZ_LICENSE, '',
  gCPP_VIM_MOZ_LINE, gCPP_APTANA_LICENSE_FIRST_LINE, gCPP_APTANA_LICENSE_LAST_LINE, gCPP_LEFT)
generate_tmp_license_file(gAPTANA_MOZ_LICENSE, gC_APTANA_MOZ_LICENSE, '',
  gC_VIM_MOZ_LINE, gC_APTANA_LICENSE_FIRST_LINE, gC_APTANA_LICENSE_LAST_LINE, gC_LEFT)
generate_tmp_license_file(gAPTANA_MOZ_LICENSE, gJS_APTANA_MOZ_LICENSE, '',
  gJS_VIM_MOZ_LINE, gJS_APTANA_LICENSE_FIRST_LINE, gJS_APTANA_LICENSE_LAST_LINE, gJS_LEFT)

generate_tmp_license_file(gAPTANA_MOZ_LICENSE, gSH_APTANA_MOZ_LICENSE, gSH_FIRST_LINE,
  gSH_VIM_MOZ_LINE, gSH_APTANA_LICENSE_FIRST_LINE, gSH_APTANA_LICENSE_LAST_LINE, gSH_LEFT)
generate_tmp_license_file(gAPTANA_MOZ_LICENSE, gPY_APTANA_MOZ_LICENSE, gPY_FIRST_LINE,
  gSH_VIM_MOZ_LINE, gSH_APTANA_LICENSE_FIRST_LINE, gSH_APTANA_LICENSE_LAST_LINE, gSH_LEFT)
generate_tmp_license_file(gAPTANA_MOZ_LICENSE, gMK_APTANA_MOZ_LICENSE, gMK_FIRST_LINE,
  gSH_VIM_MOZ_LINE, gSH_APTANA_LICENSE_FIRST_LINE, gSH_APTANA_LICENSE_LAST_LINE, gSH_LEFT)
  
# Update APTANA license for APTANA files
print 'License update for APTANA files'
for dir in gAPTANA_DIRS:
  print '  Updating folder: ' + dir
  add_license_recursive(dir)

# Update for APTANA-modified MOZILLA files
print '\nLicense update for APNATA-MODIFIED MOZILLA files'
f = open(gMODEFIED_MOZ_FILES, 'r')
for line in f:
  text = line.strip()
  if text.startswith('#') or len(text) < 1: continue
  print '    File: ' + line
  path = os.path.normpath (os.path.join (gMOZILLA_PARENT, text))
  dir, base = os.path.split(path)
  if greCPP_FILE.search(base):
    add_license(gCPP_APTANA_MOZ_LICENSE, greCPP_LICENSE_LAST_LINE, path)
  elif greC_FILE.search(base):
    add_license(gC_APTANA_MOZ_LICENSE, greCPP_LICENSE_LAST_LINE, path)
  elif grePY_FILE.search(base):
    add_license(gPY_APTANA_MOZ_LICENSE, greSH_LICENSE_LAST_LINE, path)
  elif greSH_FILE.search(base):
    add_license(gSH_APTANA_MOZ_LICENSE, greSH_LICENSE_LAST_LINE, path)
  elif greMK_FILE.search(base):
    add_license(gMK_APTANA_MOZ_LICENSE, greSH_LICENSE_LAST_LINE, path)
  else:
    print '*** ERROR *** no match for file ' + line

f.close()

# Remove the generated tmp files
os.remove(gPY_APTANA_NEW_LICENSE)
os.remove(gSH_APTANA_NEW_LICENSE)
os.remove(gCPP_APTANA_NEW_LICENSE)
os.remove(gC_APTANA_NEW_LICENSE)
os.remove(gJS_APTANA_NEW_LICENSE)
os.remove(gMK_APTANA_NEW_LICENSE)
os.remove(gPY_APTANA_MOZ_LICENSE)
os.remove(gSH_APTANA_MOZ_LICENSE)
os.remove(gCPP_APTANA_MOZ_LICENSE)
os.remove(gC_APTANA_MOZ_LICENSE)
os.remove(gJS_APTANA_MOZ_LICENSE)
os.remove(gMK_APTANA_MOZ_LICENSE)


  
