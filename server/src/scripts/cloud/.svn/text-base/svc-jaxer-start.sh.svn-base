#!/bin/bash                                                                                                                                                                  

export JAXERBASE=/opt/AptanaJaxer
cd $JAXERBASE

if [ ! -d "logs" ]; then
        mkdir "logs"
fi

# Register first before staring any Jaxers                                                                                                                                   
export LD_LIBRARY_PATH=$JAXERBASE/jaxer
# Specify the temp dir location                                                                                                                                              
export JAXER_TEMP=$JAXERBASE/tmp
export HOME=$JAXER_TEMP

if [ ! -d "tmp" ]; then
        mkdir "tmp"
        cd $JAXERBASE/jaxer
        ./jaxer -reg > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo 1>&2 "Jaxer registration failed.  Please check all depend libs exist."
            echo 1>&2 "For example: ldd jaxer"
            cd $JAXERBASE
            exit 127
        fi
        cd $JAXERBASE
fi

if [ ! -d "local_jaxer" ]; then
        mkdir "local_jaxer"
	    cp -R "jaxer/default_local_jaxer/" "local_jaxer"
fi

if [ ! -d "local_jaxer/conf" ]; then
        mkdir "local_jaxer/conf"
	    cp -R "jaxer/default_local_jaxer/conf/" "local_jaxer/conf"
fi

if [ ! -f "$JAXERBASE/local_jaxer/conf/config.js" ]; then
	if [ -f "$JAXERBASE/jaxer/default_local_jaxer/conf/config.js" ]; then	
		cp "$JAXERBASE/jaxer/default_local_jaxer/conf/config.js" "$JAXERBASE/local_jaxer/conf/config.js"
	fi
fi

if [ ! -f "$JAXERBASE/local_jaxer/conf/configLog.js" ]; then
	if [ -f "$JAXERBASE/jaxer/default_local_jaxer/conf/configLog.js" ]; then	
		cp "$JAXERBASE/jaxer/default_local_jaxer/conf/configLog.js" "$JAXERBASE/local_jaxer/conf/configLog.js"
	fi
fi

if [ ! -f "$JAXERBASE/local_jaxer/conf/configApps.js" ]; then
	if [ -f "$JAXERBASE/jaxer/default_local_jaxer/conf/configApps.js" ]; then	
		cp "$JAXERBASE/jaxer/default_local_jaxer/conf/configApps.js" "$JAXERBASE/local_jaxer/conf/configApps.js"
	fi
fi

if [ ! -f "$JAXERBASE/local_jaxer/conf/JaxerManager.cfg" ]; then
	if [ -f "$JAXERBASE/jaxer/default_local_jaxer/conf/JaxerManager.cfg" ]; then
		cp "$JAXERBASE/jaxer/default_local_jaxer/conf/JaxerManager.cfg" "$JAXERBASE/local_jaxer/conf/JaxerManager.cfg"
	fi
fi

chown -R aptana "$JAXERBASE/local_jaxer/conf"
chgrp -R aptana "$JAXERBASE/local_jaxer/conf"
chmod -R g+w "$JAXERBASE/local_jaxer/conf"

if [ ! -d "local_jaxer/extensions" ]; then
        mkdir "local_jaxer/extensions"
	    cp -R "jaxer/default_local_jaxer/extensions/" "local_jaxer/extensions"
fi

# Move into the main Jaxer directory                                                                                                                                         
cd $JAXERBASE/jaxer

# Specify the log file location URI                                                                                                                                          
JAXER_LOGFILE="|/opt/local/sbin/cronolog -a --symlink=$JAXERBASE/logs/jaxer.log $JAXERBASE/logs/jaxer_%Y%m%d.log"

# Start JaxerManager with optional second parameter                                                                                                                          
./jaxermanager --configfile=$JAXERBASE/local_jaxer/conf/JaxerManager.cfg --maxmemory=180 --log:output="$JAXER_LOGFILE" --pid-file=$JAXERBASE/jaxer/jaxer.pid --cfg:LOCAL_CONF_DIR=file:///opt/AptanaJaxer/local_jaxer/conf $2 &
