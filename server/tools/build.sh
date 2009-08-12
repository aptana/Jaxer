#!/bin/sh

arg="--loop=1 autobuild"
if [ $# -gt 0 ]
then
    arg=$@
fi

while [ true ]; do
    if [ -f ./ctrl/forceexit.ctrl ]; then
        rm -f ./ctrl/forceexit.ctrl
        echo "Exit build per forceexit."
        exit 0
    fi
        
    svn up
    ./aptan --conf conf/build.properties.working --gconf conf/global_build.properties \
      --notify --notifyto=builds-jaxer@aptana.com \
      --notifyfrom=solaris-build@aptana.com \
      --notifyhost=mail.aptana.com --logfile=logs/build.log \
      $arg
    sleep 10
done

