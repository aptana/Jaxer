# ***** BEGIN LICENSE BLOCK *****
#  Version: GPL 3
# 
#  This program is licensed under the GNU General Public license, version 3 (GPL).
#  It is derived from Mozilla software and modified by Aptana, Inc.
#  Aptana, Inc. has elected to use and license the Mozilla software 
#  under the terms of the GPL, and licenses this file to you under the terms
#  of the GPL.
#  
#  Contributor(s): Aptana, Inc.
#  The portions modified by Aptana are Copyright (C) 2007-2008 Aptana, Inc.
#  All Rights Reserved.
# 
#  This program is distributed in the hope that it will be useful, but
#  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
#  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
#  is prohibited.
# 
#  You can redistribute and/or modify this program under the terms of the GPL, 
#  as published by the Free Software Foundation.  You should
#  have received a copy of the GNU General Public License, Version 3 along
#  with this program; if not, write to the Free Software Foundation, Inc., 51
#  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
#  
#  Aptana provides a special exception to allow redistribution of this file
#  with certain other code and certain additional terms
#  pursuant to Section 7 of the GPL. You may view the exception and these
#  terms on the web at http://www.aptana.com/legal/gpl/.
#  
#  You may view the GPL, and Aptana's exception and additional terms in the file
#  titled license-jaxer.html in the main distribution folder of this program.
#  
#  Any modifications to this file must keep this entire header intact.
# 
# ***** END LICENSE BLOCK *****
# ***** BEGIN ORIGINAL ATTRIBUTION BLOCK *****
#
# The Original Code is the Mozilla build system.
#
# The Initial Developer of the Original Code is
# the Mozilla Foundation <http://www.mozilla.org/>.
# Portions created by the Initial Developer are Copyright (C) 2006
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   Benjamin Smedberg <benjamin@smedbergs.us> (Initial Code)
#
#
# ***** END ORIGINAL ATTRIBUTION BLOCK *****

ifdef LIBXUL_SDK
$(error toolkit-tiers.mk is not compatible with --enable-libxul-sdk=)
endif

include $(topsrcdir)/config/nspr/build.mk
include $(topsrcdir)/config/js/build.mk
include $(topsrcdir)/xpcom/build.mk
include $(topsrcdir)/netwerk/build.mk

TIERS += \
	external \
	gecko \
	toolkit \
	$(NULL)

#
# tier "external" - 3rd party individual libraries
#

ifndef MOZ_NATIVE_JPEG
tier_external_dirs	+= jpeg
endif

# Installer needs standalone libjar, hence standalone zlib
ifdef MOZ_INSTALLER
tier_external_dirs	+= modules/zlib/standalone
endif

ifdef MOZ_UPDATER
ifndef MOZ_NATIVE_BZ2
tier_external_dirs += modules/libbz2
endif
tier_external_dirs += modules/libmar
tier_external_dirs += other-licenses/bsdiff
endif

tier_external_dirs	+= gfx/qcms

ifeq ($(OS_ARCH),WINCE)
tier_external_dirs += modules/lib7z
endif

#
# tier "gecko" - core components
#

tier_gecko_dirs += \
		js/src/xpconnect \
		js/ctypes \
		intl/chardet \
		$(NULL)

ifdef BUILD_CTYPES
ifndef _MSC_VER
tier_gecko_staticdirs += \
		js/ctypes/libffi \
		$(NULL)
endif
endif

ifdef JAXER
tier_gecko_dirs += aptana
endif

ifdef MOZ_ENABLE_GTK2
ifdef MOZ_X11
tier_gecko_dirs     += widget/src/gtkxtbin
endif
endif

tier_gecko_dirs	+= \
		modules/libutil \
		modules/libjar \
		db \
		$(NULL)

ifdef MOZ_PERMISSIONS
tier_gecko_dirs += \
		extensions/cookie \
		extensions/permissions \
		$(NULL)
endif

ifdef MOZ_STORAGE
tier_gecko_dirs += storage
endif

ifdef MOZ_RDF
tier_gecko_dirs += rdf
endif

ifdef MOZ_JSDEBUGGER
tier_gecko_dirs += js/jsd
endif

ifdef MOZ_OGG
tier_gecko_dirs += \
		media/libfishsound \
		media/libogg \
		media/liboggplay \
		media/liboggz \
		media/libtheora \
		media/libvorbis \
		$(NULL)
