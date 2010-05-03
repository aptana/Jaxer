@echo off
SET scriptFile="%TREEROOT%\products\server\framework\scripts\auto_builder.js"
SET buildFile="%TREEROOT%\products\server\framework\build.json"

@echo Building %buildFile%
java -jar auto-builder.jar %scriptFile% %buildFile%
