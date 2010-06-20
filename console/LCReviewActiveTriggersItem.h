//
//  LCReviewActiveTriggersItem.h
//  Lithium Console
//
//  Created by James Wilson on 27/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCReviewActiveTriggersItem : NSObject 
{
	BOOL selected;
	LCEntity *entity;
}

#pragma mark "Constructors"
+ (LCReviewActiveTriggersItem *) itemForEntity:(LCEntity *)initEntity;
- (LCReviewActiveTriggersItem *) initForEntity:(LCEntity *)initEntity;

#pragma mark "Accessors"
@property (assign) BOOL selected;
@property (retain) LCEntity *entity;

@end
