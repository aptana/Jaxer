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

#import "MampTasks.h"
#import <Foundation/NSFileManager.h>
#import <Security/AuthorizationTags.h>
#import "NSStringEreg.h"
#import "Socket.h"

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
@interface MampTasks (Private)

- (BOOL)startStopSH:(NSString*) script path:(NSString *)aPath;
- (NSString *)whoami;
- (void)changeApacheUser;
- (int)launchTaskGetReturn:(NSString *)exec argsArray:(NSArray *)arg path:(NSString *)aPath;
- (BOOL)startStopSHAuth:(NSString*) script;
- (BOOL)killAuth:(int)pid;
- (BOOL)changeOwnershipOfMySqlFilesToUser;
- (BOOL)changeOwnershipOfApacheFilesToUser;
- (NSString *)getUserOfServer:(NSString *)pidFile;
- (BOOL)configTest;

// authentication
// Find out if the user has the appropriate authorization rights.
// This really needs to be called each time you need to know whether the user
// is authorized, since the AuthorizationRef can be invalidated elsewhere, or
// may expire after a short period of time.
- (BOOL)isAuthenticated;

// if the user is not already authorized, ask them to authenticate.
// return YES if the user is (or becomes) authorized.
- (BOOL)authenticate;

// Invalidate the user's AuthorizationRef to dispose of any acquired rights.
// It's a good idea to do this before you quit your application, else they
// will stay authorized as of the current time (4K78).
- (void)deauthenticate;

@end

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
@implementation MampTasks (Private)

// ---------------------------------------------------------------------------------
- (BOOL)startStopSH:(NSString*) script path:(NSString*)aPath
{

	NSMutableArray *theArgs = [NSMutableArray array];
	[theArgs addObject:script];

	if( [self launchTaskGetReturn:SH_LOCATION argsArray:theArgs path:aPath] == 0){
		return YES;
	}
	return NO;
}

// ---------------------------------------------------------------------------------
- (NSString *)whoami
{
	return NSUserName();
}

// ---------------------------------------------------------------------------------
- (void)changeApacheUser
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];
	NSMutableString *outString = [NSMutableString stringWithString:contStr];
	
	if([outString ereg:@"([\r\n]User[ \t]+)([^\r\n]+)" regs:regs]){
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],[regs objectAtIndex:2]]
			withString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],[self whoami]]
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
	}

	[outString writeToFile:HTTPDCONF_LOCATION_STRING atomically:YES];

	[regs release];    
}

// ---------------------------------------------------------------------------------
- (int)launchTaskGetReturn:(NSString *)exec argsArray:(NSArray *)args path:(NSString *)aPath
{
	NSTask        *pipeTask = [NSTask new];
	int status;
	[pipeTask setLaunchPath:exec];
	[pipeTask setArguments:args];
	[pipeTask setCurrentDirectoryPath:aPath];
	[pipeTask launch];
	[pipeTask waitUntilExit];
	status = [pipeTask terminationStatus];
	[pipeTask release];
     return status;
}

// ---------------------------------------------------------------------------------
- (BOOL)startStopSHAuth:(NSString*) script
{
    FILE* outpipe = NULL;
    char* args[2];
    OSStatus err = 0;
    NSMutableData* outputData = nil;
    NSMutableData* tempData = nil;
    NSString* outString = nil;
    int len = 0;
 
    if(![self authenticate])
        return NO;
    
    //the arguments parameter to AuthorizationExecuteWithPrivileges is
    //a NULL terminated array of C string pointers.
    args[0] = (char*)[script fileSystemRepresentation];
    args[1] = NULL;
    
	 err = AuthorizationExecuteWithPrivileges(authorizationRef,
											[ SH_LOCATION fileSystemRepresentation],
											0, args, &outpipe);
	
	if(!outpipe)
	{
		NSLog(@"Error opening pipe: %@",SH_LOCATION);
		NSBeep();
		return NO;
	}
	if(err!=0)
	{
		NSBeep();
		NSLog(@"Error %d in startStopSHAuth",err);
	}

	tempData = [NSMutableData dataWithLength:512];
	outputData = [NSMutableData data];
	
	do
	{
		[tempData setLength:512];
		len = fread([tempData mutableBytes],1,512,outpipe);
		if(len>0)
		{
			[tempData setLength:len];
			[outputData appendData:tempData];        
		}
	} while(len==512);
	
	pclose(outpipe);
	outString = [[NSString alloc]initWithData:outputData encoding:NSASCIIStringEncoding];
	[outString autorelease];
	//return ([outString rangeOfString:HTTPD_STOPPED_STRING].length > 0);

	return YES;
}

// ---------------------------------------------------------------------------------
- (BOOL)killAuth:(int)pid
{
    char *args[2];
    OSStatus err = 0;
 
    if(![self authenticate]) return NO;
    
    args[0] = (char *)[[NSString stringWithFormat:@"%i", pid] UTF8String];
    args[1] = NULL;
    
	err = AuthorizationExecuteWithPrivileges(authorizationRef, [KILL_LOCATION fileSystemRepresentation], 0, args, NULL);
	
	if(err != 0)
	{
		NSBeep();
		NSLog(@"Error %d in killAuth", err);
		return NO;
	}

	return YES;
}

// ---------------------------------------------------------------------------------
// checks httpd.conf syntax
// not used at the moment
- (BOOL)configTest {
	NSMutableArray *theArgs = [NSMutableArray array];
	[theArgs addObject:@"-t"];

	if( [self launchTaskGetReturn:HTTPD_LOCATION_STRING argsArray:theArgs path:@"."] == 0){
		return YES;
	}
	return NO;
}

// ---------------------------------------------------------------------------------
- (BOOL)changeOwnershipOfMySqlFilesToUser
{
    NSString *user = NSUserName();
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSDictionary *fileAttributes = nil;
	BOOL needsChange = NO;
	int currentArg = 2;
	char *args[5];

	args[0] = (char *)[@"-R" UTF8String];
	args[1] = (char *)[user UTF8String];
	
	if([fileManager fileExistsAtPath:MYSQL_TMP])
	{
		fileAttributes = [fileManager fileAttributesAtPath:MYSQL_TMP traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[MYSQL_TMP fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:MYSQLERROR_LOG])
	{
		fileAttributes = [fileManager fileAttributesAtPath:MYSQLERROR_LOG traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[MYSQLERROR_LOG fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if(needsChange)
	{
		OSStatus err = 0;
	 
		if(![self authenticate]) return NO;
		
		args[currentArg] = NULL;
		
		err = AuthorizationExecuteWithPrivileges(authorizationRef, [CHOWN_LOCATION fileSystemRepresentation], 0, args, NULL);
		
		if(err != 0)
		{
			NSBeep();
			NSLog(@"Error %d in changeOwnershipOfMySqlFilesToUser", err);
			return NO;
		}
	}
	
	return YES;
}

// ---------------------------------------------------------------------------------
- (BOOL)changeOwnershipOfApacheFilesToUser
{
    NSString *user = NSUserName();
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSDictionary *fileAttributes = nil;
	BOOL needsChange = NO;
	int currentArg = 2;
	char *args[11];

	args[0] = (char *)[@"-R" UTF8String];
	args[1] = (char *)[user UTF8String];
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/tmp/php"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/tmp/php" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/tmp/php" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/tmp/eaccelerator"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/tmp/eaccelerator" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/tmp/eaccelerator" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/tmp/xcache"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/tmp/xcache" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/tmp/xcache" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/tmp/phpcore"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/tmp/phpcore" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/tmp/phpcore" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/tmp/fcgi_ipc"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/tmp/fcgi_ipc" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/tmp/fcgi_ipc" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/logs/apache_error_log"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/logs/apache_error_log" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/logs/apache_error_log" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/logs/php_error.log"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/logs/php_error.log" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/logs/php_error.log" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if([fileManager fileExistsAtPath:@"/Applications/MAMP/Library/logs/cgisock"])
	{
		fileAttributes = [fileManager fileAttributesAtPath:@"/Applications/MAMP/Library/logs/cgisock" traverseLink:NO];
		
		if(fileAttributes != nil && ![[fileAttributes fileOwnerAccountName] isEqualToString:user])
		{
			args[currentArg] = (char *)[@"/Applications/MAMP/Library/logs/cgisock" fileSystemRepresentation];
			currentArg++;
			needsChange = YES;
		}
	}
	
	if(needsChange)
	{
		OSStatus err = 0;
	 
		if(![self authenticate]) return NO;
		
		args[currentArg] = NULL;
		
		err = AuthorizationExecuteWithPrivileges(authorizationRef, [CHOWN_LOCATION fileSystemRepresentation], 0, args, NULL);
		
		if(err != 0)
		{
			NSBeep();
			NSLog(@"Error %d in changeOwnershipOfMySqlFilesToUser", err);
			return NO;
		}
	}
	
	return YES;
}

// ---------------------------------------------------------------------------------
- (NSString *)getUserOfServer:(NSString *)pidFile
{
	NSStringEncoding encoding = NSASCIIStringEncoding;
	NSString *contentsPidFile = nil;
	int pid = 0;
	NSData *pidData = nil;
	NSString *user = nil;
	
	pidData = [NSData dataWithContentsOfFile:pidFile];
	if(pidData != nil) contentsPidFile = [[[NSMutableString alloc] initWithData:pidData encoding:encoding] autorelease];

	if(contentsPidFile != nil)
	{
		pid = [contentsPidFile intValue];
		
		if(pid != 0)
		{
		
			FILE *fp = popen([[NSString stringWithFormat:@"/bin/ps -o logname -p %i", pid] UTF8String], "r");
			
			if(fp != NULL)
			{
				// read
				char *buffer1 = (char*)malloc(512);
				char *buffer2 = (char*)malloc(512);
				
				if(fscanf(fp, "%s%s", buffer1, buffer2) == 2)
				{
					user = [[[NSString alloc] initWithUTF8String:buffer2] autorelease];
				}
				
				free(buffer1);
				free(buffer2);
				fflush(fp);
				pclose(fp);
			}
		}
	}
	
	return user;
}

//============================================================================
//	- (BOOL)isAuthenticated
//============================================================================
// Find out if the user has the appropriate authorization rights.
// This really needs to be called each time you need to know whether the user
// is authorized, since the AuthorizationRef can be invalidated elsewhere, or
// may expire after a short period of time.
// As far as I know, there is no way to be notified when your AuthorizationRef
// expires.
//
- (BOOL)isAuthenticated
{
    AuthorizationRights rights;
    AuthorizationRights *authorizedRights;
    AuthorizationFlags flags;
    AuthorizationItem items[12];
    char *shPath = (char *)[SH_LOCATION fileSystemRepresentation];
    OSStatus err = 0;
    BOOL authorized = NO;

    if(authorizationRef==NULL)
    {
        //If we haven't created an AuthorizationRef yet, create one now with the
        //kAuthorizationFlagDefaults flags only to get the user's current
        //authorization rights.
        rights.count=0;
        rights.items = NULL;
        
        flags = kAuthorizationFlagDefaults;
    
        err = AuthorizationCreate(&rights,
                                kAuthorizationEmptyEnvironment, flags,
                                &authorizationRef);
        
    }
    
    //There should be one item in the AuthorizationItems array for each
    //type of right you want to acquire.
    //The data in the value and valueLength is dependant on which right you
    //want to acquire. For the right to execute tools as root,
    //kAuthorizationRightExecute, they should hold a pointer to a C string 
    //containing the path to the tool you want to execute, and 
    //the length of the C string path.
    //There needs to be one item for each tool you want to execute.
    items[0].name = kAuthorizationRightExecute;
    items[0].value = shPath;
    items[0].valueLength = strlen(shPath);
    items[0].flags = 0;
        
    rights.count = 1;
    rights.items = items;
    
    flags = kAuthorizationFlagExtendRights;
    
    //Since we've specified kAuthorizationFlagExtendRights and
    //haven't specified kAuthorizationFlagInteractionAllowed, if the
    //user isn't currently authorized to execute tools as root,
    //they won't be asked for a password and err will indicate
    //an authorization failure.
    err = AuthorizationCopyRights(authorizationRef,&rights,
                        kAuthorizationEmptyEnvironment,
                        flags,&authorizedRights);

    authorized = (errAuthorizationSuccess==err);
    if(authorized)
    {
        //we don't need these items, and they need to be disposed of.
        AuthorizationFreeItemSet(authorizedRights);
    }
    return authorized;
}

//============================================================================
//	- (void)deauthenticate
//============================================================================
// Invalidate the user's AuthorizationRef to dispose of any acquired rights.
// It's a good idea to do this before you quit your application, else they
// will stay authorized as of the current time (4K78).
//
- (void)deauthenticate
{
    if(authorizationRef)
    {
        //dispose of any rights our AuthorizationRef has acquired, and null it out
        //so we get a new one next time we need one.
        AuthorizationFree(authorizationRef,kAuthorizationFlagDestroyRights);
        authorizationRef = NULL;
    }
}

//============================================================================
//	- (BOOL)fetchPassword
//============================================================================
// Fetch user's password if needed. If the user is already authorized, they
// will not be asked for their password again.
//
- (BOOL)fetchPassword
{
    AuthorizationRights rights;
    AuthorizationRights *authorizedRights;
    AuthorizationFlags flags;
    AuthorizationItem items[12];
    char *shPath = (char *)[SH_LOCATION fileSystemRepresentation];
    OSStatus err = 0;
    BOOL authorized = NO;

    items[0].name = kAuthorizationRightExecute;
    items[0].value = shPath;
    items[0].valueLength = strlen(shPath);
    items[0].flags = 0;
        
    rights.count = 1;
    rights.items = items;
       
    flags = kAuthorizationFlagInteractionAllowed 
                | kAuthorizationFlagExtendRights;

    //Here, since we've specified kAuthorizationFlagExtendRights and
    //have also specified kAuthorizationFlagInteractionAllowed, if the
    //user isn't currently authorized to execute tools as root 
    //(kAuthorizationRightExecute),they will be asked for their password. 
    //The err return value will indicate authorization success or failure.
    err = AuthorizationCopyRights(authorizationRef,&rights,
                        kAuthorizationEmptyEnvironment,
                        flags,&authorizedRights);
    authorized = (errAuthorizationSuccess==err);
    if(authorized)
    {
        AuthorizationFreeItemSet(authorizedRights);
    }                                                    
    return authorized;

}

//============================================================================
//	- (BOOL)authenticate
//============================================================================
// if the user is not already authorized, ask them to authenticate.
// return YES if the user is (or becomes) authorized.
//
- (BOOL)authenticate
{
    if(![self isAuthenticated])
    {
        return [self fetchPassword]; //added return
    }
    return [self isAuthenticated];
}

@end

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
@implementation MampTasks

// singleton
static id sharedTask = nil;

// ---------------------------------------------------------------------------------
+ (id)sharedInstance
{
	return (sharedTask ? sharedTask : [[self alloc] init]);
}

// ---------------------------------------------------------------------------------
- (id)init
{
	if(sharedTask != nil)
	{
		[self dealloc];
	}
	else
	{
		sharedTask = [super init];
		
		if(sharedTask != nil)
		{
			authorizationRef = NULL;
			mampStarted = NO;
			apacheStartetAsRoot = NO;
		}
	}
	
	return sharedTask;
}

// ---------------------------------------------------------------------------------
- (void)dealloc
{
    [self deauthenticate];
    [super dealloc];
}

// ---------------------------------------------------------------------------------
- (void)setMampStarted:(BOOL)flag
{
	mampStarted = flag;
}

// ---------------------------------------------------------------------------------
- (void)startApacheServer
{
	[self changeApacheUser];
	[self changeOwnershipOfApacheFilesToUser];
	
	if([[self getApachePortFromConf] intValue] < 1024){
		[self startStopSHAuth:START_APACHE_LOCATION_STRING];
		apacheStartetAsRoot = YES;
	}else{
		[self startStopSH:START_APACHE_LOCATION_STRING path:@"."];
		apacheStartetAsRoot = NO;
	}
}

// ---------------------------------------------------------------------------------
- (void)stopApacheServer
{
	if(!mampStarted) return;
	
	BOOL stopAsRoot = NO;
	
	if(apacheStartetAsRoot) stopAsRoot = YES;
	
	if(!stopAsRoot)
	{
		if(![[self getUserOfServer:HTTPDPID_LOCATION_STRING] isEqualToString:NSUserName()]) stopAsRoot = YES;
	}
	
	// try gracefully first
	if(stopAsRoot){
		[self startStopSHAuth:STOP_APACHE_LOCATION_STRING];
	}else{
		[self startStopSH:STOP_APACHE_LOCATION_STRING path:@"."];
	}
	
	// wait a little and if it doesn't work -> kill as root
	[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
	
	if([[NSFileManager defaultManager] fileExistsAtPath:HTTPDPID_LOCATION_STRING] && [self serverIsRunning:HTTPDPID_LOCATION_STRING])
	{
		NSLog(@"apache didn't stop -> kill");
		
		NSString *contentsPidFile = nil;
		NSData *pidData = [NSData dataWithContentsOfFile:HTTPDPID_LOCATION_STRING];;
		
		if(pidData != nil) contentsPidFile = [[[NSMutableString alloc] initWithData:pidData encoding:NSASCIIStringEncoding] autorelease];
		
		if(contentsPidFile != nil)
		{
			int pid = [contentsPidFile intValue];
			
			if(pid != 0)
			{
				[self killAuth:pid];
			}
		}
	}
	
	// wait 5 seconds and if apache doesn't run anymore -> change ownership if necessary
	int i = 0;
	
	for(i = 0; i < 10; i++)
	{
		if(![[NSFileManager defaultManager] fileExistsAtPath:HTTPDPID_LOCATION_STRING])
		{
			[self changeOwnershipOfApacheFilesToUser];
			break;
		}

		[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5]];
	}
}

// ---------------------------------------------------------------------------------
- (void)startJaxerServer
{
//	[self changeOwnershipOfJaxerFilesToUser];
	[self startStopSH:START_JAXER_LOCATION_STRING path:START_JAXER_PATH];
}

// ---------------------------------------------------------------------------------
- (void)stopJaxerServer
{
	if(!mampStarted) return;

	int pid = 0;
	
	if([[NSFileManager defaultManager] fileExistsAtPath:JAXERPID_LOCATION_STRING])
	{
		NSString *contentsPidFile = nil;
		NSData *pidData = [NSData dataWithContentsOfFile:JAXERPID_LOCATION_STRING];
		
		if(pidData != nil) contentsPidFile = [[[NSMutableString alloc] initWithData:pidData encoding:NSASCIIStringEncoding] autorelease];
		
		if(contentsPidFile != nil)
		{
			pid = [contentsPidFile intValue];
		}
	}
		
	// try gracefully first
	[self startStopSH:STOP_JAXER_LOCATION_STRING path:@"."];

	// check for 15 seconds to see if Jaxer has died
	if(pid != 0)
	{
		int count = 15;
		while(--count > 0 && [self serverIsRunning:JAXERPID_LOCATION_STRING])
		{
			[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
		}
	}
	
	// if the process is still around, kill the process and remove the pid file
	if(pid != 0 && [self serverIsRunning:JAXERPID_LOCATION_STRING])
	{
		NSString *user = [self getUserOfServer:JAXERPID_LOCATION_STRING];
		
		if([user isEqualToString:[self whoami]])
		{
			[self launchTaskGetReturn:@"/bin/kill" argsArray:[NSArray arrayWithObject:[NSString stringWithFormat:@"%i", pid]] path:@"."];
		}
		else // has to be run as root
		{
			[self killAuth:pid];
		}
	}
	
	// Remove the pid file
	if([[NSFileManager defaultManager] fileExistsAtPath:JAXERPID_LOCATION_STRING])
	{
		NSFileManager *nsf = [NSFileManager defaultManager];
		[nsf removeFileAtPath:JAXERPID_LOCATION_STRING handler:nil];
	}
}

-(BOOL)sendJaxerExitCommand:(int)userPort
{
	Socket*	 	socket;
	NSString*	command;
	
	 // Create socket, connect, and send request
        
	@try {
		
		socket = [Socket socket];
		[socket connectToHostName:@"localhost" port:userPort];

		command = @"exit\n";

		[socket writeString:command];
	
		[socket close];
	
		return true;
	} 
	@catch(NSException *e)
	{
		return false;
	}
	
	return false;
}


// ---------------------------------------------------------------------------------
- (void)startMySQLSever
{
	[self changeOwnershipOfMySqlFilesToUser];
	[self startStopSH:START_MYSQL_LOCATION_STRING path:@"."];
}

// ---------------------------------------------------------------------------------
- (void)stopMySQLSever
{
	if(!mampStarted) return;

	// try gracefully first
	if(![self startStopSH:STOP_MYSQL_LOCATION_STRING path:@"."])
	{
		// if it doesn't work -> kill
		if([[NSFileManager defaultManager] fileExistsAtPath:MYSQLPID_LOCATION_STRING])
		{
			NSString *contentsPidFile = nil;
			NSData *pidData = [NSData dataWithContentsOfFile:MYSQLPID_LOCATION_STRING];;
			
			if(pidData != nil) contentsPidFile = [[[NSMutableString alloc] initWithData:pidData encoding:NSASCIIStringEncoding] autorelease];
			
			if(contentsPidFile != nil)
			{
				int pid = [contentsPidFile intValue];
				
				if(pid != 0)
				{
					NSString *user = [self getUserOfServer:MYSQLPID_LOCATION_STRING];
					
					if([user isEqualToString:[self whoami]])
					{
						[self launchTaskGetReturn:@"/bin/kill" argsArray:[NSArray arrayWithObject:[NSString stringWithFormat:@"%i", pid]] path:@"."];
					}
					else // has to be run as root
					{
						[self killAuth:pid];
					}
				}
			}
		}
	}

	// wait 5 seconds and if mysql doesn't run anymore -> change ownership if necessary
	int i = 0;
	
	for(i = 0; i < 10; i++)
	{
		[NSThread sleepUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5]];

		if(![[NSFileManager defaultManager] fileExistsAtPath:MYSQLPID_LOCATION_STRING])
		{
			[self changeOwnershipOfMySqlFilesToUser];
			break;
		}
	}
	
	// clear log
	[@"" writeToFile:MYSQLERROR_LOG atomically:YES];
}

// ---------------------------------------------------------------------------------
- (NSString *)getApachePortFromConf
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];
	[contStr ereg:@"[\r\n]Listen +([0-9]+)" regs:regs];
	[regs autorelease];    
	return ([regs count] == 0) ? Nil : [regs objectAtIndex:1];

}

// ---------------------------------------------------------------------------------
- (int)getPHPVersFromConf
{
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];
	if([contStr rangeOfString:@"LoadModule php4"].location != NSNotFound){
		return 4;
	}else if([contStr rangeOfString:@"LoadModule php5"].location != NSNotFound){
		return 5;
	}
	return 0;
}
	
// ---------------------------------------------------------------------------------
- (int)getPHPExtensionsFromConf
{
	int outvar = 0;
	NSString *contStr = [NSString stringWithContentsOfFile:([self getPHPVersFromConf] == 4) ? PHP4_INI_LOCATION_STRING : PHP5_INI_LOCATION_STRING];
	
	if([contStr ereg:@"[\r\n]zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+ZendExtensionManager\\.so" regs:Nil]){
		outvar += ZENDOPTIMIZER_PHP_Extension;
	}
	
	if(outvar == 0 && [contStr ereg:@"[\r\n]extension[[:space:]]*=[[:space:]]*apc\\.so" regs:Nil]) {
		outvar += APC_PHP_Extension;
	}
	else if([contStr ereg:@"[\r\n]zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+eaccelerator\\.so" regs:Nil]){
		outvar += EACCELERATOR_PHP_Extension;
	}
	else if([contStr ereg:@"[\r\n]zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+xcache\\.so" regs:Nil]) {
		outvar += XCACHE_PHP_Extension;
	}
	
	return outvar;
}

// ---------------------------------------------------------------------------------
- (NSString *)getMySqlPortFromConf
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:START_MYSQL_LOCATION_STRING];

	[contStr ereg:@"[\r\n]/Applications/MAMP/Library/bin/mysqld_safe --port=([0-9]+)" regs:regs];
	[regs autorelease];    
	return ([regs count] == 0) ? Nil : [regs objectAtIndex:1];	
}

