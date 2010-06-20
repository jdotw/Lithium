//
//  LCShadowMetricField.m
//  Lithium Console
//
//  Created by James Wilson on 28/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCShadowMetricField.h"
#import "LCMetricGraphDocument.h"
#import "LCTrigger.h"

@implementation LCShadowMetricField

@synthesize metric;

- (void) awakeFromNib
{
	NSRect frameRect = NSMakeRect(NSMinX([self frame]), NSMinY([self frame]), NSWidth([self frame]), NSHeight([self frame])+4.0f);
	[self setFrame:frameRect];
	[super awakeFromNib];
	
	/* Create button */
	historyButtonGraphImage = [NSImage imageNamed:@"GraphRollOver.png"];
	historyButtonTableImage = [NSImage imageNamed:@"HistoryRollOver.png"];
	historyButton = [[NSButton alloc] initWithFrame:NSMakeRect(NSMaxX([self bounds]) - 24.0, NSMinY([self bounds]) + 2.0, 
															   [historyButtonGraphImage size].width, [historyButtonGraphImage size].height)];
	[historyButton setBordered:NO];
	[historyButton setHidden:YES];
	[historyButton setTarget:self];
	[historyButton setAction:@selector(historyButtonClicked:)];
	[[historyButton cell] setHighlightsBy:NSContentsCellMask];
	[self addSubview:historyButton];	
}

- (void) setFrame:(NSRect)value
{
	[super setFrame:value];
	[historyButton setFrame:NSMakeRect(NSMaxX([self bounds]) - [historyButtonGraphImage size].width - 2.0, NSMinY([self bounds]) + 2.0, 
									   [historyButtonGraphImage size].width, [historyButtonGraphImage size].height)];
}

- (void) updateTrackingAreas
{
	[super updateTrackingAreas];
	if (trackArea)
	{
		[self removeTrackingArea:trackArea];
		[trackArea release];
	}
	trackArea = [[NSTrackingArea alloc] initWithRect:[self bounds] 
											 options:NSTrackingMouseEnteredAndExited|NSTrackingActiveAlways 
											   owner:self
											userInfo:nil];
	[self addTrackingArea:trackArea];
}

- (void) dealloc
{
	[metric release];
	[historyButton release];
	[super dealloc];
}	

