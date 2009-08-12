/**
   File Name:          test_list_fields.js
   Description:

	Test listFields() and fetchFieldDirect() to ensure that they are passing back the correct
	data to the end user.

   Author:             Jane Tudor
   Date:               25 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_list_fields";

var conn, res;
var result, row, field, i, row_count;
var NULL = 0;

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
res = conn.query("DROP TABLE IF EXISTS t1");
if(res.error()) {
	throw ("Drop existing table failed");
}

// Execute the create table query
res = conn.query("create table t1(c1 int primary key auto_increment, c2 char(10) default 'mysql')");
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}

result = conn.listFields("t1", NULL);
if (!result) {
	throw ("conn.listFields failed.  Error: " + result.error());
}

// Initialize row count to zero.
row_count = 0;

// Process result set
while (row = result.fetchRow()) {
	result = row.fieldSeek(0);
	for(i= 0; i< row.numFields(); i++)
    {
      field = result.fetchField();
      if (!row[i])
	  	field.maxLength ("NULL");
      else if (IS_NUM(field.type()))
	   	field.maxLength (row[i]);
      else
  	   	field.maxLength (row[i]);
    }
	row_count++;		
}


// Verify the field members 
var field = result.fetchFieldDirect(0);
print("field = " + field);
if (!field) {
	throw ("result.fetchFieldDirect(0) failed.  Error: " + row.error());
}

if (field.name() != "c1") {
	throw ("Expected column name = c1.  Received" + field.name());
}

// Drop table
res = conn.query("drop table t1");
if(res.error()) {
	throw ("Drop table failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone





