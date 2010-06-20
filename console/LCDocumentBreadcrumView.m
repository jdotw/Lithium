//
//  LCDocumentBreadcrumView.m
//  Lithium Console
//
//  Created by James Wilson on 18/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocumentBreadcrumView.h"
#import "LCDocumentBreadcrumItem.h"

@interface LCDocumentBreadcrumView (private)

- (void) resetButtons;

@end

@implementation LCDocumentBreadcrumView

#pragma mark "Constructor"

- (id) init
{
	self = [super init];
	if (!self) return nil;


	return self;
}

- (void) dealloc
{
	[items release];
	[buttons release];
	[super dealloc];
}

- (void) awakeFromNib
{
	items = [[NSMutableArray array] retain];
	buttons = [[NSMutableArray array] retain];

	[self resetButtons];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	/* Slot */
	NSImage *slotImage = [NSImage imageNamed:@"BreadCrumSlot"];
	[slotImage drawInRect:[self bounds]
				 fromRect:NSMakeRect(0.0, 0.0, [slotImage size].width, [slotImage size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	
	/* Editing */
	if (editing)
	{
		NSBezierPath *editOverlayPath = [NSBezierPath bezierPathWithRect:[self bounds]];
		[[NSColor colorWithCalibratedRed:255.0/255.0 green:219.0/255.0 blue:19.0/255.0 alpha:0.25] setFill];
		[editOverlayPath fill];
		
		NSImage *overlayImage = [NSImage imageNamed:@"BreadCrumEditOverlay.png"];
		NSRect overlayRect = NSMakeRect(NSMinX([self bounds]), NSMinY([self bounds]), [overlayImage size].width, NSHeight([self bounds]));
		while (NSMinX(overlayRect) < NSMaxX([self bounds]))
		{
			[overlayImage drawInRect:overlayRect
							fromRect:NSMakeRect(0.0, 0.0, [overlayImage size].width, [overlayImage size].height)
						   operation:NSCompositeSourceOver
							fraction:1.0];
			overlayRect.origin.x = overlayRect.origin.x + overlayRect.size.width;
		}
	}
}

#pragma mark "Button Management"

- (void) resetButtons
{
	/* Remove old */
	while (buttons.count > 0)
	{
		NSButton *button = [buttons objectAtIndex:0];
		[button removeFromSuperview];
		[buttons removeObjectAtIndex:0];
	}
	
	/* Create new */
	float buttonWidth = 140.0;
	float buttonOffset = 120.0;
	int i;
	for (i=(items.count-1); i >= 0; i--)
	{
		LCDocumentBreadcrumItem *item = [items objectAtIndex:i];
		NSRect buttonFrame = NSMakeRect(NSMinX([self bounds]) + (i * buttonOffset),
										NSMinY([self bounds]), buttonWidth, NSHeight([self bounds]));
		NSButton *button = [[NSButton alloc] initWithFrame:buttonFrame];
		[button setTitle:item.title];
		[button setBordered:NO];
		[button setImagePosition:NSImageOverlaps];
		[[button cell] setHighlightsBy:NSContentsCellMask];
		[button setImage:[NSImage imageNamed:@"BreadCrumButton"]];
		[button setTarget:delegate];
		[button setAction:@selector(breadcrumClicked:)];
		[[button cell] setRepresentedObject:item];
		[self addSubview:button];
	}
}

#pragma mark "Properties"

@synthesize items;
- (void) insertObject:(id)obj inItemsAtIndex:(unsigned int)index
{
	[items insertObject:obj atIndex:index];
	[self resetButtons];
}
- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{
	[items removeObjectAtIndex:index];
	[self resetButtons];
}

@synthesize delegate;
@synthesize editing;
- (void) setEditing:(BOOL)value
{
	editing = value;
	[self setNeedsDisplay:YES];
}

@end
