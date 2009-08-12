 /*
	This file is part of MAMP
 
	MAMP is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published
	by the Free Software Foundation; either version 2, or (at your
	option) any later version.

	MAMP is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with MAMP; see the file Licence.rtf.  If not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
	
	
*/

#import <Cocoa/Cocoa.h>
#import <Security/Authorization.h>

#import "MampStrings.h"


@interface MampTasks : NSObject
{
    AuthorizationRef	authorizationRef;
	BOOL				mampStarted; // a flag to avoid stopping the servers when the user chooses to launch MAMP PRO instead of MAMP
	BOOL				apacheStartetAsRoot;
}

// singleton
+ (id)sharedInstance;

// server
- (BOOL)serverIsRunning:(NSString *) pidFile;
- (void)startApacheServer;
- (void)stopApacheServer;
- (void)stopMySQLSever;
- (void)startMySQLSever;
- (void)stopJaxerServer;
- (void)startJaxerServer;
- (BOOL)sendJaxerExitCommand:(int)userPort;

// configuration
- (NSString *)getApachePortFromConf;
- (int)getPHPVersFromConf;
- (int)getPHPExtensionsFromConf;
- (NSString *)getMySqlPortFromConf;

- (NSString *)getJaxerPortFromConf;
- (void)writeJaxerPortToConf:(NSString *)aPort;


- (NSString *)getDocumentRootFromConf;
- (void)writeDocumentRootToConf:(NSString *)docRoot;
- (void)writeApachePortToConf:(NSString *)aPort;
- (void)writePHPVersToConf:(int)vers;
- (void)writePHPExtensionsToConf:(int)value path:(NSString *)aPath;
- (void)writeMySqlPortToConf:(NSString *)aPort;

// misc
- (void)setMampStarted:(BOOL)flag;

@end

