#!/usr/bin/env python
from local_settings import cfg
from global_settings import gcfg
import os
import re
import shutil
import stat
import sys
import platform
import time
import datetime

WINDOWS = 'win'
MACOS   = 'mac'
LINUX   = 'linux'
SOLARIS = 'sunos'

def GetOS():
    if sys.platform.startswith('win'):
        return WINDOWS
    elif sys.platform.startswith('darwin'):
        return MACOS
    elif sys.platform.startswith('sunos'):
        return SOLARIS
    else:
        return LINUX

def system(cmd):
    sys.stdout.flush()
    rc = os.system(cmd)
    if not sys.platform.startswith("win"):
        rc = rc >> 8
    if rc != 0:
        print "Command failed with status %d: %s" % (rc, cmd)
        exit(rc)

def do_remove_dir(dirname):
    print "Removing ", dirname
    try:
        shutil.rmtree(dirname)
    except OSError, err:
        print "***Failed to remove ", dirname, " ***"
        print "errno=" + str(err.errno)
        print "error=" + err.strerror 
        print "filename=" + err.filename
        #Windows build always fails here
        if GetOS() != WINDOWS:
            exit(1)

def remove_dir_if_not_created_today(dirname):
    today = datetime.datetime.now().timetuple()[7]
    info = os.stat(dirname)
    dirtime = datetime.datetime.fromtimestamp(info.st_ctime)
    dirday = dirtime.timetuple()[7]
    if today != dirday:
        do_remove_dir(dirname)

def copyfile(src, dst):
    shutil.copy2(src, dst)

def replaceinfile(filepath, pattern, replacement):
    f = open(filepath, 'r+')
    contents = f.read()
    contents = re.sub(pattern, replacement, contents)
    f.seek(0)
    f.truncate(0)
    f.write(contents)
    f.close()

def unlinkifexists(filepath):
    if os.path.exists(filepath):
        os.unlink(filepath)

def unlinkifexistsexecutable(filepath):
    if os.path.exists(filepath + '.exe'):
        os.unlink(filepath + '.exe')
    elif os.path.exists(filepath):
        os.unlink(filepath)

def filesmatching(path, prefix, pattern):
    all = os.listdir(path)
    selected = []
    for filename in all:
        if re.search(pattern, filename):
            selected.append(prefix + filename)
    return selected


unlinkifexists('distro/Jaxer_package_withApache.zip')
unlinkifexists('distro/Jaxer_package.zip')
unlinkifexists('distro/Jaxer_update.zip')

# Put the build number into a header file.  Don't touch the header file unless
# the number actually changes in order to minimize recompilations.
f = open('src/mozilla/aptana/jaxerBuildId.h', 'r')
contents = f.read()
f.close()
matches = re.search(r'#define JAXER_BUILD_ID "([0-9.]+)\.([x0-9]+)(.*)"', contents)
fullversion=None
if 'BUILDNO' in os.environ:
    buildno = os.environ['BUILDNO']
    if buildno != matches.group(2):
        fullversion = matches.group(1) + '.' + buildno
        fullversionstring = fullversion + matches.group(3)
        fullversioncommas = ','.join(fullversionstring.split('.'))
        print "Updating version to %s." % fullversionstring
        replaceinfile('src/mozilla/aptana/jaxerBuildId.h', r'#define JAXER_BUILD_ID "([0-9.]+)\.(.+)"', '#define JAXER_BUILD_ID "%s"' % fullversionstring)
        replaceinfile('src/mozilla/aptana/jaxerBuildId.h', r'#define JAXER_BUILD_NUMBER "xxxx', '#define JAXER_BUILD_NUMBER "%s' % buildno)
        replaceinfile('src/mozilla/aptana/jaxerBuildId.h', r'#define JAXER_BUILD_ID_L L"([0-9.]+)\.(.+)"', '#define JAXER_BUILD_ID_L L"%s"' % fullversionstring)
        replaceinfile('src/connectors/apache/apache-modules/mod_jaxer/mod_jaxer.rc', r'X\.X\.X\.X', fullversionstring)
        replaceinfile('src/connectors/apache/apache-modules/mod_jaxer/mod_jaxer.rc', r'X\,X\,X\,X', fullversioncommas)
        if GetOS() == WINDOWS:
            replaceinfile('src/manager/win32/JaxerManager.rc', r'X\.X\.X\.X', fullversionstring)
            replaceinfile('src/manager/win32/JaxerManager.rc', r'X\,X\,X\,X', fullversioncommas)
            replaceinfile('src/mozilla/aptana/Jaxer/module.ver', r'WIN32_MODULE_PRODUCTVERSION\=.*', 'WIN32_MODULE_PRODUCTVERSION=%s' % fullversioncommas)
            replaceinfile('src/mozilla/aptana/Jaxer/module.ver', r'WIN32_MODULE_PRODUCTVERSION_STRING\=.*', 'WIN32_MODULE_PRODUCTVERSION_STRING=%s' % fullversionstring)
            replaceinfile('src/connectors/iis/jaxer_isapi/jaxer_isapi.rc', r'X\.X\.X\.X', fullversionstring)
            replaceinfile('src/connectors/iis/jaxer_isapi/jaxer_isapi.rc', r'X\,X\,X\,X', fullversioncommas)


