//
//  LCMetricValue.h
//  Lithium Console
//
//  Created by James Wilson on 1/10/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLNode.h"

@interface LCMetricValue : NSObject 
{
	float value;
	NSString *valueString;
	NSString *rawValueString;
	unsigned long timestamp;
}

- (id) initWithXmlNode:(LCXMLNode *)xmlNode;

@property (assign) float value;
@property (copy) NSString *valueString;
@property (copy) NSString *rawValueString;
@property (assign) unsigned long timestamp;

@end
