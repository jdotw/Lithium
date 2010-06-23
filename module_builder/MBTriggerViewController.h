//
//  MBTriggerViewController.h
//  ModuleBuilder
//
//  Created by James Wilson on 6/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBTrigger.h"
#import "MBOutlineViewController.h"

@interface MBTriggerViewController : MBOutlineViewController 
{

}

#pragma mark "Constructors"
+ (MBTriggerViewController *) viewForTrigger:(MBTrigger *)initTrigger;
- (MBTriggerViewController *) initWithTrigger:(MBTrigger *)iniTrigger;

#pragma mark Accessors
- (MBTrigger *) trigger;

@end
