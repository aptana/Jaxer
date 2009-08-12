/**
   File Name:          test_prepare.js
   Description:

	Test a simple prepare, using bound variables.

   Author:             Jane Tudor
   Date:               06 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare";

var conn, res, stmt;
var iquery, squery, dquery, squery, pcnt;
var ivar1, ivar2, ivar3, ivar4, ivar5, ivar6, ivar7;

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
res = conn.query("DROP TABLE IF EXISTS test_prepare");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare(col1 tinyint, col2 varchar(15), \
			 col3 int, col4 smallint, col5 bigint, col6 float, col7 double )");
if(res.error()) {
	throw ("Create table failed");
}

 // Insert some values into table, using a simple prepared statement
iquery = "INSERT INTO test_prepare VALUES(?, ?, ?, ?, ?, ?, ?)";
stmt = conn.prepare(iquery);
if(!stmt) {
	throw ("Insert prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 7){
	throw ("Param count incorrect. Expected 7.  Received: " + pcnt);
}

// Initialize the bind variables.
ivar1 = 0;
ivar2 = "string"
ivar3 = 150;
ivar4 = 1000;
ivar5 = 2;
ivar6 = 650;
ivar7 = 2500;

//try 
//{
// Create the bind variables. Using BIND_INTEGER and BIND_STRING for simplicity until full support available - TBD.
rc = stmt.bindInput(1, "ivar1", stmt.BIND_INTEGER);
rc = stmt.bindInput(2, "ivar2", stmt.BIND_STRING);
rc = stmt.bindInput(3, "ivar3", stmt.BIND_INTEGER);
rc = stmt.bindInput(4, "ivar4", stmt.BIND_INTEGER);
rc = stmt.bindInput(5, "ivar5", stmt.BIND_INTEGER);
rc = stmt.bindInput(6, "ivar6", stmt.BIND_INTEGER);
rc = stmt.bindInput(7, "ivar7", stmt.BIND_INTEGER);

// Now, execute the prepared statement to insert 10 records ... 
  for (i= 0; i < 10; i++)
  {
  	rc = stmt.execute();
	print("execute of insert returned " + rc);
	print("stmt.error returned: " + stmt.error());

    ivar1 += 2;
    ivar2 = ivar2 + i;
    ivar3 += 150;
    ivar4 += 1500;
    ivar5 += 10;
	ivar6 += 50;
	ivar7 += 100;
  }

// Close open statement.
stmt.close();

// Prepare to fetch the results
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}
  
// Now let's fetch the results
squery = "SELECT * FROM test_prepare";
stmt = conn.prepare(squery);
if(!stmt) {
	throw ("Select * prepare statement failed.  Error: " + stmt.error());
}

//// Grab the results
//res = stmt.bindResult();
// if (res){
//	throw ("Bind results failed. Error: " + res);
//}

// Execute the statement
	rc = stmt.execute();
//}
//catch (err)
//{
//	throw ("Error: " + stmt.error());
//}

ovar1 = 320;
ovar2 = "string"
ovar3 = 150;
ovar4 = 1000;
ovar5 = 2;
ovar6 = 650;
ovar7 = 2500;

// Create the bind variables. Using BIND_INTEGER and BIND_STRING for simplicity until full support available - TBD.
var arr = new Array();
arr.push("ovar1");
arr.push("ovar2");
arr.push("ovar3");
arr.push("ovar4");
arr.push("ovar5");
arr.push("ovar6");
arr.push("ovar7");
rc = stmt.bindOutput(arr);

// reset these to what they are at the beginning of the record set
ivar1 = 0;
ivar2 = "string"
ivar3 = 150;
ivar4 = 1000;
ivar5 = 2;
ivar6 = 650;
ivar7 = 2500;

// Now, fetch the statements and compare the results row by row
  for (i= 0; i < 10; i++)
  {
    rc = stmt.fetch();
	if (!rc){
		throw ("Statement fetch failed. Error: " + rc);
	}

	if (ivar1 != ovar1) {
		print("ivar1: " + ivar1 + "  ovar1: " + ovar1);
		throw ("ivar1 and ovar1 are not equal.");
	}

	if (ivar2 != ovar2) {
		throw ("ivar2 and ovar2 are not equal.");
	}

	if (ivar3 != ovar3) {
		throw ("ivar3 and ovar3 are not equal.");
	}

	if (ivar4 != ovar4) {
		throw ("ivar4 and ovar4 are not equal.");
	}
	
	if (ivar5 != ovar5) {
		print("ivar5: " + ivar5 + "  ovar5: " + ovar5);
		throw ("ivar5 and ovar5 are not equal.");
	}
	
	if (ivar6 != ovar6) {
		print("ivar6: " + ivar6 + "  ovar6: " + ovar6);
		throw ("ivar6 and ovar6 are not equal.");
	}
	
	if (ivar7 != ovar7) {
		print("ivar7: " + ivar7 + "  ovar7: " + ovar7);
		throw ("ivar7 and ovar7 are not equal.");
	}
	

    ivar1 += 2;
    ivar2 = ivar2 + i;
    ivar3 += 150;
    ivar4 += 1500;
    ivar5 += 10;
	ivar6 += 50;
	ivar7 += 100;
  }

rc = stmt.fetch();
if (rc) {
	throw ("Statement fetch succeeded but should have failed.");
}

// Close the open statement
stmt.close();
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

