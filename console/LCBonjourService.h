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
@property (nonatomic,copy) NSString *name;
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic,copy) NSString *ip;
- (void) updateDisplayString;
@property (nonatomic, assign) id parent;

@end
