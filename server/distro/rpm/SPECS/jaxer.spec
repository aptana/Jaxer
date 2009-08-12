# Jaxer spec for packaging JAXER BINARY RPM
# Part of the work is done by the build process

# Tags are case-insensitive.

Name:		jaxer

# This will be replaced by the real build number, eg 1.0.3.4567
Version:	JAXER_BUILD_VERSION
Release:	1%{?dist}

Summary: Jaxer, the world's first Ajax server from Aptana.

# It is nice to have a background-tranparent image for the product
#Icon: jaxer.xpm

Vendor: Aptana, Inc.
Packager: build <build@aptana.com>

# From general to specific separated by /
Group:web

License:GPL	

# Tmp place for the build
# Overwritten by build script
BuildRoot: /var/hudson/workspace/jaxer-ubuntu-64/trunk/products/server/distro/rpm/tmp

# This is default.  It will build the depenency list for you.

#AutoReqProv: yes

# We want a relocatable package.
Prefix: /opt/AptanaJaxer

# Detailed description of the product.  Can have multiple paragraphs.
%description
 Jaxer, the world's first Ajax server from Aptana.
 Jaxer is a server-side JavaScript application engine which enables
 the development of complete, sophisticated Ajax applications in JavaScript.
 Jaxer is licensed under the GPL and leverages the ubiquitous Mozilla engine
 used in Firefox 3. This means that Jaxer is fully compatible with the latest
 JavaScript standards, including access to all of the familiar features from
 Date to Math to window and document, and even JavaScript 1.5, 1.7, and 1.8,
 as well as Database, file and socket access, XMLHttpRequests,
 JSON and DOM scripting.

%build

#%clean
#rm -rf $RPM_BUILD_ROOT

%pre
# called just before install
INSTALL_ROOT=${RPM_INSTALL_PREFIX}
if [ -z "${INSTALL_ROOT}" ]; then
    INSTALL_ROOT=/opt/AptanaJaxer
fi
if [ -x "${INSTALL_ROOT}/scripts/stopJaxer.sh" ]; then
    echo "Executing stopJaxer.sh" 1>&2
    ${INSTALL_ROOT}/scripts/stopJaxer.sh > /dev/null 2>&1 || true
fi

%post
# called just after install
INSTALL_ROOT=${RPM_INSTALL_PREFIX}
if [ -z "${INSTALL_ROOT}" ]; then
    INSTALL_ROOT=/opt/AptanaJaxer
fi
if [ -x "${INSTALL_ROOT}/scripts/post_install.sh" ]; then
    echo "Executing post_install.sh" 1>&2
    ${INSTALL_ROOT}/scripts/post_install.sh
fi

%preun
# called just before uninstall
INSTALL_ROOT=${RPM_INSTALL_PREFIX}
if [ -z "${INSTALL_ROOT}" ]; then
    INSTALL_ROOT=/opt/AptanaJaxer
fi
echo "INSTALL_DIR=$INSTALL_ROOT" 1>&2

if [ -x "${INSTALL_ROOT}/scripts/stopJaxer.sh" ]; then
    echo "Executing stopJaxer.sh" 1>&2
    ${INSTALL_ROOT}/scripts/stopJaxer.sh > /dev/null 2>&1 || true
fi

#%postun
# called just after uninstall
#INSTALL_ROOT=${RPM_INSTALL_PREFIX}
#[ -z "${INSTALL_ROOT}" ] && INSTALL_ROOT=/opt/AptanaJaxer



%files
# List files in the RPM.  Note path must be absolute.
# This will be the final installation path, and also the path
# starting from the build root.  So your build root should have this
# folder structure.
/opt/AptanaJaxer
%defattr(-,root,root,-)
%doc



%changelog
# List changes here

