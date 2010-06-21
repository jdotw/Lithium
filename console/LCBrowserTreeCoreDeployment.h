//
//  LCBrowserTreeCoreDeployment.h
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItem.h"
#import "LCCoreDeployment.h"

@interface LCBrowserTreeCoreDeployment : LCBrowserTreeItem 
{
	LCCoreDeployment *deployment;
}

- (LCBrowserTreeCoreDeployment *) initWithDeployment:(LCCoreDeployment *)initDeployment;
@property (nonatomic,retain) LCCoreDeployment *deployment;


@end
