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
 * Any modifications to this filße must keep this entire header intact.
 *
 * ***** END LICENSE BLOCK ***** */

(function(){

var converter = CC["@mozilla.org/intl/scriptableunicodeconverter"].createInstance(CI.nsIScriptableUnicodeConverter); 
	converter.charset = "UTF-8"; 
	
var crypto  = CC["@mozilla.org/security/hash;1"].createInstance(CI.nsICryptoHash); 

/**
 * @namespace {Jaxer.Util.Crypto} The namespace that holds functions for Cryptographic hashing
 * 
 * @see https://developer.mozilla.org/en/nsICryptoHash
 */

Util.Crypto = {};

Util.Crypto.__noSuchMethod__ = function methodMissingHandler(methodName,arguments)
{
	if (['MD2', 'MD5', 'SHA1', 'SHA256', 'SHA384', 'SHA512'].indexOf(methodName) > -1) {
		return getHash(methodName, arguments[0])
	}
	
	if (['verifyMD2', 'verifyMD5', 'verifySHA1', 'verifySHA256', 'verifySHA384', 'verifySHA512'].indexOf(methodName) > -1) {
		return getHash(methodName.replace('verify',''), arguments[0]) === arguments[1];
	}
	
	throw new Jaxer.Exception('Method not found : Jaxer.Util.Crypto.' + methodName);

}

/**
 * @namespace {Jaxer.Util.File.Crypto} The namespace that holds functions for Cryptographic hashing of Files
 * 
 * @see https://developer.mozilla.org/en/nsICryptoHash
 */

//TODO - add unit tests

Util.Crypto.File = {};

Util.Crypto.File.__noSuchMethod__ = function methodMissingHandler(methodName,arguments)
{
	if (['MD2','MD5','SHA1','SHA256','SHA384','SHA512'].indexOf(methodName)>-1)
	{
		return getFileHash(methodName, arguments[0]);		
	} 
	
	if (['verifyMD2', 'verifyMD5', 'verifySHA1', 'verifySHA256', 'verifySHA384', 'verifySHA512'].indexOf(methodName) > -1) {
		return getFileHash(methodName.replace('verify',''), arguments[0]) === arguments[1];
	}

	throw new Jaxer.Exception('Method not found :Jaxer.Util.Crypo.File.'+methodName);
}

/**
 * Convert a character to a 2 digit hex string
 * 
 * @param {Object} charCode
 * 		The character code to convert
 */
function asHex(charCode) 
{ 
	Jaxer.Log.info(charCode.toString(16));
	return ("0" + charCode.toString(16)).slice(-2); 
} 

/**
 * @see https://developer.mozilla.org/en/nsICryptoHash
 * @private
 * @param {Object} hashingAlgorithm
 * @param {Object} content
 * 
 */
function getHash(hashingAlgorithm, content) 
{ 	
    Jaxer.Log.info('Getting '+hashingAlgorithm+' for String');
	var result = {}; 
	
	var data = converter.convertToByteArray(content, result); 
	
	crypto.initWithString(hashingAlgorithm,content); 
	crypto.update(data, data.length); 
	var hash = crypto.finish(false); 

	var hashString = [asHex(hash.charCodeAt(i)) for (i in hash)].join(""); 
	
	return hashString; 
};

/**
 * @see https://developer.mozilla.org/en/nsICryptoHash
 * @private
 * @param {Object} hashingAlgorithm
 * @param {Object} content
 * 
 */
function getFileHash(hashingAlgorithm, file) 
{ 	
    // TODO - needs to handle string paths and trap bad things
	 Jaxer.Log.info('Getting '+hashingAlgorithm+' for File');

	if (!file.exists)
	{
		throw Jaxer.Exception("not found");
	}
	
	var f = CC["@mozilla.org/file/local;1"].createInstance(CI.nsILocalFile);			  
	f.initWithPath(file.path);

	// open for reading
	var inputStream = CC["@mozilla.org/network/file-input-stream;1"].createInstance(CI.nsIFileInputStream);
	inputStream.init(f, 0x01, 0444, 0);

	crypto.init(crypto[hashingAlgorithm]);
	crypto.updateFromStream(inputStream, 0xffffffff);
	inputStream.close();
	
	var hash = crypto.finish(false);
	var hashString = [asHex(hash.charCodeAt(i)) for (i in hash)].join(""); 
	
	return hashString; 
}; 
 
/**
 * Generate an MD2 Hash key for the provided content.
 * 
 * Message Digest Algorithm 2 (MD2) is a cryptographic hash function developed by Ronald Rivest in 1989. 
 * The algorithm is optimized for 8-bit computers. MD2 is specified in RFC 1319. Although other algorithms 
 * have been proposed since, such as MD4, MD5 and SHA, even as of 2004[update] MD2 remains in use in public 
 * key infrastructures as part of certificates generated with MD2 and RSA.
 * 
 * @alias Jaxer.Util.Crypto.MD2
 * @param {String} content
 * 		The content used to generate the hask key.
 * @return {String} 
 * 		A string containing the MD2 hash key for the provided content
 */

/**
 * Generate an MD5 Hash key for the provided content.
 * 
 * In cryptography, MD5 (Message-Digest algorithm 5) is a widely used cryptographic hash function with a 128-bit 
 * hash value. As an Internet standard (RFC 1321), MD5 has been employed in a wide variety of security applications, 
 * and is also commonly used to check the integrity of files. However, it is now known to be partially insecure 
 * thus reducing its suitability for these purposes. An MD5 hash is typically expressed as a 32 digit hexadecimal number.
 * 
 * MD5 was designed by Ron Rivest in 1991 to replace an earlier hash function, MD4. In 1996, a flaw was found with the 
 * design of MD5. While it was not a clearly fatal weakness, cryptographers began recommending the use of other 
 * algorithms, such as SHA-1 (which has since been found vulnerable itself). 
 * 
 * In 2004, more serious flaws were discovered making further use of the algorithm for security purposes questionable.
 * In 2007 a group of researchers including Arjen Lenstra described how to create a pair of files that share the 
 * same MD5 checksum.
 * 
 * In an attack on MD5 published in December 2008, a group of researchers used this technique to fake SSL certificate 
 * validity.
 *  
 * @alias Jaxer.Util.Crypto.MD5
 * @param {String} content
 * 		The content used to generate the hash key.
 * @return {String} 
 * 		A string containing the MD5 hash key for the provided content
 */

/**
 * Generate an SHA1 Hash key for the provided content.
 * 
 * The SHA hash functions are a set of cryptographic hash functions designed by the National Security Agency (NSA) and 
 * published by the NIST as a U.S. Federal Information Processing Standard. SHA stands for Secure Hash Algorithm. 
 * 
 * @alias Jaxer.Util.Crypto.SHA1
 * @param {String} content
 * 		The content used to generate the hash key.
 * @return {String} 
 * 		A string containing the SHA1 hash key for the provided content
 */

/**
 * Generate an SHA256 Hash key for the provided content.
 * 
 * SHA-256 is novel hash function computed with a 32-bit word. 
 * 
 * @alias Jaxer.Util.Crypto.SHA256
 * @param {String} content
 * 		The content used to generate the hash key.
 * @return {String} 
 * 		A string containing the SHA256 hash key for the provided content
 */

/**
 * Generate an SHA384 Hash key for the provided content.
 * 
 * SHA-384 is simply a truncated version of the SHA512, computed 
 * with different initial values.
 * 
 * @alias Jaxer.Util.Crypto.SHA384
 * @param {String} content
 * 		The content used to generate the hash key.
 * @return {String} 
 * 		A string containing the SHA384 hash key for the provided content
 */

/**
 * Generate an SHA512 Hash key for the provided content.
 * 
 * SHA-512 is novel hash function computed with a 64-bit word. 
 * 
 * @alias Jaxer.Util.Crypto.SHA512
 * @param {String} content
 * 		The content used to generate the hash key.
 * @return {String} 
 * 		A string containing the SHA512 hash key for the provided content
 */

/**
 * Generate an MD2 Hash key for the provided file. 
 * 
 * Message Digest Algorithm 2 (MD2) is a cryptographic hash function developed by Ronald Rivest in 1989. 
 * The algorithm is optimized for 8-bit computers. MD2 is specified in RFC 1319. Although other algorithms 
 * have been proposed since, such as MD4, MD5 and SHA, even as of 2004[update] MD2 remains in use in public 
 * key infrastructures as part of certificates generated with MD2 and RSA.
 * 
 * @alias Jaxer.Util.Crypto.File.MD2
 * @param {Jaxer.File|String} file
 * 		The File used to generate the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @return {String} 
 * 		A string containing the MD2 hash key for the provided file
 */

/**
 * Generate an MD5 Hash key for the provided file.
 * 
 * In cryptography, MD5 (Message-Digest algorithm 5) is a widely used cryptographic hash function with a 128-bit 
 * hash value. As an Internet standard (RFC 1321), MD5 has been employed in a wide variety of security applications, 
 * and is also commonly used to check the integrity of files. However, it is now known to be partially insecure 
 * thus reducing its suitability for these purposes. An MD5 hash is typically expressed as a 32 digit hexadecimal number.
 * 
 * MD5 was designed by Ron Rivest in 1991 to replace an earlier hash function, MD4. In 1996, a flaw was found with the 
 * design of MD5. While it was not a clearly fatal weakness, cryptographers began recommending the use of other 
 * algorithms, such as SHA-1 (which has since been found vulnerable itself). 
 * 
 * In 2004, more serious flaws were discovered making further use of the algorithm for security purposes questionable.
 * In 2007 a group of researchers including Arjen Lenstra described how to create a pair of files that share the 
 * same MD5 checksum.
 * 
 * In an attack on MD5 published in December 2008, a group of researchers used this technique to fake SSL certificate 
 * validity.
 *  
 * @alias Jaxer.Util.Crypto.File.MD5
 * @param {Jaxer.File|String} file
 * 		The File used to generate the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @return {String} 
 * 		A string containing the MD5 hash key for the provided file
 */

/**
 * Generate an SHA1 Hash key for the provided file.
 * 
 * The SHA hash functions are a set of cryptographic hash functions designed by the National Security Agency (NSA) and 
 * published by the NIST as a U.S. Federal Information Processing Standard. SHA stands for Secure Hash Algorithm. 
 * 
 * @alias Jaxer.Util.Crypto.File.SHA1
 * @param {Jaxer.File|String} file
 * 		The File used to generate the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @return {String} 
 * 		A string containing the SHA1 hash key for the provided file
 */

/**
 * Generate an SHA256 Hash key for the provided file.
 * 
 * SHA-256 is novel hash function computed with a 32-bit word. 
 * 
 * @alias Jaxer.Util.Crypto.File.SHA256
 * @param {Jaxer.File|String} file
 * 		The File used to generate the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @return {String} 
 * 		A string containing the SHA256 hash key for the provided file
 */

/**
 * Generate an SHA384 Hash key for the provided file.
 * 
 * SHA-384 is simply a truncated version of the SHA512, computed 
 * with different initial values.
 * 
 * @alias Jaxer.Util.Crypto.File.SHA384
 * @param {Jaxer.File|String} file
 * 		The File used to generate the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @return {String} 
 * 		A string containing the SHA512 hash key for the provided file
 */

/**
 * Generate an SHA512 Hash key for the provided file.
 * 
 * SHA-512 is novel hash function computed with a 64-bit word. 
 * 
 * @alias Jaxer.Util.Crypto.File.SHA512
 * @param {Jaxer.File|String} file
 * 		The File used to generate the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @return {String} 
 * 		A string containing the SHA512 hash key for the provided file
 */

/**
 * Verify an MD2 Hash key for the provided content.
 * 
 * @alias Jaxer.Util.Crypto.verifyMD2
 * @param {String} content
 * 		The content used to Verify the hask key.
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an MD5 Hash key for the provided content.
 *  
 * @alias Jaxer.Util.Crypto.verifyMD5
 * @param {String} content
 * 		The content used to Verify the hash key.
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA1 Hash key for the provided content.
 * 
 * @alias Jaxer.Util.Crypto.verifySHA1
 * @param {String} content
 * 		The content used to Verify the hash key.
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA256 Hash key for the provided content.
 * 
 * @alias Jaxer.Util.Crypto.verifySHA256
 * @param {String} content
 * 		The content used to Verify the hash key.
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA384 Hash key for the provided content.
 * 
 * @alias Jaxer.Util.Crypto.verifySHA384
 * @param {String} content
 * 		The content used to Verify the hash key.
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA512 Hash key for the provided content.
 * 
 * @alias Jaxer.Util.Crypto.verifySHA512
 * @param {String} content
 * 		The content used to Verify the hash key.
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an MD2 Hash key for the provided file. 
 * 
 * @alias Jaxer.Util.Crypto.File.verifyMD2
 * @param {Jaxer.File|String} file
 * 		The File used to Verify the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an MD5 Hash key for the provided file.
 * 
 * @alias Jaxer.Util.Crypto.File.verifyMD5
 * @param {Jaxer.File|String} file
 * 		The File used to Verify the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA1 Hash key for the provided file.
 * 
 * @alias Jaxer.Util.Crypto.File.verifySHA1
 * @param {Jaxer.File|String} file
 * 		The File used to Verify the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */
/**

 * Verify an SHA256 Hash key for the provided file.
 * 
 * @alias Jaxer.Util.Crypto.File.verifySHA256
 * @param {Jaxer.File|String} file
 * 		The File used to Verify the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA384 Hash key for the provided file.
 * 
 * @alias Jaxer.Util.Crypto.File.verifySHA384
 * @param {Jaxer.File|String} file
 * 		The File used to Verify the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @param {String} hash
 * 		The hash value to compare.
 * @return {Boolean} 
 * 		true if the hash matches, false if not.
 */

/**
 * Verify an SHA512 Hash key for the provided file.
 * 
 * @alias Jaxer.Util.Crypto.File.verifySHA512
 * @param {Jaxer.File|String} file
 * 		The File used to Verify the hash key. Can be either
 * 		a Jaxer.File object or a String containing the absolute path to the file
 * @param {String} hash
 * 		The hash value to compare.
 * @return {String} 
 * 		true if the hash matches, false if not.
 */

// Log.trace("*** Crypto.js loaded");

})();
