/**
   File Name:          test_client_query.js
   Description:
	
	Test sample client query processing.	

   Author:             Jane Tudor
   Date:               20 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_client_query";

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

res = conn.query("DROP TABLE IF EXISTS t1");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("CREATE TABLE t1(id int primary key auto_increment, name varchar(20))");
if(res.error()) {
	throw ("Create table attempt 1 failed : " + res.error());
}

res = conn.query("CREATE TABLE t1(id int, name varchar(20))");
if(!res.error()) {
	throw ("Create table attempt 2 succeeded.  Should have failed. RES =") + res.error();
}

res = conn.query("INSERT INTO t1(name) VALUES('mysql')");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("INSERT INTO t1(name) VALUES('monty')");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("INSERT INTO t1(name) VALUES('venu')");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("INSERT INTO t1(name) VALUES('deleted')");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("INSERT INTO t1(name) VALUES('deleted')");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("UPDATE t1 SET name= 'updated' WHERE name= 'deleted'");
if(res.error()) {
	throw ("Update name failed");
}

res = conn.query("UPDATE t1 SET id= 3 WHERE name= 'updated'");
if(!res.error()) {
	throw ("Update id failed, but should have succeeded.");
}

// Drop table t1
res = conn.query("drop table t1");
if(res.error()) {
	throw ("Drop table after create and insert failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

