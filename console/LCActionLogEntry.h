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
@property (nonatomic, assign) int logID;
@property (nonatomic, assign) int timestampSeconds;
@property (nonatomic,copy) NSDate *timestamp;
@property (nonatomic,copy) NSString *comment;

@end
