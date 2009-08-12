/**
   File Name:          test_prepare_simple.js
   Description:

	Test prepare without parameters.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_noparam";

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
res = conn.query("DROP TABLE IF EXISTS my_prepare");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE my_prepare(col1 int, col2 varchar(50))");
if(res.error()) {
	throw ("Create table failed");
}

// Insert a row into the table, using a simple prepared statement
stmt = conn.prepare("INSERT INTO my_prepare VALUES(10, 'venu')");
if(!stmt) {
	throw ("Insert prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 0){
	throw ("Param count incorrect. Expected 0.  Received: " + pcnt);
}

// Execute the last prepared statement
rc = stmt.execute();
 if (rc){
	throw ("Statement execute failed. Error: " + rc);
}

// Close open statement.
stmt.close();

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

 // Test the results now, only one row should exist
res = conn.query("SELECT * FROM my_prepare");
if(res.error()) {
	throw ("Select * failed.  Error: " + res.error());
}

// Process the results - store the result and then verify that row count = 1 -- TBD
//  result= mysql_store_result(mysql);
//  mytest(result);

//  rc= my_process_result_set(result);
//  DIE_UNLESS(rc == 1);
//  mysql_free_result(result);
  
stmt.close();

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

