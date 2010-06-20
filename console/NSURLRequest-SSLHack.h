//
//  NSURLRequest-SSLHack.h
//  Lithium Console
//
//  Created by James Wilson on 10/06/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface NSObject (SSLHack)

+ (void) setAllowsAnyHTTPSCertificate:(BOOL)flag forHost:(NSString *)host;

@end