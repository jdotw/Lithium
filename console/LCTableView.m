//
//  LCTableView.m
//  Lithium Console
//
//  Created by James Wilson on 4/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTableView.h"
#import "LCRoundedBezierPath.h"

/* CoreGraphics gradient helpers */

typedef struct {
	CGFloat red1, green1, blue1, alpha1;
	CGFloat red2, green2, blue2, alpha2;
} _twoColorsType;

void _tvLinearColorBlendFunction(void *info, const CGFloat *in, CGFloat *out)
{
	_twoColorsType *twoColors = info;
	
	out[0] = (1.0 - *in) * twoColors->red1 + *in * twoColors->red2;
	out[1] = (1.0 - *in) * twoColors->green1 + *in * twoColors->green2;
	out[2] = (1.0 - *in) * twoColors->blue1 + *in * twoColors->blue2;
	out[3] = (1.0 - *in) * twoColors->alpha1 + *in * twoColors->alpha2;
}

void _tvLinearColorReleaseInfoFunction(void *info)
{
	free(info);
}

static const CGFunctionCallbacks linearFunctionCallbacks = {0,
	&_tvLinearColorBlendFunction, &_tvLinearColorReleaseInfoFunction};

@implementation LCTableView

#pragma Initialisation

- (id) init
{
	return [super init];
}

- (void) dealloc
{
	[tableContentKey release];
	[doubleClickTargetKey release];
	[doubleClickTargetSelectorName release];
	[doubleClickArgumentKey release];
	[super dealloc];
}

#pragma Binding Intervention

- (void)bind:(NSString *)binding 
	toObject:(id)observable
 withKeyPath:(NSString *)keyPath 
	 options:(NSDictionary *)options
{
	if ([binding isEqualToString:@"content"] )
	{
		tableContentController = observable;
		[tableContentKey release];
		tableContentKey = [keyPath copy];
	}
	if ([binding isEqualToString:@"doubleClickTarget"] )
	{
		doubleClickTarget = observable;
		[doubleClickTargetKey release];
		doubleClickTargetKey = [keyPath copy];
		doubleClickTargetSelectorName = [[options objectForKey:NSSelectorNameBindingOption] retain];
	}
	if ([binding isEqualToString:@"doubleClickArgument"] )
	{
		doubleClickArgument = observable;
		[doubleClickArgumentKey release];
		doubleClickArgumentKey = [keyPath copy];
	}
	
	[super bind:binding toObject:observable withKeyPath:keyPath options:options];
}

- (void)unbind:(NSString *)binding
{
	[super unbind:binding];
	
	if ( [binding isEqualToString:@"content"] )
	{
		tableContentController = nil;
		[tableContentKey release];
		tableContentKey = nil;
	}
	if ( [binding isEqualToString:@"doubleClickTarget"] )
	{
		doubleClickTarget = nil;
		[doubleClickTargetKey release];
		doubleClickTargetKey = nil;
	}
	if ( [binding isEqualToString:@"doubleClickArgument"] )
	{
		doubleClickArgument = nil;
		[doubleClickArgumentKey release];
		doubleClickArgumentKey = nil;
	}
}

#pragma Key Down Handling

- (void)keyDown:(NSEvent *)event
{
	unichar key = [[event charactersIgnoringModifiers] characterAtIndex:0];
    
	/* get flags and strip the lower 16 (device dependant) bits */
	unsigned int flags = ( [event modifierFlags] & 0x00FF );
	
	if (((key == NSDeleteCharacter) || (key == NSBackspaceCharacter)) && (flags == 0))
	{ 
		/* Delete/Backspace Key */
		if (allowDeleteKey)
		{
			if ([self selectedRow] == -1)
			{ NSBeep(); }
			else
			{ 
				/* Remove from array */
				[tableContentController removeObjectsAtArrangedObjectIndexes:[self selectedRowIndexes]]; 
			}
		}
	}
	else if (((key == NSEnterCharacter) || (key == NSCarriageReturnCharacter)) && (flags == 0))
	{
		/* Enter key */
		if ([self selectedRow] == -1)
		{ NSBeep(); }
		else
		{ 
			/* Call double-click action */
			if (doubleClickTarget && doubleClickTargetKey && doubleClickTargetSelectorName)
			{
				SEL doubleClickTargetSelector = NSSelectorFromString (doubleClickTargetSelectorName);
				[[doubleClickTarget valueForKey:@"content"] performSelector:doubleClickTargetSelector
																 withObject:[doubleClickArgument valueForKey:doubleClickArgumentKey]];
			}
		}
	}
	else if ((key == NSUpArrowFunctionKey) && ([event modifierFlags] & NSCommandKeyMask))
	{
		[tableContentController setSelectionIndex:0];		
	}
	else if ((key == NSDownArrowFunctionKey) && ([event modifierFlags] & NSCommandKeyMask))
	{
		[tableContentController setSelectionIndex:[[tableContentController arrangedObjects] count] -1];		
	}
	else
	{
		[super keyDown:event];
	}
}

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:point];
	int col = [self columnAtPoint:point];
	
	if (row == -1 || col == -1) return nil;
	
	if (![self isRowSelected:row])
	{
		[self selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
	}
	
	[[self window] makeFirstResponder:self];

	return [self menu];
}

#pragma mark "Gradient Highlights"

- (BOOL) NOisOpaque
{ return NO; }

- (id) NO_highlightColorForCell:(NSCell *)cell
{ return nil; }

- (void) NOhighlightSelectionInClipRect:(NSRect)clipRect
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
			CGContextClipToRect(context, (CGRect){{NSMinX(rowRect), NSMinY(rowRect)}, {NSWidth(rowRect),NSHeight(rowRect)}});
			NSBezierPath *path = [NSBezierPath bezierPathWithRect:rowRect];
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

#pragma mark "Drag Image Handling"

- (NSImage *)dragImageForRowsWithIndexes:(NSIndexSet *)dragRows 
							tableColumns:(NSArray *)tableColumns
								   event:(NSEvent *)dragEvent 
								  offset:(NSPointPointer)dragImageOffset
{
	/* Create image */
	NSImage *viewImage = [super dragImageForRowsWithIndexes:dragRows
											   tableColumns:tableColumns
													  event:dragEvent
													 offset:dragImageOffset];
	NSImage *dragImage = [[[NSImage alloc] initWithSize:NSMakeSize([viewImage size].width+8, [viewImage size].height+8)] autorelease];
	[dragImage lockFocus];
	NSBezierPath *path = [LCRoundedBezierPath pathInRect:NSMakeRect(0,0,[dragImage size].width,[dragImage size].height)];
	[[NSColor colorWithCalibratedWhite:0.2 alpha:0.6] setFill];
	[path fill];
	[viewImage drawInRect:NSMakeRect(4,4,[viewImage size].width, [viewImage size].height) 
				 fromRect:NSMakeRect(0,0,[viewImage size].width, [viewImage size].height) 
				operation:NSCompositeSourceOver
				 fraction:1.0];
	[dragImage unlockFocus];
	
	return dragImage;
}

@synthesize tableContentController;
@synthesize tableContentKey;
@synthesize doubleClickTarget;
@synthesize doubleClickTargetKey;
@synthesize doubleClickTargetSelectorName;
@synthesize doubleClickArgument;
@synthesize doubleClickArgumentKey;
@synthesize allowDeleteKey;

@end
