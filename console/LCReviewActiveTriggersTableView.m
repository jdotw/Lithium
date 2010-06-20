//
//  LCReviewActiveTriggersTableView.m
//  Lithium Console
//
//  Created by James Wilson on 28/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReviewActiveTriggersTableView.h"


@implementation LCReviewActiveTriggersTableView

- (void) awakeFromNib
{
	[self setDoubleAction:@selector(tableDoubleClicked)];
}

- (void) tableDoubleClicked
{
	if ([self clickedColumn] == 0)
	{ [windowController toggleSelectedClicked:self]; }
	else
	{ [windowController tableViewDoubleClicked:self]; }
}

- (void)keyDown:(NSEvent *)event
{
	unichar key = [[event charactersIgnoringModifiers] characterAtIndex:0];
	if (key == ' ')
	{ 
		[windowController toggleSelectedClicked:self];
	}
	[super keyDown:event];
}

@end
