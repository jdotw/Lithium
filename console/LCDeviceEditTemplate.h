//
//  LCDeviceEditTemplate.h
//  Lithium Console
//
//  Created by James Wilson on 9/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"

@interface LCDeviceEditTemplate : NSObject 
{
	/* Template Properties */
	NSString *name;
	NSString *desc;
	NSString *ipAddress;
	NSString *lomIpAddress;
	BOOL enabled;
	
	/* Preferred Module */
	NSString *preferredModule;
	
	/* XML Operation */
	LCXMLRequest *xmlReq;
	BOOL xmlOperationInProgress;
	
	/* Status */
	NSImage *resultIcon;
}

+ (LCDeviceEditTemplate *) templateWithProperties:(NSDictionary *)initProperties;
- (LCDeviceEditTemplate *) initWithProperties:(NSDictionary *)initProperties;

@property (nonatomic,copy) NSString *name;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,copy) NSString *ipAddress;
@property (nonatomic,copy) NSString *lomIpAddress;
@property (nonatomic, assign) BOOL enabled;

@property (nonatomic,copy) NSString *preferredModule;

@property (nonatomic,retain) LCXMLRequest *xmlReq;
@property (nonatomic, assign) BOOL xmlOperationInProgress;

@property (nonatomic,copy) NSImage *resultIcon;

- (NSDictionary *) properties;

@end
