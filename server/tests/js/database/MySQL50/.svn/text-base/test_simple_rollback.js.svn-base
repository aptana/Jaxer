/**
   File Name:          test_simple_rollback.js
   Description:

	Simple test to validate that mysql_rollback functionality is working properly.

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_simple_update_commit";

var conn, res;
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

// Disable autocommit
res = conn.autocommit(0);

 // Clean up if necessary
res = conn.query("DROP TABLE IF EXISTS my_demo_transaction");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create a table of type BDB' or 'InnoDB'
res = conn.query("CREATE TABLE my_demo_transaction( col1 int , col2 varchar(30))");
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}


 // Insert initial values into table
res = conn.query("INSERT INTO my_demo_transaction VALUES(10, 'venu')");
if(res.error()) {
	throw ("Insert failed");
}

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

 // Now insert the second row, and roll back the transaction
res = conn.query("INSERT INTO my_demo_transaction VALUES(20, 'mysql')");
if(res.error()) {
	throw ("Insert failed");
}

// Roll back the second row insert
 res= conn.rollback();
 if(conn.error()) {
	throw ("Rollback failed :", conn.error());
}

 // Now delete the first row, and roll it back
res = conn.query("DELETE FROM my_demo_transaction WHERE col1= 10");
if(res.error()) {
	throw ("Insert failed");
}

// Roll back the first row deletion
 res= conn.rollback();
 if(conn.error()) {
	throw ("Rollback failed :", conn.error());
}

  /* Get and test the results - need API support */
// result= mysql_store_result(mysql);
// mytest(result);
  
// my_process_result_set(result);
// mysql_free_result(result);

  /* test the results now, only one row should exist */
//  res= mysql_query(mysql, "SELECT * FROM my_demo_transaction");
//  myquery(res);

  /* get the result */
//  result= mysql_use_result(mysql);
//  mytest(result);

//  row= mysql_fetch_row(result);
//  mytest(row);

//  row= mysql_fetch_row(result);
//  mytest_r(row);

// Enable autocommit
res = conn.autocommit(1);

test();  // Leave this alone

