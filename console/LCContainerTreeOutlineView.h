//
//  LCContainerTreeOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCTransparentOutlineView.h"
#import "LCContainerTreeCell.h"
#import "LCContainer.h"

@interface LCContainerTreeOutlineView : LCTransparentOutlineView 
{
	
}

- (void) selectContainer:(LCContainer *)container;

@end
