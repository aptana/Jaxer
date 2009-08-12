/**
   File Name:          test_errors.js
   Description:

	Force error using multiple 'drop table' queries, and then process the 'SHOW ERRORS' result set

   Author:             Jane Tudor
   Date:               20 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_errors";

var conn, res;

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

// Create table test_non_exists

res = conn.query("DROP TABLE IF EXISTS test_non_exists");
res = conn.query("DROP TABLE test_non_exists");
if(!res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("SHOW ERRORS");
if(res.error()) {
	throw ("SHOW ERRORS query failed");
}

// Process results set - TBD
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone



