//
//  NSURLRequest-SSLHack.m
//  Lithium Console
//
//  Created by James Wilson on 10/06/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "NSURLRequest-SSLHack.h"


@implementation NSURLRequest (SSLHack)

+ (void) setAllowsAnyHTTPSCertificate:(BOOL)flag forHost:(NSString *)host
{
	[NSURLRequest setAllowsAnyHTTPSCertificate:flag forHost:host];
}

@end
