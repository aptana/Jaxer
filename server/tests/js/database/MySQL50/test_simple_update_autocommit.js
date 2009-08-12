/**
   File Name:          test_simple_update.js
   Description:

	...

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_simple_update_autocommit";

var conn, res;
var affectedRows;

// Database connection variables
var dbServer = "curt_9100";
var dbDatabase = "testdb";
var dbUser = "jane";
var dbPass = "jaxer";
var dbPort = 0;


startTest();  // Leave this alone
writeHeaderToLog( SECTION + " "+ TITLE);

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
res = conn.query("DROP TABLE IF EXISTS test_update");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to manipulate
res = conn.query("CREATE TABLE test_update(col1 int, col2 varchar(50), col3 int )");
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}

 // Insert some random values into table
res = conn.query("INSERT INTO test_update VALUES(1, 'MySQL', 100)");
if(res.error()) {
	throw ("Insert failed: " + conn.error());
}

// Verify the results 
affected_rows= res.rowCount();
if (!affected_rows == 1){
   writeHeaderToLog( "\n total affected rows: `%ld` (expected: `%ld`)",
          affected_rows, 1);
  throw ("Number of affected rows is incorrect: ", conn.error());
}

 // Update some data in this table
res = conn.query("UPDATE test_update SET col2='OurSQL' where col1=1");
if(res.error()) {
	throw ("Update failed : " + conn.error());
}

// Drop table
res = conn.query("drop table test_update;");
if(res.error()) {
	throw ("Drop table after simple update failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


