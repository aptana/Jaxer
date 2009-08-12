#!/bin/sh

ARG0="$0"
if [ -z $JAXERBASE ]; then
    SDIR=`dirname "$ARG0"`
    if [ "A$SDIR" != "A" ]; then
        cd "$SDIR"
    fi
    cd ..
    export JAXERBASE=`pwd`
fi

export ANCHOR=$JAXERBASE

OSARCH=`uname`
# Register first before staring any Jaxers
if [ "A$OSARCH" = "ADarwin" ]; then
    export DYLD_LIBRARY_PATH=$JAXERBASE/Apache22/lib
else
    export LD_LIBRARY_PATH=$JAXERBASE/Apache22/lib
fi

$JAXERBASE/Apache22/bin/apachectl stop 

