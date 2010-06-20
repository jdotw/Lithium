//
//  LCDocumentCollectionSubView.m
//  Lithium Console
//
//  Created by James Wilson on 19/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocumentCollectionSubView.h"
#import "NSGradient-Selection.h"

@implementation LCDocumentCollectionSubView

-(void)setSelected:(BOOL)flag {
	m_isSelected = flag;
}

-(id)delegate 
{
	return m_theDelegate;
}

-(void)setDelegate:(id)theDelegate 
{
	m_theDelegate = theDelegate;
}

-(BOOL)selected {
	return m_isSelected;
}

-(void)drawRect:(NSRect)rect 
{
	float padding = 10.0;
	
	NSBezierPath *outlinePath = [NSBezierPath bezierPathWithRoundedRect:NSMakeRect(NSMinX([self bounds])+padding,
																				   NSMinY([self bounds])+padding,
																				   NSWidth([self bounds])-(2*padding),
																				   NSHeight([self bounds])-(2*padding))
																xRadius:8.0 
																yRadius:8.0];
	
	NSGradient *backGradient = [[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.2 alpha:1.0]
															  endingColor:[NSColor colorWithCalibratedWhite:0.1 alpha:1.0]] autorelease];
	[backGradient drawInBezierPath:outlinePath angle:90.0];
	
	[[NSColor whiteColor] setStroke];
	[outlinePath stroke];

	[super drawRect:rect];

	if([self selected]) 
	{
		NSGradient *gradient = [NSGradient selectionGradientWithAlpha:0.8];
		[gradient drawInBezierPath:outlinePath angle:-90.0];
	}
}

-(NSView *)hitTest:(NSPoint)aPoint 
{
	if(NSPointInRect(aPoint,[self convertRect:[self bounds] toView:[self superview]])) 
	{
		return self;
	} 
	else 
	{
		return nil;    
	}
}

-(void)mouseDown:(NSEvent *)theEvent 
{
	[super mouseDown:theEvent];
	
	// check for click count above one
	if([theEvent clickCount] > 1) 
	{
		if([[self delegate] respondsToSelector:@selector(collectionViewDoubleClicked:)]) 
		{
			[[self delegate] collectionViewDoubleClicked:self];
		}
	}
}

@end
