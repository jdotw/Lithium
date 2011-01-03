//
//  LCSearchResult.h
//  Lithium Console
//
//  Created by James Wilson on 2/01/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class LCEntityAddress, LCResourceAddress;

@interface LCSearchResult : NSObject 
{
	NSInteger entityType;
	NSString *desc;
	LCEntityAddress *entityAddress;
	LCResourceAddress *resourceAddress;
	NSString *custDesc;
	NSString *siteDesc;
	NSString *devDesc;
	NSString *cntDesc;
	NSString *objDesc;
}

@property (nonatomic,assign) NSInteger entityType;
@property (nonatomic,retain) NSString *desc;
@property (nonatomic,retain) LCEntityAddress *entityAddress;
@property (nonatomic,retain) LCResourceAddress *resourceAddress;
@property (nonatomic,retain) NSString *custDesc;
@property (nonatomic,retain) NSString *siteDesc;
@property (nonatomic,retain) NSString *devDesc;
@property (nonatomic,retain) NSString *cntDesc;
@property (nonatomic,retain) NSString *objDesc;

@end
