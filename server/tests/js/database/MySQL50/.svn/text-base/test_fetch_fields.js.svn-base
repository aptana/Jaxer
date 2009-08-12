/**
   File Name:          test_fetch_strings.js
   Description:

	Add description here ...

   Author:             Jane Tudor
   Date:               25 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_fetch_null";

var conn, res;
var row, rc, field, fieldoff;
var MYSQL_TYPE_BLOB = 252;


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

// Drop table if exists

res = conn.query("DROP TABLE IF EXISTS t1");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("set session group_concat_max_len=1024");
if(res.error()) {
	throw ("Set session failed");
}

res = conn.query("create table t1 (f1 int unsigned, f2 varchar(255))");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("insert into t1 values (1,repeat('a',255)),(2,repeat('b',255))");
if(res.error()) {
	throw ("Insert failed");
}

// Commit the insert
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

res = conn.query("select f2,group_concat(f1) from t1 group by f2");
if(res.error()) {
	throw ("Select * failed");
}

field = res.fetchFields({});
if (!field[1].type() == MYSQL_TYPE_BLOB) {
	throw ("GROUP_CONCAT() result type should be MYSQL_TYPE_BLOB.  Type is:  " + field[1].type());
}

// Drop table 
res = conn.query("drop table t1");
if(res.error()) {
	throw ("Drop table failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

