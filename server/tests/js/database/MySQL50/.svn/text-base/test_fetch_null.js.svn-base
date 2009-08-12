/**
   File Name:          test_fetch_null.js
   Description:

	Add description here ...

   Author:             Jane Tudor
   Date:               21 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_fetch_null";

var conn, res;
var row, rc, field, fieldoff;


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

// Drop table if exists

res = conn.query("DROP TABLE IF EXISTS test_fetch_null");

if(res.error()) {
	throw ("Drop existing table failed");
}

res = conn.query("CREATE TABLE test_fetch_null(col1 tinyint, \
                          col2 smallint,  \
                          col3 int, col4 bigint,  \
                          col5 float, col6 double, \
                          col7 date, col8 time, \
                          col9 varbinary(10), \
                          col10 varchar(50), \
                          col11 char(20))");
if(res.error()) {
	throw ("Create table failed");
}

res = conn.query("INSERT INTO test_fetch_null (col11) VALUES (1000), (88), (389789)");
if(res.error()) {
	throw ("Insert failed");
}

// Commit the insert
res = conn.commit();
if(conn.error()) {
	throw ("Commit failed");
}

res = conn.query("SELECT * FROM test_fetch_null");
if(res.error()) {
	throw ("Select * failed");
}

// Set non-Null row count = 0 and fetch first row of data
rc = 0;
row = res.fetchRow();

while (row)
  {
	fieldoff = res.fieldSeek(0);
	field= res.fetchField();
    for (i= 0; i < 10; i++)
    {
	    if (field.flags() & !field.NOT_NULL_FLAG) {
        	 throw ("Field # = " + i + "has NOT_NULL_FLAG on");
		}
	}

	if (!(row.col11 == 1000 || row.col11 == 88 || row.col11 == 389789)) {
		throw ("Col11 should only have 1000, 88 or 389789.  Col11 has: " + row.col11());
		}
		
	rc++;	
	
	// Fetch next row until no more rows
	row = res.fetchRow();
  }

if (!rc == 3) {
	throw ("Invalid count of non-NULL rows in this table");
	}

// Drop table 
res = conn.query("drop table test_fetch_null");
if(res.error()) {
	throw ("Drop table failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone

