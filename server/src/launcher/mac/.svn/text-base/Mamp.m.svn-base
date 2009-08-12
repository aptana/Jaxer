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
#import "MAMP.h"
#import "MampTasks.h"
#import "NSStringEreg.h"

MAMP *mainMamp = nil;


@implementation MAMP

-(id)init
{
	if(self = [super init]){
		if(![[NSFileManager defaultManager] fileExistsAtPath:HTTPD_LOCATION_STRING]){
			NSRunAlertPanel( NSLocalizedString(@"wrongFolderAlertHeadline", nil),
				NSLocalizedString(@"wrongFolderAlertText", nil), 
				NSLocalizedString(@"ok", nil), nil, nil);
			[NSApp terminate:self];
		}


		SInt32 macOSVersion = 0;
		Gestalt(gestaltSystemVersion, &macOSVersion);
		
		if (macOSVersion < 0x1040)
		{
			NSRunAlertPanel( NSLocalizedString(@"key_sorry", nil),
				NSLocalizedString(@"key_osTooLow", nil), 
				NSLocalizedString(@"ok", nil), nil, nil);
			[NSApp terminate:self];
		}

#ifdef MYSQL_SUPPORT
		mysqlPidLocation = nil;
#endif
		grayImage = [NSImage imageNamed:@"light_gray.png"];
		greenImage = [NSImage imageNamed:@"light_green.png"];
		redImage = [NSImage imageNamed:@"light_red.png"];
		clickTimer = nil;
		isStartBut = YES;
		
		// create a dictionary
		NSMutableDictionary *defaultPrefs = [NSMutableDictionary dictionary];
		// put default prefs in the dictionary
		[defaultPrefs setObject: [NSNumber numberWithBool:YES] forKey: @"startServers"];
		[defaultPrefs setObject: [NSNumber numberWithBool:YES] forKey: @"stopServers"];
		[defaultPrefs setObject: [NSNumber numberWithBool:YES] forKey: @"openPage"];
		[defaultPrefs setObject: [NSNumber numberWithBool:YES] forKey: @"checkForMampPro"];
		[defaultPrefs setObject: STARTPAGE_URL_STRING forKey: @"startPage"];
		// register the dictionary of defaults
		[preferences registerDefaults: defaultPrefs];
		mainMamp = self;
		
	}
	return self;
}

- (IBAction)quitStartAll:(id)sender
{
	[progressIndicator setHidden:NO];
	[progressIndicator startAnimation:self];
	clickTimer = [[NSTimer scheduledTimerWithTimeInterval:0.2 
								target:self
								selector:@selector(doClickTimer:)
								userInfo:nil
								repeats:YES] retain];
								
	NSString *startAllText = NSLocalizedString(@"StartAllServers", nil);

	if([allButton title] == startAllText){
		isStartBut = YES;
		[[MampTasks sharedInstance] startJaxerServer];
#ifdef MYSQL_SUPPORT
		[[MampTasks sharedInstance] startMySQLSever];
#endif
		[[MampTasks sharedInstance] startApacheServer];
	}else{
		isStartBut = NO;
		[[MampTasks sharedInstance] stopJaxerServer];
#ifdef MYSQL_SUPPORT
		[[MampTasks sharedInstance] stopMySQLSever];
#endif
		[[MampTasks sharedInstance] stopApacheServer];
	}
	[progressIndicator stopAnimation:self];
	[progressIndicator setHidden:YES];
}

- (IBAction)quitStartJaxer:(id)sender
{
	[progressIndicator setHidden:NO];
	[progressIndicator startAnimation:self];
	clickTimer = [[NSTimer scheduledTimerWithTimeInterval:0.2 
								target:self
								selector:@selector(doClickTimer:)
								userInfo:nil
								repeats:YES] retain];
								
	NSString *startJaxerText = NSLocalizedString(@"StartJaxer", nil);

	if([jaxerButton title] == startJaxerText){
		isStartBut = YES;
		[[MampTasks sharedInstance] startJaxerServer];
	}else{
		isStartBut = NO;
		[[MampTasks sharedInstance] stopJaxerServer];
	}
	[progressIndicator stopAnimation:self];
	[progressIndicator setHidden:YES];
}

