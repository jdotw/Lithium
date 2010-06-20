//
//  LCDiagCustomerWebPerms.m
//  LCAdminTools
//
//  Created by James Wilson on 5/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagCustomerWebPerms.h"

#include <sys/types.h>
#include <sys/stat.h>

@implementation LCDiagCustomerWebPerms

- (LCDiagCustomerWebPerms *) initForCustomerName:(NSString *)name
{
	[super init];
	customerName = [name retain];
	return self;
}

- (void) performTest:(id)initDelegate
{
	[super performTest:initDelegate];

	struct stat sb;
	NSString *path = [NSString stringWithFormat:@"/Library/Application Support/Lithium/ClientService/Resources/htdocs/%@", customerName];
	int num = stat ([path cStringUsingEncoding:NSUTF8StringEncoding], &sb);
	if (num == 0)
	{
		if ((sb.st_mode & 0775) == 0775)
		{ [self testPassed]; }
		else
		{ [self testFailed]; }
	}
	else
	{
		[self testFailed];
	}
}

- (NSString *) testDescription
{
	return [NSString stringWithFormat:@"Check '%@' Web Root permissions", customerName];
}

@end
