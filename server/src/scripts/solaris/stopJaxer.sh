#! /bin/bash
if [ ! "${JAXERBASE}" ]; then
    JAXERBASE=/opt/AptanaJaxer
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
kill `cat $JAXERBASE/jaxer/jaxer.pid`
rm $JAXERBASE/jaxer/jaxer.pid

