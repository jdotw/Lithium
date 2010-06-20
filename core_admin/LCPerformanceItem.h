//
//  LCPerformanceItem.h
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCPerformanceItem : NSObject 
{
	NSString *processID;
	NSString *cpuPercent;
	NSString *vSize;
	NSString *rSize;
	
	NSMutableArray *items;
	NSMutableDictionary *itemDictionary;
}

#pragma mark "Items"
- (NSMutableArray *) items;
- (NSMutableDictionary *) itemDictionary;
- (void) insertObject:(id)object inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;
- (BOOL) isLeafNode;

#pragma mark "Stats Accessors"
- (NSString *) processID;
- (void) setProcessID:(NSString *)value;
- (NSString *) cpuPercent;
- (void) setCpuPercent:(NSString *)value;
- (NSString *) vSize;
- (void) setVSize:(NSString *)value;
- (NSString *) rSize;
- (void) setRSize:(NSString *)value;
- (NSString *) displayString;

@end
