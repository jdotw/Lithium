//
//  MBCoreCustomer.h
//  ModuleBuilder
//
//  Created by James Wilson on 22/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBCoreCustomer : NSObject 
{
	NSString *name;
	NSString *cluster;
	NSString *node;
	NSString *url;
}

+ (MBCoreCustomer *) customerWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)intiUrl;
- (MBCoreCustomer *) initWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)intiUrl;

- (NSString *) entityAddressString;
- (NSString *) resourceAddressString;

@property (copy) NSString *name;
@property (copy) NSString *cluster;
@property (copy) NSString *node;
@property (copy) NSString *url;


@end
