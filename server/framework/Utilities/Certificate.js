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

/**
 * @namespace {Jaxer.Util.Certificate} Namespace used for handling
 * information related to SSL certificates
 */
Util.Certificate = {};

/**
 * @classDescription {Jaxer.Util.Certificate.CertInfo} Information about
 * an SSL Certificate
 */

/**
 * Holds information about an SSL certificate and the SSL connection that 
 * returned it. It repackages the information in the Mozilla objects used
 * to construct it.
 * 
 * @constructor
 * @alias Jaxer.Util.Certificate.CertInfo
 * @param {Object} socketInfo
 * 		A network communication context that can be used to obtain more information about the connection
 * 		relevant to this certificate.
 * 		This is an instance of Mozilla's nsIInterfaceRequestor 
 * 		(http://www.xulplanet.com/references/xpcomref/ifaces/nsIInterfaceRequestor.html)
 * @param {Object} sslStatus
 * 		A Mozilla object that contains information about the certificate.
 * 		This is an instance of Mozilla's nsISSLStatus
 * 		(http://www.xulplanet.com/references/xpcomref/ifaces/nsISSLStatus.html)
 * @param {String} targetSite
 * 		The site name that was used to open the connection from which the certificate was returned
 * @return {Jaxer.Util.Certificate.CertInfo}
 * 		Returns an instance of CertInfo.
 */
Util.Certificate.CertInfo = function CertInfo(socketInfo, sslStatus, targetSite)
{
	/**
	 * The Mozilla object representing an X.509 certificate.
	 * See nsIX509Cert (http://www.xulplanet.com/references/xpcomref/ifaces/nsIX509Cert.html)
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.cert
	 * @property {Object}
	 */
	this.cert = sslStatus.serverCert;
	
	/**
	 * The primary email address of the certificate, if present.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.emailAddress
	 * @property {String}
	 */
	this.emailAddress = this.cert.emailAddress;
	
	/**
	 * The subject owning the certificate.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.subjectName
	 * @property {String}
	 */
	this.subjectName = this.cert.subjectName;
	
	/**
	 * The subject's organization.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.organization
	 * @property {String}
	 */
	this.organization = this.cert.organization;
	
	/**
	 * The subject identifying the issuer certificate.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.issuerName
	 * @property {String}
	 */
	this.issuerName = this.cert.issuerName;
	
	/**
	 * The issuer subject's organization.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.issuerOrganization
	 * @property {String}
	 */
	this.issuerOrganization = this.cert.issuerOrganization;
	
	/**
	 * This certificate's validity period: the notBefore date, followed by '/',
	 * followed by the notAfter date.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.validity
	 * @property {String}
	 */
	this.validity = this.cert.validity.notBefore + "/" + this.cert.validity.notAfter;
	
	/**
	 * The site name that was used to open the connection from which the certificate was returned.
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.targetSite
	 * @property {String}
	 */
	this.targetSite = targetSite;
	
	/**
	 * Is there a domain mismatch between the domain on this certificate and the domain that served it?
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.isDomainMismatch
	 * @property {Boolean}
	 */
	this.isDomainMismatch = sslStatus.isDomainMismatch;
	
	/**
	 * Is this certificate not trusted at this time -- because its validity period 
	 * has not yet started or has already ended?
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.isNotValidAtThisTime
	 * @property {Boolean}
	 */
	this.isNotValidAtThisTime = sslStatus.isNotValidAtThisTime;
	
	/**
	 * Is this certificate untrusted -- either because it has a missing or untrusted issuer
	 * or because it is self-signed?
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.isUntrusted
	 * @property {Boolean}
	 * @see Jaxer.Util.Certificate.CertInfo.prototype.isSelfSigned
	 */
	this.isUntrusted = sslStatus.isUntrusted;
	
	/**
	 * Is this a self-signed certificate?
	 * @alias Jaxer.Util.Certificate.CertInfo.prototype.isSelfSigned
	 * @property {Boolean}
	 */
	this.isSelfSigned = this.cert.QueryInterface(Components.interfaces.nsIX509Cert3).isSelfSigned;
	
};


})();
