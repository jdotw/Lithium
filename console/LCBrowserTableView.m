//
//  LCBrowserTableView.m
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTableView.h"
#import "LCBrowserVerticalScroller.h"
#import "LCRoundedBezierPath.h"
#import "LCBrowserTableHeaderCell.h"
#import "LCBrowserTableHeaderView.h"
#import "LCBrowserTableCornerView.h"

// CoreGraphics gradient helpers

typedef struct {
	CGFloat red1, green1, blue1, alpha1;
	CGFloat red2, green2, blue2, alpha2;
} _twoColorsType;

void _linearColorBlendFunction2(void *info, const CGFloat *in, CGFloat *out)
{
	_twoColorsType *twoColors = info;
	
	out[0] = (1.0 - *in) * twoColors->red1 + *in * twoColors->red2;
	out[1] = (1.0 - *in) * twoColors->green1 + *in * twoColors->green2;
	out[2] = (1.0 - *in) * twoColors->blue1 + *in * twoColors->blue2;
	out[3] = (1.0 - *in) * twoColors->alpha1 + *in * twoColors->alpha2;
}

void _linearColorReleaseInfoFunction2(void *info)
{
	free(info);
}

static const CGFunctionCallbacks linearFunctionCallbacks = {0,
	&_linearColorBlendFunction2, &_linearColorReleaseInfoFunction2};

@implementation LCBrowserTableView

- (void) awakeFromNib
{
	[[self enclosingScrollView] setDrawsBackground:NO];
	
	LCBrowserVerticalScroller *vertScroller = [[LCBrowserVerticalScroller alloc] initWithFrame:[[[self enclosingScrollView] verticalScroller] frame]];
	[[self enclosingScrollView] setVerticalScroller:vertScroller];
	[vertScroller autorelease];
	if ([self usesAlternatingRowBackgroundColors])
	{ 
		vertScroller.lightenWhenNotMainWindow = YES;
		vertScroller.backgroundTintColor = [NSColor colorWithCalibratedWhite:0.0 alpha:0.28];
	}
	NSEnumerator *columnEnum = [[self tableColumns] objectEnumerator];
	NSTableColumn *column;
	while (column = [columnEnum nextObject])
	{
		NSFont *font = [[[column headerCell] font] retain];
		LCBrowserTableHeaderCell *cell = [[LCBrowserTableHeaderCell alloc] initTextCell:[[column headerCell] stringValue]];
		[column setHeaderCell:cell];
		[cell autorelease];
		[[column headerCell] setFont:font];
		[font release];
	}
	if ([self headerView])
	{
		[self setHeaderView:[[LCBrowserTableHeaderView alloc] initWithFrame:[[self headerView] frame]]];
		[self setCornerView:[[LCBrowserTableCornerView alloc] initWithFrame:[[self cornerView] frame]]];
	}
//	[self setBackgroundTintColor:[NSColor colorWithCalibratedRed:32/255.0 green:38/255.0 blue:55/255.0 alpha:0.6]];
//	[self setRoundedSelection:YES];
}

- (BOOL) isOpaque
{ return NO; }

- (void) drawBackgroundInClipRect:(NSRect)clipRect
{
	if ([self usesAlternatingRowBackgroundColors])
	{
		int i;
		for (i=0; i < (NSHeight([self bounds]) / ([self rowHeight] + 2)); i++)
		{
			NSRect cellFrame = NSMakeRect(NSMinX([self bounds]),NSMinY([self bounds]) + (i * ([self rowHeight] + 2)),
										  NSWidth([self bounds]), [self rowHeight]+2);
			NSBezierPath *path;
			path = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX(cellFrame),NSMinY(cellFrame),
															   NSWidth(cellFrame),NSHeight(cellFrame))];
			
			if (i % 2)
			{
				[[NSColor colorWithCalibratedWhite:0.0 alpha:0.25] setFill];
				[path fill];
			}
			else
			{
				[[NSColor colorWithCalibratedWhite:0.0 alpha:0.30] setFill];
				[path fill];
			}
		}	
	}
}

