/**
   File Name:          test_field_flags.js
   Description:

	Test field flag settings to make sure fetchField() is working properly.

   Author:             Jane Tudor
   Date:               21 Feb 2007
*/

//Define global variables
var SECTION = "MySQL";
var VERSION = "5.0";
var TITLE   = "test_field_flags";

var conn, res, field;
var i, fieldoff;

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
res = conn.query("DROP TABLE IF EXISTS test_field_flags");
if(res.error()) {
	throw ("Drop existing table failed");
}

 // Create table test_dateformat
res = conn.query("CREATE TABLE test_field_flags(id int NOT NULL AUTO_INCREMENT PRIMARY KEY, \
                                                        id1 int NOT NULL, \
                                                        id2 int UNIQUE, \
                                                        id3 int, \
                                                        id4 int NOT NULL, \
                                                        id5 int, \
                                                        KEY(id3, id4))");          
if(res.error()) {
	throw ("Create table failed: " + conn.error());
}

 // Select * from table
res = conn.query("SELECT * FROM test_field_flags");
if(res.error()) {
	throw ("Select * failed : " + conn.error());
}

fieldoff = res.fieldSeek(0);
if (!fieldoff == 0) {
	throw ("fieldSeek() failed to return field offset == 0. Field offset returned was:" + fieldoff);
}

for(i= 0; i < res.fieldCount(); i++)
  {
    field= res.fetchField();
    if (field)
    {
      if (field.flags() & field.NOT_NULL_FLAG)
        writeHeaderToLog("Field # = " + i + "with NOT_NULL_FLAG");
      if (field.flags() & field.PRI_KEY_FLAG)
        writeHeaderToLog("Field # = " + i + "with PRI_KEY_FLAG");
      if (field.flags() & field.UNIQUE_KEY_FLAG)
        writeHeaderToLog("Field # = " + i + "with UNIQUE_KEY_FLAG");
      if (field.flags() &  field.MULTIPLE_KEY_FLAG)
        writeHeaderToLog("Field # = " + i + "with MULTIPLE_KEY_FLAG");
      if (field.flags() &  field.AUTO_INCREMENT_FLAG)
        writeHeaderToLog("Field # = " + i + "with AUTO_INCREMENT_FLAG");
	}
  }

// Drop table
res = conn.query("drop table test_field_flags");
if(res.error()) {
	throw ("Drop table failed : " + conn.error());
}
	
// Disconnect from database server
res = conn.close(); 

test();  // Leave this alone


