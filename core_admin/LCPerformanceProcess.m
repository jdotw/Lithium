//
//  LCPerformanceProcess.m
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCPerformanceProcess.h"


@implementation LCPerformanceProcess

#pragma mark "Constructors"

+ (LCPerformanceProcess *) processWithColumns:(NSArray *)columns
{
	return [[[LCPerformanceProcess alloc] initWithColumns:columns] autorelease];
}

- (LCPerformanceProcess *) initWithColumns:(NSArray *)columns
{
	[super init];

	if  ([columns count] > 5)
	{
		NSEnumerator *colEnum = [columns objectEnumerator];
		NSString *col;
		int index = 0;
		while (col = [colEnum nextObject])
		{
			if ([col isEqualToString:@""]) continue;
			switch (index)
			{
				case 1:
					[self setProcessID:col];
					break;
				case 2:
					[self setCpuPercent:col];
					break;
				case 4:
					[self setVSize:col];
					break;
				case 5:
					[self setRSize:col];
					break;
			}
			index++;
		}
	}
	
	return self;
}

@end
