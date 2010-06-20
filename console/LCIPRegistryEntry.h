//
//  LCIPRegistryEntry.h
//  Lithium Console
//
//  Created by James Wilson on 2/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"
#import "LCDeviceEditTemplate.h"

@interface LCIPRegistryEntry : LCXMLObject 
{
	NSString *displayString;
	NSString *ip;
	NSString *mask;
	NSString *network;
	NSString *hostname;
	BOOL boundToDevice;
	
	id parent;
}

#pragma mark "Dummy Properties"
- (NSMutableArray *) children;

#pragma mark "Device Template Methods"
- (LCDeviceEditTemplate *) deviceTemplate;

#pragma mark "Properties"
@property (copy) NSString *displayString;
@property (copy) NSString *ip;
@property (copy) NSString *mask;
@property (copy) NSString *network;
@property (copy) NSString *hostname;
@property (assign) BOOL boundToDevice;
@property (assign) id parent;


@end