- (void) drawRect:(NSRect)rect
{
	/* Chedk button image */
	if (metric.isGraphable)
	{
		if ([historyButton image] != historyButtonGraphImage)
		{ [historyButton setImage:historyButtonGraphImage]; }
	}
	else
	{
		if ([historyButton image] != historyButtonTableImage)
		{ [historyButton setImage:historyButtonTableImage]; }
	}

	
	/* Draw Outline */
	NSRect outlineRect = NSMakeRect(0.0, 1.0, NSWidth([self frame]), NSHeight([self frame])-1);
	if (mouseOver)
	{
		/* Draw Base Outline and Fill */
		NSBezierPath *outlinePath = [NSBezierPath bezierPathWithRoundedRect:outlineRect xRadius:(NSHeight(outlineRect)*0.5f) yRadius:(NSHeight(outlineRect)*0.5f)];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
		[outlinePath fill];
		[[NSColor colorWithCalibratedWhite:1.0 alpha:0.1] setStroke];
		[outlinePath setLineWidth:0.5];
		[outlinePath stroke];
	}
	
	if ([metric.units isEqualToString:@"%"])
	{
		/* Draw Base Outline and Fill */
		NSBezierPath *outlinePath = [NSBezierPath bezierPathWithRoundedRect:outlineRect xRadius:(NSHeight(outlineRect)*0.5f) yRadius:(NSHeight(outlineRect)*0.5f)];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
		[outlinePath fill];
		[[NSColor colorWithCalibratedWhite:1.0 alpha:0.1] setStroke];
		[outlinePath setLineWidth:0.5];
		[outlinePath stroke];
		
		/* Clip to outline */
		[[NSGraphicsContext currentContext] saveGraphicsState];
		[outlinePath addClip];
		
		/* Draw Triggers */
		CGFloat triggerBarHeight = NSHeight(outlineRect);
		for (LCTrigger *trigger in metric.children)
		{
			NSRect triggerRect = outlineRect;
			triggerRect.origin.y = NSMaxY(triggerRect) - triggerBarHeight;
			triggerRect.size.height = triggerBarHeight;
			switch (trigger.triggerType)
			{
				case TRGTYPE_LT:
					triggerRect.origin.x = 0.0;
					triggerRect.size.width *= trigger.xValue / 100.0;
					break;
				case TRGTYPE_GT:
					triggerRect.origin.x = triggerRect.size.width * (trigger.xValue / 100.0);
					triggerRect.size.width = triggerRect.size.width - triggerRect.origin.x;
					break;
				case TRGTYPE_RANGE:
					triggerRect.origin.x = triggerRect.size.width * (trigger.xValue / 100.0);
					triggerRect.size.width = (triggerRect.size.width * (trigger.yValue / 100.0)) - triggerRect.origin.x;
					break;
				default:
					continue;
			}
			NSColor *effectColor;
			switch (trigger.effect)
			{
				case 1:
				case 2:
					effectColor = [NSColor orangeColor];
					break;
				case 3:
					effectColor = [NSColor redColor];
					break;
			}
			[[effectColor colorWithAlphaComponent:0.05] setFill];
			NSBezierPath *triggerPath = [NSBezierPath bezierPathWithRect:triggerRect];
			[triggerPath fill];					
		}
		
		/* Establish percentage */
		float percentage = [[metric.currentValue rawValueString] floatValue] / 100.0f;
		NSRect gaugeRect = NSMakeRect(0.0, 0.0, NSWidth([self frame]) * percentage, NSHeight([self frame]));
		NSBezierPath *gaugePath = [NSBezierPath bezierPathWithRect:gaugeRect];
		[[NSColor colorWithCalibratedWhite:1.0 alpha:0.1] setFill];
		[gaugePath fill];
		
		/* Restore state (unclip) */
		[[NSGraphicsContext currentContext] restoreGraphicsState];
	}
	
	NSShadow* theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.35]];
	[theShadow set];
	[[self cell] drawInteriorWithFrame:NSMakeRect(1.0, 2.0, NSWidth([self bounds])-2.0, NSHeight([self bounds]))
								inView:self];
	[theShadow release];
}

- (void) removeFromSuperview
{
	[self removeTrackingArea:trackArea];
	[super removeFromSuperview];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	mouseOver = YES;
	if (metric.recordEnabled) [historyButton setHidden:NO];
	[self setNeedsDisplay:YES];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	mouseOver = NO;
	if (metric.recordEnabled) [historyButton setHidden:YES];
	[self setNeedsDisplay:YES];
}

- (IBAction) historyButtonClicked:(id)sender
{
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntities = [NSArray arrayWithObject:self.metric];
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

- (void)mouseDown:(NSEvent *)theEvent
{
}

- (void) mouseDragged:(NSEvent *)theEvent
{
    NSSize dragOffset = NSMakeSize(0.0, 0.0);
    NSPasteboard *pboard;
	
	NSImage *dragImage = [[NSImage alloc] initWithSize:[self frame].size];
	[dragImage lockFocus];
	[self drawRect:[self bounds]];
	[dragImage unlockFocus];
	[dragImage autorelease];
	
    pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
    [pboard declareTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"]  owner:self];
    [pboard setPropertyList:[NSArray arrayWithObject:[metric.entityDescriptor properties]] forType:@"LCEntityDescriptor"];
	
    [self dragImage:dragImage 
				 at:NSMakePoint(NSMinX([self bounds]), NSHeight([self frame]))
			 offset:dragOffset
			  event:theEvent pasteboard:pboard source:self slideBack:YES];	
}

@synthesize historyButton;

@end
