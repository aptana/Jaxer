/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=4 et: */
/* ***** BEGIN LICENSE BLOCK *****
 *  Version: GPL 3
 * 
 *  This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 *  This program is licensed under the GNU General Public license, version 3 (GPL).
 * 
 *  This program is distributed in the hope that it will be useful, but
 *  AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 *  NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 *  is prohibited.
 * 
 *  You can redistribute and/or modify this program under the terms of the GPL, 
 *  as published by the Free Software Foundation.  You should
 *  have received a copy of the GNU General Public License, Version 3 along
 *  with this program; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Aptana provides a special exception to allow redistribution of this file
 *  with certain other code and certain additional terms
 *  pursuant to Section 7 of the GPL. You may view the exception and these
 *  terms on the web at http://www.aptana.com/legal/gpl/.
 *  
 *  You may view the GPL, and Aptana's exception and additional terms in the file
 *  titled license-jaxer.html in the main distribution folder of this program.
 *  
 *  Any modifications to this file must keep this entire header intact.
 * 
 * ***** END LICENSE BLOCK ***** */

#include "nsCOMPtr.h"
#include "nsString.h"
#include "nsINIParser.h"
#include "nsArrayEnumerator.h"
#include "nsNetUtil.h"
#include "nsILineInputStream.h"
#include "nsDirectoryServiceDefs.h"
#include "nsAppDirectoryServiceDefs.h"
#include "nsIToolkitChromeRegistry.h"
#ifdef WIN32
#include <windows.h>
#elif defined(XP_MACOSX)
#include <CFURL.h>
#include <CFBundle.h>
#elif defined(XP_UNIX)
#include <dlfcn.h>
#include <unistd.h>                                                                                                  
#endif
#include "stdlib.h"
#include "AppDirectoryProvider.h"
#include "jaxerBuildId.h"


static nsresult
GetCurrentProcessDirectory(nsIFile** aFile)
{
    nsILocalFile* binDir;
    nsresult rv = NS_ERROR_FAILURE;

#ifdef XP_WIN
    PRUnichar buf[MAX_PATH];
    if ( ::GetModuleFileNameW(0, buf, sizeof(buf)) ) {
        // chop off the executable name by finding the rightmost backslash
        PRUnichar* lastSlash = wcsrchr(buf, L'\\');
        if (lastSlash)
            *(lastSlash + 1) = L'\0';

        rv = NS_NewLocalFile(nsDependentString(buf), PR_TRUE, &binDir);
   }
#elif defined(XP_MACOSX)
    // Works even if we're not bundled.
    CFBundleRef appBundle = CFBundleGetMainBundle();
    if (appBundle != nsnull) {
        CFURLRef bundleURL = CFBundleCopyExecutableURL(appBundle);
        if (bundleURL != nsnull) {
            CFURLRef parentURL = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault, bundleURL);
            if (parentURL) {
                // Pass PR_TRUE for the "resolveAgainstBase" arg to CFURLGetFileSystemRepresentation.
                // This will resolve the relative portion of the CFURL against it base, giving a full
                // path, which CFURLCopyFileSystemPath doesn't do.
                char buffer[PATH_MAX];
                if (CFURLGetFileSystemRepresentation(parentURL, PR_TRUE, (UInt8 *)buffer, sizeof(buffer))) {
					rv = NS_NewNativeLocalFile(nsDependentCString(buffer), PR_TRUE, &binDir);
				}
				CFRelease(parentURL);
			}
			CFRelease(bundleURL);
		}
	}
#elif defined(XP_UNIX)
    Dl_info dlinfo;
	if ( dladdr((void*)GetCurrentProcessDirectory, &dlinfo) != 0 ) {
        char* str = strdup(dlinfo.dli_fname);
        char * lastSlash = strrchr(str,'/');
        if (lastSlash) {
            *lastSlash = '\0';
            rv = NS_NewNativeLocalFile(nsDependentCString(str), PR_TRUE, &binDir);
        } else {
            *str = '\0';
            rv = NS_ERROR_FILE_UNRECOGNIZED_PATH;
        }
        if (rv == NS_ERROR_FILE_UNRECOGNIZED_PATH) {
            char* cwd = getcwd(NULL, 256);
			if (cwd) {
				rv = NS_NewNativeLocalFile(nsDependentCString(cwd), PR_TRUE, &binDir);
				if (NS_SUCCEEDED(rv)) {
					if (!((strlen(str) == 1) && (str[0] == '.'))) {
						rv = binDir->AppendRelativeNativePath(nsDependentCString(str));
					}
				}
				free(cwd);
			}
        }
        free(str);
    }
#endif
    if (NS_SUCCEEDED(rv)) {
        *aFile = binDir;
        return NS_OK;
    }

    NS_ERROR("unable to get current process directory");
    return NS_ERROR_FAILURE;
}

