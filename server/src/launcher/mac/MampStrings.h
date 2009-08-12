 /*
	This file is part of MAMP
 
	MAMP is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published
	by the Free Software Foundation; either version 2, or (at your
	option) any later version.

	MAMP is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with MAMP; see the file Licence.rtf.  If not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
	
	
*/

#include  <Cocoa/Cocoa.h>

#define HTTPD_LOCATION_STRING @"/Applications/Aptana_Jaxer/Apache22/bin/httpd"
#define START_MYSQL_LOCATION_STRING @"/Applications/Aptana_Jaxer/scripts/startMysql.sh"
#define START_APACHE_LOCATION_STRING @"/Applications/Aptana_Jaxer/scripts/startApache.sh"
#define STOP_MYSQL_LOCATION_STRING @"/Applications/Aptana_Jaxer/scripts/stopMysql.sh"
#define STOP_APACHE_LOCATION_STRING @"/Applications/Aptana_Jaxer/scripts/stopApache.sh"

#define START_JAXER_PATH			@"/Applications/Aptana_Jaxer/jaxer"
#define START_JAXER_LOCATION_STRING	@"/Applications/Aptana_Jaxer/scripts/startJaxer.sh"
#define STOP_JAXER_LOCATION_STRING	@"/Applications/Aptana_Jaxer/scripts/stopJaxer.sh"
#define JAXERPID_LOCATION_STRING	@"/Applications/Aptana_Jaxer/jaxer/jaxer.pid"
//#define JAXERSOCK_LOCATION_STRING	@"/Applications/Aptana_Jaxer/jaxer/jaxer.sock"

#define HTTPDPID_LOCATION_STRING @"/Applications/Aptana_Jaxer/Apache22/logs/httpd.pid"

#define MYSQLPID_LOCATION_STRING @"/Applications//Aptana_Jaxer/mysql/mysql.pid"
#define MYSQLSOCK_LOCATION_STRING @"/Applications//Aptana_Jaxer/mysql/mysql.sock"

#define SH_LOCATION @"/bin/sh"
#define KILL_LOCATION @"/bin/kill"
#define CHOWN_LOCATION @"/usr/sbin/chown"
#define HTTPDCONF_LOCATION_STRING @"/Applications/Aptana_Jaxer/Apache22/conf/httpd.conf"
#define STARTPAGE_URL_STRING @"/aptana/"
#define OLD_STARTPAGE_URL_STRING @"/aptana/"
#define PHP4_INI_LOCATION_STRING @"/Applications/Aptana_Jaxer/conf/php4/php.ini"
#define PHP5_INI_LOCATION_STRING @"/Applications/Aptana_Jaxer/conf/php5/php.ini"
#define NO_PHP_Extension 0
#define ZENDOPTIMIZER_PHP_Extension 1
#define APC_PHP_Extension 2
#define EACCELERATOR_PHP_Extension 4
#define XCACHE_PHP_Extension 8
#define MYSQLERROR_LOG @"/Applications/Aptana_Jaxer/logs/mysql_error_log"
#define MYSQL_TMP @"/Applications/Aptana_Jaxer/tmp/mysql"

#define preferences [NSUserDefaults standardUserDefaults]

#define DOCUMENT_ROOT_MARKER @"# JAXER DOCUMENT_ROOT !! Don't remove this line !!"
