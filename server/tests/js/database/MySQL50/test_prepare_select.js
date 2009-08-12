/**
   File Name:          test_prepare_select.js
   Description:

	Test simple select with prepare.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_select";

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

// Enable autocommit
res = conn.autocommit(1);
if(conn.error()) {
	throw ("Autocommit failed: " + conn.error());
}

 // Clean up if necessary
res = conn.query("DROP TABLE IF EXISTS test_select");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_select(id int, name varchar(50))");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table.
 res = conn.query("INSERT INTO test_select VALUES(10, 'venu')");
if(res.error()) {
	throw ("Insert statement failed.  Error: " + res.error());
}

// Commit the update
 res= conn.commit();
 if(conn.error()) {
	throw ("Commit failed :", conn.error());
}

// Select some data from the table, using a prepared statement
stmt = conn.prepare("SELECT * FROM test_select");
if(!stmt) {
	throw ("Update prepare statement failed.  Error: " + res.error());
}

// Execute the prepared statement
rc = stmt.execute();
 if (rc){
	throw ("Statement execute failed. Error: " + rc);
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 2){
	throw ("Param count incorrect. Expected 2.  Received: " + pcnt);
}

// Verify row count?  - TBD

// Close open statement.
stmt.close();

 // Clean up 
res = conn.query("DROP TABLE test_select");
if(res.error()) {
	throw ("Drop table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_select(id tinyint, id1 int, " /
                                                "  id2 float, id3 float, " /
                                                "  name varchar(50))");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table.
 res = conn.query("INSERT INTO test_select(id, id1, id2, name) VALUES(10, 5, 2.3, 'venu')");
if(res.error()) {
	throw ("Insert statement failed.  Error: " + res.error());
}

// Commit the update
 res= conn.commit();
 if(conn.error()) {
	throw ("Commit failed :", conn.error());
}

// Select some data from the table, using a prepared statement
stmt = conn.prepare("SELECT * FROM test_select");
if(!stmt) {
	throw ("Update prepare statement failed.  Error: " + res.error());
}

// Execute the prepared statement
rc = stmt.execute();
 if (rc){
	throw ("Statement execute failed. Error: " + rc);
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 2){
	throw ("Param count incorrect. Expected 2.  Received: " + pcnt);
}

// Verify row count?  - TBD

// Close open statement.
stmt.close();

// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

