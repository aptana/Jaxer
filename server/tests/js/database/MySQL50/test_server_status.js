/**
   File Name:          test_server_status.js
   Description:

	  Check that the server signals when NO_BACKSLASH_ESCAPES mode is in effect,
	  and mysql_real_escape_string() does the right thing as a result.

   Author:             Jane Tudor
   Date:               27 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_server_status";

var conn, res;
var out;
var SERVER_STATUS_NO_BACKSLASH_ESCAPES = 512;


// Database connection variables
var dbServer = "curt_9100";
var dbDatabase = "testdb";
var dbUser = "jane";
var dbPass = "jaxer";
var dbPort = 0;

startTest();  // Leave this alone

// Prepare for database connection
conn = new MySQL50();

 // Establish connection to database server
res = conn.connect(dbServer, dbUser, dbPass, dbDatabase);

// Check server status
if (!conn.stat()) {
	throw ("Server status request failed. conn.stat() returned: " + conn.stat());
}

// Now check server status to ensure that NO_BACKSLASH_ESCAPES mode is in effect - need JAXER SUPPORT !!!
//if (!(conn.serverStatus && SERVER_STATUS_NO_BACKSLASH_ESCAPES)) {
//	throw ("SERVER_STATUS_NO_BACKSLASH_ESCAPES mode not in effect. conn.serverStatus returned: " + serverStatus);
//}

out = conn.escape("a'b\\c");

if (!out == "a\\'b\\\\c") {
	throw ("Expected out = string(a\\'b\\\\c).  Received: " + out);
}

// Now set server mode to NO_BACKSLASH_ESCAPES 
res = conn.query("set sql_mode='NO_BACKSLASH_ESCAPES'");
if(res.error()) {
	throw ("Set mode on failed");
}

out = conn.escape("a'b\\c");
if (!out == "a''b\\c") {
	throw ("Expected out = string(a''b\\c).  Received: " + out);
}

// Return server mode to default value 
res = conn.query("set sql_mode=''");
if(res.error()) {
	throw ("Set mode '' failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone





