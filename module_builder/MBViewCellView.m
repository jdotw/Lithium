//
//  MBViewCellView.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/07/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBViewCellView.h"


@implementation MBViewCellView

#pragma mark "Hit Test"

- (NSView *) BROKENhitTest:(NSPoint)aPoint
{
	/* Caused selection of radio button, etc to fail -- hence BROKEN 
	 * 
	 * mmmm dimple.
	 */
	
	NSPoint windowPoint = [[self superview] convertPoint:aPoint toView:nil];
	NSPoint localPoint = [self convertPoint:windowPoint fromView:nil];
	NSView *actualView = [super hitTest:localPoint];
	if (actualView == matrix) return actualView;
	else return [self superview];
}

@end
