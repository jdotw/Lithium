//
//  LCIPRegistryNetwork.h
//  Lithium Console
//
//  Created by James Wilson on 2/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCIPRegistryEntry.h"
#import "LCXMLObject.h"

@interface LCIPRegistryNetwork : LCXMLObject 
{
	NSMutableArray *children;
	NSMutableDictionary *childDictionary;	
	
	NSString *displayString;
	NSString *address;	
	
	id parent;
}

#pragma mark "Children Properties"
@property (readonly) NSMutableArray *children;
@property (readonly) NSMutableDictionary *childDictionary;
- (void) insertObject:(LCIPRegistryEntry *)entry inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;

#pragma mark "Properties"
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic,copy) NSString *address;	
- (void) updateDisplayString;
@property (nonatomic, assign) id parent;
- (NSString *) uniqueIdentifier;

@end
