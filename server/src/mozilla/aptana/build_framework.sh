#!/bin/sh

if [ $# -ne 1 ]; then
    echo $0 OBJ_DIR 
    exit 127
fi

OBJ_DIR=$1

SrcFD=../../../framework
SrcTools=../../../../../tools
FFDIR=`echo $OBJ_DIR | sed -e 's/.*\///'`

DestFD=$OBJ_DIR/dist/bin/framework

if [ ! -d $DestFD ]; then
    mkdir -p $DestFD
fi

ServerOutput=../src/mozilla/$FFDIR/dist/bin/framework/serverFramework.js
APIOutput=../src/mozilla/$FFDIR/dist/bin/framework/api_sdoc.js
JSLibOutput=../src/mozilla/$FFDIR/dist/bin/framework/JSLib.js
ClientOutput=../src/mozilla/$FFDIR/dist/bin/framework/clientFramework.js

Inst="
SERVER_OUTPUT=\"$ServerOutput\";

JSLIB_OUTPUT=\"$JSLibOutput\";

CLIENT_OUTPUT=\"$ClientOutput\";

LICENSE_HEADER = \"framework_header.txt\";

COMPRESSED_LICENSE_HEADER = \"framework_compressed_header.txt\";

API_OUTPUT = \"$APIOutput\";
"

echo $Inst > $SrcFD/buildConfig.js

java -showversion -Xmx512m -jar \
    $SrcTools/com.aptana.autobuild/libs/auto-builder.jar \
    $SrcFD/scripts/auto_builder.js \
    $SrcFD/build.json \
    CONFIG_FILE=buildConfig.js

jsFiles="
config.js
configLog.js
configApps.js
studio_config.js
"

for afile in $jsFiles
do
    cp -f $SrcFD/$afile $DestFD/$afile
done

if [ -d $SrcFD/extensions ]
then
    cp -fr $SrcFD/extensions $DestFD
    rm -fr $DestFD/extensions/.svn
fi

if [ -d $SrcFD/src ]
then
    cp -fr $SrcFD/src $DestFD
fi




