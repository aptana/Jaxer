/**
   File Name:          test_prepare_simple.js
   Description:

	Test simple prepares of all DML statements.

   Author:             Jane Tudor
   Date:               06 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_simple";

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
res = conn.query("DROP TABLE IF EXISTS test_prepare_simple");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare_simple(id int, name varchar(50))");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table, using a simple prepared statement
iquery = "INSERT INTO test_prepare_simple VALUES(?, ?)";
stmt = conn.prepare(iquery);
if(!stmt) {
	throw ("Insert prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 2){
	throw ("Param count incorrect. Expected 2.  Received: " + pcnt);
}

// Close open statement.
stmt.close();

// Update some values in table, using a simple prepared statement
uquery = "UPDATE test_prepare_simple SET id=? WHERE id=? AND CONVERT(name USING utf8)= ?";
stmt = conn.prepare(uquery);
if(!stmt) {
	throw ("Update prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 3){
	throw ("Param count incorrect. Expected 3.  Received: " + pcnt);
}

// Close open statement.
stmt.close();


// Now delete some values from table, using a simple prepared statement with no params
dquery = "DELETE FROM test_prepare_simple WHERE id=10";
stmt = conn.prepare(dquery);
if(!stmt) {
	throw ("Delete prepared statement failed.  Error: " + res.error());
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

stmt.close();

// Now delete some values from table, using a simple prepared statement using one param
dquery = "DELETE FROM test_prepare_simple WHERE id=?";
stmt = conn.prepare(dquery);
if(!stmt) {
	throw ("Delete prepared statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 2){
	throw ("Param count incorrect. Expected 2.  Received: " + pcnt);
}

stmt.close();


// Finally, execute a select
squery = "SELECT * FROM test_prepare_simple WHERE id=? AND CONVERT(name USING utf8)= ?";
stmt = conn.prepare(squery);
if(!stmt) {
	throw ("Select prepared statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 1){
	throw ("Param count incorrect. Expected 1.  Received: " + pcnt);
}

stmt.close();

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

