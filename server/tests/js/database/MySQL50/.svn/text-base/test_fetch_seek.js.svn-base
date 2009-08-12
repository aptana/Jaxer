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

var conn, res;
var row;
var stmt = "select * from t1";

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

writeHeaderToLog ("Host Info: " + conn.hostInfo() + "and Server Version: " + conn.serverVersion());

res = conn.query("create table t1(c1 int primary key auto_increment, c2 char(10), c3 timestamp(14))");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("insert into t1(c2) values('venu'), ('mysql'), ('open'), ('source')");
if(res.error()) {
	throw ("Insert failed");
}

res = conn.query("select * from t1");
if(res.error()) {
	throw ("Select * failed");
}

// Seek to the first row 
row = res.currentRow();
writeHeaderToLog( "res.currentRow() returned: " + row);
row = res.rowSeek(row);
writeHeaderToLog( "rowSeek() returned: " + row);

// Validate that Row 3 data has indeed been returned here
while(row = res.fetchRow()) 
{ 
	if (!row.c2 == "venu"){
		throw ("rowSeek(1) did not point us to row three data.  Expected 'venu'.  Recieved: " + row.c2);
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

