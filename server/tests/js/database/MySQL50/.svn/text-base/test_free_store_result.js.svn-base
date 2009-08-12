/**
   File Name:          test_free_store_result.js
   Description:

	Add description here ...

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_free_result";

var conn, res, row, rowCnt, i;

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

// Create tables t1, t2, then attempt to insert values into t2.

res = conn.query("DROP TABLE IF EXISTS test_free_result");

if(res.error()) {
	throw ("Drop existing table failed: " + conn.error());
}

res = conn.query("create table test_free_result(c1 int primary key auto_increment)");
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}

res = conn.query("insert into test_free_result values(1), (2), (3)");
if(res.error()) {
	throw ("Insert failed: " + conn.error());
}

res = conn.query("select * from test_free_result");
if(res.error()) {
	throw ("Select * failed: " + conn.error());
}

rowCnt = res.rowCount();
if(rowCnt == 0) {
	throw ("No rows returned from Select *: " + conn.error());
}

while ((row = res.fetchRow()))
{
 	i++;
}

if (! rowCnt == i) {
	throw ("fetchRow() while loop did not return expected row count of 3.  Returned: " + i);
	}

// Drop table - should be successful now
res = conn.query("drop table test_free_result");
if(res.error()) {
	throw ("Drop table failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
