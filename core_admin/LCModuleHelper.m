//
//  LCModuleHelper.m
//  LCAdminTools
//
//  Created by James Wilson on 16/03/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LCModuleHelper.h"


@implementation LCModuleHelper

- (void) installModule:(NSString *)fullpath filename:(NSString *)filename
{
	operation = OP_INSTALL;
	[self startHelperToolCommand:@"mod_install" arguments:[NSArray arrayWithObjects:fullpath, filename, nil]];	
}
- (void) deleteModule:(NSString *)filename
{
	operation = OP_DELETE;
	[self startHelperToolCommand:@"mod_delete" arguments:[NSArray arrayWithObject:filename]];	
}

- (BOOL) processDataFromHelper:(NSString *)data
{
	NSLog(@"%@ Helper Received: '%@' for operation=%i", self, data, operation);
	NSArray *lines = [data componentsSeparatedByString:[NSString stringWithFormat:@"\n"]];
	for (NSString *line in lines)
	{
		self.status = line;
		if ([line hasPrefix:@"OK"]) self.successful = YES;
		
		/* Install module */
		if (operation == OP_INSTALL)
		{
			if ([line hasPrefix:@"OK"] || [line hasPrefix:@"ERROR"])
			{
				[delegate performSelector:@selector(moduleInstallDidFinish:) withObject:self];
				return NO;
			}
		}		

		/* Delete module */
		if (operation == OP_DELETE)
		{
			if ([line hasPrefix:@"OK"] || [line hasPrefix:@"ERROR"])
			{
				[delegate performSelector:@selector(moduleDeleteDidFinish:) withObject:self];
				return NO;
			}
		}		
		
	}
	
	return YES;
}

@end
