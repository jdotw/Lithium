//
//  LCXServiceDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 18/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXServiceDotTransformer.h"
#import "LCObject.h"

@implementation LCXServiceDotTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(LCObject *)entity
{
	NSImage *image = nil;
	
	if (!entity) return [NSImage imageNamed:@"GreyDot.tiff"];
	
	if ([[[entity properties] objectForKey:@"opstate_num"] intValue] == 0)
	{
		/* Op State is normal, use the 'state' of the service */
		NSString *state = [entity valueForMetricNamed:@"state"];
		if ([state isEqualToString:@"RUNNING"])
		{ image = [NSImage imageNamed:@"BlueDot.tiff"]; }
		else
		{ image = [NSImage imageNamed:@"GreyDot.tiff"]; }
	}
	else
	{
		/* Op State is non-normal, set to colour dot */
		switch ([[[entity properties] objectForKey:@"opstate_num"] intValue])
		{
			case 0:
				image = [NSImage imageNamed:@"GreenDot.tiff"];
				break;
			case 1:
				image = [NSImage imageNamed:@"YellowDot.tiff"];
				break;
			case 2:
				image = [NSImage imageNamed:@"YellowDot.tiff"];
				break;
			case 3:
				image = [NSImage imageNamed:@"RedDot.tiff"];
				break;
			default:
				image = [NSImage imageNamed:@"GreyDot.tiff"];	
		}
	}
	
	return image;
}


@end
