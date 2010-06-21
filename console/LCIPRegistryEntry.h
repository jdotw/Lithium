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
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic,copy) NSString *ip;
@property (nonatomic,copy) NSString *mask;
@property (nonatomic,copy) NSString *network;
@property (nonatomic,copy) NSString *hostname;
@property (nonatomic, assign) BOOL boundToDevice;
@property (nonatomic, assign) id parent;


@end
