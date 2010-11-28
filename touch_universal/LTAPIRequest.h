//
//  LTAPIRequest.h
//  Lithium
//
//  Created by James Wilson on 22/12/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

@class LTCustomer;

@interface LTAPIRequest : NSOperation 
{
	NSMutableURLRequest *urlReq;
	
	NSMutableData *receivedData;
	LTCustomer *customer;
	
	BOOL refreshInProgress;
	BOOL finished;
	BOOL debug;
	
	id delegate;
}

@property (nonatomic, assign) id delegate;
@property (nonatomic, retain) LTCustomer *customer;
@property (nonatomic, assign) BOOL refreshInProgress;
@property (nonatomic, assign) BOOL debug;

@end
