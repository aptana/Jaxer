(function() {
	
if (Jaxer.application.isInitialized) return;

var sql;

sql = "CREATE TABLE IF NOT EXISTS users " +
	"( id INTEGER PRIMARY KEY AUTO_INCREMENT" +
	", username VARCHAR(255) NOT NULL" +
	", password VARCHAR(255) NOT NULL" +
	", created DATETIME NOT NULL" +
	", last_login DATETIME NOT NULL" +
	", UNIQUE (username)" +
	")";

Jaxer.DB.execute(sql);

sql = "CREATE TABLE IF NOT EXISTS messages " +
	"( id INTEGER PRIMARY KEY AUTO_INCREMENT" +
	", room_id INTEGER NOT NULL" +
	", sent DATETIME NOT NULL" +
	", user_id INTEGER NOT NULL" +
	", contents TEXT NOT NULL" +
	")";

Jaxer.DB.execute(sql);

Jaxer.application.isInitialized = true;
			
})();
