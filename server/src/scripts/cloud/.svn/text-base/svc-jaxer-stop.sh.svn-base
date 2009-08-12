#!/usr/bin/bash

echo "Stopping JaxerManager"
export JAXERBASE=/opt/AptanaJaxer
if [ -f $JAXERBASE/jaxer/telljaxermanager ]; then
   $JAXERBASE/jaxer/telljaxermanager exit
fi
sleep 5

pid=`ps -ef | grep "jaxermanager" | grep -v "grep jaxermanager" | awk '{print $2}'`
if [ -n "$pid" ]; then
   echo "JaxerManager not stopped by tellmanager. Trying aleternate method"
else
   echo "JaxerManager Stopped"
   exit
fi

pid2=`cat $JAXERBASE/jaxer/jaxer.pid`
grepid=`ps -p $pid2 | grep "jaxer" | awk '{print $4}'`
if [ -n "$grepid" ]; then
echo "Stopping JaxerManager via pid"
kill $pid2
echo "JaxerManager stopped"
fi
rm -f $JAXERBASE/jaxer/jaxer.pid

pid3=`ps -ef | grep "jaxermanager" | grep -v "grep jaxermanager" | awk '{print $2}'`
if [ -n "$pid3" ]; then
echo "Stopping JaxerManager via direct kill"
kill $pid3
echo "JaxerManager stopped"
fi