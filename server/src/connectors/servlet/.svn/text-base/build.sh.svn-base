#!/bin/sh

# Compile the interfaces used by the servlet
javac com/aptana/jaxer/connectors/servlet/interfaces/*.java -classpath lib/servlet-api-2.5-6.1.4.jar

# Create an interfaces jar file
jar cvf servlet_interfaces.jar com/aptana/jaxer/connectors/servlet/interfaces/*.class

# Compile the filter and servlet classes
javac com/aptana/jaxer/connectors/servlet/core/*.java -classpath "lib/servlet-api-2.5-6.1.4.jar:servlet_interfaces.jar"

# Create a servlet core jar file
jar cvf servlet_core.jar com/aptana/jaxer/connectors/servlet/core/*.class

# Remove the old war file directory structure
rm -rf products

# Create the structure for the jaxer-server/callback WAR file
mkdir -p products/jaxer-server/WEB-INF/lib
cp jaxer-server/web.xml products/jaxer-server/WEB-INF
cp servlet_interfaces.jar products/jaxer-server/WEB-INF/lib
cp servlet_core.jar products/jaxer-server/WEB-INF/lib
jar cvf products/jaxer-server.war products/jaxer-server

# Create the structure of the skeleton jaxer app WAR file
mkdir -p products/jaxer-app/WEB-INF/lib
cp jaxer-app/web.xml products/jaxer-app/WEB-INF
cp servlet_interfaces.jar products/jaxer-app/WEB-INF/lib
cp servlet_core.jar products/jaxer-app/WEB-INF/lib
jar cvf products/jaxer-app.war products/jaxer-app
