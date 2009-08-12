/**
   File Name:          test_prepare_alter.js
   Description:

	Test alter table scenario in the middle of a prepare.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_alter";

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
res = conn.query("DROP TABLE IF EXISTS test_prep_alter");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prep_alter(id int, name char(20))");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table.
stmt = conn.query("INSERT INTO test_prep_alter values(10, 'venu'), (20, 'mysql')");
if(!stmt) {
	throw ("Insert statement failed.  Error: " + res.error());
}

// Insert another row, via a prepared statement this time
stmt = conn.prepare("INSERT INTO test_prep_alter VALUES(?, 'monty')");
if(!stmt) {
	throw ("Update prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 1){
	throw ("Param count incorrect. Expected 1.  Received: " + pcnt);
}

// Create the bind variables. 
rc = stmt.bindInput(1, "var1", stmt.BIND_INTEGER);
if (rc) {
	throw ("Bind input failed.  Error: " + rc);
}

var var1 = 30;

// Execute the prepared statement
rc = stmt.execute();
 if (rc){
	throw ("Statement execute failed. Error: " + rc);
}

 // Insert some values into table.
stmt = conn.query("ALTER TABLE test_prep_alter change id id_new varchar(20)");
if(!stmt) {
	throw ("Alter statement failed.  Error: " + res.error());
}

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

// Now select * from table to validate results
stmt = conn.prepare("SELECT * FROM test_prep_alter");
if(!stmt) {
	throw ("Select * prepare statement failed.  Error: " + res.error());
}

stmt.close();

// Commit the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
