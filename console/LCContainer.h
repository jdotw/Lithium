//
//  LCContainer.h
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCMetric.h"

@interface LCContainer : LCEntity 
{
	int itemListState;
	
	/* Module Builder */
	BOOL isModuleBuilder;
}

- (LCContainer *) init;
- (Class) childClass;

@property (assign) int itemListState;

@property (assign) BOOL isModuleBuilder;


@end
