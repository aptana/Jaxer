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
var query = "select t.id, p1.value, n1.value, p2.value, n2.value from t3 t LEFT JOIN t1 p1 ON (p1.id=t.param1_id) LEFT JOIN t2 p2 ON (p2.id=t.param2_id) LEFT JOIN t4 n1 ON (n1.id=p1.name_id) LEFT JOIN t4 n2 ON (n2.id=p2.name_id) where t.id=1";

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

res = conn.query("DROP TABLE IF EXISTS t1, t2, t3, t4");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("create table t3 (id int(8), param1_id int(8), param2_id int(8)) TYPE=InnoDB DEFAULT CHARSET=utf8");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("create table t1 ( id int(8), name_id int(8), value varchar(10)) TYPE=InnoDB DEFAULT CHARSET=utf8");
if(res.error()) {
	throw ("Create table failed");
}
res = conn.query("create table t2 (id int(8), name_id int(8), value varchar(10)) TYPE=InnoDB DEFAULT CHARSET=utf8");
if(res.error()) {
	throw ("Create table failed");
}
res = conn.query("create table t4(id int(8), value varchar(10)) TYPE=InnoDB DEFAULT CHARSET=utf8");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("insert into t3 values (1, 1, 1), (2, 2, null)");
if(res.error()) {
	throw ("Insert into table t3 failed");
}
res = conn.query("insert into t1 values (1, 1, 'aaa'), (2, null, 'bbb')");
if(res.error()) {
	throw ("Insert into table t1 failed");
}
res = conn.query("insert into t2 values (1, 2, 'ccc')");
if(res.error()) {
	throw ("Insert into table t2 failed");
}
res = conn.query("insert into t4 values (1, 'Name1'), (2, null)");
if(res.error()) {
	throw ("Insert into table t4 failed");
}

for (i= 0; i < 3; i++)
{
	res = conn.query(query);
	if(res.error()) {
	throw ("Query 'Select' in loop failed");
	}
}
  
// Drop table t1
res = conn.query("DROP TABLE t1, t2, t3, t4");
if(res.error()) {
	throw ("Drop table(s) failed.  Error: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone



