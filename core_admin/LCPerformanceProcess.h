//
//  LCPerformanceProcess.h
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCPerformanceItem.h"

@interface LCPerformanceProcess : LCPerformanceItem 
{
}

#pragma mark "Constructors"
+ (LCPerformanceProcess *) processWithColumns:(NSArray *)columns;
- (LCPerformanceProcess *) initWithColumns:(NSArray *)columns;

@end