// ---------------------------------------------------------------------------------
- (NSString *)getJaxerPortFromConf
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:START_JAXER_LOCATION_STRING];

	[contStr ereg:@"[\r\n]/Applications/Aptana_Jaxer/jaxer/JaxerManager --pid-file=/Applications/Aptana_Jaxer/jaxer/jaxer.pid --commandport=([0-9]+)" regs:regs];
	[regs autorelease];    
	return ([regs count] == 0) ? Nil : [regs objectAtIndex:1];	
}

// ---------------------------------------------------------------------------------
- (NSString *)getDocumentRootFromConf
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];

	[contStr ereg:[NSString stringWithFormat:@"%@%@", DOCUMENT_ROOT_MARKER, @"[\r\n]+DocumentRoot \"([^\"]+)\""] regs:regs];
	[regs autorelease];    
	return ([regs count] == 0) ? Nil : [regs objectAtIndex:1];	
}

// ---------------------------------------------------------------------------------
- (void)writeDocumentRootToConf:(NSString *)docRoot
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];
	NSMutableString *outString = nil;
	
	if([contStr ereg:[NSString stringWithFormat:@"%@%@", DOCUMENT_ROOT_MARKER, @"([\r\n]+)DocumentRoot \"([^\"]+)\""] regs:regs]){
		outString = [NSMutableString stringWithString:contStr];
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@%@%@%@", DOCUMENT_ROOT_MARKER, [regs objectAtIndex:1], @"DocumentRoot \"", [regs objectAtIndex:2], @"\""]
			withString: [NSString stringWithFormat:@"%@%@%@%@%@", DOCUMENT_ROOT_MARKER, [regs objectAtIndex:1], @"DocumentRoot \"", docRoot, @"\""]
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		
		[outString replaceOccurrencesOfString:@"<Directory \"/Applications/Aptana_Jaxer/Apache22/htdocs\">" withString:[NSString stringWithFormat:@"<Directory \"%@\">", docRoot] options:NSLiteralSearch range: NSMakeRange(0, [outString length])];
		[outString replaceOccurrencesOfString:[NSString stringWithFormat:@"<Directory \"%@\">", [regs objectAtIndex:2]] withString:[NSString stringWithFormat:@"<Directory \"%@\">", docRoot] options:NSLiteralSearch range: NSMakeRange(0, [outString length])];
		
		[outString writeToFile:HTTPDCONF_LOCATION_STRING atomically:YES];
	}
	[regs release];    
}

