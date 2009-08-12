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

@class MAMP;

extern MAMP *mainMamp;

@interface MAMP : NSObject
{
    IBOutlet id preferencesButton;
    IBOutlet id allButton;
	IBOutlet id jaxerButton;
	IBOutlet id apacheButton;
    IBOutlet id startPageButton;
    IBOutlet id quitButton;

    IBOutlet id lightApacheRed;
    IBOutlet id lightApacheGreen;
    IBOutlet id lightMySqlRed;
    IBOutlet id lightMySqlGreen;
    IBOutlet id lightJaxerRed;
    IBOutlet id lightJaxerGreen;

    IBOutlet id startServersMenuEntry;
    IBOutlet id stopServersMenuEntry;
	IBOutlet id serversMenu;
	IBOutlet id prefsWindow;
	IBOutlet id mainWindow;
	IBOutlet id mampProWarningWindow;

	IBOutlet id apachePortField;
	IBOutlet id mySqlPortField;
	IBOutlet id jaxerPortField;
	
	IBOutlet id startServersCheckbox;
	IBOutlet id stopServersCheckbox;
	IBOutlet id openPageCheckbox;
	IBOutlet id pageField;
	IBOutlet id pageButton;
	IBOutlet id progressIndicator;

	IBOutlet id phpRadioBut;
	IBOutlet id phpCachePopupButton;
	IBOutlet id zendOptimizerCheckbox;

	IBOutlet id documentRootInput;

	
	NSTimer *stateTimer;
	NSTimer *clickTimer;
	NSTimer *startPageTimer;
	NSString *mysqlPidLocation;
	
	NSImage *grayImage;
	NSImage *greenImage;
	NSImage *redImage;
	
	BOOL isStartBut;
	
	
}

- (IBAction)quitStartAll:(id)sender;
- (IBAction)quitStartJaxer:(id)sender;
- (IBAction)quitStartApache:(id)sender;

- (IBAction)openStartPage:(id)sender;
- (IBAction)closePrefs:(id)sender;
- (IBAction)changePrefs:(id)sender;
- (IBAction)openPrefs:(id)sender;

- (IBAction)startServersClicked:(id)sender;
- (IBAction)stopServersClicked:(id)sender;
- (IBAction)openPageClicked:(id)sender;

- (IBAction)php5Clicked:(id)sender;
- (IBAction)php4Clicked:(id)sender;
- (IBAction)zendClicked:(id)sender;
- (IBAction)selectApc:(id)sender;

- (IBAction)resetDefaultPorts:(id)sender;
- (IBAction)resetMampPorts:(id)sender;
- (IBAction)selectDocumentRoot:(id)sender;

- (IBAction)launchMampPro:(id)sender;
- (IBAction)launchMamp:(id)sender;

- (void)doClickTimer:(NSTimer *)aTimer;
- (void)doStartPageTimer:(NSTimer *)aTimer;
- (void)checkButtonsAndMenuState:(NSTimer *)aTimer;

- (id) progressIndicator;


@end
