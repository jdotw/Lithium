//
//  LCBrowserTreeCell.m
//  Lithium Console
//
//  Created by James Wilson on 8/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeCell.h"

#import "LCCustomer.h"
#import "LCBrowserTreeItem.h"
#import "LCBrowserTreeDevicesRoot.h"
#import "LCBrowserTreeXsanRoot.h"
#import "LCBrowserTreeFaultsRoot.h"
#import "LCBrowserTreeIncidents.h"
#import "LCBrowserTreeCases.h"
#import "LCCustomerList.h"
#import "LCBrowserTreeDiscoveryRoot.h"
#import "LCBrowserTreeServices.h"
#import "LCBrowserTreeProcesses.h"
#import "LCGroup.h"
#import "LCBrowserTreeGroupsCustomer.h"
#import "LCBrowserTreeCoreDeployment.h"
#import "LCBrowserTreeCoreCustomer.h"
#import "LCBrowserTreeNetworkScan.h"
#import "LCBrowserTreeBonjour.h"
#import "LCBrowserTreeCoreProperty.h"

@implementation LCBrowserTreeCell

#pragma mark "Drawing Method"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	if ([parentRepresentedObject class] == [LCGroup class] && [[representedObject class] isSubclassOfClass:[LCEntity class]])
	{
		/* Group Entity */
		[self drawInteriorWithFrame:cellFrame inView:controlView forGroupEntity:representedObject];
	}
	else if (([parentRepresentedObject class] == [LCBrowserTreeServices class] || [parentRepresentedObject class] == [LCBrowserTreeProcesses class])
		&& [[representedObject class] isSubclassOfClass:[LCEntity class]])
	{
		/* App (process/service) Entity */
		[self drawInteriorWithFrame:cellFrame inView:controlView forApplicationEntity:representedObject];
	}	
	else if ([[representedObject class] isSubclassOfClass:[LCEntity class]])
	{
		/* Entity */
		LCEntity *entity = (LCEntity *)representedObject;
		BOOL bold;
		if (entity.type < 3) bold = YES;
		else bold = NO;
		[self drawInteriorWithFrame:cellFrame inView:controlView forEntity:representedObject bold:bold];
	}
	else if ([representedObject class] == [LCGroup class] ||
			 [representedObject class] == [LCBrowserTreeGroupsCustomer class] || 
			 [representedObject class] == [LCBrowserTreeServices class] ||
			 [representedObject class] == [LCBrowserTreeProcesses class])
	{
		/* Group-Style Entity */
		[self drawInteriorWithFrame:cellFrame inView:controlView forEntity:representedObject bold:YES];
	}
	else if ([[representedObject class] isSubclassOfClass:[LCBrowserTreeCoreDeployment class]])
	{
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forItem:representedObject bold:YES];
	}
	else if ([[representedObject class] isSubclassOfClass:[LCBrowserTreeCoreCustomer class]])
	{
		LCBrowserTreeCoreCustomer *customer = (LCBrowserTreeCoreCustomer *) representedObject;
		[self drawEntityStyleInteriorWithFrame:cellFrame inView:controlView opState:customer.opState bold:YES];
	}
	else if ([[representedObject className] hasSuffix:@"Root"])
	{ 
		/* Root Item */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forRootItem:representedObject];
	}
	else if ([representedObject class] == [LCBrowserTreeIncidents class])
	{
		/* Incident List */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forIncidents:representedObject];		
	}
	else if ([representedObject class] == [LCBrowserTreeCases class])
	{
		/* Cases List */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forCases:representedObject];		
	}
	else if ([[representedObject class] isSubclassOfClass:[LCBrowserTreeNetworkScan class]] ||
			 [[representedObject class] isSubclassOfClass:[LCBrowserTreeBonjour class]])
	{
		/* Scan/Bonjour Discovery */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forItem:representedObject bold:NO];
	}
	else if ([[representedObject class] isSubclassOfClass:[LCBrowserTreeCoreProperty class]])
	{
		/* Core Property */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forItem:representedObject bold:NO];
	}
	else if ([[representedObject class] isSubclassOfClass:[LCBrowserTreeItem class]])
	{ 
		/* Non-Entity item */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forItem:representedObject bold:YES];
	}
	else 
	{ 
		/* Default */
		[self drawInteriorWithFrame:cellFrame inView:controlView forOtherObject:representedObject];
	}
	
	/* Draw Refresh overlay */
	if ([representedObject refreshInProgress])
	{
		NSImage *refImage = [NSImage imageNamed:@"refresh_grey_16.tif"];
		[refImage drawInRect:NSMakeRect(NSMaxX(cellFrame)-18, NSMinY(cellFrame) + (int) ((NSHeight(cellFrame) - 15.0) * 0.5), 15.0, 15.0)
					fromRect:NSMakeRect(0, 0, [refImage size].width, [refImage size].height)
				   operation:NSCompositeSourceOver
					fraction:0.8];
	}	
}

