//
//  LCVRackCableGroup.h
//  Lithium Console
//
//  Created by James Wilson on 20/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"

@interface LCVRackCableGroup : LCXMLObject
{
	NSString *uuid;
	NSString *desc;
}

#pragma mark "Accessors"
@property (nonatomic,copy) NSString *uuid;
@property (nonatomic,copy) NSString *desc;

@end
