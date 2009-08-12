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

#import "MampApplicationDelegate.h"
#import "MampTasks.h"
#import "NSStringEreg.h"


@implementation MampApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)app {
	if(mainMamp != nil){
	[[mainMamp progressIndicator]  setHidden:NO];
	[[mainMamp progressIndicator] startAnimation:self];
	
	}
		if([preferences boolForKey:@"stopServers"]){
			BOOL apacheIsRunning = [[MampTasks sharedInstance] serverIsRunning:HTTPDPID_LOCATION_STRING];
			BOOL mysqlIsRunning = [[MampTasks sharedInstance] serverIsRunning:MYSQLPID_LOCATION_STRING];
			BOOL jaxerIsRunning = [[MampTasks sharedInstance] serverIsRunning:JAXERPID_LOCATION_STRING];

			if(apacheIsRunning){
				[[MampTasks sharedInstance] stopApacheServer];
			}
			if(mysqlIsRunning){
				[[MampTasks sharedInstance] stopMySQLSever];
			}
			if(jaxerIsRunning){
				[[MampTasks sharedInstance] stopJaxerServer];
			}
		}
	return NSTerminateNow; 

}

+ (void)initialize
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString *oldPrefsPath = [@"~/Library/Preferences/com.webEdition.MAMP.plist" stringByExpandingTildeInPath];
	NSString *newPrefsPath = [@"~/Library/Preferences/com.living-e.MAMP.plist" stringByExpandingTildeInPath];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSEnumerator *keyEnumerator = nil;
	NSString *key = nil;
	NSDictionary *dict = nil;
	
	if([fileManager fileExistsAtPath:oldPrefsPath] && ![fileManager fileExistsAtPath:newPrefsPath])
	{
		dict = [NSDictionary dictionaryWithContentsOfFile:oldPrefsPath];
		keyEnumerator = [dict keyEnumerator];
		
		while((key = [keyEnumerator nextObject]) != nil)
		{
			[defaults setObject:[dict objectForKey:key] forKey:key];
		}
		
		[defaults synchronize];
	}
}

@end
