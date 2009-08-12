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

var conn, res, i;

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

res = conn.query("DROP TABLE IF EXISTS t1, t2");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("create table t1 (a int)");
if(res.error()) {
	throw ("Create table t1 failed");
}

res = conn.query("create table t2 (a int)");
if(res.error()) {
	throw ("Create table t2 failed");
}

res = conn.query("insert into t2 values (1)");
if(res.error()) {
	throw ("Insert into table t2 failed");
}

for(i= 0; i < 3; i++) {
  	res = conn.query("insert into t1 select a from t2");
	if(res.error()) {
		throw ("Insert into table t1 'select a from t2' failed.  Error: " + res.error());
	}

	res = conn.query("select * from t1");
	if(res.error()) {
		throw ("select * from t1 failed.  Error: " + res.error());
	}
	
	if (!res.rowCount() == i+1) {
		throw ("Expected res.rowCount() = i+1.  I = " + i +  " ,but received rowCount = " + res.rowCount());
	}
}

// Drop table t1
res = conn.query("drop table t1, t2");
if(res.error()) {
	throw ("Drop table  failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
