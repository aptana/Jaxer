#!/bin/bash
scriptFile=../../../products/server/framework/scripts/auto_builder.js
buildFile=../../../products/server/framework/build.json

echo Building $buildFile
java -jar auto-builder.jar $scriptFile $buildFile
