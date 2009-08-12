/**
   File Name:          test_affected_rows.js
   Description:

	Test that both mysql_affected_rows() and mysql_stmt_affected_rows()
  	return -1 on error, 0 when no rows were affected, and (positive) row
  	count when some rows were affected.

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_affected_rows";

var conn, res, rowcnt;

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

// Drop table if it exists

res = conn.query("DROP TABLE IF EXISTS t1");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("CREATE TABLE t1 (i INT UNIQUE)");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("INSERT INTO t1 VALUES (1), (2)");
if(res.error()) {
	throw ("Insert into table failed");
}
rowcnt = res.rowCount();
if(!rowcnt == 2) {
	throw ("Number of affected rows incorrect. Expected 2.  Received: " + rowcnt);
}

// Force duplicate entry '1' for key 1 error
res = conn.query("INSERT INTO t1 VALUES (1), (2)");

rowcnt = res.rowCount();
if(!rowcnt == -1) {
	throw ("Number of affected rows incorrect. Expected -1.  Received: " + rowcnt);
}

res = conn.query("UPDATE t1 SET i= 4 WHERE i = 3");

rowcnt = res.rowCount();
if(!rowcnt == 0) {
	throw ("Number of affected rows incorrect. Expected 0.  Received: " + rowcnt);
}

res = conn.query("DELETE from t1");

res = conn.query("INSERT INTO t1 VALUES (1), (2)");

rowcnt = res.rowCount();
if(!rowcnt == 2) {
	throw ("Number of affected rows incorrect. Expected 2.  Received: " + rowcnt);
}

res = conn.query("INSERT INTO t1 VALUES (1), (2)");

rowcnt = res.rowCount();
if(!rowcnt == -1) {
	throw ("Number of affected rows incorrect. Expected -1.  Received: " + rowcnt);
}

res = conn.query("UPDATE t1 SET i= 4 WHERE i = 3");

rowcnt = res.rowCount();
if(!rowcnt == 0) {
	throw ("Number of affected rows incorrect. Expected 0.  Received: " + rowcnt);
}

// Drop table t1
res = conn.query("drop table t1");
if(res.error()) {
	throw ("Drop table after create and insert failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

