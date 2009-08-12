/**
   File Name:          test_change_user.js
   Description:

	Test that once a valid server connection is established, that a mysql_change_user with a valid
	username/password with appropriate permissions can continue with productive work.

   Author:             Jane Tudor
   Date:               25 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_change_user";

var conn, res, rowcnt;
var newuser = "curt";;

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
res = conn.connect(dbServer, dbUser, dbPass);

// Enable autocommit
res = conn.autocommit(1);
if(conn.error()) {
	throw ("Autocommit failed: " + conn.error());
}

// Drop database if it exists

res = conn.query("DROP DATABASE IF EXISTS test_drop_temp_db");

if(res.error()) {
	throw ("Drop existing database failed");
}

// Create database
res = conn.query("CREATE DATABASE test_drop_temp_db");
if(conn.error()) {
	throw ("Create database failed.  Error : " + conn.error());
}

res = conn.query("CREATE TABLE test_drop_temp_db.t1(c1 int, c2 char(1))");
if(res.error()) {
	throw ("Create table failed.  Error : " + conn.error());
}

res = conn.query("INSERT INTO test_drop_temp_db.t1 VALUES(10, 'C')");
if(res.error()) {
	throw ("Insert failed.  Error : " + conn.error());
}

// Simple query to prove that we have a valid connection for current user

res = conn.query("Select * from test_drop_temp_db.t1");

if(res.error()) {
	throw ("Select with initial user failed. Error : " + conn.error());
}

// Change to a different user, same database
res = conn.changeUser(newuser, dbPass);
if (conn.error()) {
	throw ("Change user returned an error.  The error is: " + conn.error());
}

// Do a really simple query to validate that a query can be accomplished by this user - wiithout
//  running into DB permission issues
res = conn.query("Select * from test_drop_temp_db.t1");
if(res.error()) {
	throw ("Select with new user failed");
}

if (!res.rowCount == 1) {
	throw ("Expected a res.rowCount =1.  Received: " + res.rowCount());
}

// Disconnect from database server

res = conn.close(); 

test();  // Leave this alone
