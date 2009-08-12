#!/bin/sh

if [ $# -ne 1 ]; then
    echo $0 OBJ_DIR 
    exit 127
fi

OBJ_DIR=$1

CWD=`pwd`
DocGenRoot=$CWD/../../../tools/ide_crossover/libs

BINDIR=$OBJ_DIR/dist/bin
DestFD=$BINDIR/aptana/doc/api
FrameworkDir=$BINDIR/framework
JSLib=$FrameworkDir/JSLib.js

# Grab the version from .h file, eg 1.0
ShortVersion=`cat ./jaxerBuildId.h | grep "define JAXER_BUILD_ID \"" | \
    sed -e 's/[^\"]*\"\([0-9]*\.[0-9]*\).*$/\1/'`

# Clear out anything and start afresh
if [ -d $DestFD ]; then
    rm -fr $DestFD
fi
mkdir -p $DestFD

#We have to execute java in the dest dir
cd $DestFD

java -showversion -Xmx512m \
     -jar $DocGenRoot/AptanaDocGen.jar \
    -v --output-file $DestFD/combined_framework.xml \
    --import Jaxer.=JSLib. \
    --browser "Jaxer Server Framework" \
    --browser-version $ShortVersion \
    $FrameworkDir/api_sdoc.js \
    $FrameworkDir/JSLib.js \
    $FrameworkDir/config.js \
    $FrameworkDir/configApps.js \
    --browser "Jaxer Client Framework" \
    --browser-version $ShortVersion \
    $FrameworkDir/clientFramework.js \
    --filter JSLib \
    --filter-private 

java -showversion -Xmx512m \
    -jar $DocGenRoot/saxon8.jar \
    $DestFD/combined_framework.xml \
    $DocGenRoot/help_documentation.xslt \
    ReferenceName="Jaxer" \
    ReferenceDisplayName="Jaxer Framework"

cp -f $DocGenRoot/*.gif $DocGenRoot/*.png $DocGenRoot/*.js $DocGenRoot/*.css $DestFD

