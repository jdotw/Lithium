//
//  MBEnum.h
//  ModuleBuilder
//
//  Created by James Wilson on 12/05/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ModuleDocument;

@interface MBEnum : NSObject 
{
	NSMutableDictionary *properties;
	ModuleDocument *document;
}

+ (MBEnum *) enumWithLabel:(NSString *)label forValue:(NSString *)value;
- (NSXMLNode *) xmlNode;
@property (nonatomic, retain) NSMutableDictionary *properties;
@property (nonatomic, retain) NSString *value;
@property (nonatomic, retain) NSString *label;
@property (nonatomic, assign) ModuleDocument *document;

@end