// ---------------------------------------------------------------------------------
- (void)writeApachePortToConf:(NSString *)aPort
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];
	NSMutableString *outString = [NSMutableString stringWithString:contStr];
	
	if([outString ereg:@"([\r\n]Listen +)([0-9]+)" regs:regs]){
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],[regs objectAtIndex:2]]
			withString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],aPort]
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
	}
	if([outString ereg:@"([\r\n]ServerName +localhost: *)([0-9]+)" regs:regs]){
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],[regs objectAtIndex:2]]
			withString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],aPort]
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
	}
	
	[outString writeToFile:HTTPDCONF_LOCATION_STRING atomically:YES];

	[regs release];
}

// ---------------------------------------------------------------------------------
- (void)writePHPVersToConf:(int)vers
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:HTTPDCONF_LOCATION_STRING];
	NSMutableString *outString = [NSMutableString stringWithString:contStr];
	NSString *versString = [NSString stringWithFormat:@"%d",vers];
	
	if([outString ereg:@"([\r\n]LoadModule +php)([0-9]+)(_module[ \t]+modules/libphp)([0-9]+)(\\.so)" regs:regs]){
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@%@%@%@",
				[regs objectAtIndex:1],
				[regs objectAtIndex:2],
				[regs objectAtIndex:3],
				[regs objectAtIndex:4],
				[regs objectAtIndex:5]]				
				
			withString: [NSString stringWithFormat:@"%@%@%@%@%@",
				[regs objectAtIndex:1],
				versString,
				[regs objectAtIndex:3],
				versString,
				[regs objectAtIndex:5]]
				
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
	}
	
	[outString writeToFile:HTTPDCONF_LOCATION_STRING atomically:YES];

	[regs release];    
}

