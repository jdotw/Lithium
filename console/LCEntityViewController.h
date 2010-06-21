//
//  LCEntityViewController.h
//  Lithium Console
//
//  Created by James Wilson on 8/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCEntityViewController : NSObject 
{
	LCEntity *entity;
	BOOL usingGeneric;
	
	IBOutlet NSView *view;
	IBOutlet NSObjectController *objectController;
}

#pragma mark "Initialisation"
+ (LCEntityViewController *) controllerForEntity:(LCEntity *)initEntity;
- (LCEntityViewController *) initForEntity:(LCEntity *)initEntity;
- (LCEntityViewController *) initGenericForEntity:(LCEntity *)initEntity;
- (LCEntityViewController *) init;
- (void) removeViewAndContent;

#pragma mark "NIB Loading"
- (void) loadNIB;

#pragma mark "Accessor Methods"
- (LCEntity *) entity;
- (void) setEntity:(LCEntity *)newEntity;

- (NSView *) view;

- (BOOL) usingGeneric;
- (void) setUsingGeneric:(BOOL)flag;

@property (nonatomic,getter=usingGeneric,setter=setUsingGeneric:) BOOL usingGeneric;
@property (nonatomic,retain,getter=view) NSView *view;
@property (nonatomic,retain) NSObjectController *objectController;
@end
