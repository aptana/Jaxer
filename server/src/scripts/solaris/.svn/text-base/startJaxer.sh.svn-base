#! /bin/bash

cd $JAXERBASE

# Created required directories and config files if they don't exist
if [ ! -d "data" ]; then 
	mkdir "data"
fi

if [ ! -d "public" ]; then 
	mkdir "public"
fi

if [ ! -f "public/favicon.ico" ]; then
	if [ -f "jaxer/default_public/favicon.ico" ]; then
		cp "jaxer/default_public/favicon.ico" "public/favicon.ico"
	fi
fi

if [ ! -f "public/index.html" ]; then
	if [ -f "jaxer/default_public/index.html.redirect" ]; then
		cp "jaxer/default_public/index.html.redirect" "public/index.html.redirect"
	fi
fi

if [ ! -d "logs" ]; then 
	mkdir "logs"
fi

# Register first before staring any Jaxers
export LD_LIBRARY_PATH=$JAXERBASE/jaxer
# Specify the temp dir location
JAXER_TEMP=$JAXERBASE/tmp

if [ ! -d "tmp" ]; then
	mkdir "tmp"
fi

if [ ! -d "local_jaxer" ]; then 
	mkdir "local_jaxer"
	cp -R "jaxer/default_local_jaxer/" "local_jaxer"
fi

if [ ! -d "local_jaxer/conf" ]; then 
	mkdir "local_jaxer/conf"
	cp -R "jaxer/default_local_jaxer/conf/" "local_jaxer/conf"
fi

if [ ! -d "local_jaxer/data" ]; then 
	mkdir "local_jaxer/data"
	cp -R "jaxer/default_local_jaxer/data/" "local_jaxer/data"
fi

if [ ! -f "local_jaxer/conf/config.js" ]; then
	if [ -f "jaxer/default_local_jaxer/conf/config.js" ]; then	
		cp "jaxer/default_local_jaxer/conf/config.js" "local_jaxer/conf/config.js"
	fi
fi

if [ ! -f "local_jaxer/conf/configLog.js" ]; then
	if [ -f "jaxer/default_local_jaxer/conf/configLog.js" ]; then	
		cp "jaxer/default_local_jaxer/conf/configLog.js" "local_jaxer/conf/configLog.js"
	fi
fi

if [ ! -f "local_jaxer/conf/configApps.js" ]; then
	if [ -f "jaxer/default_local_jaxer/conf/configApps.js" ]; then	
		cp "jaxer/default_local_jaxer/conf/configApps.js" "local_jaxer/conf/configApps.js"
	fi
fi

if [ ! -f "local_jaxer/conf/JaxerManager.cfg" ]; then
	if [ -f "jaxer/default_local_jaxer/conf/JaxerManager.cfg" ]; then	
		cp "jaxer/default_local_jaxer/conf/JaxerManager.cfg" "local_jaxer/conf/JaxerManager.cfg"
	fi
fi

if [ ! -d "local_jaxer/extensions" ]; then 
	mkdir "local_jaxer/extensions"
	cp -R "jaxer/default_local_jaxer/extensions/" "local_jaxer/extensions"
fi

# Move into the main Jaxer directory
cd $JAXERBASE/jaxer

# Specify the log file location
JAXER_LOG_OUTPUT=$JAXERBASE/logs/jaxer.log

./jaxer -reg -tempdir $JAXER_TEMP > /dev/null 2>&1
if [ $? -ne 0 ]; then
	  echo 1>&2 "Jaxer registration failed.  Please check all depend libs exist."
	  echo 1>&2 "For example: ldd jaxer"
	  exit 127
fi

# Start JaxerManager with optional second parameter
./jaxermanager --configfile=$JAXERBASE/local_jaxer/conf/JaxerManager.cfg --maxmemory=180 --pid-file=$JAXERBASE/jaxer/jaxer.pid --cfg:tempdir=$JAXER_TEMP --log:output=$JAXER_LOG_OUTPUT $2 &