- (IBAction)quitStartApache:(id)sender
{
	[progressIndicator setHidden:NO];
	[progressIndicator startAnimation:self];
	clickTimer = [[NSTimer scheduledTimerWithTimeInterval:0.2 
								target:self
								selector:@selector(doClickTimer:)
								userInfo:nil
								repeats:YES] retain];
								
	NSString *startApacheText = NSLocalizedString(@"StartApache", nil);

	if([apacheButton title] == startApacheText){
		isStartBut = YES;
		[[MampTasks sharedInstance] startApacheServer];
	}else{
		isStartBut = NO;
		[[MampTasks sharedInstance] stopApacheServer];
	}
	[progressIndicator stopAnimation:self];
	[progressIndicator setHidden:YES];
}


- (IBAction)openStartPage:(id)sender
{
	NSString *apachePort = [[MampTasks sharedInstance] getApachePortFromConf];
	NSString *startPage = [preferences objectForKey:@"startPage"];

	BOOL isMAMPStartPage = NO;
	if([STARTPAGE_URL_STRING length] <= [startPage length]){
		isMAMPStartPage = [[startPage substringWithRange:NSMakeRange(0,  [STARTPAGE_URL_STRING length])] isEqualToString: STARTPAGE_URL_STRING];
	}else if([startPage isEqualToString: OLD_STARTPAGE_URL_STRING]){
		isMAMPStartPage = YES;
	}
	BOOL noQuestionMarkInString = [startPage rangeOfString:@"?"].location == NSNotFound;
	if(isMAMPStartPage && noQuestionMarkInString){
		[[NSWorkspace sharedWorkspace] openURL:
			[NSURL URLWithString:[NSString stringWithFormat:@"http://127.0.0.1:%@%@",apachePort,[startPage stringByAppendingFormat:@"?language=%@",NSLocalizedString(@"language", nil)]]]];
	}else{
		[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"http://127.0.0.1:%@%@",apachePort,startPage]]];
	}
	
}

-(void)awakeFromNib
{
//	NSURL *url = nil;

	BOOL apacheIsRunning = NO;
#ifdef MYSQL_SUPPORT
	BOOL mysqlIsRunning = NO;
#endif
	BOOL jaxerIsRunning = NO;
	[serversMenu setAutoenablesItems:NO];
	[self checkButtonsAndMenuState:nil];
	stateTimer = [[NSTimer scheduledTimerWithTimeInterval:4 
								target:self
								selector:@selector(checkButtonsAndMenuState:)
								userInfo:nil
								repeats:YES] retain];

	if ([[NSFileManager defaultManager] fileExistsAtPath:JAXERPID_LOCATION_STRING]) {
		jaxerIsRunning = [[MampTasks sharedInstance] serverIsRunning:JAXERPID_LOCATION_STRING];
		if (!jaxerIsRunning) {
			[[NSFileManager defaultManager] removeFileAtPath:JAXERPID_LOCATION_STRING handler:nil];
			//[[NSFileManager defaultManager] removeFileAtPath:JAXERSOCK_LOCATION_STRING handler:nil];
		}
	}
	
#ifdef MYSQL_SUPPORT
	if ([[NSFileManager defaultManager] fileExistsAtPath:MYSQLPID_LOCATION_STRING]) {
		mysqlIsRunning = [[MampTasks sharedInstance] serverIsRunning:MYSQLPID_LOCATION_STRING];
		if (!mysqlIsRunning) {
			[[NSFileManager defaultManager] removeFileAtPath:MYSQLPID_LOCATION_STRING handler:nil];
			[[NSFileManager defaultManager] removeFileAtPath:MYSQLSOCK_LOCATION_STRING handler:nil];
		}
	}
#endif
	
	if ([[NSFileManager defaultManager] fileExistsAtPath:HTTPDPID_LOCATION_STRING]) {
		apacheIsRunning = [[MampTasks sharedInstance] serverIsRunning:HTTPDPID_LOCATION_STRING];
		if (!apacheIsRunning) {
			[[NSFileManager defaultManager] removeFileAtPath:HTTPDPID_LOCATION_STRING handler:nil];
		}
	}
	
	if([preferences boolForKey:@"startServers"]){

		if(!(apacheIsRunning && 
#ifdef MYSQL_SUPPORT
				mysqlIsRunning && 
#endif
				jaxerIsRunning)){
			[progressIndicator setHidden:NO];
			[progressIndicator startAnimation:self];
			[[MampTasks sharedInstance] startJaxerServer];
#ifdef MYSQL_SUPPORT
			[[MampTasks sharedInstance] startMySQLSever];
#endif
			[[MampTasks sharedInstance] startApacheServer];
			[progressIndicator stopAnimation:self];
			[progressIndicator setHidden:YES];
		}
	}
	if([preferences boolForKey:@"openPage"]){
		[progressIndicator setHidden:NO];
		[progressIndicator startAnimation:self];
		startPageTimer = [[NSTimer scheduledTimerWithTimeInterval:5.0 
								target:self
								selector:@selector(doStartPageTimer:)
								userInfo:nil
								repeats:YES] retain];
	}
	
	[[MampTasks sharedInstance] setMampStarted:YES];
}


