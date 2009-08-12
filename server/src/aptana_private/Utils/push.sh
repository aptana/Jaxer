#!/bin/sh
if [ $# -ne 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi
srcfile=$1
destfile=`echo $srcfile | sed -e 's/.*\///'`
echo scp $srcfile build@update.aptana.com:/var/websites/update.aptana.com/downloads/jaxer/source/$destfile
scp $srcfile build@update.aptana.com:/var/websites/update.aptana.com/downloads/jaxer/source/$destfile
