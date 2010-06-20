//
//  LCName.m
//  Lithium Console
//
//  Created by James Wilson on 30/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCName.h"


@implementation LCName

+ (NSString *) parse:(NSString *)input
{
	const char *input_str = [input UTF8String];
	if (!input_str) return nil;
	char *output_str = strdup (input_str);
	
	size_t i;
	for (i=0; i < strlen(output_str); i++)
	{
		if (isalnum(output_str[i]) == 0 && output_str[i] != '-')
		{ output_str[i] = '_'; }
	}
	
	NSString *output = [NSString stringWithUTF8String:output_str];
	free (output_str);
	
	return output;
}

@end
