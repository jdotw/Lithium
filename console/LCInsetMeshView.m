//
//  LCInsetMeshView.m
//  Lithium Console
//
//  Created by James Wilson on 2/10/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCInsetMeshView.h"


@implementation LCInsetMeshView

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect 
{
	[[NSColor blackColor] setFill];
	NSRectFill([self bounds]);
	
	CGFloat meshScale = 0.21;
	NSImage *meshImage = [NSImage imageNamed:@"mesh.png"];	
	NSRect meshRect = NSMakeRect(0.0, 0.0, roundf([meshImage size].width * meshScale), roundf([meshImage size].height * meshScale));
	while (meshRect.origin.y < NSMaxY([self bounds]))
	{
		meshRect.origin.x = 0.0;
		while (meshRect.origin.x <= NSMaxX([self bounds]))
		{
			[meshImage drawInRect:meshRect
						 fromRect:NSMakeRect(0.0, 0.0, [meshImage size].width, [meshImage size].height)
						operation:NSCompositeSourceOver
						 fraction:0.30];
			meshRect.origin.x = meshRect.origin.x + NSWidth(meshRect);
		}
		meshRect.origin.y = meshRect.origin.y + NSHeight(meshRect);
	}
	
	NSGradient *innerShadowGrad = [[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.0 alpha:0.3]
																 endingColor:[NSColor clearColor]] autorelease];
	
	CGFloat innerShadowRadius = 16.0;
	[innerShadowGrad drawInRect:NSMakeRect(NSMinX([self bounds]), NSMinY([self bounds]), 
										   innerShadowRadius, NSHeight([self bounds]))
						  angle:0.0];
	[innerShadowGrad drawInRect:NSMakeRect(NSMinX([self bounds]), NSMaxY([self bounds])-innerShadowRadius, 
										   NSWidth([self bounds]), innerShadowRadius)
						  angle:-90.0];
	innerShadowGrad = [[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.0 alpha:0.1]
													 endingColor:[NSColor clearColor]] autorelease];
	
	[innerShadowGrad drawInRect:NSMakeRect(NSMaxX([self bounds])-(innerShadowRadius*0.5), NSMinY([self bounds]), 
										   innerShadowRadius*0.5, NSHeight([self bounds]))
						  angle:180.0];
	[innerShadowGrad drawInRect:NSMakeRect(NSMinX([self bounds]), NSMinY([self bounds]), 
										   NSWidth([self bounds]), innerShadowRadius*0.5)
						  angle:90.0];
	
	NSGradient *overShadowGrad = [[[NSGradient alloc] initWithStartingColor:[NSColor clearColor]
																endingColor:[NSColor colorWithCalibratedWhite:0.0 alpha:0.3]] autorelease];
	[overShadowGrad drawInRect:[self bounds] relativeCenterPosition:NSMakePoint(0.0, 0.0)];
	
}

@end
