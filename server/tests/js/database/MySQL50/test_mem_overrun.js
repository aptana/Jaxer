/**
   File Name:          test_mem_overrun.js
   Description:

	Test for a memory overrun when a table has 1000 fields with
    a row of data.

   Author:             Jane Tudor
   Date:               21 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_mem_overrun";

var conn, res, field;
var query, i;

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
res = conn.query("DROP TABLE IF EXISTS t_mem_overun");
if(res.error()) {
	throw ("Drop existing table failed");
}

// Build the create table string creating 1000 columns
query = "create table t_mem_overun("
for (i= 0; i < 1000; i++)   {
	if (!i ==0) { query = query + ", "; }
	query = query + "c" + i + " int";
}
query = query + ");"
  
// Execute the create table query
res = conn.query(query);
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}

// Build the insert string creating 1000 columns
query = "insert into t_mem_overun values("
for (i= 0; i < 1000; i++)   {
	if (!i ==0) { query = query + ", "; }
	query = query + "1";
}
query = query + ");"
  
// Execute the insert query
res = conn.query(query);
if(res.error()) {
	throw ("Insert failed: " + conn.error());
}

 // Select * from this table
res = conn.query("select * from t_mem_overun");
if(res.error()) {
	throw ("Select * failed : " + conn.error());
}
 // Need to add logic to validate the results, with fetchRow()- see my_process_result_set

// Drop table
res = conn.query("drop table t_mem_overun");
if(res.error()) {
	throw ("Drop table failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