endif

ifdef MOZ_SYDNEYAUDIO
tier_gecko_dirs += \
		media/libsydneyaudio \
		$(NULL)
endif

tier_gecko_dirs	+= \
		uriloader \
		modules/libimg \
		caps \
		parser \
		gfx \
		modules/libpr0n \
		modules/plugin \
		dom \
		view \
		widget \
		content \
		editor \
		layout \
		docshell \
		webshell \
		embedding \
		xpfe/appshell \
		$(NULL)

# Java Embedding Plugin
ifneq (,$(filter mac cocoa,$(MOZ_WIDGET_TOOLKIT)))
tier_gecko_dirs += plugin/oji/JEP
endif

ifdef MOZ_XMLEXTRAS
tier_gecko_dirs += extensions/xmlextras
endif

ifdef MOZ_WEBSERVICES
tier_gecko_dirs += extensions/webservices
endif

ifdef MOZ_UNIVERSALCHARDET
tier_gecko_dirs += extensions/universalchardet
endif

ifdef MOZ_OJI
tier_gecko_dirs	+= \
		js/src/liveconnect \
		sun-java \
		modules/oji \
		$(NULL)
endif

ifdef ACCESSIBILITY
tier_gecko_dirs    += accessible
endif

# 
# tier "toolkit" - xpfe & toolkit
#
# The division of "gecko" and "toolkit" is somewhat arbitrary, and related
# to history where "gecko" wasn't forked between seamonkey/firefox but
# "toolkit" was.
#

ifdef JAXER
tier_toolkit_dirs += chrome
tier_toolkit_dirs += toolkit/components
ifdef MOZ_CRASHREPORTER
tier_toolkit_dirs += toolkit/crashreporter
endif
else

tier_toolkit_dirs += chrome profile

# This must preceed xpfe
ifdef MOZ_JPROF
tier_toolkit_dirs        += tools/jprof
endif

tier_toolkit_dirs	+= \
	xpfe \
	toolkit/components \
	$(NULL)

endif

ifdef MOZ_ENABLE_XREMOTE
tier_toolkit_dirs += widget/src/xremoteclient
endif

ifdef MOZ_SPELLCHECK
tier_toolkit_dirs	+= extensions/spellcheck
endif

ifdef JAXER
tier_toolkit_dirs	+= toolkit/xre
else

tier_toolkit_dirs	+= toolkit

endif

ifdef MOZ_XPINSTALL
tier_toolkit_dirs     +=  xpinstall
endif

ifdef MOZ_PSM
tier_toolkit_dirs	+= security/manager
else
tier_toolkit_dirs	+= security/manager/boot/public security/manager/ssl/public
endif

ifdef MOZ_PREF_EXTENSIONS
tier_toolkit_dirs += extensions/pref
endif

# JavaXPCOM JNI code is compiled into libXUL
ifdef MOZ_JAVAXPCOM
tier_toolkit_dirs += extensions/java/xpcom/src
endif

ifndef BUILD_STATIC_LIBS
ifneq (,$(MOZ_ENABLE_GTK2))
tier_toolkit_dirs += embedding/browser/gtk
endif
endif

ifndef BUILD_STATIC_LIBS
tier_toolkit_dirs += toolkit/library
endif

ifdef MOZ_ENABLE_LIBXUL
tier_toolkit_dirs += xpcom/stub
endif

ifdef NS_TRACE_MALLOC
tier_toolkit_dirs += tools/trace-malloc
endif

ifdef MOZ_ENABLE_GNOME_COMPONENT
tier_toolkit_dirs    += toolkit/system/gnome
endif

ifndef MOZ_ENABLE_LIBCONIC
# if libconic is present, it will do its own network monitoring
ifdef MOZ_ENABLE_DBUS
tier_toolkit_dirs    += toolkit/system/dbus
endif
endif

ifdef MOZ_LEAKY
tier_toolkit_dirs        += tools/leaky
endif

ifdef MOZ_MAPINFO
tier_toolkit_dirs	+= tools/codesighs
endif

ifdef ENABLE_TESTS
tier_toolkit_dirs	+= testing/mochitest
endif

ifdef MOZ_TREE_FREETYPE
tier_external_dirs	+= modules/freetype2
endif
