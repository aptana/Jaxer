/**
   File Name:          test_create_drop.js
   Description:

	Simple create tables, insert values, and drop tables.  Using simple db queries.

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_create_drop";

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

// Create tables t1, t2, then attempt to insert values into t2.

res = conn.query("DROP TABLE IF EXISTS t1, t2;");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("create table t2 (a int);");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("create table t1 (a int);");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("insert into t2 values (3), (2), (1);");
if(res.error()) {
	throw ("Insert failed");
}

// Drop tables t1, t2
res = conn.query("drop table t1, t2;");
if(res.error()) {
	throw ("Drop table after create and insert failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

