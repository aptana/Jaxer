#!/bin/ksh

if [ $# -ne 2 ]; then
  echo "Usage $0 <Release> <Build>" 1>&2
  echo "eg:" 1>&2
  echo "    $0 1.0.0  3989" 1>&2
  exit 1
fi

RELEASE=$1
BUILD_NO=$2

SVN_BASE=https://source.aptana.com/temp_server_repo/trunk/products/server/src

SVN_DIRS="mozilla Utils manager connectors libevent google launcher"

cd
ROOT_DIR=`pwd`

cd $ROOT_DIR
if [ ! -d "${RELEASE}.${BUILD_NO}" ]; then
  mkdir -p ${RELEASE}.${BUILD_NO}
fi
rm -fr ${RELEASE}.${BUILD_NO}/* *_${RELEASE}.${BUILD_NO}.tgz

# get Jaxer src from svn
for adir in $SVN_DIRS
do
  echo exporting $adir
  svn export -r $BUILD_NO $SVN_BASE/$adir ${RELEASE}.${BUILD_NO}/$adir
done

echo Fixing code
# clern up
# 1. We do not want the JaxerPro content, or it will break the build.
APTANA_DIR=${RELEASE}.${BUILD_NO}/mozilla/aptana
sed '/JaxerPro/d' ${APTANA_DIR}/makefiles.sh > makefiles.tmp
cp makefiles.tmp ${APTANA_DIR}/makefiles.sh
sed 's/aptana\/JaxerPro/\$(NULL)/' ${APTANA_DIR}/build.mk > build.mk.tmp
cp build.mk.tmp ${APTANA_DIR}/build.mk
rm -fr ${APTANA_DIR}/JaxerPro

# 2.  Remove mod_jaxer1.3 (it is not working)
rm -fr ${RELEASE}.${BUILD_NO}/connectors/apache/apache-modules/mod_jaxer1.3


# zip them
echo taring jaxer
tar -zcf jaxer-${RELEASE}.${BUILD_NO}.tgz ${RELEASE}.${BUILD_NO}
echo taring connectors
tar -zcf connectors-${RELEASE}.${BUILD_NO}.tgz ${RELEASE}.${BUILD_NO}/connectors

echo Done

