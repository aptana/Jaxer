/**
   File Name:          test_fetch_seek.js
   Description:

	Add description here ...

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_fetch_seek";

var conn, res, field_count, field;

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

// Drop t1 if it exists

res = conn.query("DROP TABLE IF EXISTS t1");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("CREATE TABLE t1 (i INT)");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("INSERT INTO t1 VALUES (1)");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("select MIN(i) AS A1, MAX(i) AS A2, COUNT(i) AS A3 from t1 GROUP BY i");

if(res.error()) {
	throw ("Select * failed");
}

field_count = res.fieldCount();


for (i= 0; i < field_count; ++i)
{
    field= res.fetchFieldDirect(i);
	if (!field.name() == ("A" + i)) {
		throw ("Expected field.name() = A1|A2|A3. Received: " + field.name());
	}
}

// Drop table t1
res = conn.query("drop table t1");
if(res.error()) {
	throw ("Drop table after create and insert failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