# Regenerate localinc.h
localinc = "src/mozilla/aptana/JaxerPro/localinc.h"
tmpfp = open("src/mozilla/aptana/JaxerPro/localinc.h", "w")
tmpfp.write("#define TIME_NOW %d\n" % time.time())
tmpfp.close()

# Find out where mozilla is putting the stuff it builds.
mozconfig_file = 'src/mozilla/'
if GetOS() == MACOS:
    mozconfig_file = '.mozconfig-mac-universal'
elif GetOS() == LINUX and platform.machine().endswith('64'):
    mozconfig_file = '.mozconfig-linux64'
elif GetOS() == LINUX:
    mozconfig_file = '.mozconfig-linux'
elif GetOS() == WINDOWS:
    mozconfig_file = '.mozconfig-win32'
elif GetOS() == SOLARIS:
    mozconfig_file = '.mozconfig-solaris'

if gcfg['generateDebuggerSymbols'] == True:
    mozconfig_file += '_crs'
    print "\n======= Debug-symbols will be generated in this build ======="

copyfile('src/mozilla/' + mozconfig_file, 'src/mozilla/.mozconfig')

#if GetOS() == MACOS:
#    copyfile('src/mozilla/.mozconfig-mac-universal', 'src/mozilla/.mozconfig')
#elif GetOS() == LINUX and platform.machine().endswith('64'):
#    copyfile('src/mozilla/.mozconfig-linux64', 'src/mozilla/.mozconfig')
#elif GetOS() == LINUX:
#    copyfile('src/mozilla/.mozconfig-linux', 'src/mozilla/.mozconfig')
#elif GetOS() == WINDOWS:
#    copyfile('src/mozilla/.mozconfig-win32', 'src/mozilla/.mozconfig')
#elif GetOS() == SOLARIS:
#    copyfile('src/mozilla/.mozconfig-solaris', 'src/mozilla/.mozconfig')


f = open('src/mozilla/.mozconfig', 'r')
x = f.read()
f.close()
y = re.search('MOZ_OBJDIR=@TOPSRCDIR@/(.+)', x)
ffdir = y.group(1)


