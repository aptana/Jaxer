/**
   File Name:          test_prepare_syntax.js
   Description:

	Test simple prepare field results.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_syntax";

var conn, res, stmt;
var iquery, uquery, dquery, squery, pcnt;

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

 // Clean up if necessary
res = conn.query("DROP TABLE IF EXISTS test_prepare_syntax");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare_syntax("  /
                         "id int, name varchar(50), extra int)");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table, using a simple prepared statement
stmt = conn.prepare("INSERT INTO test_prepare_syntax VALUES(?");
if(!stmt) {
	throw ("Insert prepare statement failed.  Error: " + res.error());
}

 // Insert some values into table, using a simple prepared statement
stmt = conn.prepare("SELECT id, name FROM test_prepare_syntax WHERE id=? AND WHERE");
if(!stmt) {
	throw ("Select prepare statement failed.  Error: " + res.error());
}

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
