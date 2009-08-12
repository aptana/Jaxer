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

cd $JAXERBASE

# Created required directories and config files if they don't exist
./scripts/post_install.sh

OSARCH=`uname`

# Register first before staring any Jaxers
if [ "A$OSARCH" = "ADarwin" ]; then
    export DYLD_LIBRARY_PATH=$JAXERBASE/jaxer
else
    export LD_LIBRARY_PATH=$JAXERBASE/jaxer
fi

# Increase maximum memory allocated to jaxer
if [ "A$OSARCH" = "ALinux" ]; then
    ulimit -a unlimited >/dev/null 2>&1
    EXTRA_ARGS="--maxmemory=0"
else
    EXTRA_ARGS=""
fi

# Specify the temp dir location
JAXER_TEMP=$JAXERBASE/tmp

# Specify the log file location
JAXER_LOG_OUTPUT=$JAXERBASE/logs/jaxer.log

# Move into the main Jaxer directory
cd $JAXERBASE/jaxer

./jaxer -reg -tempdir $JAXER_TEMP > /dev/null 2>&1
if [ $? -ne 0 ]; then
	  echo 1>&2 "Jaxer registration failed.  Please check all depend libs exist."
	  echo 1>&2 "For example: ldd jaxer"
	  exit 127
fi

# Start JaxerManager with optional second parameter
# NOTE: Once you have decide the install location.  You should move all
#       jaxermanager options to the configfile, leaving ONLY --configfile.
#       In that way, you can use the admin page to make any config changes.
#       Otherwise the following cmdline options override your changes
#       (you did through the admin page). 
./jaxermanager --configfile=$JAXERBASE/local_jaxer/conf/JaxerManager.cfg $EXTRA_ARGS --pid-file=$JAXER_TEMP/jaxer.pid --cfg:tempdir=$JAXER_TEMP --log:output=$JAXER_LOG_OUTPUT $2 &

