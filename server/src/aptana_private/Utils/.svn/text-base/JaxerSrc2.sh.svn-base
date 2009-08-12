#!/bin/sh

if [ $# -ne 2 ]; then
  echo "Usage $0 <Release> <Build>" 1>&2
  echo "eg:" 1>&2
  echo "    $0 1.0.0  3989" 1>&2
  exit 1
fi

RELEASE=$1
BUILD_NO=$2

SVN_BASE=http://svn.aptana.local/svn/jaxer/trunk
SVN_IDEBASE=http://svn.aptana.local/svn/aptana/aptana/ide_suite/trunk/tools/com.aptana.sdoc/libs

SVN_JAXER_DIRS="
tools/com.aptana.autobuild/libs
products/server/framework
products/server/distro/jam/jaxer
products/server/src/libevent
products/server/src/launcher
products/server/src/scripts
products/server/src/connectors
products/server/src/mozilla
"

DEST_IDE_DIR="products/server/tools/ide_crossover/libs"

SVN_JAM_DIR="products/server/distro/jam"
SVN_JAM_FILES="StartServers.bat ConfigureFirewall.exe LICENSE.TXT README.TXT"

cd
ROOT_DIR=`pwd`

cd $ROOT_DIR
if [ ! -d "${RELEASE}.${BUILD_NO}" ]; then
  mkdir -p ${RELEASE}.${BUILD_NO}
fi
rm -fr ${RELEASE}.${BUILD_NO}/* *_${RELEASE}.${BUILD_NO}.tgz

# get Jaxer src tree from svn
for adir in $SVN_JAXER_DIRS
do
  echo exporting $adir
  svn export -r $BUILD_NO --username hudson --password 'hudson$$1' --force $SVN_BASE/$adir ${RELEASE}.${BUILD_NO}/$adir
  if [ $? -ne 0 ] ; then
    echo svn export failed $?
    exit 1
  fi
done

# get individual files
if [ ! -d ${RELEASE}.${BUILD_NO}/$SVN_JAM_DIR ]; then
    mkdir -p ${RELEASE}.${BUILD_NO}/$SVN_JAM_DIR 
fi
cd ${RELEASE}.${BUILD_NO}/$SVN_JAM_DIR

for afile in $SVN_JAM_FILES
do
  echo exporting $afile
  svn export -r $BUILD_NO --username hudson --password 'hudson$$1' $SVN_BASE/$SVN_JAM_DIR/$afile
  if [ $? -ne 0 ] ; then
    echo svn export file failed $?
    exit 1
  fi
done
cd $ROOT_DIR

# get ide stuff
echo exporting ide_tools
svn export --username hudson --password 'hudson$$1' $SVN_IDEBASE ${RELEASE}.${BUILD_NO}/$DEST_IDE_DIR
if [ $? -ne 0 ] ; then
  echo svn export ide_crossover failed $?
  exit 1
fi

echo Fixing code
# clern up
SRC_DIR=${RELEASE}.${BUILD_NO}/products/server/src
# 1. We do not want the JaxerPro content, or it will break the build.
APTANA_DIR=$SRC_DIR/mozilla/aptana
sed '/JaxerPro/d' ${APTANA_DIR}/makefiles.sh > makefiles.tmp
cp makefiles.tmp ${APTANA_DIR}/makefiles.sh
sed 's/aptana\/JaxerPro/\$(NULL)/' ${APTANA_DIR}/build.mk > build.mk.tmp
cp build.mk.tmp ${APTANA_DIR}/build.mk
rm -fr ${APTANA_DIR}/JaxerPro

# 2.  Remove mod_jaxer1.3 (it is not working)
rm -fr $SRC_DIR/connectors/apache/apache-modules/mod_jaxer1.3

#2b. remove aptana_iws (branch)
rm -fr $SRC_DIR/mozilla/aptana_iws

# 3.  Need to replace xxxx by build ID
#3a jaxerBuildId.h
sed -e 's/xxxx/'$BUILD_NO'/' $APTANA_DIR/jaxerBuildId.h > jaxerBuildId.h.$$
cp jaxerBuildId.h.$$ $APTANA_DIR/jaxerBuildId.h

#3b Jaxer/module.ver
sed -e 's/\(WIN32_MODULE_PRODUCTVERSION[^=]*\)=.*/\1='$BUILD_NO'/' $APTANA_DIR/Jaxer/module.ver > jaxerModule.ver.$$
cp jaxerModule.ver.$$ $APTANA_DIR/Jaxer/module.ver

#3c manager/module.ver
sed -e 's/\(WIN32_MODULE_PRODUCTVERSION[^=]*\)=.*/\1='$BUILD_NO'/' $APTANA_DIR/manager/module.ver > managerModule.ver.$$
cp managerModule.ver.$$ $APTANA_DIR/manager/module.ver

#3d mod_jaxer.rc
MOD_JAXER_DIR=$SRC_DIR/connectors/apache/apache-modules/mod_jaxer
sed -e 's/X\,X\,X\,X/'$RELEASE.$BUILD_NO'/' $MOD_JAXER_DIR/mod_jaxer.rc > mod_jaxer.rc.$$
cp mod_jaxer.rc.$$ $MOD_JAXER_DIR/mod_jaxer.rc

#3e jaxer_isapi.rc
ISAPI_DIR=$SRC_DIR/connectors/iis/jaxer_isapi
sed -e 's/X\,X\,X\,X/'$RELEASE.$BUILD_NO'/' $ISAPI_DIR/jaxer_isapi.rc > jaxer_isapi.rc.$$
cp jaxer_isapi.rc.$$ $ISAPI_DIR/jaxer_isapi.rc

# finally, remove tmp files
rm -f *.$$

# 4.  Permission (should be fixed in svn)
chmod a+x $SRC_DIR/connectors/servlet/build.sh

# 5.  A couple of pro_* files should be removed
rm -f $SRC_DIR/../framework/pro_framework_header.txt
rm -f $SRC_DIR/../framework/pro_framework_compressed_header.txt

# zip them
echo taring jaxer
tar -zcf jaxer-${RELEASE}.${BUILD_NO}.tgz ${RELEASE}.${BUILD_NO}
echo taring connectors
tar -zcf connectors-${RELEASE}.${BUILD_NO}.tgz ${RELEASE}.${BUILD_NO}/products/server/src/connectors ${RELEASE}.${BUILD_NO}/products/server/src/mozilla/aptana/jaxerBuildId.h ${RELEASE}.${BUILD_NO}/products/server/src/mozilla/aptana/jaxerProtocolVersion.h

echo Done

