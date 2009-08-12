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

(function() {

var log = Log.forModule("Socket");

/**
 * @classDescription {Jaxer.Socket} Network socket utility object for simple 
 * 		character-based (non-binary) socket access.
 */

/**
 * The constructor of a network socket object used for character-based (non-binary) operations
 * 
 * @alias Jaxer.Socket
 * @constructor
 * @return {Jaxer.Socket}
 * 		Returns an instance of Socket.
 */
function Socket()
{
	this._transportService = Components.classes["@mozilla.org/network/socket-transport-service;1"].
			getService(Components.interfaces.nsISocketTransportService);	
}

/**
 * Open the socket for communication
 * 
 * @alias Jaxer.Socket.prototype.open
 * @param {String} host
 * 		The host to connect to
 * @param {Number} port
 * 		The port on which to connect
 */
Socket.prototype.open = function open(host, port)
{
	
	this._transport = this._transportService.createTransport(null,0,host,port,null);

	if (!this._transport) {
		throw ("Cannot connect to server '" + host + ":" + port);
	}
	
	this._transport.setTimeout(Components.interfaces.nsISocketTransport.TIMEOUT_READ_WRITE, 5);
	
	// Set up output stream
	
	this._ostream = this._transport.openOutputStream(Components.interfaces.nsITransport.OPEN_BLOCKING,0,0);		
	
	this._outcharstream = Components.classes["@mozilla.org/intl/converter-output-stream;1"].
		createInstance(Components.interfaces.nsIConverterOutputStream);
	this._outcharstream.init(this._ostream, "UTF8", 80, 0x0);

	// Set up input stream
	
	this._istream = this._transport.openInputStream(Components.interfaces.nsITransport.OPEN_BLOCKING,0,0);
		
	this._incharstream = Components.classes["@mozilla.org/intl/converter-input-stream;1"].
		createInstance(Components.interfaces.nsIConverterInputStream);
	this._incharstream.init(this._istream, "UTF8", 80, 0x0);

};

/**
 * Close the socket
 * 
 * @alias Jaxer.Socket.prototype.close
 */
Socket.prototype.close = function close() 
{
	try {
		this._incharstream.close();
		this._outcharstream.close();
    }
	catch(e) {
		// ignore this exception, we're just trying to close this socket down
	}
	
	this._transport.close(0);
};

/**
 * Write a string to the socket
 * 
 * @alias Jaxer.Socket.prototype.writeString
 * @param {String} data
 * 		The text to write
 */
Socket.prototype.writeString = function writeString(data) 
{
	this._outcharstream.writeString(data);
};

/**
 * Read a single line from the socket
 * 
 * @alias Jaxer.Socket.prototype.readLine
 * @return {String}
 * 		The text read in
 */
//Socket.prototype.readLine = function()
//{
//	if (this._incharstream instanceof Components.interfaces.nsIUnicharLineInputStream) 
//	{
//		var line = {};
//		this._incharstream.readLine(line);
//		return line.value;
//	}
//};
Socket.prototype.readLine = function readLine()
{ // this fake newline function is required to workaround the issue in jxr-140
	var buf = "";
	var str = {value : "0"};
	
	while (str.value != "") 
	{
		this._incharstream.readString(1, str);
		if ( str.value != '\n' && str.value != '\r')
		{
			buf += str.value;
		}
		if ( str.value == '\n') {
			return buf;
		}	
	}
};


/**
 * Read characters from the socket into a string
 * 
 * @alias Jaxer.Socket.prototype.readString
 * @param {Number} count
 * 		How many characters to read
 * @return {String}
 * 		The text read in
 */
Socket.prototype.readString = function readString(count)
{
	var str = {}
	this._incharstream.readString(count, str);
	return str.value;
};

/**
 * How many bytes (not characters) are currently available on the stream?
 * 
 * @alias Jaxer.Socket.prototype.available
 * @return {Number}
 * 		the number of bytes available
 */
Socket.prototype.available = function available()
{
	return this._istream.available();
};

/**
 * Flush the socket's output stream
 * 
 * @alias Jaxer.Socket.prototype.flush
 */
Socket.prototype.flush = function flush()
{
	this._outcharstream.flush();
};

frameworkGlobal.Socket = Jaxer.Socket = Socket;

Log.trace("*** Socket.js loaded");

})();