# Build Jaxer...
if cfg['skipBuilds'] == False:

    path = os.path.abspath('src/mozilla').replace('\\', '/')
    framework = path + '/' + ffdir + '/dist/bin/framework'
    connectors = path + '/' + ffdir + '/dist/bin/connectors'
    bindir = path + '/' + ffdir + '/dist/bin/'
    
    dirs_to_be_removed = [
        bindir,
        path + '/' + ffdir + '/i386/dist/bin/',
        path + '/' + ffdir + '/ppc/dist/bin/',
        path + '/' + ffdir + '/dist/crashreporter-symbols/',
        path + '/' + ffdir + '/i386/dist/crashreporter-symbols/',
        path + '/' + ffdir + '/ppc/dist/crashreporter-symbols/'
    ]

    #Remove working dirs if needed
    if os.path.exists('src/mozilla/' + ffdir):
        if gcfg['doCleanBuild'] == True:
            do_remove_dir('src/mozilla/' + ffdir)
            print "\n======= This is a clean build of Jaxer ======="
        else:
            remove_dir_if_not_created_today('src/mozilla/' + ffdir)

    for adir in dirs_to_be_removed:
        try:
            if os.path.exists(adir):
                shutil.rmtree(adir)
        except OSError, err:
            print "***Failed to remove ", adir, " *** "
            exit(1)

    #Create dirs if not exist
    if GetOS() == MACOS:
        if not os.path.exists(path + '/' + ffdir + '/ppc/dist/bin'):
            os.makedirs(path + '/' + ffdir + '/ppc/dist/bin')
        if not os.path.exists(path + '/' + ffdir + '/i386/dist/bin'):
            os.makedirs(path + '/' + ffdir + '/i386/dist/bin')
    else:
        if not os.path.exists(bindir):
            os.makedirs(bindir)


    # Build google_breakpad that we will use late
    print "\n===== Building google_breakpad."
    if GetOS() == WINDOWS:
        system('cd src/google/google_breakpad/src/client/windows && MSBuild.exe breakpad_client.sln /p:Configuration=release')
        #system('cd src/google/google_breakpad/src/tools/windows/dump_syms && MSBuild.exe dump_syms.vcproj /p:Configuration=release')
    elif GetOS() == MACOS:
        system('cd src/google/google_breakpad && export CFLAGS="-arch ppc -arch i386" && ./configure && make')
        #system('cd src/google/google_breakpad/src/tools/mac/dump_syms && make')
    else:
        system('cd src/google/google_breakpad && ./configure && make')

    # Build libevent
    print "\n===== Building libevent."
    if GetOS() == MACOS:
        system('cd src/libevent/libevent-1.4.2-rc && export CFLAGS="-arch ppc -arch i386" && ./configure && make')
    elif GetOS() != WINDOWS:
        system('cd src/libevent/libevent-1.4.2-rc && ./configure && make')
        copyfile('src/libevent/libevent-1.4.2-rc/.libs/libevent-1.4.so.2',
                 'src/mozilla/' + ffdir + '/dist/bin/')

    # Build Manager.
#    print "\n===== Building JaxerManager."
#    if GetOS() == SOLARIS:
#        #system('cd src/manager/unix && gcc -DSOLARIS -I../../libevent/libevent-1.4.2-rc -o jaxermanager jaxermanager.cpp -L../../libevent/libevent-1.4.2-rc/.libs -levent -lxnet -lstdc++')
#        system('cd src/manager/unix && gmake -f Makefile.sun clean && gmake -f Makefile.sun')
#        copyfile('src/manager/unix/jaxermanager',
#                 'src/mozilla/' + ffdir + '/dist/bin/jaxermanager')
#    elif GetOS() == LINUX:
#        #system('cd src/manager/unix && g++ -I../../libevent/libevent-1.4.2-rc -L../../libevent/libevent-1.4.2-rc/.libs -levent -lrt -o jaxermanager jaxermanager.cpp')
#        if (platform.architecture()[0] == "32bit"):
#            system('cd src/manager/unix && make -f Makefile.linux clean && make -f Makefile.linux')
#        else:
#            system('cd src/manager/unix && g++ -DLINUX_64 -I../../libevent/libevent-1.4.2-rc -L../../libevent/libevent-1.4.2-rc/.libs -levent -lrt -o jaxermanager jaxermanager.cpp')
#        copyfile('src/manager/unix/jaxermanager',
#                 'src/mozilla/' + ffdir + '/dist/bin/jaxermanager')
#    elif GetOS() == MACOS:
#        if gcfg['generateDebuggerSymbols'] == True:
#            system('cd src/manager/unix && bash ./buildMacUniversal.sh GENERATE_SYMBOLS=1')
#        else:
#            system('cd src/manager/unix && bash ./buildMacUniversal.sh')
            