static nsresult
DeleteRegFiles(nsIFile *aDir)
{
	nsCOMPtr<nsIFile> file;
	nsresult rv = aDir->Clone(getter_AddRefs(file));
    NS_ENSURE_SUCCESS(rv, rv);
    NS_ENSURE_TRUE(file, NS_ERROR_FAILURE);

	file->AppendNative(NS_LITERAL_CSTRING("compreg_" JAXER_BUILD_ID ".dat"));
	file->Remove(PR_FALSE);

	file->SetNativeLeafName(NS_LITERAL_CSTRING("xpti_" JAXER_BUILD_ID ".dat"));
	file->Remove(PR_FALSE);

	file->SetNativeLeafName(NS_LITERAL_CSTRING(".autoreg"));
	file->Remove(PR_FALSE);

	return NS_OK;
}

static void
LoadPlatformDirectory(nsIFile* aBundleDirectory,
                      nsCOMArray<nsIFile> &aDirectories)
{
	nsresult rv;
	nsCOMPtr<nsIFile> platformDir;
	rv = aBundleDirectory->Clone(getter_AddRefs(platformDir));
	if (NS_FAILED(rv)) {
		return;
	}

	platformDir->AppendNative(NS_LITERAL_CSTRING("platform"));

#ifdef TARGET_OS_ABI
	nsCOMPtr<nsIFile> platformABIDir;
	rv = platformDir->Clone(getter_AddRefs(platformABIDir));
	if (NS_FAILED(rv)) {
		return;
	}
#endif

	platformDir->AppendNative(NS_LITERAL_CSTRING(OS_TARGET));

	PRBool exists;
	if (NS_SUCCEEDED(platformDir->Exists(&exists)) && exists) {
		aDirectories.AppendObject(platformDir);
	}

#ifdef TARGET_OS_ABI
	platformABIDir->AppendNative(NS_LITERAL_CSTRING(TARGET_OS_ABI));
	if (NS_SUCCEEDED(platformABIDir->Exists(&exists)) && exists) {
		aDirectories.AppendObject(platformABIDir);
	}
#endif
}

static void
LoadExtensionDirs(nsINIParser &parser, const char *aSection, nsCOMArray<nsIFile> &aDirectories)
{
	nsresult rv;
	PRInt32 i = 0;
	do {
		nsCAutoString buf("Extension");
		buf.AppendInt(i++);

		nsCAutoString path;
		rv = parser.GetString(aSection, buf.get(), path);
		if (NS_FAILED(rv)) {
			return;
		}

		nsCOMPtr<nsILocalFile> dir = do_CreateInstance("@mozilla.org/file/local;1", &rv);
		if (NS_FAILED(rv)) {
			continue;
		}

		rv = dir->SetPersistentDescriptor(path);
		if (NS_FAILED(rv)) {
			continue;
		}

		aDirectories.AppendObject(dir);
		LoadPlatformDirectory(dir, aDirectories);
	} while (PR_TRUE);
}

static void
LoadDirsIntoArray(nsCOMArray<nsIFile>& aSourceDirs,
                  const char *const* aAppendList,
                  nsCOMArray<nsIFile>& aDirectories)
{
	nsCOMPtr<nsIFile> appended;
	PRBool exists;

	for (PRInt32 i = 0; i < aSourceDirs.Count(); ++i) {
		aSourceDirs[i]->Clone(getter_AddRefs(appended));
		if (!appended) {
			continue;
		}

		for (const char *const *a = aAppendList; *a; ++a) {
			appended->AppendNative(nsDependentCString(*a));
		}

		if (NS_SUCCEEDED(appended->Exists(&exists)) && exists) {
			aDirectories.AppendObject(appended);
		}
	}
}

static const char *const kAppendCompDir[] = { "components", nsnull };
static const char *const kAppendChromeManifests[] = { "chrome.manifest", nsnull };
static const char *const kAppendPrefDir[] = { "defaults", "preferences", nsnull };

/////////////////////////////////////////
// AppDirectoryProvider
//
AppDirectoryProvider::AppDirectoryProvider()
: mBinDir(nsnull),
mTmpDir(nsnull),
mCleanupRegFiles(PR_FALSE),
mTempPath(),
mExtPath(),
mExtensionDirs(nsnull)
{
}

nsresult
AppDirectoryProvider::InitBinDir()
{
    nsresult rv = NS_OK;
    if (!mBinDir) {
        rv = GetCurrentProcessDirectory(getter_AddRefs(mBinDir));
        if (NS_FAILED(rv))
        {
            rv = NS_GetSpecialDirectory(NS_XPCOM_CURRENT_PROCESS_DIR,
                                        getter_AddRefs(mBinDir));
        }
    }
    return rv;
}

