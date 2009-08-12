/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 3
 *
 * This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 * This program is licensed under the GNU General Public license, version 3 (GPL).
 *
 * This program is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 * is prohibited.
 *
 * You can redistribute and/or modify this program under the terms of the GPL, 
 * as published by the Free Software Foundation.  You should
 * have received a copy of the GNU General Public License, Version 3 along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * Aptana provides a special exception to allow redistribution of this file
 * with certain other code and certain additional terms
 * pursuant to Section 7 of the GPL. You may view the exception and these
 * terms on the web at http://www.aptana.com/legal/gpl/.
 * 
 * You may view the GPL, and Aptana's exception and additional terms in the file
 * titled license-jaxer.html in the main distribution folder of this program.
 * 
 * Any modifications to this file must keep this entire header intact.
 *
 * ***** END LICENSE BLOCK ***** */

(function(){

var log = Log.forModule("SMTP");

/**
 * @namespace {Jaxer.SMTP} A namespace object holding functions and members used
 * for sending emails via SMTP (Simple Mail Transfer Protocol).
 */
var SMTP = {};

/*
 * The default port used to connect to an SMTP server. Initially set to 25.
 * 
 * @alias Jaxer.SMTP.DEFAULT_PORT
 * @property {Number}
 */
SMTP.DEFAULT_PORT = 25;

/**
 * Sends an email via SMTP
 * 
 * @alias Jaxer.SMTP.sendEmail
 * @param {String} mailhost
 * 		The host to connect to
 * @param {Number} mailport
 * 		The port to connect on
 * @param {String} from
 * 		The address this message is coming from
 * @param {String} to
 * 		The address this message is going to
 * @param {String} subject
 * 		The subject of the message
 * @param {String} msg
 * 		The body of the message
 * @param {Array} headers
 * 		Array of extra msg headers, [['header1','value1'],['header2','value2'], ... ]
 */
SMTP.sendEmail = function sendEmail(mailhost, mailport, from, to, subject, msg, headers)
{
	var s = new Jaxer.Socket();
	s.open(mailhost, mailport);
	log.trace('connecting to ' + mailhost + " on port " + mailport);
	
	// Read in initial welcome string
	res = s.readLine();
	log.trace('<' + res);
	
	if (Jaxer.Util.String.startsWith(res, "220") == false) 
	{
		throw new Exception("SMTP: introduction resulted in: " + res);
	}
	
	s.writeString("HELO " + "localhost\r\n");
	log.trace('>' + "HELO " + "localhost\r\n");

	s.flush();
	res = s.readLine();
//	res = s.readString(1024);
	log.trace('<' + res );
	
	if (Jaxer.Util.String.startsWith(res, "250") == false) 
	{
		throw new Exception("SMTP: expected 250 return code when sending HELO, received: " + res);
	}
	
	s.writeString("MAIL FROM:<" + from + ">\r\n");
	log.trace('>' + "MAIL FROM:<" + from + ">\r\n");
	
	s.flush();
	res = s.readLine();
	log.trace('<' + res + ": bytes available " + s.available());

	if (Jaxer.Util.String.startsWith(res, "250") == false) 
	{
		throw new Exception("SMTP: expected 250 return code when sending MAIL FROM, received: " + res);
	}
	
	if (to.constructor == to.__parent__.Array) 
	{
		for (var i = 0; i < to.length; i++) 
		{
			s.writeString("RCPT TO:<" + to[i] + ">\r\n");
			log.trace('>' + "RCPT TO:<" + to[i] + ">\r\n");
			s.flush();
			res = s.readLine();
			log.trace('<' + res);
			
			if (Jaxer.Util.String.startsWith(res, "250") == false) 
			{
				throw new Exception("SMTP: expected 250 return code when sending RCPT TO, received: " + res);
			}
		}
	}
	else 
	{
		s.writeString("RCPT TO:<" + to + ">\r\n");
		log.trace('>' + "RCPT TO:<" + to + ">\r\n");
		s.flush();
		res = s.readLine();
		log.trace('<' + res);
		
		if (Jaxer.Util.String.startsWith(res, "250") == false) 
		{
			throw new Exception("SMTP: expected 250 return code when sending RCPT TO, received: " + res);
		}
	}
	
	s.writeString("DATA\r\n");
	log.trace('>' + "DATA\r\n");
	s.flush();
	res = s.readLine();
	log.trace('<' + res);
	
	if (Jaxer.Util.String.startsWith(res, "354") == false) 
	{
		throw new Exception("SMTP: expected 354 return code when sending DATA, received: " + res);
	}
	
	s.writeString("From: " + from + "\r\n");
	log.trace('>' + "From: " + from + "\r\n");

	if (to.constructor == to.__parent__.Array) 
	{
		s.writeString("To: ");
		log.trace('>' + "To: ");
		for (var i = 0; i < to.length; i++) 
		{
			if(i>0) { s.writeString(","); }
			s.writeString(to[i]);
			log.trace('>' + to[i]);
		}
		s.writeString("\r\n");
	}
	else
	{
		s.writeString("To: " + to + "\r\n");
		log.trace('>' + "To: " + to + "\r\n");
	}

	s.writeString("Subject: " + subject + "\r\n");
	log.trace('>' + "Subject: " + subject + "\r\n");
	s.writeString("Date: " + new Date().toString() + "\r\n");
	log.trace('>' + "Date: " + new Date().toString() + "\r\n");

	// output any extra headers
	if (headers && headers.length > 0)
	{
		for (var i = 0; i < headers.length; i++) 
		{
			s.writeString(headers[i][0]+": " + headers[i][1] + "\r\n");
			log.trace('>' + headers[i][0]+": " + headers[i][1] + "\r\n");
		}
	}

	// Finish the header section
	s.writeString("\r\n");
	
	var msgLines = msg.split('\n');
	for (var i = 0; i < msgLines.length; i++) 
	{
		var line = msgLines[i];
		if (Jaxer.Util.String.startsWith(line, ".")) 
		{
			line = "." + line;
		}
		s.writeString(line + "\n");
		log.trace('>' + "To: " + to + "\r\n");
	}
	
	s.writeString("\r\n.\r\n");
	s.flush();
	res = s.readLine();
	log.trace('<' + res);
	
	if (Jaxer.Util.String.startsWith(res, "250") == false) 
	{
		throw new Exception("SMTP: expected 250 return code when closing conversation, received: " + res);
	}
	
	s.writeString("QUIT\r\n");
	log.trace('>' + "QUIT\r\n");
	s.flush();
	s.close();
}

/**
 * Sends an email message object via SMTP
 * 
 * @alias Jaxer.SMTP.sendMessage
 * @param {String} mailhost
 * 		The host to connect to
 * @param {Number} mailport
 * 		The port to connect on
 * @param {Jaxer.SMTP.MailMessage} mailMessage
 * 		The Jaxer.SMTP.MailMessage object to send
 */
SMTP.sendMessage = function sendMessage(mailhost, mailport, mailMessage)
{
	SMTP.sendEmail(mailhost, mailport, mailMessage._from, mailMessage._recipients, mailMessage._subject, mailMessage._body, mailMessage._extraHeaders);
}

/**
 * @classDescription {Jaxer.SMTP.MailMessage} A structure holding email message
 * information.
 */

/**
 * A structure holding email message information
 * 
 * @constructor
 * @alias Jaxer.SMTP.MailMessage
 * @return {Jaxer.SMTP.MailMessage}
 * 		Returns an instance of MailMessage.
 */
function MailMessage()
{
	this._date = new Date();
	this._from = "";
	this._recipients = [];
	this._subject = "";
	this._body = "";
	this._extraHeaders = [];
}

/**
 * Adds a recipient to the message
 * 
 * @alias Jaxer.SMTP.MailMessage.prototype.addRecipient
 * @param {String} recipient
 * 		The email address
 */
MailMessage.prototype.addRecipient = function addRecipient(recipient)
{
	this._recipients.push(recipient);
};

/**
 * Sets the "From" address on the message
 * 
 * @alias Jaxer.SMTP.MailMessage.prototype.setFrom
 * @param {String} from
 * 		The email address from which this message is coming
 */
MailMessage.prototype.setFrom = function setFrom(from)
{
	this._from = from;
};

/**
 * Sets the "Subject" of the message
 * 
 * @alias Jaxer.SMTP.MailMessage.prototype.setSubject
 * @param {String} subject
 * 		The subject text
 */
MailMessage.prototype.setSubject = function setSubject(subject)
{
	this._subject = subject;
};

/** 
 * Sets the timestamp on the message
 * 
 * @alias Jaxer.SMTP.MailMessage.prototype.setDate
 * @param {Object} date
 * 		The date it's sent
 */
MailMessage.prototype.setDate = function setDate(date)
{
	this._date = date;
};

/** 
 * Sets extra SMTP headers on the message, the provided header parameter is an two element array consisting of the
 * header name value pair.
 * 
 * @alias Jaxer.SMTP.MailMessage.prototype.addHeader,
 * @param {Array} header
 * 		The header to add.
 */
MailMessage.prototype.addHeader = function addHeader(header)
{
	this._extraHeaders.push(header);
};

/**
 * Sets the body (contents) of the message
 * 
 * @alias Jaxer.SMTP.MailMessage.prototype.setBody
 * @param {String} body
 * 		The text of the message
 */
MailMessage.prototype.setBody = function setBody(body)
{
	this._body = body;
};

SMTP.MailMessage = MailMessage;
frameworkGlobal.SMTP = Jaxer.SMTP = SMTP;

Log.trace("*** SMTP.js loaded");

})();
