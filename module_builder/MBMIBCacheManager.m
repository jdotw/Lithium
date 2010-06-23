//
//  MBMIBCacheManager.m
//  ModuleBuilder
//
//  Created by James Wilson on 25/04/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBMIBCacheManager.h"


@implementation MBMIBCacheManager

- (void) awakeFromNib
{
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(cleanCache:) name:@"NSApplicationWillTerminateNotification" object:nil];
}

- (void) cleanCache:(NSNotification *)note
{
	int pid = getpid ();
	NSString *cachePath = [[NSString stringWithFormat:@"~/Library/Caches/com.lithiumcorp.modulebuilder/%i", pid] stringByExpandingTildeInPath];
	[[NSFileManager defaultManager] removeItemAtPath:cachePath error:nil];
}

@end
