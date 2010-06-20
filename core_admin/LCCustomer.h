//
//  LCCustomer.h
//  LCAdminTools
//
//  Created by James Wilson on 23/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCCustomer : NSObject 
{
	NSString *name;
	NSString *desc;
}

+ (LCCustomer *) customerWithName:(NSString *)initName desc:(NSString *)initDesc;
@property (copy) NSString *name;
@property (copy) NSString *desc;


@end
