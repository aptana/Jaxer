#!/usr/bin/env python
import email
import email.mime.base
import email.mime.text
import email.mime.multipart
import email.Utils
import logging
import os
import smtplib
import sys
import tempfile
import urllib

# Add aptana lib dir to module path, derived from abspath of this file within lib dir.
_aptanalibpath = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if (_aptanalibpath not in sys.path):
    sys.path.append(_aptanalibpath)

import aptana.archive
import aptana.boot
import aptana.net

__version__ = "$Revision: 3374 $"[11:-2]


class NotificationError(Exception):
    """Error occured while sending a notification."""


class Notifier(object):
    """Notification abstract interface, each implementor will handle their own transport mechanism."""

    def __init__(self):
        self._hostname = None
        self._sender = None
        self._subject = ""
        self._message = ""
        self._recipients = []
        self._attachments = {}
        self._compressall = False
        self._enabled = True
        self._instrumentation = aptana.boot.Instrumentation(logging.getLogger(self.__class__.__name__))

    def __str__(self):
        return "%s: enabled=%d, compressall=%d, hostname=%s, sender=%s, recipients=%s, subject=%s, message=%s, instrumentation=%s" % (self.__class__.__name__, self.enabled, self.compressall, str(self._hostname), str(self._sender), str(self._recipients), str(self._subject), str(self._message)[0:32], self.instrumentation)

    #
    # attributes:
    #

    def getinstrumentation(self):
        return self._instrumentation

    def setinstrumentation(self, instrumentation):
        self._instrumentation.changeinstrumentation(instrumentation)

    instrumentation = property(fget=getinstrumentation, fset=setinstrumentation, fdel=None, doc=None)

    def gethostname(self):
        return self._hostname

    def sethostname(self, hostname):
        self._hostname = hostname

    hostname = property(fget=gethostname, fset=sethostname, fdel=None, doc=None)

    def getsender(self):
        return self._sender

    def setsender(self, sender):
        self._sender = sender

    sender = property(fget=getsender, fset=setsender, fdel=None, doc=None)

    def getsubject(self):
        return self._subject

    def setsubject(self, subject):
        self._subject = subject

    subject = property(fget=getsubject, fset=setsubject, fdel=None, doc=None)

    def getmessage(self):
        return self._message

    def setmessage(self, message):
        self._message = message

    message = property(fget=getmessage, fset=setmessage, fdel=None, doc=None)

    def getcompressall(self):
        return self._compressall

    def setcompressall(self, compressall):
        if (compressall is None):
            compressall = False
        self._compressall = compressall

    compressall = property(fget=getcompressall, fset=setcompressall, fdel=None, doc=None)


    def attachcompressed(self, attachments, filename, mimemsg):
        tempzipfile = None
        tempzipfilename = None
        try:
            tempzipfiledescriptor, tempzipfilename = tempfile.mkstemp(".zip", filename)
            zip = aptana.archive.ZipArchiver(tempzipfilename)
            zip.instrumentation = self.instrumentation
            for url in attachments:
                attachmentname = os.path.basename(url)
                zip.include(url,attachmentname)
            zip.assemble()
            
            part = email.mime.base.MIMEBase("application","zip")
            part.add_header("Content-Disposition", "attachment; filename=\"%s.%s\"" % (filename,"zip"))
            tempzipfile = open(tempzipfilename, "r+b")
            part.set_payload(tempzipfile.read())
            email.encoders.encode_base64(part)
            mimemsg.attach(part)
        finally:
            if (tempzipfile is not None):
                tempzipfile.close()
            #if (os.path.exists(tempzipfilename)):
                #print
                #os.remove(tempzipfilename)

    def getenabled(self):
        return self._enabled

    def setenabled(self, enabled):
        if (enabled is None):
            enabled = False
        self._enabled = enabled

    enabled = property(fget=getenabled, fset=setenabled, fdel=None, doc=None)

    #
    # Notifier interface:
    #

    def addrecipient(self, recipient):
        self._recipients.append(recipient)

    def clearrecipients(self):
        self._recipients = []

    def addattachment(self, url, compress=False, mimetype=["application","octet-stream"]):
        self._attachments[url] = {
            'compress': compress,
            'mimetype': mimetype
        }

    def clearattachments(self):
        self._attachments = {}

    def notify(self):
        pass


