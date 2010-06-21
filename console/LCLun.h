//
//  LCLun.h
//  Lithium Console
//
//  Created by James Wilson on 12/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCLun : NSObject 
{
	LCEntity *object;
	NSString *wwn;
	
	NSMutableArray *uses;
	NSMutableDictionary *useDictionary;
}

#pragma mark "Constructors"
+ (LCLun *) lunWithObject:(LCEntity *)initObject wwn:(NSString *)initWWN;
- (LCLun *) initWithObject:(LCEntity *)initObject wwn:(NSString *)initWWN;
- (void) dealloc;

#pragma mark "Accessors"
- (LCEntity *) object;
- (void) setObject:(LCEntity *)newObject;
- (NSString *) wwn;
- (void) setWwn:(NSString *)newWwn;
- (NSMutableArray *) uses;
- (void) insertObject:(id)obj inUsesAtIndex:(unsigned int)index;
- (void) removeObjectFromUsesAtIndex:(unsigned int)index;
- (NSMutableDictionary *) useDictionary;

@property (nonatomic,retain,getter=object,setter=setObject:) LCEntity *object;
@property (nonatomic,retain,getter=uses) NSMutableArray *uses;
@property (nonatomic,retain,getter=useDictionary) NSMutableDictionary *useDictionary;
@end
