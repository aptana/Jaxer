/**
   File Name:          test_prepare_ext.js
   Description:

	Test simple prepare with all possible types.

   Author:             Jane Tudor
   Date:               17 March 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_prepare_ext";

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
res = conn.query("DROP TABLE IF EXISTS test_prepare_ext");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table to work with
res = conn.query("CREATE TABLE test_prepare_ext" /
               "(" /
               " c1  tinyint," /
               " c2  smallint," /
               " c3  mediumint," /
               " c4  int," /
               " c5  integer," /
               " c6  bigint," /
               " c7  float," /
               " c8  double," /
               " c9  double precision," /
               " c10 real," / 
               " c11 decimal(7, 4)," /
               " c12 numeric(8, 4)," /
               " c13 date," /
               " c14 datetime," /
               " c15 timestamp(14)," /
               " c16 time," /
               " c17 year," /
               " c18 bit," /
               " c19 bool," /
               " c20 char," /
               " c21 char(10)," /
               " c22 varchar(30)," /
               " c23 tinyblob," /
               " c24 tinytext," /
               " c25 blob," /
               " c26 text," /
               " c27 mediumblob," /
               " c28 mediumtext," /
               " c29 longblob," /
               " c30 longtext," /
               " c31 enum('one', 'two', 'three')," /
               " c32 set('monday', 'tuesday', 'wednesday')");
			   
if(res.error()) {
	throw ("Create table failed");
}

 // Prepare to insert some values into table, using all variable types
stmt = conn.prepare("INSERT INTO test_prepare_ext(c1, c2, c3, c4, c5, c6) VALUES(?, ?, ?, ?, ?, ?)");
if(!stmt) {
	throw ("Insert prepare statement failed.  Error: " + res.error());
}

// Verify that the param count is as expected.
pcnt = stmt.paramCount();
 if (!pcnt == 6){
	throw ("Param count incorrect. Expected 6.  Received: " + pcnt);
}

// Create the bind variables. 
rc = stmt.bindInput(1, "var1", stmt.BIND_TINY);
rc = stmt.bindInput(2, "var2", stmt.BIND_SHORT);
rc = stmt.bindInput(2, "var3", stmt.BIND_LONG);
rc = stmt.bindInput(2, "var4", stmt.BIND_LONG);
rc = stmt.bindInput(2, "var5", stmt.BIND_LONG);
rc = stmt.bindInput(2, "var6", stmt.BIND_LONGLONG);
if (rc) {
	throw ("Bind input failed.  Error: " + rc);
}

var var1 = tdata;
var var2 = sdata;
var var3 = ndata;
var var4 = ndata;
var var5 = ndata;
var var6 = bdata;

// Integer to integer 
for (nData= 0; nData<10; nData++, tData++, sData++, bData++)
{
	// Execute the last prepared statement
	rc = stmt.execute();
 	if (rc){
		throw ("Statement execute failed. Error: " + rc);
	}
}

// Now, fetch the results ....
stmt = conn.prepare("SELECT c1, c2, c3, c4, c5, c6 FROM test_prepare_ext");
if(!stmt) {
	throw ("Select prepare statement failed.  Error: " + res.error());
}

rc = stmt.execute();
if (rc){
	throw ("Statement execute failed. Error: " + rc);
}

// Check the results, as in check_stmt(stmt); Expected nData = rc	--- TBD

   
// Close open statement.
stmt.close();

// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone
