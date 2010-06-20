//
//  LCReviewActiveTriggersWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 27/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCReviewActiveTriggersWindowController : NSWindowController 
{
	/* Device */
	LCEntity *device;
	
	/* UI Elements */
	IBOutlet NSArrayController *itemArrayController;
	IBOutlet NSObjectController *controllerAlias;
	
	/* Items */
	NSMutableArray *items;
	
	/* Toggle */
	BOOL toggleState;

	/* XML Ops */
	BOOL xmlOperationInProgress;
	BOOL errorEncountered;
	NSString *statusString;
	NSImage *statusIcon;
	LCXMLRequest *xmlReq;
	NSMutableDictionary *xmlProperties;
	NSString *xmlElement;
	NSMutableString *xmlString;
	
	/* Parent window */
	NSWindow *parentWindow;
}

#pragma mark "Constructors"
- (LCReviewActiveTriggersWindowController *) initForDevice:(LCEntity *)initDevice;

#pragma mark Item Management
- (void) resetItems;

#pragma mark UI Actions
- (IBAction) cancelClicked:(id)sender;
- (IBAction) disableSelectedClicked:(id)sender;
- (IBAction) toggleAllClicked:(id)sender;
- (IBAction) toggleSelectedClicked:(id)sender;
- (IBAction) tableViewDoubleClicked:(id)sender;

#pragma mark Accessors
@property (retain) LCEntity *device;
@property (assign) NSWindow *parentWindow;
@property (assign) BOOL xmlOperationInProgress;

@end
