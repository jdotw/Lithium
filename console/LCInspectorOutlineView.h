//
//  LCInspectorOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTransparentOutlineView.h"

@interface LCInspectorOutlineView : LCTransparentOutlineView
{
	NSTableColumn *viewColumn;
	NSMutableArray *visibleViews;
}

#pragma mark "Reset views"
- (void) resetViews;
- (void) resetSubviewsOnly;

@property (retain) NSTableColumn *viewColumn;
@property (retain) NSMutableArray *visibleViews;
@end
