/**
   File Name:          test_escape_string.js
   Description:

	 Test mysql_real_escape_string() with gbk charset
	
	 The important part is that 0x27 (') is the second-byte in a invalid
	 two-byte GBK character here. But 0xbf5c is a valid GBK character, so
	 it needs to be escaped as 0x5cbf27

   Author:             Jane Tudor
   Date:               26 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_escape_string";

var conn, res, rowcnt;
var out, len, sQuery;
var DATA_IN =  "\xef\xbb\xbf\x27\xbf\x10";
var DATA_OUT = "\xef\xbb\xbf\x5c\x27\xbf\x10";

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
res = conn.optionString(conn.SET_CHARSET_NAME, "gbk");
res = conn.connect(dbServer, dbUser, dbPass, dbDatabase);

out = conn.escape(DATA_IN);

// No escaping should have actually happened.  Verify that this is the case.
if (!(out == DATA_OUT)) {
	throw ("Expected DATA_OUT =" + DATA_OUT + "; received out = " + out);
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone





