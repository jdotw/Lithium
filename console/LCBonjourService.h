//
//  LCBonjourService.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"
#import "LCDeviceEditTemplate.h"

@interface LCBonjourService : LCXMLObject 
{
	NSString *name;
	NSString *displayString;
	NSString *ip;	
	
	id parent;
}

#pragma mark "Device Template Methods"
- (LCDeviceEditTemplate *) deviceTemplate;

#pragma mark "Properties"
@property (copy) NSString *name;
@property (copy) NSString *displayString;
@property (copy) NSString *ip;
- (void) updateDisplayString;
@property (assign) id parent;

@end
