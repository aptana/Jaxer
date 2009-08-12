/**
   File Name:          test_prepare_multi_statements.js
   Description:

	Verify that a prepared statement cannot contain several SQL statements
	
   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_multi_statements";

var conn, res, stmt;

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
res = conn.query("DROP TABLE IF EXISTS test_prepare_multi");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare_multi(id int, name varchar(50))");
if(res.error()) {
	throw ("Create table failed");
}

 // Prepare a multi statement select - should fail
stmt = conn.prepare("select 1; select 'another value'");
if(stmt) {
	throw ("Multi statement prepare did not fail, but should have.");
}

// Release the statement
stmt.close();


// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
