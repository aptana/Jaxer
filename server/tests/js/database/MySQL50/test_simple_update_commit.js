/**
   File Name:          test_simple_update.js
   Description:

	...

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_simple_update_commit";

var conn, res;
var rowcnt;

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
res = conn.query("DROP TABLE IF EXISTS test_update");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_update(col1 int, col2 varchar(50), col3 int )");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table
res = conn.query("INSERT INTO test_update VALUES(1, 'MySQL', 100)");
if(res.error()) {
	throw ("Insert failed");
}

// Verify the results 
rowcnt = res.rowCount();
 if (!rowcnt == 1){
	throw ("Number of affected rows incorrect. Expected 1.  Received: " + rowcnt);
}

// Commit the insert
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

 // Update some data in this table
res = conn.query("UPDATE test_update SET col2='YourSQL' where col1=1");
if(res.error()) {
	throw ("Update failed : " + conn.error());
}

// Commit the update
 res= conn.commit();
 if(conn.error()) {
	throw ("Commit failed :", conn.error());
}

// Drop table
res = conn.query("drop table test_update;");
if(res.error()) {
	throw ("Drop table after simple update failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


