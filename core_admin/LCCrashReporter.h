//
//  LCCrashReporter.h
//  Lithium Console
//
//  Created by James Wilson on 15/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCCrashReporter : NSObject 
{
	IBOutlet NSWindow *reporterSheet;
	IBOutlet NSWindow *mainWindow;
	
	NSString *userDescription;
	NSString *userEmail;
}

#pragma mark "Initialisation"
- (void) checkForCrashes;

#pragma mark "UI Actions"
- (IBAction) reportClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Accessors"
- (NSString *) userDescription;
- (void) setUserDescription:(NSString *)string;
- (NSString *) userEmail;
- (void) setUserEmail:(NSString *)string;

@end
