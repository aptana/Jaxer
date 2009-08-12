/**
   File Name:          test_prepare_null.js
   Description:

	Test simple prepares using NULLS.

   Author:             Jane Tudor
   Date:               06 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_null";

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
res = conn.query("DROP TABLE IF EXISTS test_null");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_null(col1 int, col2 varchar(50))");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert by prepare, using wrong column name
iquery = "INSERT INTO test_null(col3, col2) VALUES(?, ?)";
stmt = conn.prepare(iquery);
if(stmt) {
	throw ("Expected insert by prepare with wrong column name to fail.");
}

 // Insert by prepare, using wrong column name
iquery = "INSERT INTO test_null(col1, col2) VALUES(?, ?)";
stmt = conn.prepare(iquery);
if(!stmt) {
	throw ("Expected insert by prepare with correct column name to succeed.");
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 2){
	throw ("Param count incorrect. Expected 2.  Received: " + pcnt);
}

// Create the bind variables. Using BIND_INTEGER and BIND_STRING for simplicity until full support available - TBD.
rc = stmt.bindInput(1, "var1", stmt.BIND_INTEGER);
rc = stmt.bindInput(2, "var2", stmt.BIND_STRING);
if (rc) {
	throw ("Bind input failed.  Error: " + rc);
}

var var1 = 10;
var var2 = "mystring";

// Now, execute the prepared statement to insert 10 records ... 
  for (i= 0; i < 10; i++)
  {
  	rc = stmt.execute();
 	if (rc){
		throw ("Statement execute failed. Error: " + rc);
	}
  }
 
// re-bind with MYSQL_TYPE_NULL
rc = stmt.bindInput(1, "var1", stmt.MYSQL_TYPE_NULL);
rc = stmt.bindInput(2, "var2", stmt.MYSQL_TYPE_NULL);
if (rc) {
	throw ("Bind input failed.  Error: " + rc);
} 	

// Now, execute the prepared statement to insert 10 additional records ... 
  for (i= 0; i < 10; i++)
  {
  	rc = stmt.execute();
 	if (rc){
		throw ("Statement execute failed. Error: " + rc);
	}
  }
  
stmt.close();

// Now fetch the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

 // Insert by prepare, using wrong column name
squery = "SELECT * FROM test_null";
stmt = conn.prepare(squery);
if(!stmt) {
	throw ("Expected SELECT * by prepare to succeed.");
}

rc = stmt.execute();
if (rc){
	throw ("Statement execute failed. Error: " + rc);
}

// Grab the results
res = stmt.bindResult();
 if (res){
	throw ("Bind results failed. Error: " + res);
}

// Execute the statement
rc = stmt.execute();
if (rc){
	throw ("Statement execute failed. Error: " + rc)
}

rc = 0;
while (stmt.fetch() != MYSQL_NO_DATA) {
	rc ++;
}

if (! rc == 20 ) {
	throw ("Expected 20 rows in result set.  Received: " + rc);
}
 
// Close open statement
stmt.close();
  	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone






