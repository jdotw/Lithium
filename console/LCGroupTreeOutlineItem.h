//
//  LCGroupTreeOutlineItem.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCGroupTreeOutlineItem : NSObject 
{
	LCEntity *entity;
	
	float rowHeight;
	NSMutableArray *children;
	BOOL isGroupTreeLeaf;
}

#pragma mark "Constructor"
- (id) initWithEntity:(LCEntity *)initEntity;

#pragma mark Accessors
@property (retain) LCEntity *entity;
@property (assign) float rowHeight;
@property (copy) NSMutableArray *children;
@property (assign) BOOL isGroupTreeLeaf;
- (NSString *) uniqueIdentifier;


@end
