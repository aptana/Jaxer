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

#default port
ManagerCmdPort=4328
## Get cmd line
while getopts 'p:' OPTION
do
  case $OPTION in
  p)  ManagerCmdPort="$OPTARG"
      ;;
  ?)  echo "Usage: $(basename $0) [-p managerCmdPort]" >&2
      exit 1
      ;;
  esac
done

shift $(($OPTIND - 1 ))
if [ $# -ge 1 ]; then
    echo "Usage: $(basename $0) [-p managerCmdPort]" >&2
    exit 2
fi


$JAXERBASE/jaxer/telljaxermanager -p $ManagerCmdPort stop
sleep 5
kill `cat $JAXERBASE/tmp/jaxer.pid` > /dev/null 2>&1
rm $JAXERBASE/tmp/jaxer.pid > /dev/null 2>&1


