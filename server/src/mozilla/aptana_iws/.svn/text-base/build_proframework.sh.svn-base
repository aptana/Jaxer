#!/bin/sh

if [ $# -ne 1 ]; then
    echo $0 OBJ_DIR 
    exit 127
fi

OBJ_DIR=$1

SrcFD=../../../framework
SrcTools=../../../../../tools
FFDIR=`echo $OBJ_DIR | sed -e 's/.*\///'`

DestFD=$OBJ_DIR/dist/bin/proframework

if [ ! -d $DestFD ]; then
    mkdir -p $DestFD
fi

ServerOutput=../src/mozilla/$FFDIR/dist/bin/proframework/serverFramework.js
APIOutput=../src/mozilla/$FFDIR/dist/bin/proframework/api_sdoc.js
JSLibOutput=../src/mozilla/$FFDIR/dist/bin/proframework/JSLib.js
ClientOutput=../src/mozilla/$FFDIR/dist/bin/proframework/clientFramework.js

Inst="
SERVER_OUTPUT=\"$ServerOutput\";

JSLIB_OUTPUT=\"$JSLibOutput\";

CLIENT_OUTPUT=\"$ClientOutput\";

LICENSE_HEADER = \"pro_framework_header.txt\";

COMPRESSED_LICENSE_HEADER = \"pro_framework_compressed_header.txt\";

API_OUTPUT = \"$APIOutput\";
"

echo $Inst > $SrcFD/buildProConfig.js

java -showversion -Xmx512m -jar \
    $SrcTools/com.aptana.autobuild/libs/auto-builder.jar \
    $SrcFD/scripts/auto_builder.js \
    $SrcFD/build.json \
    CONFIG_FILE=buildProConfig.js


if [ -d $SrcFD/src ]
then
    cp -fr $SrcFD/src $DestFD
fi