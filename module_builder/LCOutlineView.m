//
//  LCOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCOutlineView.h"
#import "LCRoundedBezierPath.h"

@implementation LCOutlineView

// CoreGraphics gradient helpers

typedef struct {
	float red1, green1, blue1, alpha1;
	float red2, green2, blue2, alpha2;
} _twoColorsType;

void _linearColorBlendFunction4(void *info, const float *in, float *out)
{
	_twoColorsType *twoColors = info;
	
	out[0] = (1.0 - *in) * twoColors->red1 + *in * twoColors->red2;
	out[1] = (1.0 - *in) * twoColors->green1 + *in * twoColors->green2;
	out[2] = (1.0 - *in) * twoColors->blue1 + *in * twoColors->blue2;
	out[3] = (1.0 - *in) * twoColors->alpha1 + *in * twoColors->alpha2;
}

void _linearColorReleaseInfoFunction4(void *info)
{
	free(info);
}

static const CGFunctionCallbacks linearFunctionCallbacks = {0,
	&_linearColorBlendFunction4, &_linearColorReleaseInfoFunction4};

#pragma Initialisation

- (id) init
{
	return [super init];
}

- (void) dealloc
{
	[outlineContentKey release];
	[doubleClickTargetKey release];
	[doubleClickTargetSelectorName release];
	[doubleClickArgumentKey release];
	[super dealloc];
}

#pragma Binding Intervention

- (void) bind:(NSString *)binding 
	toObject:(id)observable
 withKeyPath:(NSString *)keyPath 
	 options:(NSDictionary *)options
{
	if ([binding isEqualToString:@"content"] )
	{
		outlineContentController = observable;
		[outlineContentKey release];
		outlineContentKey = [keyPath copy];
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
		outlineContentController = nil;
		[outlineContentKey release];
		outlineContentKey = nil;
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
	
	if (((key == NSEnterCharacter) || (key == NSCarriageReturnCharacter)) && (flags == 0))
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
		[outlineContentController setSelectionIndex:0];		
	}
	else if ((key == NSDownArrowFunctionKey) && ([event modifierFlags] & NSCommandKeyMask))
	{
		[outlineContentController setSelectionIndex:[[outlineContentController arrangedObjects] count] -1];		
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
		[self selectRow:row byExtendingSelection:NO]; 
	}
	
	return [self menu];
}

#pragma mark "Highlight Methods"

- (BOOL) GRADisOpaque
{ return NO; }

- (id) GRAD_highlightColorForCell:(NSCell *)cell
{ return nil; }

- (void) GRADhighlightSelectionInClipRect:(NSRect)clipRect
{
	// Take the color apart
	NSColor *alternateSelectedControlColor = [NSColor alternateSelectedControlColor];
	float hue, saturation, brightness, alpha;
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
		alternateSelectedControlColor = [[alternateSelectedControlColor colorUsingColorSpaceName:NSDeviceWhiteColorSpace] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
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
	static const float domainAndRange[8] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
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
			//			NSBezierPath *path = [self outlinePath:rowRect];
			//			[path addClip];
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

- (void)selectRow:(int)row byExtendingSelection:(BOOL)extend;
{
	[super selectRow:row byExtendingSelection:extend];
	[self setNeedsDisplay:YES]; 
}

- (void)deselectRow:(int)row;
{
	[super deselectRow:row];
	[self setNeedsDisplay:YES]; 
}

@end
