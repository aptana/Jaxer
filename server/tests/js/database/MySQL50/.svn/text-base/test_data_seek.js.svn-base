/**
   File Name:          test_data_seek.js
   Description:

	  Test a crash when invalid/corrupted .frm is used in the SHOW TABLE STATUS, leveraging data_seek 
	  to process the results.

   Author:             Jane Tudor
   Date:               22 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_data_seek";

var conn, res, field;
var result, row, rc;

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
res = conn.query("DROP TABLE IF EXISTS test_data_seek");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table
res = conn.query("create table test_data_seek(c1 int primary key auto_increment, c2 char(10), c3 timestamp(14))");
if(res.error()) {
	throw ("Create table failed");
}

// Insert some records
res = conn.query("insert into test_data_seek(c2) values('venu'), ('mysql'), ('open'), ('source')");
if(res.error()) {
	throw ("Insert failed");
}

// Read back the data
res = conn.query("select * from test_data_seek");
if(res.error()) {
	throw ("Select * failed");
}

rc = res.dataSeek(3);
if (!rc) {
	throw ("dataSeek()failed");
}

// Validate that Row 3 data has indeed been returned here
while(row = res.fetchRow()) 
{ 
	if (!row.c2 == "open"){
		throw ("dataSeek(3) did not point us to row three data.  Expected 'open'.  Recieved: " + row.c2);
	}    
}

 // Drop table again if necessary
res = conn.query("DROP TABLE IF EXISTS test_data_seek");
if(res.error()) {
	throw ("Drop existing table failed");
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

