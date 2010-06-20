//
//  LCActionLogEntry.h
//  Lithium Console
//
//  Created by James Wilson on 3/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"

@interface LCActionLogEntry : LCXMLObject 
{
	int logID;
	int timestampSeconds;
	NSDate *timestamp;
	NSString *comment;
}

#pragma mark "Properties"
@property (assign) int logID;
@property (assign) int timestampSeconds;
@property (copy) NSDate *timestamp;
@property (copy) NSString *comment;

@end
