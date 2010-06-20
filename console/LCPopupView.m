//
//  LCPopupView.m
//  Lithium Console
//
//  Created by James Wilson on 1/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCPopupView.h"
#import <Quartz/Quartz.h>

#import "LCTriangleBezierPath.h"

#define kTriangleWidth 16.0
#define kRoundedRadius 4.0

@implementation LCPopupView

- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) 
	{
		/* Set defaults */
		triangleWidth = 16.0;
		roundedRadius = 4.0;
		
		/* Setup layer (shadow) */
		[self setWantsLayer:YES];
		CGColorRef shadowColor = CGColorCreateGenericGray(0.0, 1.0);
		[[self layer] setShadowColor:shadowColor];
		[[self layer] setShadowOpacity:0.6];
		[[self layer] setShadowRadius:3.0];
		[[self layer] setShadowOffset:CGSizeMake(4.0, -4.0)];
		CGColorRelease(shadowColor);
		
		/* Create text field */
		float fontSize = 11.0;
		textField = [[LCShadowTextField alloc] initWithFrame:NSMakeRect(NSMinX(self.usableRect),
																		roundf((NSMidY(self.usableRect) - (fontSize * 0.5))),
																		NSWidth(self.usableRect),
																		fontSize + 2.0)];
		[textField setAutoresizingMask:NSViewHeightSizable|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin];
		[textField setFont:[NSFont systemFontOfSize:fontSize]];
		[textField setBackgroundColor:[NSColor clearColor]];
		[textField setTextColor:[NSColor whiteColor]];
		[textField setBordered:NO];
		[textField setAlignment:NSCenterTextAlignment];
		[self addSubview:textField];
    }
    return self;
}

+ (NSBezierPath *) pathForPopupInRect:(NSRect)rect
{
	/* Draws the rounded rect + triangle */
	NSBezierPath *path = [NSBezierPath bezierPath];
	
	/* Begin at centre of triangle */
	[path moveToPoint:NSMakePoint(NSMidX(rect), NSMinY(rect))];
	
	/* Draw triangle angle */
	[path lineToPoint:NSMakePoint(NSMidX(rect) - (0.5 * kTriangleWidth), NSMinY(rect)+kTriangleWidth)];
	
	/* Draw bottom-left side of box */
	[path lineToPoint:NSMakePoint(NSMinX(rect) + kRoundedRadius, NSMinY(rect)+kTriangleWidth)];
	
	/* Draw bottom-left curve */
	[path curveToPoint:NSMakePoint(NSMinX(rect), NSMinY(rect) + kTriangleWidth + kRoundedRadius)
		 controlPoint1:NSMakePoint(NSMinX(rect), NSMinY(rect) + kTriangleWidth)
		 controlPoint2:NSMakePoint(NSMinX(rect), NSMinY(rect) + kTriangleWidth)];
	
	/* Draw left side of box */
	[path lineToPoint:NSMakePoint(NSMinX(rect), NSMaxY(rect)-kRoundedRadius)];
	
	/* Draw top-left curve */
	[path curveToPoint:NSMakePoint(NSMinX(rect)+kRoundedRadius, NSMaxY(rect))
		 controlPoint1:NSMakePoint(NSMinX(rect), NSMaxY(rect))
		 controlPoint2:NSMakePoint(NSMinX(rect), NSMaxY(rect))];
	
	/* Draw top of box */
	[path lineToPoint:NSMakePoint(NSMaxX(rect)-kRoundedRadius, NSMaxY(rect))];
	
	/* Draw top-right curve */
	[path curveToPoint:NSMakePoint(NSMaxX(rect), NSMaxY(rect)-kRoundedRadius)
		 controlPoint1:NSMakePoint(NSMaxX(rect), NSMaxY(rect))
		 controlPoint2:NSMakePoint(NSMaxX(rect), NSMaxY(rect))];
	
	/* Draw right side of box */
	[path lineToPoint:NSMakePoint(NSMaxX(rect), NSMinY(rect)+kTriangleWidth+kRoundedRadius)];
	
	/* Draw bottom-right curve */
	[path curveToPoint:NSMakePoint(NSMaxX(rect)-kRoundedRadius, NSMinY(rect)+kTriangleWidth)
		 controlPoint1:NSMakePoint(NSMaxX(rect), NSMinY(rect)+kTriangleWidth)
		 controlPoint2:NSMakePoint(NSMaxX(rect), NSMinY(rect)+kTriangleWidth)];
	
	/* Draw bottom-right side of box */
	[path lineToPoint:NSMakePoint(NSMidX(rect) + (kTriangleWidth * 0.5),
								  NSMinY(rect) + kTriangleWidth)];
	
	/* Draw triangle angle */
	[path lineToPoint:NSMakePoint(NSMidX(rect), NSMinY(rect))];
	
	/* Close */
	[path closePath];
	
	return path;
}

- (void)drawRect:(NSRect)dirtyRect 
{
    // Drawing code here.
	NSRect innerRect = NSMakeRect(NSMinX([self bounds])+1.0, NSMinY([self bounds])+1.0,
								  NSWidth([self bounds])-2.0, NSHeight([self bounds])-2.0);
	NSBezierPath *backPath = [LCPopupView pathForPopupInRect:innerRect];
	[backPath setLineWidth:1.0];
	NSGradient *gradient = [[NSGradient alloc] initWithStartingColor:[NSColor whiteColor] endingColor:[NSColor blackColor]];
	[gradient drawInBezierPath:backPath angle:-90.0];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.8] setFill];
	[backPath fill];
	[[NSColor colorWithCalibratedWhite:1.0 alpha:1.0] setStroke];
	[backPath stroke];
	
}

@synthesize triangleWidth;
@synthesize roundedRadius;
@synthesize textField;

- (NSRect) usableRect
{
	/* TextRect */
	return NSMakeRect(NSMinX([self bounds]), NSMinY([self bounds]) + triangleWidth,
					  NSWidth([self bounds]), NSHeight([self bounds]) - triangleWidth);
	
	
}

@end