- (void)doClickTimer:(NSTimer *)aTimer
{
	[self checkButtonsAndMenuState:aTimer];
	
	BOOL apacheIsRunning = [[MampTasks sharedInstance] serverIsRunning:HTTPDPID_LOCATION_STRING];
#ifdef MYSQL_SUPPORT
	BOOL mysqlIsRunning = [[MampTasks sharedInstance] serverIsRunning:MYSQLPID_LOCATION_STRING];
#endif
	BOOL jaxerIsRunning = [[MampTasks sharedInstance] serverIsRunning:JAXERPID_LOCATION_STRING];
	
	if((isStartBut && 
			apacheIsRunning && 
#ifdef MYSQL_SUPPORT
			mysqlIsRunning && 
#endif
			jaxerIsRunning) || 
			((!isStartBut) && 
			(!apacheIsRunning) && 
#ifdef MYSQL_SUPPORT
			(!mysqlIsRunning) && 
#endif
			(!jaxerIsRunning))){
		[aTimer invalidate];
		[aTimer release];
	}
}

- (void)doStartPageTimer:(NSTimer *)aTimer
{
	BOOL apacheIsRunning = [[MampTasks sharedInstance] serverIsRunning:HTTPDPID_LOCATION_STRING];
#ifdef MYSQL_SUPPORT
	BOOL mysqlIsRunning = [[MampTasks sharedInstance] serverIsRunning:MYSQLPID_LOCATION_STRING];
#endif
	BOOL jaxerIsRunning = [[MampTasks sharedInstance] serverIsRunning:JAXERPID_LOCATION_STRING];
	
	if(apacheIsRunning && 
#ifdef MYSQL_SUPPORT
			mysqlIsRunning && 
#endif
			jaxerIsRunning){
		[self openStartPage: self];
		[aTimer invalidate];
		[aTimer release];
	}
	
	[progressIndicator stopAnimation:self];
	[progressIndicator setHidden:YES];
}

