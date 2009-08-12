# /bin/bash

export JAXERBASE=/opt/AptanaJaxer

$JAXERBASE/scripts/startJaxer.sh $1 $2
sleep 3
$JAXERBASE/scripts/startApache.sh