class SmtpNotifier(Notifier):
    """SMTP E-mail notifier."""

    def connect(self, hostname, debuglevel=0):
        if (debuglevel is None):
            debuglevel = 0
        smtp = smtplib.SMTP()
        smtp.set_debuglevel(debuglevel)
        smtp.connect(hostname)
        return smtp

    def notify(self):
        if (self.enabled is False):
            return

        self.instrumentation.say(str(self) + ":notify()")
        smtp = None
        try:
            mimemsg = email.mime.multipart.MIMEMultipart()

            # headers
            mimemsg["From"]     = self._sender
            mimemsg["To"]       = email.Utils.COMMASPACE.join(self._recipients)
            mimemsg["Date"]     = email.Utils.formatdate(localtime=True)
            mimemsg["Subject"]  = self._subject

            # body
            body = email.mime.text.MIMEText(self.message)
            mimemsg.attach(body)

            # attachments
            self.attach(mimemsg, self._attachments, self.compressall)

            # transport
            smtp = self.connect(self.hostname, self.instrumentation.logger.level)
            smtp.sendmail(self.sender, self._recipients, mimemsg.as_string())
        finally:
            if (smtp is not None):
                smtp.quit()

    def attach(self, mimemsg, attachments, compressall=False):
        if (len(attachments) == 0):
            return

        # create a single zip attachment
        if (compressall):
            part = self.attachcompressed(attachments.keys, "attachments", mimemsg)

        # add separate attachments
        else:
            for url in attachments:
                filename = os.path.basename(url)
                file = None

                # TODO
                # It would be cool to have smart compressing:
                # compress = (Always|Never|if size > maxunzippedsize)

                # Create a zip file for each attachment with compress flag set
                if attachments[url]['compress']:
                    part = self.attachcompressed([url], filename, mimemsg)

                # Just attach the file without compressing
                else :
                    try:
                        attachmime = attachments[url]['mimetype']
                        part = email.mime.base.MIMEBase(attachmime[0], attachmime[1])
                        part.add_header("Content-Disposition", "attachment; filename=\"%s\"" % filename)

                        file = urllib.urlopen(aptana.net.urlnormalize(url))
                        part.set_payload(file.read())

                        # this check is probably too simplistic?
                        if (attachmime[0] is not "text"):
                            email.Encoders.encode_base64(part)

                        mimemsg.attach(part)
                    finally:
                        if (file is not None):
                            file.close()


#TODO: Use this class to keep track of attachments within each Notifier instance.
class MIMEAttachment(email.mime.base.MIMEBase):
    COMPRESS_MODE_NEVER  = False
    COMPRESS_MODE_ALWAYS = True
    COMPRESS_MODE_AUTO   = -1
    COMPRESS_ZIP         = "zip"
    COMPRESS_GZIP        = "gzip"

    def __init__(self, url, _maintype="application", _subtype="octet-stream", compressionmode=COMPRESS_MODE_NEVER, compressionalgorithm=COMPRESS_ZIP, compressionthreshold=0, **_params):
        super(MIMEAttachment, self).__init__(_maintype, _subtype, **_params)
        self._url = url
        self._compressionmode = compressionmode
        self._compressionalgorithm = compressionalgorithm
        self._compressionthreshold = compressionthreshold

    def geturl(self):
        return self._url

    def seturl(self, url):
        self._url = url

    url = property(fget=geturl, fset=seturl, fdel=None, doc=None)

    def getcompressionmode(self):
        return self._compressionmode

    def setcompressionmode(self, compressionmode):
        self._compressionmode = compressionmode

    compressionmode = property(fget=getcompressionmode, fset=setcompressionmode, fdel=None, doc=None)

    def getcompressionalgorithm(self):
        return self._compressionalgorithm

    def setcompressionalgorithm(self, compressionalgorithm):
        self._compressionalgorithm = compressionalgorithm

    compressionalgorithm = property(fget=getcompressionalgorithm, fset=setcompressionalgorithm, fdel=None, doc=None)

    def getcompressionthreshold(self):
        return self._compressionthreshold

    def setcompressionthreshold(self, compressionthreshold):
        self._compressionthreshold = compressionthreshold

    compressionthreshold = property(fget=getcompressionthreshold, fset=setcompressionthreshold, fdel=None, doc=None)

    def compress(self):
        """Compress the byte stream located at url."""
        #TODO

    def encode(self):
        """Perform smart encoding of the byte stream located at url."""
        #REVIEW: this check is probably too simplistic?
        if (self.get_content_maintype() is not "text"):
            email.Encoders.encode_base64(self)


def main(argv=None):
    if (argv is None):
        argv = sys.argv[1:]

    if (len(argv) < 5):
        print "Usage: %s hostname sender recipient subject message" % \
            (sys.argv[0])
    else:
        notifier = SmtpNotifier()
        notifier.instrumentation.logger.level = logging.DEBUG
        notifier.hostname = argv[0]
        notifier.sender = argv[1]
        notifier.addrecipient(argv[2])
        notifier.subject = argv[3]
        notifier.message = argv[4]
        notifier.addattachment(os.path.abspath(__file__), compress=True)
        notifier.addattachment(os.path.abspath(os.path.join(os.path.dirname(__file__), "__init__.py")), mimetype=["text","plaintext"])
        notifier.notify()
        print notifier
 
if (__name__ == "__main__"):
    main()
