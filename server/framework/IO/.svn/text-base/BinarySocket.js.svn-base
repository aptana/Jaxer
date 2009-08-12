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

var log = Log.forModule("BinarySocket");

/**
 * @classDescription {Jaxer.BinarySocket} Network socket utility object for simple 
 * 		binary socket access.
 */

/**
 * The constructor of a network socket object used for binary data operations
 * 
 * @alias Jaxer.BinarySocket
 * @constructor
 * @return {Jaxer.BinarySocket}
 * 		Returns an instance of BinarySocket.
 */
function BinarySocket()
{
	this._transportService = Components.classes["@mozilla.org/network/socket-transport-service;1"].
			getService(Components.interfaces.nsISocketTransportService);	
}

const BinaryInputStream = Components.Constructor("@mozilla.org/binaryinputstream;1", "nsIBinaryInputStream", "setInputStream");
const BinaryOutputStream = Components.Constructor("@mozilla.org/binaryoutputstream;1", "nsIBinaryOutputStream", "setOutputStream");

/**
 * Open the socket for communication
 * 
 * @alias Jaxer.BinarySocket.prototype.open
 * @param {String} host
 * 		The host to connect to
 * @param {Number} port
 * 		The port on which to connect
 */
BinarySocket.prototype.open = function open(host, port)
{
	
	this._transport = this._transportService.createTransport(null,0,host,port,null);

	if (!this._transport) {
		throw ("Cannot connect to server '" + host + ":" + port);
	}
	
	this._transport.setTimeout(Components.interfaces.nsISocketTransport.TIMEOUT_READ_WRITE, 5);
	
	// Set up output stream
	this._ostream = this._transport.openOutputStream(Components.interfaces.nsITransport.OPEN_BLOCKING,0,0);
	this._bostream = new BinaryOutputStream(this._ostream);
	
	// Set up input stream
	this._istream = this._transport.openInputStream(Components.interfaces.nsITransport.OPEN_BLOCKING,0,0);
	this._bistream = new BinaryInputStream(this._istream);

};

/**
 * Close the socket
 * 
 * @alias Jaxer.BinarySocket.prototype.close
 */
BinarySocket.prototype.close = function close() 
{
	try {
		this._bostream.close();
		this._bistream.close();
    }
	catch(e) {
		// ignore this exception, we're just trying to close this socket down
	}

	this._transport.close(0);
};

/**
 * Write a binary (byte) data array of integers to the socket
 * 
 * @alias Jaxer.BinarySocket.prototype.writeByteArray
 * @param {Array} data
 * 		The binary data array to write
 */
BinarySocket.prototype.writeByteArray = function writeByteArray(data) 
{
	this._bostream.writeByteArray(data, data.length);
};

/**
 * Write a binary (byte) data string to the socket
 * 
 * @alias Jaxer.BinarySocket.prototype.writeByteString
 * @param {Array} data
 * 		The binary data string to write
 */
BinarySocket.prototype.writeByteString = function writeByteString(data) 
{
	this._bostream.writeBytes(data, data.length);
};

/**
 * Read binary data from the socket into an array of bytes (integers)
 * 
 * @alias Jaxer.BinarySocket.prototype.readByteArray
 * @param {Number} count
 * 		How many bytes to read
 * @return {Array}
 * 		The bytes read in, as an array of integers
 */
BinarySocket.prototype.readByteArray = function readByteArray(count)
{
	return this._bistream.readByteArray(count);
};

/**
 * Read binary data from the socket into a string
 * 
 * @alias Jaxer.BinarySocket.prototype.readByteString
 * @param {Number} count
 * 		How many bytes to read
 * @return {String}
 * 		The bytes read in, as a string
 */
BinarySocket.prototype.readByteString = function readByteString(count)
{
	return this._bistream.readBytes(count);
};

/**
 * How many bytes are currently available on the stream?
 * 
 * @alias Jaxer.BinarySocket.prototype.available
 * @return {Number}
 * 		the number of bytes available
 */
BinarySocket.prototype.available = function available()
{
	return this._istream.available();
};

/**
 * Flush the socket's output stream
 * 
 * @alias Jaxer.BinarySocket.prototype.flush
 */
BinarySocket.prototype.flush = function flush()
{
	this._bostream.flush();
};

frameworkGlobal.BinarySocket = Jaxer.BinarySocket = BinarySocket;

Log.trace("*** BinarySocket.js loaded");

})();
