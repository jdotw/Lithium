//
//  LTAPIRequest.h
//  Lithium
//
//  Created by James Wilson on 22/12/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

@class LTCustomer;

@interface LTAPIRequest : NSObject 
{
	NSMutableData *receivedData;
	LTCustomer *customer;
	
	BOOL refreshInProgress;
}

@property (nonatomic, retain) LTCustomer *customer;
@property (nonatomic, assign) BOOL refreshInProgress;

@end