#        copyfile('src/manager/unix/JaxerManager',
#                 'src/mozilla/' + ffdir + '/ppc/dist/bin/JaxerManager')
#        copyfile('src/manager/unix/JaxerManager',
#                 'src/mozilla/' + ffdir + '/i386/dist/bin/JaxerManager')
#    elif GetOS() == WINDOWS:
#        system('cd src/manager/win32 && MSBuild.exe JaxerManager.sln /p:Configuration=release')
#        copyfile('src/manager/win32/Release/JaxerManager.exe',
#                 'src/mozilla/' + ffdir + '/dist/bin/JaxerManager.exe')
#        copyfile('src/manager/win32/Release/JaxerManager.pdb',
#                 'src/mozilla/' + ffdir + '/dist/bin/JaxerManager.pdb')

#    if GetOS() != MACOS:
#        copyfile('src/manager/JaxerManager.cfg',
#                 'src/mozilla/' + ffdir + '/dist/bin/JaxerManager.cfg')

    # Build LogServer
    print "\n===== Building JaxerLogger."
    if GetOS() == WINDOWS:
        system('cd src/Utils/JaxerLog && MSBuild.exe JaxerLog.vcproj')
        copyfile('src/Utils/JaxerLog/Release/JaxerLogger.exe',
                 'src/mozilla/' + ffdir + '/dist/bin/JaxerLogger.exe')
    elif GetOS() == SOLARIS:
        system('cd src/Utils/JaxerLog && gcc -I../../libevent/libevent-1.4.2-rc -o jaxerlogger LogServerUnix.cpp -L../../libevent/libevent-1.4.2-rc/.libs -levent -lxnet -lstdc++')
        copyfile('src/Utils/JaxerLog/jaxerlogger',
                 'src/mozilla/' + ffdir + '/dist/bin/jaxerlogger')
    elif GetOS()+'x' == LINUX:
        system('cd src/Utils/JaxerLog && g++ -I../../libevent/libevent-1.4.2-rc -L../../libevent/libevent-1.4.2-rc/.libs -levent -lrt -o jaxerlogger LogServerUnix.cpp')
        copyfile('src/Utils/JaxerLog/jaxerlogger',
                 'src/mozilla/' + ffdir + '/dist/bin/jaxerlogger')
    elif GetOS() == MACOS:
        system('cd src/Utils/JaxerLog && bash ./buildMacUniversal.sh')
        copyfile('src/Utils/JaxerLog/JaxerLogger',
                 'src/mozilla/' + ffdir + '/ppc/dist/bin/JaxerLogger')
        copyfile('src/Utils/JaxerLog/JaxerLogger',
                 'src/mozilla/' + ffdir + '/i386/dist/bin/JaxerLogger')


    print "\n===== Building Jaxer." 
    if GetOS() == WINDOWS:
        system('bash.exe --login -c "cd %s; make -f client.mk build && %s/dist/bin/Jaxer.exe -reg"' % (path, ffdir))
    elif GetOS() == MACOS:
        os.environ['LD_LIBRARY_PATH'] = '.'
        system('cd %s && touch configure && make -f client.mk build && cd %s/dist/bin && ./Jaxer -reg' % (path, ffdir))
    elif GetOS() == LINUX:
        os.environ['LD_LIBRARY_PATH'] = '.'
        system('cd %s && touch configure && make -f client.mk build && cd %s/dist/bin' % (path, ffdir))
        system('cd %s/%s/dist/bin && ./jaxer -reg' % (path, ffdir))
    elif GetOS() == SOLARIS:
        os.environ['LD_LIBRARY_PATH'] = '.'
        system('cd %s && touch configure && gmake -f client.mk build && cd %s/dist/bin' % (path, ffdir))
        system('cd %s/%s/dist/bin && ./jaxer -reg' % (path, ffdir))

    # On Mac, these files have to be copied after Jaxer is built
    if GetOS() == MACOS:
        copyfile('src/libevent/libevent-1.4.2-rc/.libs/libevent-1.4.2.dylib',
                 'src/mozilla/' + ffdir + '/universal/final/libevent-1.4.2.dylib')
        copyfile('src/manager/JaxerManager.cfg',
                 'src/mozilla/' + ffdir + '/universal/final/JaxerManager.cfg')


    # Cleanup Jaxer and XPCOM files
    jaxer = 'src/mozilla/' + ffdir + '/dist/bin'
    path = os.path.abspath(jaxer)
    xpts = ' '.join(filesmatching('%s/components' % path, 'components/', r'\.xpt$'))
    if GetOS() == WINDOWS:
        system('cd %s && ../sdk/bin/xpt_link.exe components/components.xpt_ %s' % (path, xpts))
    else:
        system('cd %s && ../sdk/bin/xpt_link components/components.xpt_ %s' % (path, xpts))
    system('rm %s/components/*.xpt' % path)
    system('mv %s/components/components.xpt_ %s/components/components.xpt' % (path, path))
    unlinkifexists(jaxer + '/components/compreg.dat')
    unlinkifexists(jaxer + '/components/xpti.dat')
    unlinkifexistsexecutable(jaxer + '/mangle')
    unlinkifexistsexecutable(jaxer + '/shlibsign')
    unlinkifexistsexecutable(jaxer + '/xpcshell')
    unlinkifexistsexecutable(jaxer + '/xpicleanup')
    unlinkifexistsexecutable(jaxer + '/xpidl')
    unlinkifexistsexecutable(jaxer + '/xpt_dump')
    unlinkifexistsexecutable(jaxer + '/xpt_link')

    if not os.path.exists('src/mozilla/' + ffdir + '/dist/bin/framework'):
        os.mkdir('src/mozilla/' + ffdir + '/dist/bin/framework')
    if not os.path.exists('src/mozilla/' + ffdir + '/dist/bin/connectors'):
        os.mkdir('src/mozilla/' + ffdir + '/dist/bin/connectors')
    if GetOS() == WINDOWS:
        # jemalloc replaces MSVCR (only for Jaxer though)
        copyfile('c:/windows/system32/msvcp71.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/msvcp71.dll')
        copyfile('c:/windows/system32/msvcr71.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/msvcr71.dll')
        copyfile('src/microsoft/msvcr80.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/msvcr80.dll')
        copyfile('src/microsoft/msvcp80.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/msvcp80.dll')
        copyfile('src/microsoft/msvcm80.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/msvcm80.dll')
        copyfile('src/microsoft/Microsoft.VC80.CRT.manifest',
                 'src/mozilla/' + ffdir + '/dist/bin/Microsoft.VC80.CRT.manifest')
        copyfile('src/microsoft/msvcr80.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/msvcr80.dll')
        copyfile('src/microsoft/msvcp80.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/msvcp80.dll')
        copyfile('src/microsoft/msvcm80.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/msvcm80.dll')
        copyfile('src/microsoft/Microsoft.VC80.CRT.manifest',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/Microsoft.VC80.CRT.manifest')

    # Build JavaScript framework.
#    print "\n===== Building JavaScript framework."
#    os.environ['TREEROOT'] = os.path.abspath('..')
#    f = open('framework/buildConfig.js', 'w')
#    f.write('SERVER_OUTPUT = "../src/mozilla/' + ffdir + '/dist/bin/framework/serverFramework.js";\n')
#    f.write('JSLIB_OUTPUT = "../src/mozilla/' + ffdir + '/dist/bin/framework/JSLib.js";\n')
#    f.write('CLIENT_OUTPUT = "../src/mozilla/' + ffdir + '/dist/bin/framework/clientFramework.js";\n')
#    f.write('LICENSE_HEADER = "framework_header.txt";\n')
#    f.write('COMPRESSED_LICENSE_HEADER = "framework_compresed_header.txt";\n')
#    f.close()
#    clientFramework = 'products/server/src/mozilla/' + ffdir + '/dist/bin/framework/clientFramework.js'
#    compressedFramework = 'products/server/src/mozilla/' + ffdir + '/dist/bin/framework/clientFramework_compressed.js'
#    if GetOS() == WINDOWS:
#        system('cd ../tools/com.aptana.autobuild/libs && build.bat')
#    else:
#        system('cd ../tools/com.aptana.autobuild/libs && bash ./build.sh')
#    copyfile('framework/config.js', framework + '/config.js')
#    copyfile('framework/configLog.js', framework + '/configLog.js')
#    copyfile('framework/configApps.js', framework + '/configApps.js')
#    copyfile('framework/studio_config.js', framework + '/studio_config.js')
#    if os.path.exists(framework + '/extensions'):
#        shutil.rmtree(framework + '/extensions')
#    for root, dirs, files in os.walk('framework/extensions'):
#        destdir = framework + root.partition('framework')[2]
#        os.mkdir(destdir)
#        root = root + '/'
#        destdir = destdir + '/'
#        for file in files:
#            copyfile(root + file, destdir + file)
#        dirs.remove('.svn')

    # Build servlet.
    print "\n===== Building servlet."
    if GetOS() == WINDOWS: # win32 javac must native ';' as a classpath sep even though win32 bash can understand ':'
        system('cd src/connectors/servlet && build.bat')
    else:
        system('cd src/connectors/servlet && bash ./build.sh')
    copyfile('src/connectors/servlet/products/jaxer-app.war',    'src/mozilla/' + ffdir + '/dist/bin/connectors')
    copyfile('src/connectors/servlet/products/jaxer-server.war', 'src/mozilla/' + ffdir + '/dist/bin/connectors')

    # Build mod_jaxer.
    print "\n===== Building mod_jaxer."
    if GetOS() == WINDOWS:
        system('cd src/connectors/apache/apache-modules/mod_jaxer && MSBuild.exe mod_jaxer.vcproj')
        copyfile('src/connectors/apache/apache-modules/mod_jaxer/Release/mod_jaxer.so',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/mod_jaxer.so')
        # Build mod_jaxer20 (for apache 2.0).
        # print "\n===== Building mod_jaxer20."
        # system('cd src/apache-modules/mod_jaxer && nmake /f Makefile.win _apache20r')
        copyfile('src/connectors/apache/apache-modules/mod_jaxer/Release20/mod_jaxer20.so',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/mod_jaxer20.so')
    elif GetOS() == MACOS:
        system('cd src/connectors/apache/apache-modules/mod_jaxer && bash ./buildMacUniversal.sh')
        copyfile('src/connectors/apache/apache-modules/mod_jaxer/.libs/mod_jaxer.so',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/mod_jaxer.so')
    else:
        system('cd src/connectors/apache/apache-modules/mod_jaxer && ' + cfg['apxs-name'] + ' -c *.c')
        copyfile('src/connectors/apache/apache-modules/mod_jaxer/.libs/mod_jaxer.so',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/mod_jaxer.so')
        # print "\n===== Building mod_rewrite on linux (One time only)."
        # system('cd src/httpd-2.2.6/modules/mappers && ' + cfg['apxs-name'] + ' -c -o mod_rewrite.so mod_rewrite.c')
        # copyfile('src/httpd-2.2.6/modules/mappers/.libs/mod_rewrite.so',
        #          'src/../distro/jam/Apache22-local/modules/mod_rewrite.so')

    # Build jaxer_isapi.
    print "\n===== Building jaxer_isapi."
    if GetOS() == WINDOWS:
        system('cd src/connectors/iis/jaxer_isapi && nmake /f Makefile.win')
        copyfile('src/connectors/iis/jaxer_isapi/Release/jaxer_isapi.dll',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/jaxer_isapi.dll')
        copyfile('src/connectors/iis/jaxer_isapi/jaxer_isapi.conf',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/jaxer_isapi.conf')
        copyfile('src/connectors/iis/jaxer_isapi/ReadMe.txt',
                 'src/mozilla/' + ffdir + '/dist/bin/connectors/jaxer_isapi_readme.txt')

    # Build Jaxer Launcher
    if GetOS() == MACOS:
        print "\n===== Building Jaxer Launcher."
        system('cd src/launcher/mac && xcodebuild')

    # Build tellJaxerManager.
#    if GetOS() == WINDOWS:
#        print "\n===== Building tellJaxerManager."
#        system('cd src/Utils/tellManager && cl.exe /D "WIN32" /nologo /Ox /MT tellManager.cpp /link ws2_32.lib')
#        copyfile ('src/Utils/tellManager/tellManager.exe',
#                  'src/mozilla/' + ffdir + '/dist/bin/tellJaxerManager.exe')
#    elif GetOS() == MACOS:
#        system('cd src/Utils/tellManager && bash ./buildMacUniversal.sh')
#        copyfile('src/Utils/tellManager/tellJaxerManager',
#                 'src/mozilla/' + ffdir + '/dist/bin/tellJaxerManager')
#    elif GetOS() == LINUX:
#        system('cd src/Utils/tellManager && g++ -lrt tellManager.cpp')
#        copyfile('src/Utils/tellManager/a.out',
#                 'src/mozilla/' + ffdir + '/dist/bin/telljaxermanager')
#    elif GetOS() == SOLARIS:
#        system('cd src/Utils/tellManager && gcc tellManager.cpp -lxnet -lstdc++')
#        copyfile('src/Utils/tellManager/a.out',
#                 'src/mozilla/' + ffdir + '/dist/bin/telljaxermanager')

# Build jam distribution.
print "\n===== Building jam distributions."
sys.stdout.flush()

# First clone the distribution in svn (minus the svn directories) to produce a
# master.
if os.path.exists('jam'):
    print "\n  === Remove jam folder."
    shutil.rmtree('jam')

print "\n  === Copy data from distro/jam."
for root, dirs, files in os.walk('distro/jam'):
    destdir = root.partition('distro/')[2]
    os.mkdir(destdir)
    root = root + '/'
    destdir = destdir + '/'

    for file in files:
        copyfile(root + file, destdir + file)

    if '.svn' in dirs:
        dirs.remove('.svn')

    # We don't ship MySQL any more in any package.
    if 'mysql' in dirs:
        dirs.remove('mysql')

    if GetOS() != SOLARIS and 'Apache22-solaris' in dirs:
        print "\n  === Removing Apache22-solaris."
        dirs.remove('Apache22-solaris')

    if GetOS() != MACOS and 'Apache22-mac-universal' in dirs:
        print "\n  === Removing Apache22-mac-universal."
        dirs.remove('Apache22-mac-universal')

    if GetOS() != WINDOWS and 'Apache22' in dirs:
        print "\n  === Removing Apache22."
        dirs.remove('Apache22')

    if GetOS() != LINUX and 'Apache22-cent32' in dirs:
        print "\n  === Removing Apache22-cent32."
        dirs.remove('Apache22-cent32')

# Now copy the built stuff into the master clone.
print "\n  === Copying data from src/mozilla/" + ffdir + "/dist/bin to jam/jaxer."
for root, dirs, files in os.walk('src/mozilla/' + ffdir + '/dist/bin'):
    destdir = root.replace('src/mozilla/' + ffdir + '/dist/bin', 'jam/jaxer')
    if not os.path.exists(destdir):
        os.mkdir(destdir)
    root = root + '/'
    destdir = destdir + '/'
    if destdir == 'jam/jaxer/':
        if GetOS() == WINDOWS:
            if 'firefox.exe' in files:
                files.remove('firefox.exe')  
            if 'JaxerPro.exe' in files:
                files.remove('JaxerPro.exe')  
        else:
            if 'firefox' in files:
                files.remove('firefox')
            if 'firefox-bin' in files:
                files.remove('firefox-bin')
            if 'jaxerpro' in files:
                files.remove('jaxerpro')
            if 'JaxerPro' in files:
                files.remove('JaxerPro')
        if 'chrome' in dirs:
            dirs.remove('chrome')
        if 'dictionaries' in dirs:
            dirs.remove('dictionaries')
        if 'plugins' in dirs:
            dirs.remove('plugins')
        if 'searchplugins' in dirs:
            dirs.remove('searchplugins')
    for file in files:
        copyfile(root + file, destdir + file)

print "\n  === Renaming/Removing extra stuff."
if GetOS() == LINUX:
    print "\n   == Renaming jam/Apache22-local to jam/Apache22."
    os.rename('jam/Apache22-local', 'jam/Apache22')
    shutil.copytree('src/scripts/unix', 'jam/scripts')
#    shutil.rmtree('jam/scripts/.svn')
    os.remove('jam/StartServers.bat')
    os.remove('jam/ConfigureFirewall.exe')
elif GetOS() == MACOS:
    print "\n   == Renaming jam/Apache22-mac-universal to jam/Apache22."
    os.rename('jam/Apache22-mac-universal', 'jam/Apache22')

    print "\n   == Extra process for MAC."
    shutil.copytree('src/launcher/mac/build/Default/Jaxer Launcher.app', 'jam/Jaxer Launcher.app')
    shutil.copytree('src/scripts/unix', 'jam/scripts')
#    shutil.rmtree('jam/scripts/.svn')
    os.remove('jam/StartServers.bat')
    os.remove('jam/ConfigureFirewall.exe')
elif GetOS() == SOLARIS:
    print "\n   == Renaming jam/Apache22-local to jam/Apache22."
    os.rename('jam/Apache22-local', 'jam/Apache22')
    shutil.copytree('src/scripts/solaris', 'jam/scripts')
#    shutil.rmtree('jam/scripts/.svn')
    os.remove('jam/StartServers.bat')
    os.remove('jam/ConfigureFirewall.exe')

print "\n  === Copying chrome (should be reviewed/removed in the future)"
os.mkdir('jam/jaxer/chrome');
copyfile('src/mozilla/' + ffdir +'/dist/bin/chrome/en-US.manifest',
         'jam/jaxer/chrome/en-US.manifest')
copyfile('src/mozilla/' + ffdir +'/dist/bin/chrome/en-US.jar',
         'jam/jaxer/chrome/en-US.jar')

# Copy the build-time generated framework tests as well.
#shutil.copytree('framework/tests', 
#         'jam/jaxer/aptana/diagnostics/unit_tests_jaxer')

#copyfile('framework/testRunner.html',
#         'jam/jaxer/aptana/diagnostics/testRunner.html')
#copyfile('framework/unitTestingBoth.js',
#         'jam/jaxer/aptana/diagnostics/unitTestingBoth.js')
#copyfile('framework/unitTestingServerOnly.js',
#         'jam/jaxer/aptana/diagnostics/unitTestingServerOnly.js')
#copyfile('framework/unitTestingHeader.js',
#         'jam/jaxer/aptana/diagnostics/unitTestingHeader.js')

# Create a version file in NSIS format
print "\n  === Creating a version file in NSIS format."
lines = open("src/mozilla/aptana/jaxerBuildId.h").readlines()
for line in lines:
    if line.find("#define JAXER_BUILD_ID") >= 0:
        f = open("jam/jaxer/version.txt", "w")
        f.write('!define JAXER_BUILD_ID ' + line.split()[2])
        f.close()
        break

# Create docs
#print "\n  === Creating docs."
#if (cfg['makeDocs']):
#    system('python make-docs.py %s' % (fullversion))

# Create subset distributions and zip them.
print "\n===== Creating and zipping distributions."
if GetOS() == WINDOWS:
    topdirname = 'Aptana Jaxer'
elif GetOS() == MACOS:
    topdirname = 'Aptana_Jaxer'
else:
    topdirname = 'AptanaJaxer'

if os.path.exists(topdirname):
    shutil.rmtree(topdirname)

os.rename('jam', topdirname)

if GetOS() == WINDOWS or GetOS() == LINUX or GetOS() == SOLARIS:
    system('zip -9rX distro/Jaxer_package_withApache.zip "' + topdirname + '"')
    shutil.rmtree(topdirname + '/Apache22') 

if GetOS() == MACOS:
    print "\n===== Building DMG."
    system('cd "' + topdirname + '/.." && cd dmgbuilder && ./dmgbuilder dmgbuild.config')
    print "\n===== DMG building done."

if GetOS() != MACOS:
    print "\n===== Building Package."
    system('zip -9rX distro/Jaxer_package.zip "' + topdirname + '"')

print "\n===== Building update.zip..."
system('cd "' + topdirname + '" && zip -9rX ../distro/Jaxer_update.zip jaxer')
print "\n===== Done building update.zip."
#shutil.rmtree(topdirname) # leave it here, it's useful for debugging and will be cleaned on the next run anyway
