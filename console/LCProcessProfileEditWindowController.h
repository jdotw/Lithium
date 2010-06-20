//
//  LCProcessProfileEditWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 15/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"
#import "LCProcessProfile.h"
#import "LCXMLRequest.h"

@interface LCProcessProfileEditWindowController : NSWindowController 
{
	/* UI Elements */
	IBOutlet NSTextField *titleTextField;
	IBOutlet NSButton *okButton;
	
	/* Profile Properties */
	NSString *desc;
	NSString *match;
	NSString *argumentsMatch;
	
	/* Related Objects */
	LCProcessProfile *profile;
	LCDevice *device;
	NSWindow *windowForSheet;
	
	/* XML Properties */
	LCXMLRequest *xmlRequest;
	BOOL xmlOperationInProgress;
	NSString *status;
	BOOL shouldClose;	
	BOOL sheetShown;
	BOOL addProfile;
	BOOL removeProfile;	
}

#pragma mark "Constructors"
- (id) initForNewProfileMatch:(NSString *)initMatch device:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindow;
- (id) initWithProfileToEdit:(LCProcessProfile *)initProfile device:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindow;
- (id) initWithProfileToDelete:(LCProcessProfile *)initProfile device:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindow;

#pragma mark "UI Actions"
- (IBAction) saveClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Properties"
@property (retain) LCDevice *device;
@property (retain) LCProcessProfile *profile;
@property (retain) NSWindow *windowForSheet;
@property (copy) NSString *desc;
@property (copy) NSString *match;
@property (copy) NSString *argumentsMatch;
@property (assign) BOOL xmlOperationInProgress;
@property (copy) NSString *status;

@end
