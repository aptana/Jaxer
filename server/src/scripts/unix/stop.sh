#!/bin/sh

ARG0="$0"
SDIR=`dirname "$ARG0"`
if [ "A$SDIR" != "A" ]; then
    cd "$SDIR"
fi

cd ..
export JAXERBASE=`pwd`
export ANCHOR=`pwd`

$JAXERBASE/scripts/stopJaxer.sh $*
$JAXERBASE/scripts/stopApache.sh