- (void)checkButtonsAndMenuState:(NSTimer *)aTimer
{
	NSString *startAllText = NSLocalizedString(@"StartAllServers", nil);
	NSString *stopAllText = NSLocalizedString(@"StopAllServers", nil);
	NSString *startJaxerText = NSLocalizedString(@"StartJaxer", nil);
	NSString *stopJaxerText = NSLocalizedString(@"StopJaxer", nil);
	NSString *startApacheText = NSLocalizedString(@"StartApache", nil);
	NSString *stopApacheText = NSLocalizedString(@"StopApache", nil);
		
	BOOL apacheIsRunning = [[MampTasks sharedInstance] serverIsRunning:HTTPDPID_LOCATION_STRING];
#ifdef MYSQL_SUPPORT
	BOOL mysqlIsRunning = [[MampTasks sharedInstance] serverIsRunning:MYSQLPID_LOCATION_STRING];
#endif
	BOOL jaxerIsRunning = [[MampTasks sharedInstance] serverIsRunning:JAXERPID_LOCATION_STRING];
	
	if(apacheIsRunning){
		[lightApacheRed setImage:grayImage];
		[lightApacheGreen setImage:greenImage];
	}else{
		[lightApacheRed setImage:redImage];
		[lightApacheGreen setImage:grayImage];
	}

	if(apacheIsRunning){
		if([apacheButton title] != stopApacheText){
			[apacheButton setTitle:stopApacheText];
		}
	}else{
		if([apacheButton title] != startApacheText){
			[apacheButton setTitle:startApacheText];
		}
	}


#ifdef MYSQL_SUPPORT
	if(mysqlIsRunning){
		[lightMySqlRed setImage:grayImage];
		[lightMySqlGreen setImage:greenImage];
	}else{
		[lightMySqlRed setImage:redImage];
		[lightMySqlGreen setImage:grayImage];
	}
#endif
	if(jaxerIsRunning){
		[lightJaxerRed setImage:grayImage];
		[lightJaxerGreen setImage:greenImage];
	}else{
		[lightJaxerRed setImage:redImage];
		[lightJaxerGreen setImage:grayImage];
	}	
	
	if(jaxerIsRunning){
		if([jaxerButton title] != stopJaxerText){
			[jaxerButton setTitle:stopJaxerText];
		}
	}else{
		if([jaxerButton title] != startJaxerText){
			[jaxerButton setTitle:startJaxerText];
		}
	}
	
	
	if(
#ifdef MYSQL_SUPPORT
		mysqlIsRunning && 
#endif
		apacheIsRunning && 
		jaxerIsRunning){

		if([allButton title] != stopAllText){
			[allButton setTitle:stopAllText];
			[startServersMenuEntry setEnabled:NO];
			[stopServersMenuEntry setEnabled:YES];
		}
	}else{
		if([allButton title] != startAllText){
			[allButton setTitle:startAllText];
			[startServersMenuEntry setEnabled:YES];
			[stopServersMenuEntry setEnabled:NO];
		}
	}
	
	
}

- (void)dealloc
{

#ifdef MYSQL_SUPPORT
	[mysqlPidLocation release];
#endif
	if(stateTimer != nil){
		[stateTimer invalidate];
		[stateTimer release];
	}
	if(clickTimer != nil){
		[clickTimer invalidate];
		[clickTimer release];
	}
	if(startPageTimer != nil){
		[clickTimer invalidate];
		[clickTimer release];
	}
    [super dealloc];
}