// ---------------------------------------------------------------------------------
- (void)writePHPExtensionsToConf:(int)value path:(NSString *)aPath
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:aPath];
	NSMutableString *outString = [NSMutableString stringWithString:contStr];
	
	// APC
	if([outString ereg:@"([\r\n]);(extension[[:space:]]*=[[:space:]]*)apc\\.so" regs:regs]){
	
		if( (value & APC_PHP_Extension) != 0 && (value & ZENDOPTIMIZER_PHP_Extension) == 0){
		
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@%@apc.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}else if([outString ereg:@"([\r\n])(extension[[:space:]]*=[[:space:]]*)apc\\.so" regs:regs]){
		
		if( (value & APC_PHP_Extension) == 0 || (value & ZENDOPTIMIZER_PHP_Extension) != 0){
	
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@;%@apc.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}

	// eAccelerator
	if([outString ereg:@"([\r\n]);(zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+)eaccelerator\\.so" regs:regs]){
	
		if( (value & EACCELERATOR_PHP_Extension) != 0){
		
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@%@eaccelerator.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}else if([outString ereg:@"([\r\n])(zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+)eaccelerator\\.so" regs:regs]){
		
		if( (value & EACCELERATOR_PHP_Extension) == 0){
	
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@;%@eaccelerator.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}
	
	// XCache
	if([outString ereg:@"([\r\n]);[[:space:]]*(zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+)xcache\\.so" regs:regs]){
	
		if( (value & XCACHE_PHP_Extension) != 0){
		
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@%@xcache.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}else if([outString ereg:@"([\r\n])(zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+)xcache\\.so" regs:regs]){
		
		if( (value & XCACHE_PHP_Extension) == 0){
	
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@;%@xcache.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}
	
	// Zend Optimizer
	if([outString ereg:@"([\r\n]);(zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+)ZendExtensionManager\\.so" regs:regs]){
	
		if( (value & ZENDOPTIMIZER_PHP_Extension) != 0){
		
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@%@ZendExtensionManager.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}else if([outString ereg:@"([\r\n])(zend_extension[[:space:]]*=[[:space:]]*[^\r\n]+)ZendExtensionManager\\.so" regs:regs]){
		
		if( (value & ZENDOPTIMIZER_PHP_Extension) == 0){
	
			[outString replaceOccurrencesOfString:[regs objectAtIndex:0]				
					
				withString: [NSString stringWithFormat: @"%@;%@ZendExtensionManager.so",
				[regs objectAtIndex:1],				
				[regs objectAtIndex:2]]				
					
				options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		}
	}
	
	[outString writeToFile:aPath atomically:YES];

	[regs release];

}

// ---------------------------------------------------------------------------------
- (void)writeMySqlPortToConf:(NSString *)aPort
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:START_MYSQL_LOCATION_STRING];
	NSMutableString *outString = nil;
	
	if([contStr ereg:@"(/Applications/MAMP/Library/bin/mysqld_safe --port=)([0-9]+)" regs:regs]){
		outString = [NSMutableString stringWithString:contStr];
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],[regs objectAtIndex:2]]
			withString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],aPort]
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		[outString writeToFile:START_MYSQL_LOCATION_STRING atomically:YES];
	}
	[regs release];    
}

// ---------------------------------------------------------------------------------
- (void)writeJaxerPortToConf:(NSString *)aPort
{
	NSMutableArray *regs = [[NSMutableArray alloc] init];
	NSString *contStr = [NSString stringWithContentsOfFile:START_JAXER_LOCATION_STRING];
	NSMutableString *outString = nil;
	
	if([contStr ereg:@"(/Applications/Aptana_Jaxer/jaxer/JaxerManager --pid-file=/Applications/Aptana_Jaxer/jaxer/jaxer.pid --commandport=)([0-9]+)" regs:regs]){
		outString = [NSMutableString stringWithString:contStr];
		[outString replaceOccurrencesOfString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],[regs objectAtIndex:2]]
			withString: [NSString stringWithFormat:@"%@%@",[regs objectAtIndex:1],aPort]
			options: NSLiteralSearch range: NSMakeRange(0, [outString length])];
		[outString writeToFile:START_JAXER_LOCATION_STRING atomically:YES];
	}
	[regs release];    
}

