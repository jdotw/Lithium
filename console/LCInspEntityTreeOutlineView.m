//
//  LCInspEntityTreeOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspEntityTreeOutlineView.h"


@implementation LCInspEntityTreeOutlineView

- (void) awakeFromNib
{
	[super awakeFromNib];
	[self setDelegate:self];

}

#pragma mark "Outline Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	if ([[self tableColumns] indexOfObject:tableColumn] == 0)
	{ [cell setEntity:[item representedObject]]; }
}

- (void)outlineViewItemWillExpand:(NSNotification *)notification
{
}

- (void)outlineViewItemDidExpand:(NSNotification *)notification
{
}

- (void)outlineViewItemWillCollapse:(NSNotification *)notification
{
}

- (void)outlineViewItemDidCollapse:(NSNotification *)notification
{
}



@end