nsresult
AppDirectoryProvider::InitTmpDir()
{
    nsresult rv = NS_OK;
    if (!mTmpDir) {
        if (mTempPath.Length() > 0) {
            nsCOMPtr<nsILocalFile> tmpFile;
            rv = NS_NewLocalFile(NS_ConvertUTF8toUTF16(mTempPath), PR_TRUE, getter_AddRefs(tmpFile));
			if (NS_SUCCEEDED(rv)) {
				mTmpDir = tmpFile;
			} else {
				fprintf(stderr, "Jaxer: Specified temporary directory path could not be resolved.");
			}
        }
		if (!mTmpDir) {
            rv = InitBinDir();
            NS_ENSURE_SUCCESS(rv, rv);
            rv = mBinDir->GetParent(getter_AddRefs(mTmpDir));
            NS_ENSURE_SUCCESS(rv, rv);
            rv = mTmpDir->AppendNative(NS_LITERAL_CSTRING("tmp"));
            NS_ENSURE_SUCCESS(rv, rv);
        }

        rv = mTmpDir->Create(nsIFile::DIRECTORY_TYPE, 0775); // may return NS_ERROR_FILE_ALREADY_EXISTS
        if (rv != NS_ERROR_FILE_ALREADY_EXISTS)
            NS_ENSURE_SUCCESS(rv, rv);
		if (mCleanupRegFiles) {
			mCleanupRegFiles = PR_FALSE;
			DeleteRegFiles(mTmpDir);
		}
        return NS_OK;
    }
    return rv;
}

nsresult
AppDirectoryProvider::InitExtensionsDirs()
{
    nsresult rv;
	if (!mExtensionDirs) {
		mExtensionDirs = new nsCOMArray<nsIFile>();

		nsCOMPtr<nsILocalFile> extensionsDir;
		if (mExtPath.Length() > 0) {
            nsCOMPtr<nsILocalFile> dir;
            rv = NS_NewLocalFile(NS_ConvertUTF8toUTF16(mExtPath), PR_TRUE, getter_AddRefs(dir));
			if (NS_SUCCEEDED(rv)) {
				extensionsDir = dir;
			} else {
				fprintf(stderr, "Jaxer: Specified extensions path could not be resolved.");
			}
		}
		if (!extensionsDir) {
			rv = InitBinDir();
			NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

			nsCOMPtr<nsIFile> dir;
			mBinDir->Clone(getter_AddRefs(dir));
            NS_ENSURE_TRUE(dir, NS_ERROR_FAILURE);

			dir->AppendNative(NS_LITERAL_CSTRING("extensions"));
			extensionsDir = do_QueryInterface(dir);
		}
		NS_ENSURE_TRUE(extensionsDir, NS_ERROR_FAILURE);

		PRBool exists;
		PRBool isDir;
		if (NS_SUCCEEDED(extensionsDir->Exists(&exists)) && exists
			&& NS_SUCCEEDED(extensionsDir->IsDirectory(&isDir)) && isDir) {
			nsCOMPtr<nsISimpleEnumerator> entries;
			rv = extensionsDir->GetDirectoryEntries(getter_AddRefs(entries));
			NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);

			PRBool hasMore;
			while (NS_SUCCEEDED(entries->HasMoreElements(&hasMore)) && hasMore) {
				nsCOMPtr<nsISupports> elem;
				entries->GetNext(getter_AddRefs(elem));
				nsCOMPtr<nsILocalFile> dir(do_QueryInterface(elem));
				NS_ENSURE_TRUE(dir, NS_ERROR_FAILURE);

				if (NS_SUCCEEDED(dir->IsDirectory(&isDir)) && !isDir) {
					PRInt64 fileSize;
					PRInt32 len;
					if (NS_FAILED(dir->GetFileSize(&fileSize)) || !(len = nsInt64(fileSize)) || (len > 512)) {
						continue;
					}
					nsCString buffer;
					NS_ENSURE_TRUE(EnsureStringLength(buffer, len), NS_ERROR_OUT_OF_MEMORY);
					
					PRFileDesc *fd;
					rv = dir->OpenNSPRFileDesc(PR_RDONLY, 444, &fd);
					if (NS_FAILED(rv)) {
						continue;
					}
					len = PR_Read(fd, buffer.BeginWriting(), len);
					PR_Close(fd);

					buffer.Truncate(len);
					rv = NS_NewLocalFile(NS_ConvertUTF8toUTF16(buffer), PR_TRUE, getter_AddRefs(dir));
					if (NS_FAILED(rv) || NS_FAILED(dir->Exists(&exists)) || !exists) {
						continue;
					}					
				}
				if (NS_FAILED(dir->IsDirectory(&isDir)) || !isDir) {
					continue;
				}
				nsCOMPtr<nsIFile> installRdf;
				dir->Clone(getter_AddRefs(installRdf));
				NS_ENSURE_TRUE(installRdf, NS_ERROR_FAILURE);
				installRdf->AppendNative(NS_LITERAL_CSTRING("install.rdf"));

				if (NS_SUCCEEDED(installRdf->Exists(&exists)) && exists) {
					mExtensionDirs->AppendObject(dir);
					LoadPlatformDirectory(dir, *mExtensionDirs);
				}
			}
		}
	}
	return NS_OK;
}