- (void) drawRect:(NSRect)rect
{
	if (backgroundTintColor)
	{
		NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:[self bounds]];
		[[self backgroundTintColor] setFill];
		[backPath fill];
	}
	[super drawRect:rect];
}

- (id)_highlightColorForCell:(NSCell *)cell
{ return nil; }

- (void) ROUNDhighlightSelectionInClipRect:(NSRect)clipRect
{
	NSBezierPath *outlinePath = [self outlinePath:clipRect];
	[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setFill];
	[outlinePath fill];
}

- (NSBezierPath *) outlinePath:(NSRect)frame
{
	float arcWidth=15;
	float height = frame.size.height;
	float width = frame.size.width;
	float x = frame.origin.x;
	float y = frame.origin.y;
	NSBezierPath *path = [NSBezierPath bezierPath];
	
	/* Bottom line */
	[path moveToPoint:NSMakePoint(x+arcWidth,y)];
	[path lineToPoint:NSMakePoint(x+(width-arcWidth), y)];
	
	/* Bottom right */
	[path curveToPoint:NSMakePoint(x+width,y+arcWidth) controlPoint1:NSMakePoint(x+width,y) controlPoint2:NSMakePoint(x+width,y)];
	
	/* Re-size bar */
	//	[path moveToPoint:NSMakePoint(x+(width-8), y)];
	//	[path lineToPoint:NSMakePoint(x+width, y+8)];
	
	/* Right line */
	[path lineToPoint:NSMakePoint(x+width,y+(height-arcWidth))];
	
	/* Top right */
	[path curveToPoint:NSMakePoint(x+(width-arcWidth), y+height) controlPoint1:NSMakePoint(x+width, y+height) controlPoint2:NSMakePoint(x+width, y+height)];
	
	/* Top line */
	[path lineToPoint:NSMakePoint(x+arcWidth, y+height)];
	
	/* Top leftt */
	[path curveToPoint:NSMakePoint(x, y+(height-arcWidth)) controlPoint1:NSMakePoint(x, y+height) controlPoint2:NSMakePoint(x, y+height)];
	
	/* Left line */
	[path lineToPoint:NSMakePoint(x, y+arcWidth)];
	
	/* Bottom left */
	[path curveToPoint:NSMakePoint(x+arcWidth, y) controlPoint1:NSMakePoint(x, y) controlPoint2:NSMakePoint(x, y)];
	
	return path;
}