// ---------------------------------------------------------------------------------
- (BOOL)serverIsRunning:(NSString *) pidFile
{
    FILE* outpipe = NULL;
    NSMutableData* outputData = nil;
    NSMutableData* tempData = nil;
    NSString* outString = nil;
    NSString* popenArgs = nil;
    NSString* smbd = nil;
    int len = 0;

	if(![[NSFileManager defaultManager] fileExistsAtPath: pidFile]){
		return NO;
	}
	
    
    popenArgs = [@"/bin/cat " stringByAppendingString: pidFile ];   
    
    outpipe = popen([popenArgs fileSystemRepresentation],"r");
    if(!outpipe)
    {
        NSLog(@"Error opening pipe: %@",popenArgs);
        NSBeep();
        return nil;
    }
    
    tempData = [NSMutableData dataWithLength:512];
    outputData = [NSMutableData data];
    
    do
    {
        [tempData setLength:512];
        len = fread([tempData mutableBytes],1,512,outpipe);
        if(len>0)
        {
            [tempData setLength:len];
            [outputData appendData:tempData];        
        }
    } while(len==512);
    
    pclose(outpipe);
    outString = [[NSString alloc]initWithData:outputData encoding:NSASCIIStringEncoding];
    smbd = [NSString stringWithFormat:@"%d",[outString intValue]];
    [outString release];

    
    outpipe = NULL;
    outputData = nil;
    tempData = nil;
    outString = nil;
    popenArgs = nil;
    len = 0;
    
    popenArgs = [NSString stringWithFormat:@"/bin/ps -x -p%d",[smbd intValue]];    
    outpipe = popen([popenArgs UTF8String],"r");
    if(!outpipe)
    {
        NSLog(@"Error opening pipe: %@",popenArgs);
        NSBeep();
        return nil;
    }
    
    tempData = [NSMutableData dataWithLength:512];
    outputData = [NSMutableData data];
    
    do
    {
        [tempData setLength:512];
        len = fread([tempData mutableBytes],1,512,outpipe);
        if(len>0)
        {
            [tempData setLength:len];
            [outputData appendData:tempData];        
        }
    } while(len==512);
    
    pclose(outpipe);
    outString = [[NSString alloc]initWithData:outputData encoding:NSASCIIStringEncoding];
    [outString autorelease];
    
    if (([outString rangeOfString:smbd].length > 0) && ([smbd intValue] != 0)) return YES;
    return NO;
}

@end
