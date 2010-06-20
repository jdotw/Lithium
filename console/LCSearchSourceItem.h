//
//  LCSearchSourceItem.h
//  Lithium Console
//
//  Created by James Wilson on 25/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCSearchRequest.h"

@interface LCSearchSourceItem : NSObject 
{
	LCCustomer *customer;
	NSArray *searchRequests;
	
	NSString *searchString;
	BOOL searchInProgress;
	
	NSString *displayString;	
	
	NSMutableArray *results;
}

#pragma mark "Search Methods"
- (void) search;

#pragma mark "Properties"
@property (retain) LCCustomer *customer;
@property (copy) NSArray *searchRequests;
@property (copy) NSString *displayString;
@property (copy) NSString *searchString;
@property (readonly) NSMutableArray *results;
- (void) insertObject:(LCEntity *)entity inResultsAtIndex:(unsigned int)index;
- (void) removeObjectFromResultsAtIndex:(unsigned int)index;

@end