- (void) highlightSelectionInClipRect:(NSRect)clipRect
{
	// Take the color apart
	NSColor *alternateSelectedControlColor = [NSColor alternateSelectedControlColor];
	CGFloat hue, saturation, brightness, alpha;
	[[alternateSelectedControlColor colorUsingColorSpaceName:NSDeviceRGBColorSpace] getHue:&hue saturation:&saturation brightness:&brightness alpha:&alpha];
	alpha=0.8;
	
	// Create synthetic darker and lighter versions
	NSColor *lighterColor = [NSColor colorWithDeviceHue:hue
											 saturation:MAX(0.0, saturation-.12) brightness:MIN(1.0,
																								brightness+0.30) alpha:alpha];
	NSColor *darkerColor = [NSColor colorWithDeviceHue:hue
											saturation:MIN(1.0, (saturation > .04) ? saturation+0.12 :
														   0.0) brightness:MAX(0.0, brightness-0.045) alpha:alpha];
	
	// If this view isn't key, use the gray version of the dark color.
	// Note that this varies from the standard gray version that NSCell
	//	returns as its highlightColorWithFrame: when the cell is not in a
	//	key view, in that this is a lot darker. Mike and I think this is
	//	justified for this kind of view -- if you're using the dark
	//		selection color to show the selected status, it makes sense to
	//		leave it dark.
	NSResponder *firstResponder = [[self window] firstResponder];
	if (![firstResponder isKindOfClass:[NSView class]] || ![(NSView *)firstResponder isDescendantOf:self] || ![[self window] isKeyWindow]) 
	{
		lighterColor = [lighterColor colorWithAlphaComponent:0.4];
		darkerColor = [darkerColor colorWithAlphaComponent:0.4];
	}
	
	// Set up the helper function for drawing washes
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	_twoColorsType *twoColors = malloc(sizeof(_twoColorsType)); 
	// We malloc() the helper data because we may draw this wash during
	// printing, in which case it won't necessarily be evaluated
	// immediately. We need for all the data the shading function needs
	// to draw to potentially outlive us.
	[lighterColor getRed:&twoColors->red1 green:&twoColors->green1
					blue:&twoColors->blue1 alpha:&twoColors->alpha1];
	[darkerColor getRed:&twoColors->red2 green:&twoColors->green2
				   blue:&twoColors->blue2 alpha:&twoColors->alpha2];
	static const CGFloat domainAndRange[8] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	CGFunctionRef linearBlendFunctionRef = CGFunctionCreate(twoColors, 1, domainAndRange, 4, domainAndRange, &linearFunctionCallbacks);
	
	NSIndexSet *selectedRowIndexes = [self selectedRowIndexes];
	unsigned int rowIndex = [selectedRowIndexes indexGreaterThanOrEqualToIndex:0];
	
	while (rowIndex != NSNotFound) 
	{
		unsigned int endOfCurrentRunRowIndex, newRowIndex = rowIndex;
		do 
		{
			endOfCurrentRunRowIndex = newRowIndex;
			newRowIndex = [selectedRowIndexes indexGreaterThanIndex:endOfCurrentRunRowIndex];
		} while (newRowIndex == endOfCurrentRunRowIndex + 1);
		
		NSRect rowRect = NSUnionRect([self rectOfRow:rowIndex], [self rectOfRow:endOfCurrentRunRowIndex]);				
		NSRect topBar, washRect;
		NSDivideRect(rowRect, &topBar, &washRect, 0.0, NSMinYEdge);
		
		// Draw a soft wash underneath it
		CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(context); 
		{
			CGContextClipToRect(context, (CGRect){{NSMinX(rowRect), NSMinY(rowRect)}, {NSWidth(rowRect),NSHeight(rowRect)}});
			NSBezierPath *path;
			if (roundedSelection)
			{ path = [LCRoundedBezierPath pathInRect:rowRect]; }
			else
			{ path = [NSBezierPath bezierPathWithRect:rowRect]; }
			[path addClip];
			CGShadingRef cgShading = CGShadingCreateAxial(colorSpace, CGPointMake(0, NSMinY(rowRect)), CGPointMake(0,NSMaxY(rowRect)), linearBlendFunctionRef, NO, NO);
			CGContextDrawShading(context, cgShading);
			CGShadingRelease(cgShading);
		} 
		CGContextRestoreGState(context);
		
		rowIndex = newRowIndex;
	}
	
	CGFunctionRelease(linearBlendFunctionRef);
	CGColorSpaceRelease(colorSpace);	
}

- (void)selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend
{
	if (!denySelection)
	{ [super selectRowIndexes:indexes byExtendingSelection:extend]; }
	[self setNeedsDisplay:YES];
}

@synthesize denySelection;

- (void)deselectRow:(NSInteger)row;
{
	[super deselectRow:row];
	[self setNeedsDisplay:YES]; 
}

- (NSColor *) backgroundTintColor
{ return backgroundTintColor; }

- (void) setBackgroundTintColor:(NSColor *)value
{ 
	[backgroundTintColor release];
	backgroundTintColor = [value retain];
	LCBrowserVerticalScroller *scroller = [[LCBrowserVerticalScroller alloc] initWithFrame:[[[self enclosingScrollView] verticalScroller] frame]];
	[scroller setBackgroundTintColor:value];
	[[self enclosingScrollView] setVerticalScroller:scroller];
	[scroller autorelease];
	[self setNeedsDisplay:YES];
}

- (BOOL) roundedSelection
{ return roundedSelection; }

- (void) setRoundedSelection:(BOOL)value
{ roundedSelection = value; }

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	if ([self delegate] != self && [[self delegate] respondsToSelector:@selector(menuForEvent:)])
	{ return [[self delegate] performSelector:@selector(menuForEvent:) withObject:event]; }
	else
	{ return [super menuForEvent:event]; }
}

@end
