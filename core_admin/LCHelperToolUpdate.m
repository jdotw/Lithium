//
//  LCHelperToolUpdate.m
//  LCAdminTools
//
//  Created by James Wilson on 12/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LCHelperToolUpdate.h"


@implementation LCHelperToolUpdate

- (id) initWithArchive:(NSString *)initArchive extractTo:(NSString *)initExtract containingPackage:(NSString *)initPackage delegate:(id)initDelegate
{
	[super init];
	
	self.delegate = initDelegate;
	[super startHelperToolCommand:@"update" arguments:[NSArray arrayWithObjects:initArchive, initExtract, initPackage, nil]];
	
	return self;
}

- (BOOL) processDataFromHelper:(NSString *)data
{
	NSArray *lines = [data componentsSeparatedByString:[NSString stringWithFormat:@"\n"]];
	for (NSString *line in lines)
	{
		if ([line hasPrefix:@"installer:%"])
		{
			NSString *percentString = [line stringByReplacingOccurrencesOfString:@"installer:%" withString:@""];
			float percent = [percentString floatValue] * 100.0;
			self.progress = percent;
		}
		else if ([line hasPrefix:@"installer:PHASE:The software was successfully installed"])
		{
			[self informDelegteHelperFinished];
			return NO;
		}
		else if ([line hasPrefix:@"installer:PHASE:"])
		{
			self.status = [line stringByReplacingOccurrencesOfString:@"installer:PHASE:" withString:@""];
		}
	}
	
	return YES;
}

@end