void
AppDirectoryProvider::DoShutdown()
{
	mBinDir = nsnull;
	mTmpDir = nsnull;
	if (mExtensionDirs) {
		mExtensionDirs->Clear();
		delete mExtensionDirs;
		mExtensionDirs = nsnull;
	}
}

////////////////////////////////////////////////////////////////////////
// nsISupports
//
NS_IMPL_QUERY_INTERFACE2(AppDirectoryProvider,
                  nsIDirectoryServiceProvider,
                  nsIDirectoryServiceProvider2)

NS_IMETHODIMP_(nsrefcnt)
AppDirectoryProvider::AddRef()
{
  return 1;
}

NS_IMETHODIMP_(nsrefcnt)
AppDirectoryProvider::Release()
{
  return 0;
}

////////////////////////////////////////////////////////////////////////
// nsIDirectoryServiceProvider
//
NS_IMETHODIMP
AppDirectoryProvider::GetFile
    (const char *aProperty, PRBool *aPersistent,
                               nsIFile* *aFile)
{
    nsresult rv;
	*aFile = nsnull;
    if ( !strcmp(aProperty, NS_XPCOM_INIT_CURRENT_PROCESS_DIR)
        || !strcmp(aProperty, NS_XPCOM_CURRENT_PROCESS_DIR)
        || !strcmp(aProperty, NS_OS_CURRENT_PROCESS_DIR)
        || !strcmp(aProperty, NS_GRE_DIR)) {
        rv = InitBinDir();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
        return mBinDir->Clone(aFile);
    } 
    if (!strcmp(aProperty, NS_OS_TEMP_DIR)
		/*|| !strcmp(aProperty, NS_APP_USER_PROFILE_50_DIR)*/) {
        rv = InitTmpDir();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
        return mTmpDir->Clone(aFile);
    }
    if (!strcmp(aProperty, NS_XPCOM_COMPONENT_REGISTRY_FILE)) {
        rv = InitTmpDir();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
        rv = mTmpDir->Clone(aFile);
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
        return (*aFile)->AppendNative(NS_LITERAL_CSTRING("compreg_" JAXER_BUILD_ID ".dat"));
    }
    if (!strcmp(aProperty, NS_XPCOM_XPTI_REGISTRY_FILE)) {
        rv = InitTmpDir();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
        rv = mTmpDir->Clone(aFile);
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
        return (*aFile)->AppendNative(NS_LITERAL_CSTRING("xpti_" JAXER_BUILD_ID ".dat"));
    }

    return NS_ERROR_FAILURE;
}

NS_IMETHODIMP
AppDirectoryProvider::GetFiles(const char* aProperty,
                                nsISimpleEnumerator** aResult)
{
    nsresult rv;
    *aResult = nsnull;
	if (!strcmp(aProperty, NS_XPCOM_COMPONENT_DIR_LIST)) {
		rv = InitExtensionsDirs();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
		nsCOMArray<nsIFile> directories;
		LoadDirsIntoArray(*mExtensionDirs, kAppendCompDir, directories);
		return NS_NewArrayEnumerator(aResult, directories);
	}
	if (!strcmp(aProperty, NS_EXT_PREFS_DEFAULTS_DIR_LIST)) {
		rv = InitExtensionsDirs();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
		nsCOMArray<nsIFile> directories;
		LoadDirsIntoArray(*mExtensionDirs, kAppendPrefDir, directories);
		return NS_NewArrayEnumerator(aResult, directories);
	}
	if (!strcmp(aProperty, NS_CHROME_MANIFESTS_FILE_LIST)) {
		rv = InitExtensionsDirs();
        NS_ENSURE_SUCCESS(rv, NS_ERROR_FAILURE);
		nsCOMArray<nsIFile> files;
		nsCOMPtr<nsIFile> chromeDir;
        rv = NS_GetSpecialDirectory(NS_APP_CHROME_DIR,
                                    getter_AddRefs(chromeDir));
		if (NS_SUCCEEDED(rv) && chromeDir) {
			files.AppendObject(chromeDir);
		}
		LoadDirsIntoArray(*mExtensionDirs, kAppendChromeManifests, files);
		return NS_NewArrayEnumerator(aResult, files);
	}

    return NS_ERROR_FAILURE;
}
