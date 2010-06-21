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

@property (nonatomic, assign) int profileID;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,copy) NSString *match;
@property (nonatomic,copy) NSString *argumentsMatch;
@property (nonatomic,retain) LCObject *object;

@end
