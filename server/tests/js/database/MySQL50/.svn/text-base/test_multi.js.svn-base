/**
   File Name:          test_multi.js
   Description:

	  Test ...

   Author:             Jane Tudor
   Date:               22 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_multi";

var conn, res, field, param, rowcnt;


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
res = conn.query("DROP TABLE IF EXISTS t1, t2");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table t1
res = conn.query("create table t1 (a int, b int)");
if(res.error()) {
	throw ("Create table failed");
}

 // Create table t2
res = conn.query("create table t2 (a int, b int)");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert into t1
res = conn.query("insert into t1 values (3, 3), (2, 2), (1, 1)");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert into t2
res = conn.query("insert into t2 values (3, 3), (2, 2), (1, 1)");
if(res.error()) {
	throw ("Create table failed");
}

param = 1;
rowsleft = 3;
for(i= 0; i < 3; i++)
{
	res = conn.query("update t1, t2 set t1.b=10, t2.b=10 where t1.a=t2.a and t1.b=" + param);
	if(res.error()) {
		throw ("Update failed");
	}

	res = conn.query("delete t1, t2 from t1, t2 where t1.a=t2.a and t1.b=10");
	if(res.error()) {
		throw ("Delete failed");
	}

	// Process select * from t1
	res = conn.query("select * from t1");
	if(res.error()) {
		throw ("Select t1 failed");
	}
	rowcnt = res.rowCount();

	if (!rowcnt == (3 - param)) {
		throw ("Row count incorrect.  Expected 3-param.  Param = " + param + "; Rowcnt = " + rowcnt);
	}	
	// my_process_stmt_result();
	
	// Process select * from t2
	res = conn.query("select * from t2");
	if(res.error()) {
		throw ("Select t2 failed");
	}
	// my_process_stmt_result();
	param ++;
}

 // Drop table again if necessary
res = conn.query("DROP TABLE t1, t2");
if(res.error()) {
	throw ("Drop existing table failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

