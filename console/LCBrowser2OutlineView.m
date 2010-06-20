//
//  LCBrowser2OutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowser2OutlineView.h"
#import "LCRoundedBezierPath.h"
#import "LCEntity.h"
#import "LCBrowser2Controller.h"

// CoreGraphics gradient helpers

typedef struct {
	CGFloat red1, green1, blue1, alpha1;
	CGFloat red2, green2, blue2, alpha2;
} _twoColorsType;

void _linearColorBlendFunction3(void *info, const CGFloat *in, CGFloat *out)
{
	_twoColorsType *twoColors = info;
	
	out[0] = (1.0 - *in) * twoColors->red1 + *in * twoColors->red2;
	out[1] = (1.0 - *in) * twoColors->green1 + *in * twoColors->green2;
	out[2] = (1.0 - *in) * twoColors->blue1 + *in * twoColors->blue2;
	out[3] = (1.0 - *in) * twoColors->alpha1 + *in * twoColors->alpha2;
}

void _linearColorReleaseInfoFunction3(void *info)
{
	free(info);
}

static const CGFunctionCallbacks linearFunctionCallbacks = {0,
	&_linearColorBlendFunction3, &_linearColorReleaseInfoFunction3};

@implementation LCBrowser2OutlineView

- (void) awakeFromNib
{
	[self setDelegate:self];
}

- (id)_highlightColorForCell:(NSCell *)cell
{ return nil; }

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
		lighterColor = [[lighterColor colorUsingColorSpaceName:NSDeviceWhiteColorSpace] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
		darkerColor = [[darkerColor colorUsingColorSpaceName:NSDeviceWhiteColorSpace] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
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
		NSDivideRect(rowRect, &topBar, &washRect, 1.0, NSMinYEdge);
		
		// Draw a soft wash underneath it
		CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(context); 
		{
			CGContextClipToRect(context, (CGRect){{NSMinX(washRect), NSMinY(washRect)}, {NSWidth(washRect),NSHeight(washRect)}});
			CGShadingRef cgShading = CGShadingCreateAxial(colorSpace, CGPointMake(0, NSMinY(washRect)), CGPointMake(0,NSMaxY(washRect)), linearBlendFunctionRef, NO, NO);
			CGContextDrawShading(context, cgShading);
			CGShadingRelease(cgShading);
		} 
		CGContextRestoreGState(context);
		
		rowIndex = newRowIndex;
	}
	
	CGFunctionRelease(linearBlendFunctionRef);
	CGColorSpaceRelease(colorSpace);	
}

- (void) selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend
{
	[super selectRowIndexes:indexes byExtendingSelection:extend];
	[self setNeedsDisplay:YES];
}

- (void)deselectRow:(NSInteger)row;
{
	[super deselectRow:row];
	[self setNeedsDisplay:YES]; 
}

#pragma mark "Outline Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	[cell setEntity:[item representedObject]];
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

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	/* Call super to handle selection */
	[super menuForEvent:event];
	
	/* Selection-Specific Handling */
	NSArray *selectedEntities = [(LCBrowser2Controller *)browserController selectedEntities];
	LCEntity *selectedEntity = nil;
	if ([selectedEntities count] > 0) selectedEntity = [selectedEntities objectAtIndex:0];
	if ([[selectedEntity typeInteger] intValue] == 1)
	{
		/* Customer selected */
		[connectToMenu setEnabled:NO];
		[editSiteMenu setAction:nil];
		[removeSiteMenu setAction:nil];
		[addDeviceMenu setAction:nil];
		[editDeviceMenu setAction:nil];
		[removeDeviceMenu setAction:nil];
		[addServiceMenu setAction:nil];
		[manageServiceScriptsMenu setAction:nil];
	}
	else if ([[selectedEntity typeInteger] intValue] == 2)
	{
		/* Site selected */
		[connectToMenu setEnabled:NO];		
		[editSiteMenu setAction:@selector(editSiteClicked:)];
		[removeSiteMenu setAction:@selector(removeSiteClicked:)];
		[addDeviceMenu setAction:@selector(addNewDeviceClicked:)];
		[editDeviceMenu setAction:nil];
		[removeDeviceMenu setAction:nil];
		[addServiceMenu setAction:nil];
		[manageServiceScriptsMenu setAction:nil];
	}
	else if ([[selectedEntity typeInteger] intValue] == 3)
	{
		/* Device selected */
		[connectToMenu setEnabled:YES];
		[editSiteMenu setAction:nil];
		[removeSiteMenu setAction:nil];
		[addDeviceMenu setAction:@selector(addNewDeviceClicked:)];
		[editDeviceMenu setAction:@selector(editDeviceClicked:)];
		[removeDeviceMenu setAction:@selector(removeDeviceClicked:)];
		[addServiceMenu setAction:@selector(addNewServiceClicked:)];
		[manageServiceScriptsMenu setAction:@selector(manageServiceScriptsClicked:)];
	}
	
	return [self menu];
}

@synthesize browserController;
@synthesize connectToMenu;
@synthesize addSiteMenu;
@synthesize editSiteMenu;
@synthesize removeSiteMenu;
@synthesize addDeviceMenu;
@synthesize editDeviceMenu;
@synthesize removeDeviceMenu;
@synthesize addServiceMenu;
@synthesize manageServiceScriptsMenu;
@end
