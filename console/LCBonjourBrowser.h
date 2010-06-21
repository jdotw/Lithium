//
//  LCBonjourBrowser.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBonjourService.h"
#import "LCXMLObject.h"

@interface LCBonjourBrowser : LCXMLObject 
{
	NSString *service;
	NSString *desc;
	NSString *displayString;
	
	NSMutableArray *children;
	NSMutableDictionary *childDictionary;
	
	id parent;
}

#pragma mark "Services Accessors"
@property (readonly) NSMutableArray *children;
@property (readonly) NSMutableDictionary *childDictionary;
- (void) insertObject:(LCBonjourService *)service inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;

#pragma mark "Properties"
@property (nonatomic,copy) NSString *service;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,copy) NSString *displayString;
- (void) updateDisplayString;
@property (nonatomic, assign) id parent;
- (NSString *) uniqueIdentifier;

@end