- (IBAction)changePrefs:(id)sender
{

	NSString *apachePort = [apachePortField stringValue];
#ifdef MYSQL_SUPPORT
	NSString *mySqlPort = [mySqlPortField stringValue];
#endif
	NSString *jaxerPort = [jaxerPortField stringValue];
	NSString *startPage = [pageField stringValue];
	NSString *docRoot = [documentRootInput stringValue];
	BOOL openPage = [openPageCheckbox state] == NSOnState;
	BOOL startServers = [startServersCheckbox state] == NSOnState;
	BOOL stopServers = [stopServersCheckbox state] == NSOnState;
	int phpVersion = [[phpRadioBut selectedCell] tag];
	int phpExtensionsFlag = 0;

	if([zendOptimizerCheckbox state] == NSOnState){
		phpExtensionsFlag += ZENDOPTIMIZER_PHP_Extension;
	}
	
	if(phpExtensionsFlag == 0 && [phpCachePopupButton indexOfSelectedItem] == 2) {
		phpExtensionsFlag += APC_PHP_Extension;
	}
	else if([phpCachePopupButton indexOfSelectedItem] == 3){
		phpExtensionsFlag += EACCELERATOR_PHP_Extension;
	}
	else if([phpCachePopupButton indexOfSelectedItem] == 4) {
		phpExtensionsFlag += XCACHE_PHP_Extension;
	}


	if([docRoot length] == 0){
		[documentRootInput setStringValue:[[MampTasks sharedInstance] getDocumentRootFromConf]];
		NSBeep();
		return;
	}

	if([apachePort intValue] < 1){
		[apachePortField setStringValue:[[MampTasks sharedInstance] getApachePortFromConf]];
		NSBeep();
		return;
	}

#ifdef MYSQL_SUPPORT
	if([mySqlPort intValue] < 1024){
		[mySqlPortField setStringValue:[[MampTasks sharedInstance] getMySqlPortFromConf]];
		NSBeep();
		return;
	}
#endif

	if([jaxerPort intValue] < 1024){
		[jaxerPortField setStringValue:[[MampTasks sharedInstance] getJaxerPortFromConf]];
		NSBeep();
		return;
	}
	
#ifdef MYSQL_SUPPORT
	if([apachePort intValue] == [mySqlPort intValue]){
		NSBeep();
		return;
	}
#endif
	
	[prefsWindow orderOut: sender];
	[NSApp endSheet:prefsWindow returnCode:1];

	[progressIndicator setHidden:NO];
	[progressIndicator startAnimation:self];

	[[MampTasks sharedInstance] writeApachePortToConf:apachePort];
#ifdef MYSQL_SUPPORT
	[[MampTasks sharedInstance] writeMySqlPortToConf:mySqlPort];
#endif
	[[MampTasks sharedInstance] writeJaxerPortToConf:jaxerPort];
	//[[MampTasks sharedInstance] writePHPVersToConf:phpVersion];
	[[MampTasks sharedInstance] writeDocumentRootToConf:docRoot];
//	[[MampTasks sharedInstance] writePHPExtensionsToConf:phpExtensionsFlag path:PHP4_INI_LOCATION_STRING];
//	[[MampTasks sharedInstance] writePHPExtensionsToConf:phpExtensionsFlag path:PHP5_INI_LOCATION_STRING];
	
	[preferences setBool:startServers  forKey:@"startServers"];
	[preferences setBool:stopServers  forKey:@"stopServers"];
	[preferences setObject:startPage  forKey:@"startPage"];
	[preferences setBool:openPage  forKey:@"openPage"];
	
	BOOL apacheIsRunning = [[MampTasks sharedInstance] serverIsRunning:HTTPDPID_LOCATION_STRING];
#ifdef MYSQL_SUPPORT
	BOOL mysqlIsRunning = [[MampTasks sharedInstance] serverIsRunning:MYSQLPID_LOCATION_STRING];
#endif
	BOOL jaxerIsRunning = [[MampTasks sharedInstance] serverIsRunning:JAXERPID_LOCATION_STRING];

	if(jaxerIsRunning){
		[[MampTasks sharedInstance] stopJaxerServer];
		[[MampTasks sharedInstance] startJaxerServer];
	}
#ifdef MYSQL_SUPPORT
	if(mysqlIsRunning){
		[[MampTasks sharedInstance] stopMySQLSever];
		[[MampTasks sharedInstance] startMySQLSever];
	}
#endif
	if(apacheIsRunning){
		[[MampTasks sharedInstance] stopApacheServer];
		[[MampTasks sharedInstance] startApacheServer];
	}
	
	[progressIndicator stopAnimation:self];
	[progressIndicator setHidden:YES];
}

- (IBAction)closePrefs:(id)sender
{
	[prefsWindow orderOut: sender];
	[NSApp endSheet:prefsWindow returnCode:1];
}

