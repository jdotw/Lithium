//
//  LCProcessProfile.h
//  Lithium Console
//
//  Created by James Wilson on 15/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"
#import "LCObject.h"

@interface LCProcessProfile : LCXMLObject 
{
	int profileID;
	NSString *desc;
	NSString *match;
	NSString *argumentsMatch;
	
	LCObject *object;
}

@property (assign) int profileID;
@property (copy) NSString *desc;
@property (copy) NSString *match;
@property (copy) NSString *argumentsMatch;
@property (retain) LCObject *object;

@end
