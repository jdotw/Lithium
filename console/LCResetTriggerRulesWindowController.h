//
//  LCResetTriggerRulesWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 28/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCResetTriggerRulesWindowController : NSWindowController
{
	/* Entity */
	LCEntity *entity;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;

	/* XML Ops */
	BOOL xmlOperationInProgress;
	BOOL errorEncountered;
	NSString *statusString;
	NSImage *statusIcon;
	LCXMLRequest *xmlReq;
	NSMutableDictionary *xmlProperties;
	NSString *xmlElement;
	NSMutableString *xmlString;	
}

#pragma mark "Constructor"
- (LCResetTriggerRulesWindowController *) initForEntity:(LCEntity *)initEntity;

#pragma mark "UI Actions"
- (IBAction) cancelClicked:(id)sender;
- (IBAction) resetClicked:(id)sender;

#pragma mark "Properties"
@property (retain) LCEntity *entity;
@property (assign) BOOL xmlOperationInProgress;

@end
