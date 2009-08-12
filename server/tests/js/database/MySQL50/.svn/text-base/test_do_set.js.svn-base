/**
   File Name:          test_do_set.js
   Description:

	  Test 'do @var' and 'set @var' queries.

   Author:             Jane Tudor
   Date:               22 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_do_set";

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
res = conn.query("DROP TABLE IF EXISTS test_do_set");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table
res = conn.query("create table test_do_set (a int)");
if(res.error()) {
	throw ("Create table failed");
}

for (i= 0; i < 3; i++)
  {
  	res = conn.query("do @var:=(1 in (select * from test_do_set))");
	if(res.error()) { 
	throw ("do @var failed"); 	
	}
  	res = conn.query("set @var=(1 in (select * from test_do_set))");
	if(res.error()) { 
	throw ("set @var failed"); 	
	}
}

 // Drop table again if necessary
res = conn.query("DROP TABLE IF EXISTS test_do_set");
if(res.error()) {
	throw ("Drop existing table failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


