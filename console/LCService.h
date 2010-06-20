//
//  LCService.h
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCScriptedTask.h"
#import "LCServiceScript.h"
#import "LCServiceScriptList.h"
#import "LCObject.h"
#import "LCDevice.h"
#import "LCXMLRequest.h"

@interface LCService : LCScriptedTask 
{
	/* Related Objects */
	LCObject *object;	
}

#pragma mark "Constructors"
+ (LCService *) serviceWithObject:(LCEntity *)initObject;
+ (LCService *) newServiceForDevice:(LCEntity *)initDevice;
- (id) initServiceWithObject:(LCObject *)initObject;
- (id) initNewServiceForDevice:(LCDevice *)initDevice;

#pragma mark "Accessors"
@property (retain) LCObject *object;

@end