#pragma mark "Root Item"

- (void) drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forRootItem:(LCBrowserTreeItem *)item
{
	float xOffset = 0.0;
	
	/* Draw icon */
	if ([item treeIcon])
	{
		[[item treeIcon] setFlipped:YES];
		[[item treeIcon] drawInRect:NSMakeRect(cellFrame.origin.x+7, NSMidY(cellFrame) - 6, 16, 16)
						   fromRect:NSMakeRect(0, 0, [[item treeIcon] size].width, [[item treeIcon] size].height)
						  operation:NSCompositeSourceOver
						   fraction:1.0];	
		xOffset = (cellFrame.size.height * 0.8) + 2;
	}
	
	/* Remeber defaults */
	NSFont *defaultFont = [self font];
	
	/* Set bold */
	[self setFont:[NSFont boldSystemFontOfSize:11.0]];

	/* Draw under coat */
	[self setTextColor:[NSColor colorWithCalibratedWhite:1.0 alpha:0.5]];
	[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x+0.0+xOffset,
											cellFrame.origin.y+1.0,
											cellFrame.size.width - xOffset, cellFrame.size.height) inView:controlView];	
	
	/* Draw text */
	[self setTextColor:[NSColor darkGrayColor]];
	[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x+xOffset,
											cellFrame.origin.y,
											cellFrame.size.width - xOffset, cellFrame.size.height) inView:controlView];		
	
	/* Restore defaults */
	[self setTextColor:[NSColor blackColor]];
	[self setFont:defaultFont];	
}


#pragma mark "Entity Drawing"

- (void) drawEntityStyleInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView opState:(int)opState bold:(BOOL)bold
{
	/* Draw dot */
	NSImage *image = nil;
	switch (opState)
	{
		case -3:
			image = [NSImage imageNamed:@"NoDot.tiff"];
			break;
		case -2:
			image = [NSImage imageNamed:@"BlueDotFlipped.tiff"];
			break;
		case 0:
			image = [NSImage imageNamed:@"GreenDotFlipped.tiff"];
			break;
		case 1:
			image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
			break;
		case 2:
			image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
			break;
		case 3:
			image = [NSImage imageNamed:@"RedDotFlipped.tiff"];
			break;
		default:
			image = [NSImage imageNamed:@"GreyDotFlipped.tiff"];	
	}
	float fraction;
	if ([self outlineViewEnabled]) fraction = 1.0;
	else fraction = 0.5;
	[image drawInRect:NSMakeRect(cellFrame.origin.x + 1.0, cellFrame.origin.y + 1.0, 
								 cellFrame.size.height - 2.0, cellFrame.size.height - 2.0)
			 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
			operation:NSCompositeSourceOver
			 fraction:fraction];

	if (bold && !selected)
	{
		/* Remeber defaults */
		NSFont *defaultFont = [self font];
		
		/* Set bold */
		[self setFont:[[NSFontManager sharedFontManager] convertFont:[self font] toHaveTrait:NSBoldFontMask]];
		
		/* Draw under coat */
		[self setTextColor:[NSColor colorWithCalibratedWhite:1.0 alpha:0.5]];
		[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x+13.0,
												cellFrame.origin.y+1.0,
												cellFrame.size.width - 13.0, cellFrame.size.height - 1.0) inView:controlView];	
		
		/* Draw text */
		[self setTextColor:[NSColor colorWithCalibratedWhite:0.2 alpha:1.0]];
		[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x + 13.0,
												cellFrame.origin.y,
												cellFrame.size.width - 13.0, cellFrame.size.height - 1.0) inView:controlView];		
		
		/* Restore defaults */
		[self setTextColor:[NSColor blackColor]];
		[self setFont:defaultFont];	
		
	}
	else 
	{
		/* Create reduced-size rectangle */
		NSRect textRect = NSMakeRect(cellFrame.origin.x + 13.0, cellFrame.origin.y,
									 cellFrame.size.width - 13.0, cellFrame.size.height - 1.0);
		
		/* Draw and restore colour */
		[super drawInteriorWithFrame:textRect inView:controlView];	
	}

}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forEntity:(LCEntity *)entity bold:(BOOL)bold
{
	if ([[[entity parent] name] isEqualTo:@"xsanvol"])
	{
		[self setStringValue:[NSString stringWithFormat:@"%@ @ %@", [entity displayString], [[entity device] displayString]]];
	}
	
	[self drawEntityStyleInteriorWithFrame:cellFrame 
									inView:controlView 
								   opState:entity.opState
									  bold:bold];
}

