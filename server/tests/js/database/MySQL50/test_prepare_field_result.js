/**
   File Name:          test_prepare_field_result.js
   Description:

	Test simple prepare field results.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_field_result";

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

 // Clean up if necessary
res = conn.query("DROP TABLE IF EXISTS test_prepare_field_result");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare_field_result(int_c int, " /
                         "var_c varchar(50), ts_c timestamp(14), " /
                         "char_c char(4), date_c date, extra tinyint)");
if(res.error()) {
	throw ("Create table failed");
}

 // Select a row from table, using a simple prepared statement
stmt = conn.prepare("SELECT int_c, var_c, date_c as date, ts_c, char_c FROM " /
                " test_prepare_field_result as t1 WHERE int_c=?");
if(!stmt) {
	throw ("Insert prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 1){
	throw ("Param count incorrect. Expected 1.  Received: " + pcnt);
}

//  result= mysql_stmt_result_metadata(stmt);
//  mytest(result);

/*
  verify_prepare_field(result, 0, "int_c", "int_c", MYSQL_TYPE_LONG,
                       "t1", "test_prepare_field_result", current_db, 11, 0);
  verify_prepare_field(result, 1, "var_c", "var_c", MYSQL_TYPE_VAR_STRING,
                       "t1", "test_prepare_field_result", current_db, 50, 0);
  verify_prepare_field(result, 2, "date", "date_c", MYSQL_TYPE_DATE,
                       "t1", "test_prepare_field_result", current_db, 10, 0);
  verify_prepare_field(result, 3, "ts_c", "ts_c", MYSQL_TYPE_TIMESTAMP,
                       "t1", "test_prepare_field_result", current_db, 19, 0);
  verify_prepare_field(result, 4, "char_c", "char_c",
                       (mysql_get_server_version(mysql) <= 50000 ?
                        MYSQL_TYPE_VAR_STRING : MYSQL_TYPE_STRING),
 */					   
					   
//  verify_field_count(result, 5);

//  mysql_free_result(result);

stmt.close();

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
