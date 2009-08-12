/**
   File Name:          test_view_sp_list_fields.js
   Description:

	Test ...

   Author:             Jane Tudor
   Date:               21 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_view_sp_list_fields";

var conn, res, field;

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
res = conn.query("DROP FUNCTION IF EXISTS f1");
if(res.error()) {
	throw ("Drop existing function failed");
}
res = conn.query("DROP TABLE IF EXISTS v1, t1, t2");
if(res.error()) {
	throw ("Drop existing table failed");
}
res = conn.query("DROP VIEW IF EXISTS v1, t1, t2");
if(res.error()) {
	throw ("Drop existing view failed");
}

// Create necessary functions, tables, and views for test
res = conn.query("create function f1 () returns int return 5");
if(res.error()) {
	throw ("Create function failed");
}
res = conn.query("create table t1 (s1 char,s2 char)");
if(res.error()) {
	throw ("Create table t1 failed");
}
res = conn.query("create table t2 (s1 int)");
if(res.error()) {
	throw ("Create table t2 failed");
}
res = conn.query("create view v1 as select s2,sum(s1) - \
	count(s2) as vx from t1 group by s2 having sum(s1) - count(s2) < (select f1() \
	from t2);");
if(res.error()) {
	throw ("Drop view failed");
}

fieldlist = res.listFields("v1");
if (!fieldlist.numFields() == 0) {
	throw ("Expected numFields from listFields > 0.  Returned :" + fieldlist.numFields());
}
  
 // Clean up functions, tables and views
res = conn.query("DROP FUNCTION f1");
if(res.error()) {
	throw ("Drop function failed : " + conn.error());
}
res = conn.query("DROP VIEW v1");
if(res.error()) {
	throw ("Drop view failed : " + conn.error());
}
res = conn.query("DROP TABLE t1, t2");
if(res.error()) {
	throw ("Drop table t1,t2 failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone



