//
//  LCPerformanceGroup.h
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCPerformanceItem.h"

@interface LCPerformanceGroup : LCPerformanceItem 
{
	NSString *name;
}

#pragma mark "Constructors"
+ (LCPerformanceGroup *) performanceGroupNamed:(NSString *)initName;
- (LCPerformanceGroup *) initWithName:(NSString *)initName;
- (void) dealloc;
	
#pragma mark "Total Calculations"
- (void) updateTotals;

#pragma mark "Obsolescence Checking"
- (void) checkProcessExistence:(NSMutableDictionary *)processDict;

#pragma mark "Accessors"
- (NSString *) name;
- (void) setName:(NSString *)string;
- (NSString *) displayString;

@end
