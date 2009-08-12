#!/bin/sh

if [ $# -ne 2 ]; then
    echo $0 OBJDIR OS_ARCH
    exit 127
fi
    
OBJDIR=$1
OS_ARCH=$2

echo Generate version.txt
grep "#define JAXER_BUILD_ID " jaxerBuildId.h | sed -e 's/\#/\!/' > $OBJDIR/dist/bin/version.txt

echo Generate framework.
./build_framework.sh $1
ret=$?
if [ $ret -ne 0 ]; then
    echo "Generate Framework failed ($ret)"
    exit $ret
fi

echo Generate framework.
./build_docs.sh $1
ret=$?
if [ $ret -ne 0 ]; then
    echo "Build Jaxer document ($ret)"
    exit $ret
fi

if [ -f build_proframework.sh ]; then
    echo Generate proframework.
    ./build_proframework.sh $1
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "Generate ProFramework failed ($ret)"
        exit $ret
    fi
fi

# copy some build-generated stuff from framework into diagnostics
FRAMEWORK_DIR=../../../framework
TEST_DIR=$OBJDIR/dist/bin/aptana/diagnostics
ExtraFiles="
testRunner.html
unitTestingBoth.js
unitTestingServerOnly.js
unitTestingHeader.js
"

if [ ! -d $TEST_DIR ]; then
    mkdir -p $TEST_DIR
fi

cp -fr $FRAMEWORK_DIR/tests $TEST_DIR/unit_tests_jaxer
for afile in $ExtraFiles
do
  cp -f $FRAMEWORK_DIR/$afile $TEST_DIR
done

# copy libevent to bin if exists
evtLib=
if [ $OS_ARCH = Darwin ]; then
  evtLib=$OBJDIR/../../libevent/libevent-1.4.2-rc/.libs/libevent-1.4.2.dylib
elif [ $OS_ARCH != WINNT ]; then
  evtLib=$OBJDIR/../../libevent/libevent-1.4.2-rc/.libs/libevent-1.4.so.2
fi
if [ -f "$evtLib" ]; then
  echo copy libevent to bin
  cp -f $evtLib $OBJDIR/dist/bin/
fi

# Package all xpt files into a single file
echo Package all xpt files into a single file
$OBJDIR/dist/bin/xpt_link $OBJDIR/dist/bin/components/components.xpt_ \
    $OBJDIR/dist/bin/components/*.xpt

ret=$?
if [ $ret -ne 0 ]; then
    echo xpt_link failed with $ret
    exit $ret
fi

rm -f $OBJDIR/dist/bin/components/*.xpt
mv $OBJDIR/dist/bin/components/components.xpt_ $OBJDIR/dist/bin/components/components.xpt

# Remove certain files
FilesToBeRemoved="
components/compreg.dat
components/xpti.dat
mangle
mangle.exe
shlibsign
shlibsign.exe
xpcshell
xpcshell.exe
xpicleanup
xpicleanup.exe
xpidl
xpidl.exe
xpt_dump
xpt_dump.exe
xpt_link
xpt_link.exe
"

echo Clean certain files
for afile in $FilesToBeRemoved
do
    if [ -f $OBJDIR/dist/bin/$afile ]; then
       rm -f $OBJDIR/dist/bin/$afile
    fi
done


