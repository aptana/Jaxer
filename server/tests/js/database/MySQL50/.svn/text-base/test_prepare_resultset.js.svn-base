/**
   File Name:          test_prepare_resultset.js
   Description:

	Test simple prepare-resultset() info.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_resultset";

var conn, res, stmt;
var iquery, uquery, dquery, squery, pcnt;

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
res = conn.query("DROP TABLE IF EXISTS test_prepare_resultset");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare_resultset(id int, name varchar(50), extra double)");
if(res.error()) {
	throw ("Create table failed");
}

 // Select * from table, using a simple prepared statement
stmt = conn.prepare("SELECT * FROM test_prepare_resultset");
if(!stmt) {
	throw ("Select * statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 0){
	throw ("Param count incorrect. Expected 0.  Received: " + pcnt);
}

// Validate the results - TBD
//  result= mysql_stmt_result_metadata(stmt);
//  mytest(result);
//  my_print_result_metadata(result);
//  mysql_free_result(result);
  
// Close open statement.
stmt.close();

// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

