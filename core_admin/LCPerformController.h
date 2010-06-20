//
//  LCPerformController.h
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCPerformanceGroup.h"
#import "LCPerformanceProcess.h"

@interface LCPerformController : NSObject 
{
	NSMutableArray *items;
	LCPerformanceGroup *lithiumGroup;
	LCPerformanceGroup *rrdtoolGroup;
	LCPerformanceGroup *postgresGroup;
	BOOL vacuumInProgress;
}

#pragma mark "Initialisation"
- (void) awakeFromNib;
+ (LCPerformController *) masterController;

#pragma mark "Refresh process list"
- (void) refreshProcessList;
	
#pragma mark "Group Accessors"
- (LCPerformanceGroup *) lithiumGroup;
- (void) setLithiumGroup:(LCPerformanceGroup *)item;
- (LCPerformanceGroup *) rrdtoolGroup;
- (void) setRrdtoolGroup:(LCPerformanceGroup *)item;
- (LCPerformanceGroup *) postgresGroup;
- (void) setPostgresGroup:(LCPerformanceGroup *)item;

#pragma mark "Accessors"
- (BOOL) vacuumInProgress;
- (void) setVacuumInProgress:(BOOL)flag;

@end
