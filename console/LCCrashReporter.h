//
//  LCCrashReporter.h
//  Lithium Console
//
//  Created by James Wilson on 15/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCCrashReporter : NSObject 
{
	IBOutlet NSWindow *reporterSheet;
	
	NSString *userDescription;
	NSString *userEmail;
}

#pragma mark "Initialisation"
- (void) awakeFromNib;

#pragma mark "UI Actions"
- (IBAction) reportClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Accessors"
- (NSString *) userDescription;
- (void) setUserDescription:(NSString *)string;
- (NSString *) userEmail;
- (void) setUserEmail:(NSString *)string;

@property (retain) NSWindow *reporterSheet;
@property (retain,getter=userDescription,setter=setUserDescription:) NSString *userDescription;
@property (retain,getter=userEmail,setter=setUserEmail:) NSString *userEmail;
@end
