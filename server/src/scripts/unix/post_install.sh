#!/bin/sh

#This script is run just after the package is installed.
#It creats certain folders/files

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

ROOT_DIRS="data logs tmp local_jaxer public"
for adir in $ROOT_DIRS
do
    if [ ! -d "$adir" ]; then
        mkdir $adir
    fi
done


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

for adir in conf data extensions
do
    if [ ! -d "local_jaxer/$adir" ]; then 
        mkdir "local_jaxer/$adir"
	cp -R "jaxer/default_local_jaxer/$adir/" "local_jaxer/$adir"
    fi
done

for afile in `ls jaxer/default_local_jaxer/conf` 
do
    if [ ! -f "local_jaxer/conf/$afile" ]; then
        cp "jaxer/default_local_jaxer/conf/$afile" "local_jaxer/conf/$afile"
    fi
done

#Change permissions to 777 (for now)
chmod -R 777 tmp logs local_jaxer data

