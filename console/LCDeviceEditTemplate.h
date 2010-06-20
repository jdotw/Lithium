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

@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (copy) NSString *ipAddress;
@property (copy) NSString *lomIpAddress;
@property (assign) BOOL enabled;

@property (copy) NSString *preferredModule;

@property (retain) LCXMLRequest *xmlReq;
@property (assign) BOOL xmlOperationInProgress;

@property (copy) NSImage *resultIcon;

- (NSDictionary *) properties;

@end
