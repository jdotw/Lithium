//
//  MBMib.h
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBMib : NSObject 
{
	NSMutableDictionary *properties;
}

#pragma mark Constructors
+ (MBMib *) mibFromFile:(NSString *)path;
- (MBMib *) initWithFile:(NSString *)path;
- (void) dealloc;

#pragma mark Accessors
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;
- (NSString *) filename;
- (void) setFilename:(NSString *)string;
- (NSData *) mibData;
- (void) setMibData:(NSData *)data;

@end