#pragma mark "Group Entity Drawing"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forGroupEntity:(LCEntity *)entity
{
	[self setStringValue:[entity longDisplayString]];
	[self drawEntityStyleInteriorWithFrame:cellFrame 
									inView:controlView 
								   opState:entity.opState
									  bold:NO];	
}

#pragma mark "Application Entity Drawing"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forApplicationEntity:(LCEntity *)entity
{
	[self setStringValue:[NSString stringWithFormat:@"%@ on %@ at %@", [entity displayString], [entity.device displayString], [entity.site displayString]]];
	[self drawEntityStyleInteriorWithFrame:cellFrame 
									inView:controlView 
								   opState:entity.opState
									  bold:NO];	
}

#pragma mark "Non-Entity Item Drawing"

- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forItem:(LCBrowserTreeItem *)item bold:(BOOL)bold
{
	NSImage *image;
	float xOffset = 0.0;
	
	/* Draw icon */
	if ([item treeIcon])
	{
		[[item treeIcon] setFlipped:YES];
		[[item treeIcon] drawInRect:NSMakeRect(cellFrame.origin.x+3, cellFrame.origin.y+1, cellFrame.size.height-2, cellFrame.size.height-2)
				 fromRect:NSMakeRect(0, 0, [[item treeIcon] size].width, [[item treeIcon] size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];	
		xOffset = (cellFrame.size.height) + 2;
	}
	
	/* Draw dot */
	switch (item.opState)
	{
		case -3:
			image = [NSImage imageNamed:@"NoDot.tiff"];
			break;
		case -2:
			image = [NSImage imageNamed:@"BlueDotFlipped.tiff"];
			break;
		case 0:
			image = [NSImage imageNamed:@"GreenDotFlipped.tiff"];
			break;
		case 1:
			image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
			break;
		case 2:
			image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
			break;
		case 3:
			image = [NSImage imageNamed:@"RedDotFlipped.tiff"];
			break;
		default:
			image = [NSImage imageNamed:@"GreyDotFlipped.tiff"];	
	}
	if (!item.opState)
	{ image = nil; }
	if (image)
	{
		float fraction;
		if ([self outlineViewEnabled]) fraction = 1.0;
		else fraction = 0.5;
		[image drawInRect:NSMakeRect(cellFrame.origin.x + xOffset, cellFrame.origin.y + 1.0, cellFrame.size.height - 2, cellFrame.size.height - 2)
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver
				 fraction:fraction];
		xOffset += 14;
	}
	
	if (bold && !selected)
	{
		/* Remeber defaults */
		NSFont *defaultFont = [self font];

		/* Set bold */
		[self setFont:[[NSFontManager sharedFontManager] convertFont:[self font] toHaveTrait:NSBoldFontMask]];
		
		/* Draw under coat */
		[self setTextColor:[NSColor colorWithCalibratedWhite:1.0 alpha:0.5]];
		[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x+0.0+xOffset,
												cellFrame.origin.y+1.0,
												cellFrame.size.width - xOffset, cellFrame.size.height) inView:controlView];	
		
		/* Draw text */
		[self setTextColor:[NSColor colorWithCalibratedWhite:0.2 alpha:1.0]];
		[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x+xOffset,
												cellFrame.origin.y,
												cellFrame.size.width - xOffset, cellFrame.size.height) inView:controlView];		
		
		/* Restore defaults */
		[self setTextColor:[NSColor blackColor]];
		[self setFont:defaultFont];	
	}
	else {
		/* Create reduced-size rectangle */
		if ([self outlineViewEnabled])
		{ [self setTextColor:[NSColor blackColor]]; }
		else
		{ [self setTextColor:[NSColor grayColor]]; }
		NSRect textRect = NSMakeRect(cellFrame.origin.x + xOffset,
									 cellFrame.origin.y,
									 cellFrame.size.width - xOffset, cellFrame.size.height - 1.0);
		[super drawInteriorWithFrame:textRect inView:controlView];	
		[self setTextColor:[NSColor blackColor]];		
	}

	
}

