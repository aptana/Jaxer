function getAuthenticatedUser()
{
	var username = Jaxer.session.username;
	if (typeof username == "undefined") return null;
	var rs = Jaxer.DB.execute("SELECT * FROM users WHERE username = ?", [username]);
	if (rs.rows.length == 0)
	{
		return null;
	}
	return rs.rows[0];
}

function makeAuthenticated(user)
{
	Jaxer.session.username = user.username;
}

function initAuthentication()
{
	try
	{
		var user = getAuthenticatedUser();
		if (user)
		{
			fromTemplate('loginComponent', 'loginAuthenticated');
			$('authenticatedUsername').innerHTML = user.username;
			Jaxer.clientData.isAuthenticated = true;
		}
		else
		{
			fromTemplate('loginComponent', 'loginRequest');
			Jaxer.clientData.isAuthenticated = false;
		}
	}
	catch (e)
	{
		Jaxer.Log.error("Error trying to get authenticated user: " + e);
	}
}

