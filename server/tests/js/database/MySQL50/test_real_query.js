/**
   File Name:          test_real_query.js
   Description:

	Add description here ...

   Author:             Jane Tudor
   Date:               16 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_real_query";

var conn, res;
var rc, i;
var query= "CALL p1()";

// Database connection variables
var dbServer = "curt_9100";
var dbDatabase = "testdb";
var dbUser = "jane";
var dbPass = "jaxer";
var dbPort = 0;

startTest();  // Leave this alone

// Prepare for database connection
conn = new MySQL50();

 // Establish connection to database server using CLIENT_MULTI_* statements
// res = conn.connect(dbServer, dbUser, dbPass, dbDatabase, CLIENT_MULTI_STATEMENTS|CLIENT_MULTI_RESULTS);
res = conn.connect(dbServer, dbUser, dbPass, dbDatabase);
if(conn.error()) {
	throw ("Unable to connect using CLIENT_MULTI_STATEMENTS|CLIENT_MULTI_RESULTS");
}

// Drop procedure p1 if it exists.
res = conn.query("drop procedure if exists p1");

if(res.error()) {
	throw ("Drop procedure if exists failed: " + conn.error());
}

res = conn.query("create procedure p1() select 1");
if(res.error()) {
	throw ("Create procedure failed: " + conn.error());
}

// Execute a mysql_real_query()
res = conn.query(query);
if(res.error()) {
	throw ("Insert failed for query = " + query + " with rc =" ) + conn.error();
}

res = conn.close();

// Execute a mysql_real_query for a second time
res = conn.query(query);
if(res.error()) {
	throw ("Insert failed");
}

if (res && conn.error() == CR_COMMANDS_OUT_OF_SYNC) {
	throw ("Got error as expected");
}

// Expect a valid result set here
res = conn.nextResult();
if (res.error()) {
	throw ("Expected mysql_next_result to return 0 here");
	}
	
res = conn.close();

// Do not expect valid result set here
res = conn.nextResult();
if (!res.error()) {
	throw ("Expected mysql_next_result to return -1 here");
	}

/* The second problem is not reproducible: add the test case */
  for (i = 0; i < 100; i++)
  {
    if (conn.realQuery("query, strlen(query)"))
    {
      throw ("\n i=" + i + query + "failed:" + conn.error());
      break;
    }
  }
res = conn.query("drop procedure p1");

if(res.error()) {
	throw ("Drop procedure failed: " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


