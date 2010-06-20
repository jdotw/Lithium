//
//  LCSSceneScrollerView.m
//  Lithium Console
//
//  Created by James Wilson on 1/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSSceneScrollerView.h"
#import "LCEntity.h"
#import "LCIncident.h"
#import "LCCustomer.h"

@implementation LCSSceneScrollerView

#pragma mark "Constructors"

- (LCSSceneScrollerView *) initWithOverlay:(LCSSceneOverlay *)initOverlay inFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	
	/* Set overlay */
	overlay = [initOverlay retain];		

	return self;
}

- (void) removeViewAndContent
{
	[self removeFromSuperview];
}

- (void) dealloc
{
	[overlay release];
	[super dealloc];
}

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rect
{
	/* Get incidents */
	NSMutableArray *incidents = [[(LCCustomer *)[[overlay entity] customer] activeIncidentsList] incidentsForEntity:[overlay entity]];
	
	/* Get text size */
    NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSFont systemFontOfSize: 7], NSFontAttributeName,
        nil];	
	NSSize textSize = [@"test" sizeWithAttributes:attr];
	
	/* Loop through incidents */
	int line = 1;
	LCIncident *inc;
	for (inc in incidents)
	{
		NSPoint textPoint = NSMakePoint(0,[self bounds].size.height - (line * textSize.height));
		NSMutableString *incString = [NSMutableString string];
		NSString *custString = @"";
		NSString *siteString = @"";
		NSString *devString = @"";
		NSString *cntString = @"";
		NSString *objString = @"";
		NSString *metString = @"";
		NSString *trgString = @"";
		int i;
		for (i = ([[[overlay entity] typeInteger] intValue] + 1); i < [[[inc entity] typeInteger] intValue]; i++)
		{
			NSString *entityString = [[[inc entity] entityInHierarchyOfType:i] displayString];
			switch (i)
			{
				case 1: 
					custString = entityString;
					break;
				case 2: 
					siteString = entityString;
					break;
				case 3: 
					devString = entityString;
					break;
				case 4: 
					cntString = entityString;
					break;
				case 5: 
					objString = entityString;
					break;
				case 6: 
					metString = entityString;
					break;
				case 7: 
					trgString = entityString;
					break;
			}
			[incString appendFormat:@"%@ ", entityString];
		}
		
		if ([[[overlay entity] typeInteger] intValue] >= 3)
		{
			/* Device or higher */
			incString = [NSMutableString stringWithFormat:@"%@ %@ %@ %@", objString, metString, trgString, cntString];
		}
		
		NSDictionary *incAttr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSFont systemFontOfSize: 8], NSFontAttributeName,
			[[inc entity] opStateColor], NSForegroundColorAttributeName,
			nil];

		[incString drawAtPoint:textPoint withAttributes:incAttr];
		
		line++;
	}
}

#pragma mark "Hit Test"

- (NSView *)hitTest:(NSPoint)aPoint
{
	return [self superview];
}

@synthesize overlay;
@end
