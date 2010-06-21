//
//  LCXMLNode.h
//  Lithium Console
//
//  Created by James Wilson on 13/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCXMLNode : NSObject 
{
	NSString *name;
	NSMutableString *value;
	
	NSMutableDictionary *properties;
	NSMutableArray *children;
}

@property (nonatomic,copy) NSString *name;
@property (readonly) NSMutableString *value;

@property (readonly) NSMutableDictionary *properties;
@property (readonly) NSMutableArray *children;

@end
