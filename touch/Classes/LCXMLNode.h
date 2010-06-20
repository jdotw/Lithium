//
//  LCXMLNode.h
//  Lithium Console
//
//  Created by James Wilson on 13/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface LCXMLNode : NSObject 
{
	NSString *name;
	NSMutableString *value;
	
	NSMutableDictionary *properties;
	NSMutableArray *children;
}

@property (copy) NSString *name;
@property (readonly) NSMutableString *value;

@property (readonly) NSMutableDictionary *properties;
@property (readonly) NSMutableArray *children;

@end
