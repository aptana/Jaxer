@echo off

rem Compile the interfaces used by the servlet
javac com\aptana\jaxer\connectors\servlet\interfaces\*.java -classpath lib\servlet-api-2.5-6.1.4.jar

rem Create an interfaces jar file
jar cvf servlet_interfaces.jar com\aptana\jaxer\connectors\servlet\interfaces\*.class

rem Compile the filter and servlet classes
javac com\aptana\jaxer\connectors\servlet\core\*.java -classpath lib\servlet-api-2.5-6.1.4.jar;servlet_interfaces.jar

rem Create a servlet core jar file
jar cvf servlet_core.jar com\aptana\jaxer\connectors\servlet\core\*.class

rem Remove the old war file directory structure
rmdir /s /q products

rem Create the structure for the jaxer-server/callback WAR file
mkdir products\jaxer-server\WEB-INF\lib
xcopy /y jaxer-server\web.xml products\jaxer-server\WEB-INF
xcopy /y servlet_interfaces.jar products\jaxer-server\WEB-INF\lib
xcopy /y servlet_core.jar products\jaxer-server\WEB-INF\lib
jar cvf products\jaxer-server.war products\jaxer-server

rem Create the structure of the skeleton jaxer app WAR file
mkdir products\jaxer-app\WEB-INF\lib
xcopy /y jaxer-app\web.xml products\jaxer-app\WEB-INF
xcopy /y servlet_interfaces.jar products\jaxer-app\WEB-INF\lib
xcopy /y servlet_core.jar products\jaxer-app\WEB-INF\lib
jar cvf products\jaxer-app.war products\jaxer-app

