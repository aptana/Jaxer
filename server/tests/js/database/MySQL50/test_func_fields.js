/**
   File Name:          test_func_fields.js
   Description:

	Test FUNCTION field info / DATE_FORMAT() table_name .

   Author:             Jane Tudor
   Date:               21 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_func_fields";

var conn, res, field;
var affectedRows;

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

// Enable autocommit
res = conn.autocommit(1);
if(conn.error()) {
	throw ("Autocommit failed: " + conn.error());
}

 // Clean up if necessary
res = conn.query("DROP TABLE IF EXISTS test_dateformat");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table test_dateformat
res = conn.query("CREATE TABLE test_dateformat(id int,ts timestamp)");            
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}

 // Insert values into table
res = conn.query("INSERT INTO test_dateformat(id) values(10)");
if(res.error()) {
	throw ("Insert failed: " + conn.error());
}

 // Read the ts column from this table
res = conn.query("SELECT ts FROM test_dateformat");
if(res.error()) {
	throw ("Select ts failed : " + conn.error());
}

field = res.fetchField();
if (!field.table() == "test_dateformat") {
	throw ("Expected a table name of 'test_dateformat'. Table name returned was:" + field.table());
}

field = res.fetchField();
if (field) {
	throw ("Second fetch should have failed with 'no more fields' indicator.  Field object returned:" ) + field;
}
  
 // Verify DATE_FORMAT
res = conn.query("SELECT DATE_FORMAT(ts, '%Y') AS 'venu' FROM test_dateformat");
if(res.error()) {
	throw ("Select DATE_FORMAT failed : " + conn.error());
}

field = res.fetchField();
if (!field.table() == "") {
	throw ("fetchField() should have returned an empty field.table() value.  Table name returned:" ) + field.table();
}

field = res.fetchField();
if (field) {
	throw ("Second fetch should have failed with 'no more fields' indicator.  Field object returned:" ) + field;
}

 // Verify FIELD ALIAS
res = conn.query("SELECT DATE_FORMAT(ts, '%Y')  AS 'YEAR' FROM test_dateformat");
if(res.error()) {
	throw ("Select DATE_FORMAT as 'YEAR' failed : " + conn.error());
}

field = res.fetchField();
if (!field.name() == "YEAR") {
	throw ("field.name expected to be 'YEAR'.  field.name returned was :" ) + field.name();
}
if (!field.orgName() == "") {
	throw ("field.orgName expeced to be 'NULL'.  orgName returned:" ) + field.orgName();
}

field = res.fetchField();
if (field) {
	throw ("Second fetch should have failed with 'no more fields' indicator.  Field object returned:" ) + field;
}

// Drop table
res = conn.query("drop table test_dateformat;");
if(res.error()) {
	throw ("Drop table after simple update failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