#pragma mark "Faults Item Drawing"

- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forIncidents:(LCBrowserTreeItem *)item
{
	[self drawItemInteriorWithFrame:cellFrame inView:controlView forItem:item bold:NO];
	
	NSArray *incidents = [[LCCustomerList masterArray] valueForKeyPath:@"@distinctUnionOfArrays.activeIncidentsList.incidents"];
	if (incidents.count > 0)
	{
		NSString *countString = [NSString stringWithFormat:@"%i", incidents.count];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
							  [NSFont boldSystemFontOfSize:10.0], NSFontAttributeName,
							  [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
							  nil];	
		NSSize stringSize = [countString sizeWithAttributes:attr];
		
		float rowHeight = NSHeight(cellFrame);
		float badgeHeight = rowHeight - 2.0;
		float badgeWidth = stringSize.width + badgeHeight; // +BadgeHeight for rounded corners 
		NSRect badgeRect = NSMakeRect(NSMaxX(cellFrame) - badgeWidth, 
									  NSMinY(cellFrame) + 1.0, 
									  badgeWidth, badgeHeight);
		NSBezierPath *badgePath = [NSBezierPath bezierPathWithRoundedRect:badgeRect
																  xRadius:(badgeHeight * 0.5) 
																  yRadius:(badgeHeight * 0.5)];
		[[NSColor colorWithCalibratedRed:133.0/256.0 green:152.0/256 blue:191.0/256.0 alpha:0.7] setFill];
		[badgePath fill];
		
		NSRect stringRect = NSMakeRect(NSMinX(badgeRect) + (stringSize.height * 0.5) + 1.0,
									   NSMinY(badgeRect) + 1.0, 
									   NSWidth(badgeRect) - badgeHeight, 
									   badgeHeight);
		[countString drawInRect:stringRect withAttributes:attr];
	}
}

- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forCases:(LCBrowserTreeItem *)item
{
	[self drawItemInteriorWithFrame:cellFrame inView:controlView forItem:item bold:NO];
	
	NSArray *cases = [[LCCustomerList masterArray] valueForKeyPath:@"@distinctUnionOfArrays.openCasesList.cases"];
	if (cases.count > 0)
	{
		NSString *countString = [NSString stringWithFormat:@"%i", cases.count];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
							  [NSFont boldSystemFontOfSize:10.0], NSFontAttributeName,
							  [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
							  nil];	
		NSSize stringSize = [countString sizeWithAttributes:attr];
		
		float rowHeight = NSHeight(cellFrame);
		float badgeHeight = rowHeight - 2.0;
		float badgeWidth = stringSize.width + badgeHeight; // +BadgeHeight for rounded corners 
		NSRect badgeRect = NSMakeRect(NSMaxX(cellFrame) - badgeWidth + 1.0, NSMinY(cellFrame) + ((rowHeight - badgeHeight) * 0.5), badgeWidth, badgeHeight);
		NSBezierPath *badgePath = [NSBezierPath bezierPathWithRoundedRect:badgeRect
																  xRadius:(badgeHeight * 0.5) 
																  yRadius:(badgeHeight * 0.5)];
		[[NSColor colorWithCalibratedRed:133.0/256.0 green:152.0/256 blue:191.0/256.0 alpha:0.7] setFill];
		[badgePath fill];
		
		NSRect stringRect = NSMakeRect(NSMinX(badgeRect) + (stringSize.height * 0.5),
									   NSMinY(badgeRect) + 0.5, NSWidth(badgeRect) - badgeHeight, badgeHeight);
		[countString drawInRect:stringRect withAttributes:attr];
	}
}

#pragma mark "Other Object Drawing"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forOtherObject:(id)object
{
	NSImage *image = nil;
	float xOffset = 0.0;
	
	/* Draw icon */
	if ([object respondsToSelector:@selector(typeInteger)])
	{
		switch ([[object typeInteger] intValue])
		{
			case 1:
				//			image = [NSImage imageNamed:@"network_16.tif"];
				break;
			case 2:
				//			image = [NSImage imageNamed:@"network_16.tif"];
				break;
			case 3:
				image = nil;
				break;
			case 4:
				image = nil;
				break;
			case 5:
				image = nil;
				break;			
			default:
				image = nil;	
		}
		if (image)
		{
			[image setFlipped:YES];
			[image drawInRect:NSMakeRect(cellFrame.origin.x+1, cellFrame.origin.y+(0.18 * cellFrame.size.height), cellFrame.size.height * 0.8, cellFrame.size.height * 0.8)
					 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
					operation:NSCompositeSourceOver
					 fraction:1.0];	
			xOffset = (cellFrame.size.height * 0.8) + 2;
		}
	}
	
	/* Draw dot */
	if ([object respondsToSelector:@selector(opstate)])
	{
		switch ((int)[object opstate])
		{
			case -3:
				image = [NSImage imageNamed:@"NoDot.tiff"];
				break;
			case -2:
				image = [NSImage imageNamed:@"BlueDotFlipped.tiff"];
				break;
			case 0:
				image = [NSImage imageNamed:@"GreenDotFlipped.tiff"];
				break;
			case 1:
				image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
				break;
			case 2:
				image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
				break;
			case 3:
				image = [NSImage imageNamed:@"RedDotFlipped.tiff"];
				break;
			default:
				image = [NSImage imageNamed:@"GreyDotFlipped.tiff"];	
		}
		float fraction;
		if ([self outlineViewEnabled]) fraction = 1.0;
		else fraction = 0.5;
		[image drawInRect:NSMakeRect(cellFrame.origin.x + xOffset, cellFrame.origin.y+(0.18 * cellFrame.size.height), cellFrame.size.height * 0.8, cellFrame.size.height * 0.8)
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver
				 fraction:fraction];
		xOffset += 12.0;
	}
	
	/* Create reduced-size rectangle */
	if ([self outlineViewEnabled])
	{ [self setTextColor:[NSColor blackColor]]; }
	else
	{ [self setTextColor:[NSColor grayColor]]; }
	NSRect textRect = NSMakeRect(cellFrame.origin.x + xOffset,
								 cellFrame.origin.y,
								 cellFrame.size.width,cellFrame.size.height);	
	
	/* Draw and restore colour */
	[super drawInteriorWithFrame:textRect inView:controlView];	
	[self setTextColor:[NSColor blackColor]];
}

#pragma mark "Observed Object"

@synthesize representedObject;
@synthesize parentRepresentedObject;
@synthesize outlineViewEnabled;
@synthesize selected;

@end
