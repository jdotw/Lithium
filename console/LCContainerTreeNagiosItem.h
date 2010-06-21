//
//  LCContainerTreeNagiosItem.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCContainer.h"
#import "LCContainerTreeItem.h"

@interface LCContainerTreeNagiosItem : LCContainerTreeItem 
{
	LCContainer *nagiosContainer;
}

@property (nonatomic,retain) LCContainer *nagiosContainer;

@end
