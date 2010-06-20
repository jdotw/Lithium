//
//  LC49ImportTask.h
//  LCAdminTools
//
//  Created by James Wilson on 2/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LC49ImportTask : NSObject 
{
	NSString *name;
	NSString *result;
	NSImage *icon;
	
	double progressPercent;
	BOOL isIndeterminate;
}

@property (copy) NSString *name;
@property (copy) NSString *result;
@property (copy) NSImage *icon;

@property (assign) double progressPercent;
@property (assign) BOOL isIndeterminate;

- (void) setSucceeded;
- (void) setFailed;

@end
