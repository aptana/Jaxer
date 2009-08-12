function validateUsername(candidate)
{
	return /^[\w\-]+$/.test(candidate) ? "" : "username must be alphanumeric or -";
}
validateUsername.runat = "both";

function validatePassword(candidate)
{
	return /^[\w\-\$]{6,}$/.test(candidate) ? "" : "password must be 6 or more alphanumeric or - or $ characters";
}
validatePassword.runat = "both";

function saveCredentials(username, password)
{
	var error = "";
	var errors = [];
	error = validateUsername(username);
	if (error != "") errors.push(error);
	error = validatePassword(password);
	if (error != "") errors.push(error);
	if (errors.length > 0) return errors.join("; ");
	
	var rs;
	rs = Jaxer.DB.execute("SELECT * FROM users WHERE username = ?", [username]);
	if (rs.rows.length != 0)
	{
		var user = rs.rows[0];
		var previousPassword = user.password;
		if (previousPassword == password) // validate them
		{
			makeAuthenticated(user);
			return ""; 
		}
		return "username already in use";
	}
	
	var values = "('" + [username, password].join("', '") + "', NOW(), NOW())";
	try
	{
		var now = new Date();
		Jaxer.DB.execute("INSERT INTO users (username, password, created, last_login) VALUES (?, ?, ?, ?)",
			[username, password, now, now]);
		rs = Jaxer.DB.execute("SELECT * FROM users WHERE username = ?", [username]);
		if (rs.rows.length != 0)
		{
			var user = rs.rows[0];
			makeAuthenticated(user);
			return "";
		}
		else
		{
			Jaxer.Log.warn("Could not find the user '" + username + "' that was just created!");
			return "Could not find the account that should have just been created";
		}
	}
	catch (e)
	{
		Jaxer.Log.warn("User error when trying to save credentials: " + e);
		return "Could not create username/password";
	}
}
saveCredentials.proxy = true;
	
function checkCredentials(username, password)
{
	var rs = Jaxer.DB.execute("SELECT * FROM users WHERE username = ? AND password = ?",
		[username, password]);
	if (rs.rows.length == 0)
	{
		return "";
	}
	var user = rs.rows[0];
	makeAuthenticated(user);
	return user.username;
}
checkCredentials.proxy = true;

function removeCredentials()
{
	delete Jaxer.session.username;
}
removeCredentials.proxy = true;

// Clientside functions
function changeAuthentication(isAuthenticated)
{
	if (typeof window.onAuthenticateChange == "function") onAuthenticateChange(isAuthenticated);
}
changeAuthentication.runat = "client";

function validate(validationFunction, candidate)
{
	if (candidate == "") return; // Not done filling this out, perhaps
	var error = validationFunction(candidate);
	if (error != "")
	{
		alert(error);
	}
}
validate.runat = "client";

function login()
{
	var username = $('username').value;
	var password = $('password').value;
	var username = checkCredentials(username, password);
	if (username != "")
	{
		fromTemplate('loginComponent', 'loginAuthenticated');
		setTimeout("$('authenticatedUsername').innerHTML = '" + username + "'", 0);
		changeAuthentication(true);
	}
	else
	{
		$('loginMessage').innerHTML = "Sorry, try again";
	}
}
login.runat = "client";

function logout()
{
	removeCredentials();
	fromTemplate('loginComponent', 'loginRequest');
	setTimeout("$('loginMessage').innerHTML = 'successfully logged out'", 0);
	changeAuthentication(false);
}
logout.runat = "client";

function showCreateAccount()
{
	fromTemplate('loginComponent', 'loginCreate');
}
showCreateAccount.runat = "client";

function createAccount()
{
	var username = $('newUsername').value;
	var password = $('newPassword').value;
	var confirmPassword = $('confirmPassword').value;
	if (password != confirmPassword)
	{
		alert("Please enter the same password twice to proceed");
		return;
	}
	var error = saveCredentials(username, password);
	if (error == "")
	{
		fromTemplate('loginComponent', 'loginAuthenticated');
		setTimeout("$('authenticatedUsername').innerHTML = '" + username + "'", 0);
		changeAuthentication(true);
	}
	else
	{
		setTimeout("$('createAccountMessage').innerHTML = '" + error + "'", 0);
	}
}
createAccount.runat = "client";

function cancelCreateAccount()
{
	fromTemplate('loginComponent', 'loginRequest');
}
cancelCreateAccount.runat = "client";

function handleReturn(evt, handler)
{
	if (!evt) evt = window.event;
	if ((evt.which && evt.which == 13) || 
	    (evt.keyCode && evt.keyCode == 13))
	{
		handler();
		return false; 
	}
}
handleReturn.runat = "client";