- (IBAction)openPrefs:(id)sender
{

	NSString *apachePort = [[MampTasks sharedInstance] getApachePortFromConf];
#ifdef MYSQL_SUPPORT
	NSString *mysqlPort = [[MampTasks sharedInstance] getMySqlPortFromConf];
#endif
	NSString *jaxerPort = [[MampTasks sharedInstance] getJaxerPortFromConf];
	int phpVersion = [[MampTasks sharedInstance] getPHPVersFromConf];
	int phpExtensionsFlag = [[MampTasks sharedInstance] getPHPExtensionsFromConf];
	NSString *docRoot = [[MampTasks sharedInstance] getDocumentRootFromConf];
	
	if(apachePort != nil){
		[apachePortField setStringValue:apachePort];
	}else{
		NSLog(@"MAMP ERROR: Can't read apache port");
	}
#ifdef MYSQL_SUPPORT
	if(mysqlPort != nil){
		[mySqlPortField setStringValue:mysqlPort];
	}else{
		NSLog(@"MAMP ERROR: Can't read mysql port");
	}
#endif
	if(jaxerPort != nil){
		[jaxerPortField setStringValue:jaxerPort];
	}else{
		NSLog(@"MAMP ERROR: Can't read jaxer port");
	}	
	if(docRoot != nil){
		[documentRootInput setStringValue:docRoot];
	}else{
		NSLog(@"MAMP ERROR: Can't read document root");
	}

	[phpRadioBut selectCellWithTag: phpVersion];

	if( (phpExtensionsFlag & ZENDOPTIMIZER_PHP_Extension) != 0){
		[zendOptimizerCheckbox setState:NSOnState];
	}else{
		[zendOptimizerCheckbox setState:NSOffState];
	}
	
	
	if( (phpExtensionsFlag & ZENDOPTIMIZER_PHP_Extension) == 0 &&  (phpExtensionsFlag & APC_PHP_Extension) != 0) {
		[phpCachePopupButton selectItemAtIndex:2];
	}
	else if( (phpExtensionsFlag & EACCELERATOR_PHP_Extension) != 0){
		[phpCachePopupButton selectItemAtIndex:3];
	}
	else if( (phpExtensionsFlag & XCACHE_PHP_Extension) != 0) {
		[phpCachePopupButton selectItemAtIndex:4];
	}
	else {
		[phpCachePopupButton selectItemAtIndex:0];
	}

	[pageField setStringValue:[preferences objectForKey:@"startPage"]];

	if([preferences boolForKey:@"openPage"]){
		[openPageCheckbox setState:NSOnState];
	}else{
		[openPageCheckbox setState:NSOffState];
	}
	
	if([preferences boolForKey:@"startServers"]){
		[startServersCheckbox setState:NSOnState];
	}else{
		[startServersCheckbox setState:NSOffState];
	}
	
	if([preferences boolForKey:@"stopServers"]){
		[stopServersCheckbox setState:NSOnState];
	}else{
		[stopServersCheckbox setState:NSOffState];
	}
	
	[NSApp beginSheet:prefsWindow 
		modalForWindow:mainWindow
		modalDelegate:self 
		didEndSelector:nil
		contextInfo:nil];
}

- (IBAction)startServersClicked:(id)sender
{
}

- (IBAction)php5Clicked:(id)sender
{
}

- (IBAction)php4Clicked:(id)sender
{
}

- (IBAction)zendClicked:(id)sender
{
	if([sender state] == NSOnState && [phpCachePopupButton indexOfSelectedItem] == 2) [phpCachePopupButton selectItemAtIndex:0];
}

- (IBAction)selectApc:(id)sender
{
	//
}

- (IBAction)stopServersClicked:(id)sender
{
}

- (IBAction)openPageClicked:(id)sender
{
}

- (IBAction)resetDefaultPorts:(id)sender
{
	[apachePortField setStringValue:@"80"];
#ifdef MYSQL_SUPPORT
	[mySqlPortField setStringValue:@"3306"];
#endif
	[jaxerPortField setStringValue:@"5081"];
}

- (IBAction)resetMampPorts:(id)sender
{
	[apachePortField setStringValue:@"8081"];
#ifdef MYSQL_SUPPORT
	[mySqlPortField setStringValue:@"8889"];
#endif
	[jaxerPortField setStringValue:@"5081"];
}

- (id) progressIndicator
{
	return progressIndicator;
}

- (IBAction)selectDocumentRoot:(id)sender
{
	NSOpenPanel *op = [NSOpenPanel openPanel];
	int iResult;

	[op setCanChooseDirectories : YES ];
	[op setCanChooseFiles : NO ];

	iResult = [ op runModalForDirectory : [[MampTasks sharedInstance] getDocumentRootFromConf] file: nil types: nil];

	if (iResult == NSOKButton) [documentRootInput setStringValue: [op filename] ];
}

- (IBAction)launchMampPro:(id)sender
{
	[NSApp stopModalWithCode:NSAlertDefaultReturn];
}

- (IBAction)launchMamp:(id)sender
{
	[NSApp stopModalWithCode:NSAlertAlternateReturn];
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	if([menuItem action] == @selector(selectApc:) && [zendOptimizerCheckbox state] == NSOnState) return NO;
	
	return YES;
}

@end

