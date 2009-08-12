#!/bin/sh

SrcRoot="../.."
APXS=/home/cltbld/jzhang/bin/apxs
$APXS \
-c -DAPACHE2 \
-L $SrcRoot/mozilla/aptana/database/jxMySQL50/lib_linux \
-I $SrcRoot/mozilla/aptana/database/jxMySQL50/include_linux \
-l mysqlclient  -lm \
-c mod_auth_mysql.c

#Add the following if you want to turn on the loggings
#-D_DO_APTANA_DEBUG \
