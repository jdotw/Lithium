//
//  LCGroupTreeOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCTransparentOutlineView.h"

@interface LCGroupTreeOutlineView : LCTransparentOutlineView 
{
	NSMutableArray *displayedSubviews;
}

@end
