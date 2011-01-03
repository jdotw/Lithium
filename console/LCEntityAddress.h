//
//  LCEntityAddress.h
//  Lithium Console
//
//  Created by James Wilson on 15/08/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

@interface LCEntityAddress : NSObject 
{
	/* Address variables */
	NSNumber *type;
	NSString *cust;
	NSString *site;
	NSString *dev;
	NSString *cnt;
	NSString *obj;
	NSString *met;
	NSString *trg;
}

+ (LCEntityAddress *) addressWithString:(NSString *)string;
+ (LCEntityAddress *) addressForEntity:(id)entity;
- (LCEntityAddress *) initWithString:(NSString *)string;
- (LCEntityAddress *) initForEntity:(id)entity;
- (LCEntityAddress *) init;

- (NSString *)typeString;

@property (readonly,getter=typeString) NSString *typeString;
@property (readonly,getter=addressString) NSString *addressString;
@property (readonly) NSNumber *type;
@property (readonly) NSString *cust;
@property (readonly) NSString *site;
@property (readonly) NSString *dev;
@property (readonly) NSString *cnt;
@property (readonly) NSString *obj;
@property (readonly) NSString *met;
@property (readonly) NSString *trg;
@end
